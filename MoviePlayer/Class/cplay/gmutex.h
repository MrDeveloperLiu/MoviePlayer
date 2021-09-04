//
//  gmutex.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gmutex_h
#define gmutex_h

#include <stdio.h>
#include "gbase.h"

typedef struct GMutex GMutex;

GMutex *g_mutex_alloc(void);

void g_mutex_release(GMutex *m);

int g_mutex_lock(GMutex *m);

int g_mutex_unlock(GMutex *m);


typedef struct GCond GCond;

GCond *g_cond_alloc(void);

void g_cond_release(GCond *c);

int g_cond_wait(GCond *c, GMutex *mutex);

int g_cond_wait_timeout(GCond *c, GMutex *mutex, long ms);

int g_cond_signal(GCond *c);

int g_cond_broadcast(GCond *c);

struct GMutex {
    pthread_mutex_t tid;
};

struct GCond {
    pthread_cond_t tid;
};

#endif /* gmutex_h */
