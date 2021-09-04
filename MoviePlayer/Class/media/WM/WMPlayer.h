//
//  WMPlayer.h
//  BLEShoes
//
//  Created by Heaton on 2017/12/18.
//  Copyright © 2017年 wangming. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>

typedef void (*WMPlayerAudioOut) (void *buffer, int32_t size, void **ref, void *userdata);

struct WMPlayerAudio {
    int32_t mBufferSize;
    int32_t mSampleRate;
    int32_t mChannels;
    int32_t mBitsPerChannel;
    int32_t mFrameSize;
    int32_t mSampleSize;
    WMPlayerAudioOut mAudioInput;
    void *mUserData;
};

typedef struct WMPlayerAudio WMPlayerAudio;

@interface WMPlayer : NSObject
@property(nonatomic, readonly) AudioStreamBasicDescription format;
@property(nonatomic, assign) CGFloat volume;
@property(nonatomic, readonly) WMPlayerAudio audio;

- (instancetype)initWithAudio:(WMPlayerAudio)audio volume:(CGFloat)volume;
- (void)start;
- (void)stop;
@end
