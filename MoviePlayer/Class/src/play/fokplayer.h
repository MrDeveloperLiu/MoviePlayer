//
//  fokplayer.hpp
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fokplayer_hpp
#define fokplayer_hpp

#include <stdio.h>

#define FokPacketQueueMaxSize  15
#define FokPacketQueueMinSize  3
#define FokNullPacketDuration 100
#define FokFrameSyncDiff 500

struct FokPlayer {
    AVFormatContext *ctx;
    int             play;
    int             abort;
    pthread_mutex_t lock;
    
    FokVideoEngine  *videoeng;
    FokAudioEngine  *audioeng;
    
    FokRender       *video_render;
    FokRender       *audio_render;
    
    FokClock        vclock;
    FokClock        aclock;
    
    FokThread       *read_thread;
    FokThread       *vrefresh_thread;
    FokThread       *arefresh_thread;

    pthread_mutex_t vpk_lock;
    FokQueue        *vpk_queue;
    pthread_mutex_t apk_lock;
    FokQueue        *apk_queue;
};

FokPlayer *fok_player_init(void);

void fok_player_free(FokPlayer *player);

FokStatus fok_player_open_input(FokPlayer *player, const char *url);

void fok_player_close_input(FokPlayer *player);

void fok_player_play(FokPlayer *player);

void fok_player_pause(FokPlayer *player);

void fok_player_set_video_render(FokPlayer *player, void *render, FokRenderCallback callback);

void fok_player_set_audio_render(FokPlayer *player, void *render, FokRenderCallback callback);


#endif /* fokplayer_hpp */
