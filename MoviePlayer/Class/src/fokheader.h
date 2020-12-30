//
//  fokheader.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/1.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fokheader_h
#define fokheader_h

#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libavutil/avutil.h"
#include "libavutil/time.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"

#define FokMAX(a, b) ((a > b) ? a : b)
#define FokMIN(a, b) ((a < b) ? a : b)
#define FokTrue     1
#define FokFalse    0

enum FokStatus {
    FokStatusOK,
    FokStatusError
};
typedef enum FokStatus FokStatus;

enum FokType {
    FokTypeVideo = 1,
    FokTypeAudio
};
typedef enum FokType FokType;

typedef struct FokThread FokThread;
typedef struct FokTime FokTime;
typedef struct FokRender FokRender;
typedef struct FokQueueTask FokQueueTask;
typedef struct FokQueue FokQueue;
typedef struct FokFrame FokFrame;
typedef struct FokPlayer FokPlayer;
typedef struct FokVideoEngine FokVideoEngine;
typedef struct FokAudioEngine FokAudioEngine;
typedef struct FokPlayerDecoder FokPlayerDecoder;

#include "fok_util.h"
#include "fok_thread.h"
#include "fok_clock.h"
#include "fok_queue.h"
#include "fok_frame.h"
#include "fok_frame_render.h"
#include "fok_audio_engine.h"
#include "fok_video_engine.h"
#include "fokplayer.h"
#include "fok_decoder.h"

#endif /* fokheader_h */
