//
//  gavcodec.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gavcodec_h
#define gavcodec_h

#include <stdio.h>
#include "gframe.h"

typedef struct GCodec GCodec;
typedef void(*GCodecDecodeFn)(GCodec *c, GFrame *f, GPointer info);

struct GCodec {
    AVCodec *codec;
    AVCodecContext *ctx;
    AVStream *stream;
};

GCodec *g_codec_alloc(AVStream *s);

void g_codec_release(GCodec *c);

#endif /* gavcodec_h */
