//
//  fok_audio_decoder.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_audio_decoder_h
#define fok_audio_decoder_h

#include <stdio.h>


struct FokAudioEngine {
    FokPlayer           *player;
    FokQueue            *queue;
    FokPlayerDecoder    *decoder;
    struct SwrContext   *swr;
    float               frenquency;
    pthread_mutex_t     mutex;
};

FokAudioEngine *fok_audio_decoderpar_alloc(FokPlayer *player, AVStream *stream);

void fok_audio_decoderpar_free(FokAudioEngine *decoderpar);

void fok_audio_decode_packet(FokAudioEngine *decoderpar, AVPacket *packet);

FokQueueTask *fok_audio_decoderpar_dequeue(FokAudioEngine *decoderpar);

void fok_audio_decoderpar_next(FokAudioEngine *decoderpar);


#endif /* fok_audio_decoder_h */
