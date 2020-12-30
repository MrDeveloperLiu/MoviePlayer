//
//  fok_frame.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_frame.h"

FokFrame *fok_frame_alloc(void)
{
    FokFrame *f = fok_malloc(sizeof(FokFrame));
    return f;
}

void fok_frame_free(FokFrame *frame)
{
    fok_free(frame->buf);
    fok_free(frame);
}

void fok_frame_set_data(FokFrame *frame, uint8_t *data, int size)
{
    if (!frame || !data) {
        return;
    }    
    frame->buf = fok_malloc(size);
    if (frame->buf)
        memcpy(frame->buf, data, size);
    frame->buf_size = size;
}

void fok_frame_fill(FokFrame *frame, AVFrame *fframe)
{
    if (!frame || !fframe) {
        return;
    }
    frame->dts = fframe->pkt_dts;
    frame->pts = fframe->pts;
    frame->duration = fframe->pkt_duration;
    
    frame->w = fframe->width;
    frame->h = fframe->height;

    frame->channels = fframe->channels;
    frame->sample_rate = fframe->sample_rate;
}

FokFrame *fok_frame_copy(const FokFrame *f)
{
    if (!f) {
        return NULL;
    }
    FokFrame *frame = fok_frame_alloc();
    frame->buf_type = f->buf_type;
    fok_frame_set_data(frame, f->buf, f->buf_size);
    
    frame->dts = f->dts;
    frame->pts = f->pts;
    frame->duration = f->duration;
    
    frame->w = f->w;
    frame->h = f->h;

    frame->channels = f->channels;
    frame->sample_rate = f->sample_rate;
    return frame;
}
