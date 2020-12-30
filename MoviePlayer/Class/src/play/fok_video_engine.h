//
//  fok_video_decoder.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_video_decoder_h
#define fok_video_decoder_h

#include <stdio.h>


struct FokVideoEngine {
    FokPlayer           *player;
    FokQueue            *queue;
    FokPlayerDecoder    *decoder;
    float               fps;
    struct SwsContext   *sws;
    pthread_mutex_t     mutex;
};

FokVideoEngine *fok_video_decoderpar_alloc(FokPlayer *player, AVStream *stream);

void fok_video_decoderpar_free(FokVideoEngine *decoderpar);

void fok_video_decode_packet(FokVideoEngine *decoderpar, AVPacket *packet);

FokQueueTask *fok_video_decoderpar_dequeue(FokVideoEngine *decoderpar);

void fok_video_decoderpar_next(FokVideoEngine *decoderpar);

#endif /* fok_video_decoder_h */
