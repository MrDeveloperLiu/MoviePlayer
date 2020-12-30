//
//  fok_audio_decoder.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_audio_engine.h"

void fok_audio_frame_received(FokAudioEngine *decoderpar, FokFrame *frame);

FokAudioEngine *fok_audio_decoderpar_alloc(FokPlayer *player, AVStream *stream)
{
    if (!player || !stream) {
        return NULL;
    }
    FokAudioEngine *decoderpar = fok_malloc(sizeof(FokAudioEngine));
    decoderpar->decoder = fok_decoder_alloc(player, stream, FokTypeAudio);
    decoderpar->decoder->stream_index = av_find_best_stream(player->ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    decoderpar->player = player;
    decoderpar->queue = fok_queue_alloc(FokQueueFIFO);
    decoderpar->frenquency = fok_av_audio_frenquency(decoderpar->decoder->codec_ctx);
    fok_av_stream_timebase_get(stream, decoderpar->decoder->codec_ctx, 0.025, NULL, &decoderpar->decoder->time_base);
    decoderpar->swr = swr_alloc_set_opts(NULL,
                                         AV_CH_LAYOUT_STEREO,
                                         AV_SAMPLE_FMT_S16,
                                         decoderpar->decoder->codec_ctx->sample_rate,
                                         decoderpar->decoder->codec_ctx->channel_layout,
                                         decoderpar->decoder->codec_ctx->sample_fmt,
                                         decoderpar->decoder->codec_ctx->sample_rate,
                                         0,
                                         NULL);
    swr_init(decoderpar->swr);
    pthread_mutex_init(&decoderpar->mutex, NULL);
    return decoderpar;
}

void fok_audio_decoderpar_free(FokAudioEngine *decoderpar)
{
    if (decoderpar->swr) {
        swr_free(&decoderpar->swr);
    }
    pthread_mutex_destroy(&decoderpar->mutex);
    fok_decoder_free(decoderpar->decoder);
    fok_queue_free(decoderpar->queue);
    fok_free(decoderpar);
}

void fok_audio_decode_avframe(FokAudioEngine *decoderpar, AVFrame *frame)
{
    if (!decoderpar || !frame) {
        return;
    }
    FokPlayerDecoder *decoder = decoderpar->decoder;
    if (!decoder) {
        return;
    }
    int size = av_samples_get_buffer_size(NULL, decoder->codec_ctx->channels, frame->nb_samples, AV_SAMPLE_FMT_S16, 1);
    uint8_t *outbuf[AV_NUM_DATA_POINTERS] = { fok_malloc(size), NULL };
    
    if (swr_convert(decoderpar->swr, outbuf, size, (const uint8_t **)frame->data, frame->nb_samples) > 0) {
        //copy f
        FokFrame *fok = fok_frame_alloc();
        fok->buf_type = FokTypeAudio;
        fok_frame_fill(fok, frame);
        //data
        fok_frame_set_data(fok, outbuf[0], size);
        //enqueue
        pthread_mutex_lock(&decoderpar->mutex);
        fok_queue_enqueue_func(decoderpar->queue, fok, fok->buf_size, NULL, (FokTaskFreeFunc)fok_frame_free);
        pthread_mutex_unlock(&decoderpar->mutex);
        //cb
        fok_audio_frame_received(decoderpar, fok);
    }
    
    av_yuvbuffer_array_free(outbuf);
}

void fok_audio_decode_packet(FokAudioEngine *decoderpar, AVPacket *packet)
{
    fok_decoder_decode_packet(decoderpar->decoder, packet, (FokPlayerDecoderRunable)fok_audio_decode_avframe, decoderpar);
}

FokQueueTask *fok_audio_decoderpar_dequeue(FokAudioEngine *decoderpar)
{
    if (!decoderpar || !decoderpar->queue) {
        return NULL;
    }
    pthread_mutex_lock(&decoderpar->mutex);
    FokQueueTask *t = fok_queue_task_dequeue(decoderpar->queue);
    pthread_mutex_unlock(&decoderpar->mutex);
    return t;
}

void fok_audio_decoderpar_next(FokAudioEngine *decoderpar)
{
    if (!decoderpar || !decoderpar->queue) {
        return;
    }
    fok_queue_task_next(decoderpar->queue);
}

void fok_audio_frame_received(FokAudioEngine *decoderpar, FokFrame *frame)
{
    //cb
    if (decoderpar->decoder->frame_decode_cb)
    {
        (*(decoderpar->decoder->frame_decode_cb))(decoderpar->decoder, frame, decoderpar);
    }
}

