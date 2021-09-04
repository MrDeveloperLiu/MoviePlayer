//
//  gbase.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gbase.h"

void *gmalloc(size_t size)
{
    void *p = malloc(size);
    if (p) {
        memset(p, 0, size);
    }else{
        gprintf("gmalloc(%d)  failed!", size);
    }
    return p;
}

void gfree(void *p)
{
    if (p) {
        free(p);        
    }
}

void gfreep(void **p)
{
    if (*p) {
        free(*p);
        *p = NULL;
    }
}

void gprintf(const char *fmt, ...)
{
    char message[1024];
    va_list arg;
    va_start(arg, fmt);
    vsprintf(message, fmt, arg);
    va_end(arg);
    printf("[Glog]: %s\n", message);
}

int gusleep(int c)
{
    if (c <= 0 || c >= INT_MAX) {
        return 0;
    }
    return usleep(c);
}

AVStream *g_av_get_stream(AVFormatContext *ctx, enum AVMediaType type)
{
    for (int i = 0; i < ctx->nb_streams; i++)
    {
        AVStream *stream = ctx->streams[i];
        if (stream->codecpar->codec_type == type)
            return stream;
    }
    return NULL;
}

float g_av_stream_timebase_get(AVStream *st, AVCodecContext *codec_ctx, float default_tm_base, float *p_fps)
{
    float fps, timebase;
    
    if (st->time_base.den && st->time_base.num)
        timebase = av_q2d(st->time_base);
    else if(codec_ctx->time_base.den && codec_ctx->time_base.num)
        timebase = av_q2d(codec_ctx->time_base);
    else
        timebase = default_tm_base;
                
         
    if (st->avg_frame_rate.den && st->avg_frame_rate.num)
        fps = av_q2d(st->avg_frame_rate);
    else if (st->r_frame_rate.den && st->r_frame_rate.num)
        fps = av_q2d(st->r_frame_rate);
    else
        fps = 1.0 / timebase;
            
    if (p_fps)
        *p_fps = fps;
    return timebase;
}


float g_av_audio_frenquency(AVCodecContext *codec_ctx)
{
    return (codec_ctx->frame_size / (float)codec_ctx->sample_rate) * 1000;
}

int g_avg_display(float fps)
{
    return (1 / fps) * 1000;
}
