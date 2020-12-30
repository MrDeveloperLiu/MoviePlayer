//
//  WMPlayer+Render.m
//  MoviePlayer
//
//  Created by 刘杨 on 2020/3/16.
//  Copyright © 2020 liu. All rights reserved.
//

#import "WMPlayer+Render.h"


@implementation WMPlayer (Render)

- (void)render:(FokFrame *)frame{
//    FokFrame *f = fok_frame_copy(frame);
    FokFrame *f = NULL;
    [self playWithData:frame->buf length:frame->buf_size context:f];
}

@end
