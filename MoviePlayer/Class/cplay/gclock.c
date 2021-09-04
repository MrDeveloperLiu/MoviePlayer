//
//  gclock.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/26.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gclock.h"


int64_t g_clock_now(void)
{
    struct timeval tm;
    gettimeofday(&tm, NULL);
#if GClockUnixTime
    int64_t ts = tm.tv_sec + tm.tv_usec / 1000000;
#else
    int64_t ts = tm.tv_sec * 1000 + tm.tv_usec / 1000;
#endif
    return ts;
}

GClock g_clock_make(void)
{
    GClock c = {0};
#if GClockUnixTime
    c.delta = 1;
#else
    c.delta = 1000;
#endif
    return c;
}

void g_clock_tick(GClock *c, int64_t pts, int64_t ts)
{
    if (!c)
        return;
    
    c->ts = ts;
    c->pts = pts;
}

double g_clock_get_current(GClock c)
{
    double tm = c.pts / (double)c.delta;
    return tm;
}

int64_t g_clock_diff(GClock lc, GClock rc)
{
    if (lc.pts <= 0 || rc.pts <= 0) {
        return 0;
    }
    return rc.pts - lc.pts;
}
