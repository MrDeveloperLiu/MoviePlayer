//
//  gmutex.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gmutex.h"

GMutex * g_mutex_alloc(void)
{
    GMutex *m = (GMutex *)gmalloc(sizeof(GMutex));
    if (m) {
        pthread_mutex_init(&m->tid, NULL);
    }
    return m;
}

void g_mutex_release(GMutex *m)
{
    if (m) {
        pthread_mutex_destroy(&m->tid);
    }
    gfree(m);
}

int g_mutex_lock(GMutex *m)
{
    if (!m )
        return -1;
    
    return pthread_mutex_lock(&m->tid);
}

int g_mutex_unlock(GMutex *m)
{
    if (!m )
        return -1;
    
    return pthread_mutex_unlock(&m->tid);
}



GCond *g_cond_alloc(void)
{
    GCond *c = (GCond *)gmalloc(sizeof(GCond));
    if (c) {
        pthread_cond_init(&c->tid, NULL);
    }
    return c;
}

void g_cond_release(GCond *c)
{
    if (c) {
        pthread_cond_destroy(&c->tid);
    }
    gfree(c);
}

int g_cond_wait(GCond *c, GMutex *mutex)
{
    if (!c || !mutex)
        return -1;
    
    return pthread_cond_wait(&c->tid, &mutex->tid);
}

int g_cond_wait_timeout(GCond *c, GMutex *mutex, long ms)
{
    if (!c || !mutex)
        return -1;
    
    int retval;
    struct timeval delta;
    struct timespec abstime;
    gettimeofday(&delta, NULL);

    abstime.tv_sec = delta.tv_sec + (ms / 1000);
    abstime.tv_nsec = (delta.tv_usec + (ms % 1000) * 1000) * 1000;
    
    if (abstime.tv_nsec > 1000000000) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }

    while (1) {
        retval = pthread_cond_timedwait(&c->tid, &mutex->tid, &abstime);
        if (retval == 0)
            return 0;
        else if (retval == EINTR)
            continue;
        else if (retval == ETIMEDOUT)
            return -2;
        else
            break;
    }
    return -1;
}

int g_cond_signal(GCond *c)
{
    if (!c)
        return -1;

    return pthread_cond_signal(&c->tid);
}

int g_cond_broadcast(GCond *c)
{
    if (!c)
        return -1;

    return pthread_cond_broadcast(&c->tid);
}


