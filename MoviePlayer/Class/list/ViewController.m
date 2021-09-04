//
//  ViewController.m
//  MoviePlayer
//
//  Created by 刘杨 on 2020/3/13.
//  Copyright © 2020 liu. All rights reserved.
//

#import "ViewController.h"
#import <AVFoundation/AVFoundation.h>
#import "WMPlayer.h"
#import "OpenGLView20+Render.h"

#include "goplay.h"

#define kUseMp4 0

@interface ViewController () <AVCaptureVideoDataOutputSampleBufferDelegate>
{
    GPlayer *_gp;
}
@property (nonatomic) GPlayer *gp;
@property (nonatomic, strong) OpenGLView20 *glView;
@property (nonatomic, strong) WMPlayer *wmPlayer;

@property (nonatomic, strong) UIImage *nBImage;
@property (nonatomic, strong) UIImage *nSImage;;
@property (nonatomic, assign) BOOL nTranslucent;
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

- (void)viewWillAppear:(BOOL)animated{
    [super viewWillAppear:animated];
    self.nBImage = [self.navigationController.navigationBar backgroundImageForBarMetrics:UIBarMetricsDefault];
    self.nSImage = self.navigationController.navigationBar.shadowImage;
    self.nTranslucent = self.navigationController.navigationBar.translucent;    
    [self.navigationController.navigationBar setBackgroundImage:[UIImage new] forBarMetrics:UIBarMetricsDefault];
    [self.navigationController.navigationBar setShadowImage:[UIImage new]];
    [self.navigationController.navigationBar setTranslucent:YES];
    [self openGPlayer];
}

- (void)viewWillDisappear:(BOOL)animated{
    [super viewWillDisappear:animated];
    [self.navigationController.navigationBar setBackgroundImage:self.nBImage forBarMetrics:UIBarMetricsDefault];
    [self.navigationController.navigationBar setShadowImage:self.nSImage];
    [self.navigationController.navigationBar setTranslucent:self.nTranslucent];
    [self closeGPlayer];
}

void GPlayerRenderVideo(GPlayerRender r, GQueueTask *t, GFrame *f)
{
    ViewController *s = (__bridge ViewController *)r.owner;
    [s.glView render:f];
    g_queue_task_release(t);
}

void GPlayerOpenAudio(GPlayerRender r, AVCodecContext *ctx)
{
    ViewController *s = (__bridge ViewController *)r.owner;
    WMPlayerAudio audio = {0};
    audio.mChannels = s.gp->video_param.channels;
    audio.mBitsPerChannel = 16;
    audio.mSampleRate = ctx->sample_rate;
    audio.mFrameSize = ctx->frame_size;
    audio.mSampleSize = audio.mFrameSize * audio.mChannels * (audio.mBitsPerChannel / 8);
    audio.mBufferSize = (0x8010 & 0xFF) / 8 * audio.mChannels * audio.mSampleSize;
    audio.mAudioInput = g_player_audio_buffer;
    audio.mUserData = s.gp;
    s.wmPlayer = [[WMPlayer alloc] initWithAudio:audio volume:1];
}
void GPlayerOpenAudioStart(GPlayerRender r)
{
    ViewController *s = (__bridge ViewController *)r.owner;
    [s.wmPlayer start];
}
void GPlayerOpenAudioStop(GPlayerRender r)
{
    ViewController *s = (__bridge ViewController *)r.owner;
    [s.wmPlayer stop];
}

- (void)closeGPlayer{
    g_player_close_input(_gp);
    g_player_release(_gp);
}

- (void)openGPlayer{
    _gp = g_player_alloc();
    _gp->video_render = (GPlayerRender){
        .owner = (__bridge GPointer)self,
        .render_frame = GPlayerRenderVideo,
        .open_engine = NULL
    };
    _gp->audio_render = (GPlayerRender){
        .owner = (__bridge GPointer)self,
        .render_frame = NULL,
        .open_engine = GPlayerOpenAudio,
        .start = GPlayerOpenAudioStart,
        .stop = GPlayerOpenAudioStop,
    };
    g_player_open_input(_gp, [_liveAddr UTF8String]);
    g_player_play(_gp);
}


@end

