//
//  gframe.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gframe_h
#define gframe_h

#include <stdio.h>
#include "gbase.h"
#include "gclock.h"

typedef struct GFrame GFrame;

typedef struct GPlayer GPlayer;

enum GFrameType {
    GFrameType_Audio = 1,
    GFrameType_Video,
    GFrameType_Subtitle
};
typedef enum GFrameType GFrameType;

struct GFrame {
    GFrameType type;
    AVFrame *frame;
    uint8_t *buf[AV_NUM_DATA_POINTERS];
    int linesize[AV_NUM_DATA_POINTERS];
    int ret;
};

GFrame *g_frame_alloc(void);
GFrame *g_frame_alloc_with_avframe(void);
void g_frame_release(GFrame *f);

void g_frame_fill_video_buf(GFrame *f, GPlayer *player);
void g_frame_fill_audio_buf(GFrame *f, GPlayer *player);

int64_t g_frame_geta_duration(GFrame *f, GClock c);
int64_t g_frame_getv_duration(GFrame *f, GClock c);
int64_t g_frame_get_pts(GFrame *f);

#endif /* gframe_h */
