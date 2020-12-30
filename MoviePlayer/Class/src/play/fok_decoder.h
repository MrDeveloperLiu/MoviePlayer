//
//  fokplayer_decoder.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/3.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fokplayer_decoder_h
#define fokplayer_decoder_h

#include <stdio.h>

typedef void (*FokPlayerDecoderCallback) (FokPlayerDecoder *decoder, FokFrame *frame, void *ctx);
struct FokPlayerDecoder {
    FokType             type;

    AVCodec             *codec;
    AVCodecContext      *codec_ctx;
    AVStream            *stream;
    int                 stream_index;
    float               time_base;
    int64_t             start_time;
    
    FokPlayerDecoderCallback frame_decode_cb;
};

typedef void (*FokPlayerDecoderRunable) (void *ctx, AVFrame *frame);

FokPlayerDecoder *fok_decoder_alloc(FokPlayer *player, AVStream *stream, FokType type);

void fok_decoder_free(FokPlayerDecoder *decoder);

int fok_decoder_pair_packet(FokPlayerDecoder *decoder, AVPacket *packet);

void fok_decoder_decode_packet(FokPlayerDecoder *decoder, AVPacket *packet, FokPlayerDecoderRunable runable, void *ctx);

void fok_decoder_set_decode_callback(FokPlayerDecoder *decoder, FokPlayerDecoderCallback cb);


#endif /* fokplayer_decoder_h */
