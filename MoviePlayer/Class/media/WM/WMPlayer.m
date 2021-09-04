//
//  WMPlayer.m
//  BLEShoes
//
//  Created by Heaton on 2017/12/18.
//  Copyright © 2017年 wangming. All rights reserved.
//

#import "WMPlayer.h"
#include "fok_audio_queue_ios_render.h"

@interface WMPlayer()
{
    AudioQueueBufferRef mBuf[FokAudioBufSize];
}
@property (nonatomic, readonly) AudioQueueRef audioQueue;
@property (nonatomic, readonly) NSLock *lock;
@end


@implementation WMPlayer

- (void)dealloc{
    [self stop];
}

- (instancetype)initWithAudio:(WMPlayerAudio)audio volume:(CGFloat)volume{
    if(self = [super init]){
        _audio = audio;
        _volume = volume;
        
        fok_audio_queue_init_format(&_format,
                                    audio.mSampleRate,
                                    audio.mChannels,
                                    audio.mBitsPerChannel);

        
        if (AudioQueueNewOutput(&_format,
                                AudioPlayerAQInputCallback,
                                (__bridge void * _Nullable)self,
                                NULL,
                                kCFRunLoopCommonModes,
                                0,
                                &_audioQueue) != noErr){
            NSLog(@"AudioQueueNewOutput Fail");
        }
        if (AudioQueueSetParameter(_audioQueue, kAudioQueueParam_Volume, _volume) != noErr){
            NSLog(@"set Volume Fail");
        }
        _lock = [[NSLock alloc] init];
        [self audioSession];
        UInt32 size = audio.mSampleSize;
        for (int i = 0; i < FokAudioBufSize; i++)
        {
            AudioQueueAllocateBuffer(_audioQueue, size, &mBuf[i]);
            mBuf[i]->mAudioDataByteSize = size;
            memset(mBuf[i]->mAudioData, 0, size);
            AudioQueueEnqueueBuffer(_audioQueue, mBuf[i], 0, NULL);
        }
    }
    return self;
}

- (void)setVolume:(CGFloat)volume{
    if (_volume != volume) {
        [_lock lock];
        _volume = volume;
        if (AudioQueueSetParameter(_audioQueue, kAudioQueueParam_Volume, _volume) != noErr){
            NSLog(@"set Volume Fail");
        }
        [_lock unlock];
    }
}

- (void)audioSession{
    NSError *error = nil;
    @try {
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback
                                               error:&error];
    } @catch (NSException *exception) {
        NSLog(@"AVAudioSession : %@", error);
    }
}

- (void)start{
    if (!_audioQueue)
        return;
    
    AudioQueueStart(_audioQueue, NULL);
}



- (void)stop{
    if (!_audioQueue)
        return;
    
    AudioQueueStop(_audioQueue, true);
    for (int i = 0; i < FokAudioBufSize; i++)
        AudioQueueFreeBuffer(_audioQueue, mBuf[i]);
    AudioQueueDispose(_audioQueue, true);
    _audioQueue = nil;
}

static void AudioPlayerAQInputCallback(void *inUserData,
                                       AudioQueueRef audioQueueRef,
                                       AudioQueueBufferRef audioQueueBufferRef)
{
    WMPlayer *player = (__bridge WMPlayer *)(inUserData);
    if (player.audio.mAudioInput) {
        player.audio.mAudioInput(audioQueueBufferRef->mAudioData,
                                 audioQueueBufferRef->mAudioDataByteSize,
                                 &audioQueueBufferRef->mUserData,
                                 player.audio.mUserData);
    }
    AudioQueueEnqueueBuffer(audioQueueRef, audioQueueBufferRef, 0, NULL);
}
@end

