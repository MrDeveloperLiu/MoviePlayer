//
//  gthread.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/23.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gthread.h"

static void * g_thread_entry(void *t)
{
    GThread *thread = t;
    pthread_setname_np(thread->name);
    if (thread->fn) {
        thread->fn(thread, thread->data);
    }
    return NULL;
}

GThread *g_thread_alloc(const char *name, GThreadFn fn, GPointer data)
{
    GThread *t = (GThread *)gmalloc(sizeof(GThread));
    if (t) {
        strlcpy(t->name, name, sizeof(t->name) - 1);
        t->fn = fn;
        t->data = data;
        pthread_create(&t->tid, NULL, g_thread_entry, t);
    }
    return t;
}

void g_thread_release(GThread *t)
{
    if (t) {
        pthread_exit(&t->tid);
    }
    gfree(t);
}

int g_thread_join(GThread *t)
{
    if (!t)
        return -1;
    
    return pthread_join(t->tid, NULL);
}

int g_thread_detach(GThread *t)
{
    if (!t)
        return -1;
    
    return pthread_detach(t->tid);
}

int g_thread_set_policy(GThread *t, GThreadPolicy policy)
{
    if (!t)
        return -1;
    struct sched_param param;
    int sched;
    if (pthread_getschedparam(t->tid, &sched, &param) != 0) {
        return -1;
    }
    if (policy == GThreadPolicyLow) {
        param.sched_priority = sched_get_priority_min(sched);
    }else if (policy == GThreadPolicyHigh) {
        param.sched_priority = sched_get_priority_max(sched);
    }else{
        int max = sched_get_priority_max(sched);
        int min = sched_get_priority_min(sched);
        param.sched_priority = min + ((max - min) / 2);
    }
    return pthread_setschedparam(t->tid, sched, &param);
}
