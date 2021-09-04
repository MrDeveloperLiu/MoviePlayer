//
//  gthreadlocal.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/25.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gthreadlocal.h"

GThreadLocal *g_thread_local_alloc(const char *name, GThreadFn fn, GPointer data)
{
    GThreadLocal *t = (GThreadLocal *)gmalloc(sizeof(GThreadLocal));
    if (t) {
        t->thread = g_thread_alloc(name, fn, data);
        t->cond = g_cond_alloc();
        t->mutex = g_mutex_alloc();
    }
    return t;
}

void g_thread_local_release(GThreadLocal **t)
{
    if (*t) {
        g_thread_release((*t)->thread);
        g_cond_release((*t)->cond);
        g_mutex_release((*t)->mutex);
    }
    gfreep((void **)t);
}

int g_thread_local_wait(GThreadLocal *t)
{
    if (!t)
        return -1;
    g_mutex_lock(t->mutex);
    t->waiting = 1;
    int ret = g_cond_wait(t->cond, t->mutex);
    g_mutex_unlock(t->mutex);
    return ret;
}

int g_thread_local_wait_timeout(GThreadLocal *t, long ms)
{
    if (!t)
        return -1;
    g_mutex_lock(t->mutex);
    t->waiting = 1;
    int ret = g_cond_wait_timeout(t->cond, t->mutex, ms);
    g_mutex_unlock(t->mutex);
    return ret;
}

int g_thread_local_signal(GThreadLocal *t)
{
    if (!t)
        return -1;

    t->waiting = 0;
    return g_cond_signal(t->cond);
}

int g_thread_local_exit(GThreadLocal *t)
{
    if (!t)
        return -1;
    
    t->exit = 1;
    return g_cond_broadcast(t->cond);
}

int g_thread_local_join(GThreadLocal *t)
{
    if (!t)
        return -1;
    
    return g_thread_join(t->thread);
}
