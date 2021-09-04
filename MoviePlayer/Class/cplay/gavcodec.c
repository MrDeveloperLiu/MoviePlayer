//
//  gavcodec.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gavcodec.h"


GCodec *g_codec_alloc(AVStream *s)
{
    GCodec *c = (GCodec *)gmalloc(sizeof(GCodec));
    if (c) {
        c->stream = s;
        c->codec = avcodec_find_decoder(c->stream->codecpar->codec_id);
        if (!c->codec) {
            gprintf("Err: codec.codec is null");
        }
        c->ctx = avcodec_alloc_context3(c->codec);
        if (!c->ctx) {
            gprintf("Err: codec.ctx is null");
        }
        if (avcodec_parameters_to_context(c->ctx, c->stream->codecpar) < 0) {
            gprintf("Err: codec avcodec_parameters_to_context() failed");
        }
        if (avcodec_open2(c->ctx, c->codec, NULL) < 0) {
            gprintf("Err: codec avcodec_open2() failed");
        }
    }
    return c;
}

void g_codec_release(GCodec *c)
{
    if (c) {
        avcodec_free_context(&c->ctx);
    }
    gfree(c);
}
