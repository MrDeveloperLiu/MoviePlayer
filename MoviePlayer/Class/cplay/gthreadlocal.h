//
//  gthreadlocal.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gthreadlocal_h
#define gthreadlocal_h

#include <stdio.h>
#include "gthread.h"
#include "gmutex.h"

typedef struct GThreadLocal GThreadLocal;

struct GThreadLocal {
    GThread *thread;
    GMutex *mutex;
    GCond *cond;
    int waiting;
    int exit;
};

GThreadLocal *g_thread_local_alloc(const char *name, GThreadFn fn, GPointer data);

void g_thread_local_release(GThreadLocal **t);

int g_thread_local_wait(GThreadLocal *t);

int g_thread_local_wait_timeout(GThreadLocal *t, long ms);

int g_thread_local_signal(GThreadLocal *t);

int g_thread_local_exit(GThreadLocal *t);

int g_thread_local_join(GThreadLocal *t);

#endif /* gthreadlocal_h */
