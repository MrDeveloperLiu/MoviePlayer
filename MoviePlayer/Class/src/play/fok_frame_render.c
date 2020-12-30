//
//  fok_frame_render.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_frame_render.h"

FokRender *fok_frame_render_alloc(void *p, int type, FokRenderCallback callback)
{
    FokRender *r = fok_malloc(sizeof(FokRender));
    r->p = p;
    r->type = type;
    r->callback = callback;
    return r;
}

void fok_frame_render_free(FokRender *render)
{
    fok_free(render);
}

void fok_frame_render_receive(FokRender *render, FokFrame *frame){
    if (!render || !frame) {
        return;
    }
    if (render->callback) {
        render->callback(render->p, frame);
    }
}
