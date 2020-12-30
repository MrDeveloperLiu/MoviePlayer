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

#define FokAudioBufSize 20

AudioStreamBasicDescription fok_audio_queue_init_format(Float64 sampleRate, UInt32 channelsPerFrame, UInt32 bitsPerChannel);


void fok_audio_queue_init_buffers(AudioQueueRef queue, AudioQueueBufferRef **buf, size_t size, UInt32 buf_len);

void fok_audio_queue_dispose_buffers(AudioQueueRef queue, AudioQueueBufferRef **buf, size_t size);

int fok_audio_queue_enqueue_buffer(AudioQueueRef queue, AudioQueueBufferRef *buf, size_t size, uint8_t *data, UInt32 data_len);

void fok_audio_queue_idle_buffer(AudioQueueRef queue, AudioQueueBufferRef *buf, size_t size, AudioQueueBufferRef buffer);

#endif /* fok_audio_queue_ios_render_h */
