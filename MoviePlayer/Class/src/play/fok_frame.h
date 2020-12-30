//
//  fok_frame.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_frame_h
#define fok_frame_h

#include <stdio.h>

struct FokFrame {
    FokType         buf_type;
    int             buf_size;
    uint8_t         *buf;
    
    int             w;
    int             h;
    
    int64_t         pts;
    int64_t         dts;
    
    int             sample_rate;
    int             channels;
    
    float           duration;
};

FokFrame *fok_frame_alloc(void);

void fok_frame_free(FokFrame *frame);

void fok_frame_set_data(FokFrame *frame, uint8_t *data, int size);

void fok_frame_fill(FokFrame *frame, AVFrame *fframe);

FokFrame *fok_frame_copy(const FokFrame *f);

#endif /* fok_frame_h */
