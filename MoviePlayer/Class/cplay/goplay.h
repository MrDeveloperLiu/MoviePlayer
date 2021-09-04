//
//  goplay.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef goplay_h
#define goplay_h

#include <stdio.h>
#include "gqueue.h"
#include "gavcodec.h"
#include "gthreadlocal.h"
#include "gclock.h"
#include "gmsgqueue.h"

#define GPlayerBothLimit 0
#define GPlayerWaitDuration 100
#define GPlayerSyncDuration 300
#define GPlayerDropDuration 2000


typedef struct GPlayer GPlayer;
typedef struct GPlayerRender GPlayerRender;

GPlayer *g_player_alloc(void);

void g_player_release(GPlayer *p);

void g_player_open_input(GPlayer *p, const char *url);

void g_player_close_input(GPlayer *p);

void g_player_play(GPlayer *p);

void g_player_pause(GPlayer *p);

void g_player_audio_buffer(void *buffer, int32_t size, void **ref, void *userdata);

struct GPlayerRender {
    GPointer owner;
    void (*open_engine)(GPlayerRender r, AVCodecContext *ctx);
    void (*render_frame) (GPlayerRender r, GQueueTask *t, GFrame *f);
    void (*start) (GPlayerRender r);
    void (*stop) (GPlayerRender r);
};

struct GVideoParamter {
    float avg_fps;
    float avf_frenqucy;
    
    
    int aqueue_max;
    int aqueue_min;

    int vqueue_max;
    int vqueue_min;
    
    int channels;
    enum AVSampleFormat sample_fmt;
    enum AVPixelFormat pixel_fmt;
};

struct GPlayer {
    GMutex *lock;
    GMutex *pkt_lock;
    
    struct GVideoParamter video_param;
    AVFormatContext *ctx;
    struct SwrContext *swr;
    struct SwsContext *sws;

    int playing;
    int abort;
    
    GThreadLocal *packet_thread;
    GThreadLocal *video_thread;
                
    GQueue *audio_queue;
    GQueue *video_queue;
    
    GCodec *video_codec;
    GCodec *audio_codec;
    
    GPlayerRender video_render;
    GPlayerRender audio_render;
    
    GClock audio_clk;
    GClock video_clk;
    
    GMessageQueue *msg_queue;
};


enum GPlayerMsgType {
    GPlayerMsgType_Play = 1,
    GPlayerMsgType_Pause,
    GPlayerMsgType_OpenInput,
};


#endif /* goplay_h */
