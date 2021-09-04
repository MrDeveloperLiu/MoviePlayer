//
//  gclock.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/26.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gclock_h
#define gclock_h

#include <stdio.h>

#define GClockUnixTime 0

typedef struct GClock GClock;

struct GClock {
    int64_t pts;
    int64_t ts;
    int64_t delta;
    int64_t timebase;
};

int64_t g_clock_now(void);

GClock g_clock_make(void);

void g_clock_tick(GClock *c, int64_t pts, int64_t ts);

double g_clock_get_current(GClock c);

int64_t g_clock_diff(GClock lc, GClock rc);

#endif /* gclock_h */
