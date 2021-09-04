//
//  gthread.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gthread_h
#define gthread_h

#include <stdio.h>
#include "gmutex.h"

typedef struct GThread GThread;

enum GThreadPolicy {
    GThreadPolicyNormal,
    GThreadPolicyLow,
    GThreadPolicyHigh,
};
typedef enum GThreadPolicy GThreadPolicy;


typedef void(*GThreadFn)(GThread *t, GPointer data);

GThread *g_thread_alloc(const char *name, GThreadFn fn, GPointer data);

void g_thread_release(GThread *t);

int g_thread_join(GThread *t);

int g_thread_detach(GThread *t);

int g_thread_set_policy(GThread *t, GThreadPolicy policy);

struct GThread {
    pthread_t tid;
    char name[256];
    GThreadFn fn;
    GPointer data;
};



#endif /* gthread_h */
