//
//  fok_audio_queue_ios_render.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/30.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_audio_queue_ios_render.h"


void fok_audio_queue_init_format(AudioStreamBasicDescription *desc, Float64 sampleRate, UInt32 channelsPerFrame, UInt32 bitsPerChannel)
{

    desc->mSampleRate = sampleRate;
    desc->mFormatID = kAudioFormatLinearPCM;
    desc->mFormatFlags = kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    desc->mFramesPerPacket = 1;
    desc->mChannelsPerFrame = channelsPerFrame;
    desc->mBitsPerChannel = bitsPerChannel;
    desc->mBytesPerFrame = desc->mBitsPerChannel * desc->mChannelsPerFrame / 8; 
    desc->mBytesPerPacket = desc->mBytesPerFrame * desc->mFramesPerPacket;
}

int32_t fok_audio_buffer_size(int32_t framesize, int32_t channels, int32_t bits_per_channel)
{
    return framesize * channels * (bits_per_channel / 8);
}


