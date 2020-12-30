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
@property (nonatomic, readonly) AudioQueueBufferRef *mBuf;
@property (nonatomic, readonly) AudioQueueRef audioQueue;
@property (nonatomic, readonly) AudioQueueTimelineRef timeline;
@property (nonatomic, readonly) NSLock *lock;
@end


@implementation WMPlayer

- (void)dealloc{
    _block = nil;
    [self stop];
}

- (instancetype)initSampleRate:(Float64)sampleRate
               ChannelsNumber:(UInt32)channels
               BitsPerChannel:(UInt32)bitsPerChannel
                     frameSize:(UInt32)frameSize
                       volume:(CGFloat)volume{
    if(self = [super init]){
        _sampleRate = sampleRate;
        _channels = channels;
        _volume = volume;
        _bitsPerChannel = bitsPerChannel;
        _frameSize = frameSize;
        _lock = [[NSLock alloc] init];
        [self resetSetting];
    }
    return self;
}

- (void)resetSetting{
    @try {
        NSError *error = nil;
        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:&error];
    } @catch (NSException *exception) {
        NSLog(@"[[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:&error] : %@", exception);
    }

    AudioStreamBasicDescription format = fok_audio_queue_init_format(_sampleRate, _channels, _bitsPerChannel);
    
    OSStatus state = AudioQueueNewOutput(&format,
                                         AudioPlayerAQInputCallback,
                                         (__bridge void * _Nullable)self,
                                         NULL,
                                         NULL,
                                         0,
                                         &_audioQueue);
    if(state != noErr){
        NSLog(@"AudioQueueNewOutput Fail");
    }
    state = AudioQueueSetParameter(_audioQueue, kAudioQueueParam_Volume, _volume);
    if(state != noErr){
        NSLog(@"set Volume Fail");
    }
    
    UInt32 size = _frameSize * _channels * (_bitsPerChannel / 8);
    fok_audio_queue_init_buffers(_audioQueue, &_mBuf, FokAudioBufSize, size);
    
    AudioQueueCreateTimeline(_audioQueue, &_timeline);
    AudioQueueStart(_audioQueue, NULL);
}

static void AudioPlayerAQInputCallback(void *inUserData, AudioQueueRef audioQueueRef, AudioQueueBufferRef audioQueueBufferRef)
{
    WMPlayer *player = (__bridge WMPlayer *)(inUserData);
    if (player.block) {
        player.block(audioQueueBufferRef->mUserData);
    }
    [player.lock lock];
    fok_audio_queue_idle_buffer(player.audioQueue, player.mBuf, FokAudioBufSize, audioQueueBufferRef);
    [player.lock unlock];
}

- (AudioTimeStamp)getCurrentTs{
    AudioTimeStamp ts;
    Boolean outTimelineDiscontinuity = YES;
    AudioQueueGetCurrentTime(self.audioQueue, self.timeline, &ts, &outTimelineDiscontinuity);
    return ts;
}

- (void)playWithData:(uint8_t *)data length:(uint32_t)length context:(void *)context{
    [_lock lock];
    fok_audio_queue_enqueue_buffer(_audioQueue, _mBuf, FokAudioBufSize, data, length);
    [_lock unlock];
}

- (void)stop{
    if (_audioQueue)
    {
        AudioQueueStop(_audioQueue, true);
        
        fok_audio_queue_dispose_buffers(_audioQueue, &_mBuf, FokAudioBufSize);
        if (_timeline) {
            AudioQueueDisposeTimeline(_audioQueue, _timeline);
        }
        
        AudioQueueDispose(_audioQueue, true);
        _audioQueue = nil;
    }
}
@end

