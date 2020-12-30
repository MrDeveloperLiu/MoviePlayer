//
//  fok_audio_queue_ios_render.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/30.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_audio_queue_ios_render.h"


AudioStreamBasicDescription fok_audio_queue_init_format(Float64 sampleRate, UInt32 channelsPerFrame, UInt32 bitsPerChannel)
{
    AudioStreamBasicDescription format = {0};
    format.mSampleRate = sampleRate;
    format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    format.mFramesPerPacket = 1;
    format.mChannelsPerFrame =channelsPerFrame;
    format.mBitsPerChannel = bitsPerChannel;
    format.mBytesPerFrame = (format.mBitsPerChannel / 8) * format.mChannelsPerFrame;
    format.mBytesPerPacket = format.mBytesPerFrame * format.mFramesPerPacket;
    return format;
}

void fok_audio_queue_init_buffers(AudioQueueRef queue, AudioQueueBufferRef **buf, size_t size, UInt32 buf_len)
{
    if (!buf) {
        return;
    }
    AudioQueueBufferRef *newbuf = calloc(size, sizeof(AudioQueueBufferRef));
    *buf = newbuf;

    for (int i = 0; i < size; i++) {
        AudioQueueBufferRef buffer = NULL;
        AudioQueueAllocateBuffer(queue, buf_len, &buffer);
        memset(buffer->mAudioData, 0, buffer->mAudioDataBytesCapacity);
        buffer->mAudioDataByteSize = 0;
        newbuf[i] = buffer;
    }
}

void fok_audio_queue_dispose_buffers(AudioQueueRef queue, AudioQueueBufferRef **buf, size_t size)
{
    if (!buf) {
        return;
    }
    AudioQueueBufferRef *newbuf = *buf;
    for (int i = 0; i < size; i++) {
        AudioQueueBufferRef buffer = newbuf[i];
        AudioQueueFreeBuffer(queue, buffer);
    }
    free(newbuf);
    *buf = NULL;
}

int fok_audio_queue_enqueue_buffer(AudioQueueRef queue, AudioQueueBufferRef *buf, size_t size, uint8_t *data, UInt32 data_len)
{
    if (!buf) {
        return -1;
    }
    for (int i = 0; i < size; i++) {
        AudioQueueBufferRef buffer = buf[i];
        if (!buffer->mAudioDataByteSize)
        {
            buffer->mAudioDataByteSize = data_len;
            memcpy(buffer->mAudioData, data, data_len);
            AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);
            return 0;
        }
    }
    return -1;
}

void fok_audio_queue_idle_buffer(AudioQueueRef queue, AudioQueueBufferRef *buf, size_t size, AudioQueueBufferRef buffer)
{
    if (!buffer) {
        return;
    }
    memset(buffer->mAudioData, 0, buffer->mAudioDataByteSize);
    buffer->mAudioDataByteSize = 0;
}
