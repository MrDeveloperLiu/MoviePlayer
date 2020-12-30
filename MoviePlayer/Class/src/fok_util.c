//
//  fok_util.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_util.h"

void *fok_malloc(size_t size)
{
    void *p = malloc(size);
    if (!p) {
        fok_log("malloc error -> size:%d", size);
    }
    memset(p, 0, size);
    return p;
}

void fok_free(void *p)
{
    if (!p)
        return;
    
    free(p);
}

void fok_freep(void *p)
{
    if (!p)
        return;
    
    void *val;
    memcpy(&val, p, sizeof(val));
    memcpy(p, &(void *){0}, sizeof(val));
    free(val);
}

AVStream *fok_av_get_stream(AVFormatContext *ctx, enum AVMediaType type)
{
    for (int i = 0; i < ctx->nb_streams; i++) {
        AVStream *stream = ctx->streams[i];
        if (stream->codecpar->codec_type == type)
        {
            return stream;
        }
    }
    return NULL;
}

void fok_av_stream_timebase_get(AVStream *st, AVCodecContext *codec_ctx, float default_tm_base, float *p_fps, float *p_timebase)
{
    float fps, timebase;
    
    if (st->time_base.den && st->time_base.num)
    {
        timebase = av_q2d(st->time_base);
    }
    else if(codec_ctx->time_base.den && codec_ctx->time_base.num)
    {
        timebase = av_q2d(codec_ctx->time_base);
    }
    else
    {
        timebase = default_tm_base;
    }
                
         
    if (st->avg_frame_rate.den && st->avg_frame_rate.num)
    {
        fps = av_q2d(st->avg_frame_rate);
    }
    else if (st->r_frame_rate.den && st->r_frame_rate.num)
    {
        fps = av_q2d(st->r_frame_rate);
    }
    else
    {
        fps = 1.0 / timebase;
    }
            
    if (p_fps)
    {
        *p_fps = fps;
    }
    if (p_timebase)
    {
        *p_timebase = timebase;
    }
        
}


float fok_av_audio_frenquency(AVCodecContext *codec_ctx)
{
    return (codec_ctx->frame_size / (float)codec_ctx->sample_rate) * 1000;
}

int fok_usleep(fok_usec tm)
{
    return usleep(tm);
}


void fok_log(const char *fmt, ...)
{
#if FokLogLevel
    char message[1024];
    va_list arg;
    va_start(arg, fmt);
    vsprintf(message, fmt, arg);
    va_end(arg);
    printf("FokLOG: %s \n", message);
#endif
}
