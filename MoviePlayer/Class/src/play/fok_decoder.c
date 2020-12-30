//
//  fokplayer_decoder.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/3.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_decoder.h"

FokPlayerDecoder *fok_decoder_alloc(FokPlayer *player, AVStream *stream, FokType type)
{
    if (!player || !stream) {
        return NULL;
    }
    FokPlayerDecoder *decoder = fok_malloc(sizeof(FokPlayerDecoder));
    decoder->stream = stream;
    decoder->type = type;
    decoder->codec = avcodec_find_decoder(stream->codecpar->codec_id);
    if (!decoder->codec) {
        goto fail;
    }
    decoder->codec_ctx = avcodec_alloc_context3(decoder->codec);
    if (!decoder->codec_ctx) {
        goto fail;
    }
    if (avcodec_parameters_to_context(decoder->codec_ctx, stream->codecpar) < 0) {
        goto fail;
    }
    if (avcodec_open2(decoder->codec_ctx, decoder->codec, NULL) < 0) {
        goto fail;
    }        
    decoder->start_time = stream->start_time;
    
    return decoder;
    
fail:
    fok_decoder_free(decoder);
    return NULL;
}

void fok_decoder_free(FokPlayerDecoder *decoder)
{
    if (decoder->codec_ctx) {
        avcodec_free_context(&decoder->codec_ctx);
    }
    fok_free(decoder);
}

void fok_decoder_decode_packet(FokPlayerDecoder *decoder, AVPacket *packet, FokPlayerDecoderRunable runable, void *ctx)
{
    if (!decoder || !packet) {
        return;
    }
    if (packet->stream_index != decoder->stream_index) {
        return;
    }
    if (avcodec_send_packet(decoder->codec_ctx , packet) != 0) {
        return;
    }
    AVFrame *frame = av_frame_alloc();
    if (avcodec_receive_frame(decoder->codec_ctx , frame) == 0) {
        if (runable) {
            runable(ctx, frame);
        }
    }
    av_frame_free(&frame);
}

int fok_decoder_pair_packet(FokPlayerDecoder *decoder, AVPacket *packet)
{
    if (!decoder) {
        return 0;
    }
    if (packet->stream_index == decoder->stream_index) {
        return 1;
    }
    return 0;
}

void fok_decoder_set_decode_callback(FokPlayerDecoder *decoder, FokPlayerDecoderCallback cb)
{
    if (!decoder) {
        return;
    }
    decoder->frame_decode_cb = cb;
}
