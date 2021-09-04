//
//  gframe.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gframe.h"
#include "goplay.h"

GFrame *g_frame_alloc(void)
{
    GFrame *f = (GFrame *)gmalloc(sizeof(GFrame));
    return f;
}
GFrame *g_frame_alloc_with_avframe(void)
{
    GFrame *f = g_frame_alloc();
    if (f) {
        f->frame = av_frame_alloc();
    }
    return f;
}

void g_frame_release(GFrame *f)
{
    if (f) {
        if (f->type == GFrameType_Audio) {
            gfree(f->buf[0]);
        }else if (f->type == GFrameType_Video) {
            gfree(f->buf[0]);
            gfree(f->buf[1]);
            gfree(f->buf[2]);
        }
        if (f->frame) {
            av_frame_free(&f->frame);
        }
    }
    gfree(f);
}

int64_t g_frame_geta_duration(GFrame *f, GClock c)
{
    if (!f || c.pts <= 0) {
        return 0;
    }
    int64_t ds = g_frame_get_pts(f) - c.pts;
    return ds;
}

int64_t g_frame_getv_duration(GFrame *f, GClock c)
{
    if (!f || c.pts <= 0) {
        return 0;
    }
    int64_t ds = g_frame_get_pts(f) - c.pts;
    return ds;
}

int64_t g_frame_get_pts(GFrame *f)
{
    AVFrame *frame = f->frame;
    return frame->pts;
}

void g_frame_fill_audio_buf(GFrame *f, GPlayer *player)
{
    if (!f || !player)
        return;
    
    AVFrame *frame = f->frame;
    int size = av_samples_get_buffer_size(NULL, player->video_param.channels, frame->nb_samples, player->video_param.sample_fmt, 0);
    f->buf[0] = (uint8_t *)gmalloc(sizeof(uint8_t)*size);
    f->linesize[0] = size;
    f->ret = swr_convert(player->swr, f->buf, size, (const uint8_t **)frame->data, frame->nb_samples);
}

void g_frame_fill_video_buf(GFrame *f, GPlayer *player)
{
    if (!f || !player)
        return;

    AVFrame *frame = f->frame;
    
    int size = frame->width * frame->height;
    
    f->buf[0] = (uint8_t *)gmalloc(sizeof(uint8_t)*size);
    f->buf[1] = (uint8_t *)gmalloc(sizeof(uint8_t)*(size >> 1));
    f->buf[2] = (uint8_t *)gmalloc(sizeof(uint8_t)*(size >> 1));
    
    f->linesize[0] = frame->width;
    f->linesize[1] = frame->width >> 1;
    f->linesize[2] = frame->width >> 1;
    
    f->ret = sws_scale(player->sws, (const uint8_t **)frame->data, frame->linesize, 0, frame->height, f->buf, f->linesize);
}
