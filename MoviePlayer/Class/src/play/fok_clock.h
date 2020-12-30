//
//  fok_clock.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/4.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_clock_h
#define fok_clock_h

#include <stdio.h>

struct FokTime {
    int64_t num;
    int64_t den;
};

struct FokClock {
    FokTime     time;
    int64_t     ts;
    int64_t     total;
    int64_t     serial;
};

typedef struct FokClock FokClock;

FokTime fok_time_make(void);

FokClock fok_clock_make(void);

void fok_clock_display(FokClock *clock, int64_t timestamp, int64_t pts);

void fok_clock_increase(FokClock *clock, int64_t timestamp, int64_t display);

int64_t fok_clock_diff(FokClock *clock, int64_t timestamp);

float fok_clock_get_time(FokClock clock);

float fok_time_duration(FokTime time);

float fok_time_interval_duration(FokTime time, int64_t interval);

#endif /* fok_clock_h */
