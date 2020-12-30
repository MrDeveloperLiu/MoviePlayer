//
//  fok_video_decoder.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_video_engine.h"

void fok_video_frame_received(FokVideoEngine *decoderpar, FokFrame *frame);

FokVideoEngine *fok_video_decoderpar_alloc(FokPlayer *player, AVStream *stream)
{
    if (!player || !stream) {
        return NULL;
    }
    FokVideoEngine *decoderpar = fok_malloc(sizeof(FokVideoEngine));
    decoderpar->decoder = fok_decoder_alloc(player, stream, FokTypeVideo);
    decoderpar->decoder->stream_index = av_find_best_stream(player->ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    decoderpar->player = player;
    decoderpar->queue = fok_queue_alloc(FokQueueFIFO);
    fok_av_stream_timebase_get(stream, decoderpar->decoder->codec_ctx, 0.04, &decoderpar->fps, &decoderpar->decoder->time_base);
    decoderpar->sws = sws_getContext(decoderpar->decoder->codec_ctx->width,
                                     decoderpar->decoder->codec_ctx->height,
                                     decoderpar->decoder->codec_ctx->pix_fmt,
                                     decoderpar->decoder->codec_ctx->width,
                                     decoderpar->decoder->codec_ctx->height,
                                     AV_PIX_FMT_YUV420P,
                                     SWS_FAST_BILINEAR,
                                     NULL,
                                     NULL,
                                     NULL);
    pthread_mutex_init(&decoderpar->mutex, NULL);
    return decoderpar;
}

void fok_video_decoderpar_free(FokVideoEngine *decoderpar)
{
    if (decoderpar->sws) {
        sws_freeContext(decoderpar->sws);
    }
    pthread_mutex_destroy(&decoderpar->mutex);
    fok_decoder_free(decoderpar->decoder);
    fok_queue_free(decoderpar->queue);
    fok_free(decoderpar);
}

void fok_video_decode_avframe(FokVideoEngine *decoderpar, AVFrame *frame)
{
    if (!decoderpar || !frame) {
        return;
    }
    FokPlayerDecoder *decoder = decoderpar->decoder;
    if (!decoder) {
        return;
    }
    int size = frame->width * frame->height;
    uint8_t *buffer[4] = { fok_malloc(size), fok_malloc(size >> 1), fok_malloc(size >> 1), NULL};
    int linsize[4] = { frame->width, frame->width >> 1, frame->width >> 1, 0};
    
    if (sws_scale(decoderpar->sws, (const uint8_t **)frame->data, frame->linesize, 0, frame->height, buffer, linsize) > 0) {
        //copy f
        FokFrame *fok = fok_frame_alloc();
        fok->buf_type = FokTypeVideo;
        fok_frame_fill(fok, frame);
        //data
        int size = 0;
        uint8_t *data = av_yuvbuffer_alloc(buffer, linsize, frame->height, &size);
        fok_frame_set_data(fok, data, size);
        av_yuvbuffer_free(data);
        
        //enqueue
        pthread_mutex_lock(&decoderpar->mutex);
        fok_queue_enqueue_func(decoderpar->queue, fok, fok->buf_size, NULL, (FokTaskFreeFunc)fok_frame_free);
        pthread_mutex_unlock(&decoderpar->mutex);
        //cb
        fok_video_frame_received(decoderpar, fok);
    }
    
    av_yuvbuffer_array_free(buffer);
}

void fok_video_decode_packet(FokVideoEngine *decoderpar, AVPacket *packet)
{
    fok_decoder_decode_packet(decoderpar->decoder, packet, (FokPlayerDecoderRunable)fok_video_decode_avframe, decoderpar);
}

FokQueueTask *fok_video_decoderpar_dequeue(FokVideoEngine *decoderpar)
{
    if (!decoderpar || !decoderpar->queue) {
        return NULL;
    }
    pthread_mutex_lock(&decoderpar->mutex);
    FokQueueTask *t = fok_queue_task_dequeue(decoderpar->queue);
    pthread_mutex_unlock(&decoderpar->mutex);
    return t;
}

void fok_video_decoderpar_next(FokVideoEngine *decoderpar)
{
    if (!decoderpar || !decoderpar->queue) {
        return;
    }
    fok_queue_task_next(decoderpar->queue);
}


void fok_video_frame_received(FokVideoEngine *decoderpar, FokFrame *frame)
{
    //cb
    if (decoderpar->decoder->frame_decode_cb)
    {
        (*(decoderpar->decoder->frame_decode_cb))(decoderpar->decoder, frame, decoderpar);
    }
}
