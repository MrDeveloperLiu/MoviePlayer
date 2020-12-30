//
//  LavCode.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#import "LavCode.h"

void av_yuvbuffer_array_free(uint8_t **buffer)
{
    if (buffer == NULL) {
        return;
    }
    if (buffer[0]) {
        free(buffer[0]);
    }
    if (buffer[1]) {
        free(buffer[1]);
    }
    if (buffer[2]) {
        free(buffer[2]);
    }
}

uint8_t *av_yuvbuffer_alloc(uint8_t *buffer[], int linesize[], int height, int *size)
{
    if (buffer == NULL || linesize == NULL) {
        return NULL;
    }
    int y = linesize[0] * height;
    int u = (linesize[1] >> 1) * height;
    int v = (linesize[2] >> 1) * height;
    int s = y + u + v;
    uint8_t *retVal = fok_malloc(s);
    memcpy(retVal, buffer[0], y);
    memcpy(retVal + y, buffer[1], u);
    memcpy(retVal + (y + u), buffer[2], v);
    if (size != NULL) {
        *size = s;
    }
    return retVal;
}

void av_yuv_data_get_size(size_t *sample_size, int *sample_width, int *sample_height, int vw, int vh)
{
    sample_width[0] = vw;
    sample_width[1] = vw >> 1;
    sample_width[2] = vw >> 1;
    
    sample_height[0] = vh;
    sample_height[1] = vh >> 1;
    sample_height[2] = vh >> 1;

    sample_size[0] = sample_width[0] * sample_height[0];
    sample_size[1] = sample_width[1] * sample_height[1];
    sample_size[2] = sample_width[2] * sample_height[2];
}

uint8_t *av_data_alloc(uint8_t *buffer[], int w, int h, int *size)
{
    int y = w * h;
    int u = y >> 1;
    int v = u;
    int s = y + u + v;
    uint8_t *retVal = fok_malloc(s);
    memcpy(retVal, buffer[0], y);
    memcpy(retVal + y, buffer[1], u);
    memcpy(retVal + (y + u), buffer[2], v);
    if (size) {
        *size = s;
    }
    return retVal;
}

void av_yuvbuffer_free(void *buffer)
{
    if (!buffer) {
        return;
    }
    free(buffer);
}
