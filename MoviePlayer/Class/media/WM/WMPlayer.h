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


@interface WMPlayer : NSObject
// 采样率
@property(nonatomic, readonly) Float64 sampleRate;
// 声道数
@property(nonatomic, readonly) UInt32  channels;
// 量化数
@property(nonatomic, readonly) UInt32  bitsPerChannel;
// 音量
@property(nonatomic, readonly) CGFloat volume;

@property(nonatomic, readonly) UInt32  frameSize;

@property(nonatomic, copy) void (^block)(void *context);
/**
 初始化播放器参数
 
 @param sampleRate     音频采样率
 @param channels       音频声道数 1位单声道
 @param bitsPerChannel 每个采样点的量化数,一般为8或16
 @param volume         音量
 @return player
 
 */
- (instancetype)initSampleRate:(Float64)sampleRate
                ChannelsNumber:(UInt32)channels
                BitsPerChannel:(UInt32)bitsPerChannel
                    frameSize:(UInt32)frameSize
                       volume:(CGFloat)volume;
// 播放的数据流数据
- (void)playWithData:(uint8_t *)data length:(uint32_t)length context:(void *)context;
- (AudioTimeStamp)getCurrentTs;
- (void)stop;
@end
