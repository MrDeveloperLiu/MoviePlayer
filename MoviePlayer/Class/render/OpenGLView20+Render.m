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
    CGFloat scaleFactor = _videoW / deviceSize.width;
    CGFloat w = deviceSize.width * scale;
    CGFloat h = scaleFactor * _videoH;
    return CGRectMake(0, 0, w, h);
}

- (void)render:(FokFrame *)frame{
    if ([NSThread isMainThread])
    {
        [self displayYUV420pData:frame->buf width:frame->w height:frame->h];
    }
    else
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            [self displayYUV420pData:frame->buf width:frame->w height:frame->h];
        });        
    }
}

@end
