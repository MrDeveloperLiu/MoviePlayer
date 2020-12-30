//
//  fok_clock.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/4.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_clock.h"

FokTime fok_time_make(void)
{
    FokTime t = {0};
    t.den = 1000;
    return t;
}

FokClock fok_clock_make(void)
{
    FokClock c = {0};
    c.time = fok_time_make();
    return c;
}

void fok_clock_display(FokClock *clock, int64_t timestamp, int64_t pts)
{
    clock->time.num = pts;
    clock->ts = timestamp;
    if (clock->serial >= INT64_MAX) {
        clock->serial = 0;
    }else{
        clock->serial++;
    }
}

void fok_clock_increase(FokClock *clock, int64_t timestamp, int64_t display)
{
    clock->time.num += display;
    clock->ts = timestamp;    
    if (clock->serial >= INT64_MAX) {
        clock->serial = 0;
    }else{
        clock->serial++;
    }
}

int64_t fok_clock_diff(FokClock *clock, int64_t timestamp)
{
    if (clock->ts <= 0) {
        return 0;
    }
    return timestamp - clock->ts;
}

float fok_clock_get_time(FokClock clock)
{
    return fok_time_duration(clock.time);
}

float fok_time_duration(FokTime time)
{
    return (float)time.num / (float)time.den;
}

float fok_time_interval_duration(FokTime time, int64_t interval)
{
    return (float)interval / (float)time.den;
}
