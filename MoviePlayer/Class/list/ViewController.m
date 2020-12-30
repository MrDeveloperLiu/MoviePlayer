//
//  ViewController.m
//  MoviePlayer
//
//  Created by 刘杨 on 2020/3/13.
//  Copyright © 2020 liu. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "WMPlayer+Render.h"
#import "OpenGLView20+Render.h"


#define kUseMp4 0

@interface ViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    FokPlayer *_player;
}

@property (nonatomic, strong) OpenGLView20 *glView;
@property (nonatomic, strong) WMPlayer *wmPlayer;
@end

@implementation ViewController

- (void)dealloc{
    
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    CGRect frame = CGRectMake(0, 0, CGRectGetWidth(self.view.bounds), CGRectGetHeight(self.view.bounds));
    _glView = [[OpenGLView20 alloc] initWithFrame:frame];
    [self.view addSubview:_glView];
}

- (void)viewDidAppear:(BOOL)animated{
    [super viewDidAppear:animated];
    [self openVideo];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    
    [_wmPlayer stop];
    _wmPlayer = nil;
    fok_player_pause(_player);
    fok_player_close_input(_player);
    fok_player_free(_player);
}

- (void)openVideo{
    _player = fok_player_init();
#if kUseMp4
    NSBundle *b = [NSBundle bundleWithPath:[NSBundle.mainBundle pathForResource:@"res" ofType:@"bundle"]];
    NSString *mp4 = [b pathForResource:@"1" ofType:@"mp4"];
    NSURL *mp4URL = [NSURL fileURLWithPath:mp4];
    NSString *videoURL = mp4URL.absoluteString;
#else
    NSString *videoURL = _liveAddr;
#endif
    
    fok_player_open_input(_player, videoURL.UTF8String);
    fok_player_set_video_render(_player, (__bridge void *)self, FokVideoRenderCallback);
    fok_player_set_audio_render(_player, (__bridge void *)self, FokAudioRenderCallback);
    
    
    AVCodecContext *ctx = _player->audioeng->decoder->codec_ctx;
    _wmPlayer = [[WMPlayer alloc] initSampleRate:ctx->sample_rate
                                  ChannelsNumber:ctx->channels
                                  BitsPerChannel:ctx->bits_per_coded_sample
                                       frameSize:ctx->frame_size
                                          volume:1];
    
//    __weak __typeof(self) ws = self;
//    [_wmPlayer setBlock:^(void *context) {
//        __strong __typeof(ws) self = ws;
//        FokFrame *f = context;
//        fok_frame_free(f);
//    }];
    
    fok_player_play(_player);
}

void FokVideoRenderCallback(void *p, FokFrame *frame)
{
    ViewController *s = (__bridge ViewController *)p;
    if (s->_player->abort) {
        return;
    }
    [s.glView render:frame];
}

void FokAudioRenderCallback(void *p, FokFrame *frame)
{
    ViewController *s = (__bridge ViewController *)p;
    if (s->_player->abort) {
        return;
    }
    [s.wmPlayer render:frame];
}


@end

