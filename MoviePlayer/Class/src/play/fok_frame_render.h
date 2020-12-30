//
//  fok_frame_render.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_frame_render_h
#define fok_frame_render_h

#include <stdio.h>

typedef void (*FokRenderCallback)(void *p, FokFrame *frame);
struct FokRender {
    void                *p;
    FokType             type;
    FokRenderCallback   callback;
};

FokRender *fok_frame_render_alloc(void *p, int type, FokRenderCallback callback);

void fok_frame_render_free(FokRender *render);

void fok_frame_render_receive(FokRender *render, FokFrame *frame);

#endif /* fok_frame_render_h */
