//
//  fok_util.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_util_h
#define fok_util_h

#include <stdio.h>
#include "unistd.h"

#define fok_usec useconds_t

void *fok_malloc(size_t size);

void fok_free(void *p);

void fok_freep(void *p);

AVStream *fok_av_get_stream(AVFormatContext *ctx, enum AVMediaType type);

void fok_av_stream_timebase_get(AVStream *st, AVCodecContext *codec_ctx, float default_tm_base, float *p_fps, float *p_timebase);

float fok_av_audio_frenquency(AVCodecContext *codec_ctx);

int fok_usleep(fok_usec tm);


#define FokLogLevel 1
void fok_log(const char *fmt, ...);

#endif /* fok_util_h */
