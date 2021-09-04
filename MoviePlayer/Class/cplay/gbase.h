//
//  gbase.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gbase_h
#define gbase_h

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define GFnCall(fn, ...) if (fn) fn(__VA_ARGS__);
#define GMAX(A, B) ((A) > (B) ? (A): (B))
#define GMIN(A, B) ((A) < (B) ? (A): (B))

typedef void* GPointer;

void *gmalloc(size_t size);

void gfree(void *p);
void gfreep(void **p);

void gprintf(const char *fmt, ...);

int gusleep(int c);

AVStream *g_av_get_stream(AVFormatContext *ctx, enum AVMediaType type);
float g_av_stream_timebase_get(AVStream *st, AVCodecContext *codec_ctx, float default_tm_base, float *p_fps);
float g_av_audio_frenquency(AVCodecContext *codec_ctx);
int g_avg_display(float fps);

#endif /* gbase_h */
