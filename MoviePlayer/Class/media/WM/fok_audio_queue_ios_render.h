//
//  fok_audio_queue_ios_render.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/30.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_audio_queue_ios_render_h
#define fok_audio_queue_ios_render_h

#include <stdio.h>

#import <AudioToolbox/AudioToolbox.h>

#define FokAudioBufSize 3

void fok_audio_queue_init_format(AudioStreamBasicDescription *desc, Float64 sampleRate, UInt32 channelsPerFrame, UInt32 bitsPerChannel);

int32_t fok_audio_buffer_size(int32_t framesize, int32_t channels, int32_t bits_per_channel);

#endif /* fok_audio_queue_ios_render_h */
