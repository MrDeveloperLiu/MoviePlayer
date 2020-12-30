//
//  LavCode.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef LavCode_h
#define LavCode_h

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#else
#include <stdio.h>
#endif



void av_yuvbuffer_array_free(uint8_t **buffer);
uint8_t *av_data_alloc(uint8_t *buffer[], int w, int h, int *size);

uint8_t *av_yuvbuffer_alloc(uint8_t *buffer[], int linesize[], int height, int *size);
void av_yuv_data_get_size(size_t *sample_size, int *sample_width, int *sample_height, int vw, int vh);
void av_yuvbuffer_free(void *buffer);

#endif /* LavCode_h */
