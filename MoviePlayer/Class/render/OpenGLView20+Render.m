//
//  OpenGLView20+Render.m
//  MoviePlayer
//
//  Created by 刘杨 on 2020/3/16.
//  Copyright © 2020 liu. All rights reserved.
//

#import "OpenGLView20+Render.h"


@implementation OpenGLView20 (Render)

- (CGRect)glViewRect{
    CGFloat scale = UIScreen.mainScreen.scale;
    CGSize deviceSize = UIScreen.mainScreen.bounds.size;
    CGSize videoSize = CGSizeMake(_videoW, _videoH);
    
    CGFloat fw = deviceSize.width / videoSize.width;
    CGFloat fh = deviceSize.height / videoSize.height;
    CGFloat scaleFactor = (fh > fw) ? fh: fw;
    
    if (videoSize.width > videoSize.height)
    {
        CGFloat w = videoSize.width * scaleFactor;
        CGFloat h = videoSize.height * scaleFactor;
        CGFloat x = MAX((deviceSize.width * scale - w) / 2, 0);
        CGFloat y = MAX((deviceSize.height * scale - h) / 2, 0);
        return CGRectMake(x, y, w, h);
    }
    
    CGFloat w = videoSize.width * scaleFactor;
    CGFloat h = videoSize.height * scaleFactor;
    return CGRectMake(0, 0, w * scale, h * scale);
}

- (void)render:(GFrame *)frame{
    AVFrame *f = frame->frame;
    
    uint8_t *buffer = av_yuvbuffer_alloc(frame->buf, frame->linesize, f->height, NULL);
    if ([NSThread isMainThread])
    {
        [self displayYUV420pData:buffer width:f->width height:f->height];
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), ^
        {
            [self displayYUV420pData:buffer width:f->width height:f->height];
        });        
    }    
    av_yuvbuffer_free(buffer);
}


@end
