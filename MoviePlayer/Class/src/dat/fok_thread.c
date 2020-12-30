//
//  fok_thread.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/18.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_thread.h"

static void *fok_thread_entry(void *ctx)
{
    FokThread *t = ctx;    
    if (strlen(t->name) > 0) {
        pthread_setname_np(t->name);
    }
    if (t->runable) {
        t->runable(t, t->data);
    }
    return NULL;
}

void fok_thread_free(FokThread *thread)
{
    thread->runable = NULL;
    thread->data = NULL;
    pthread_mutex_destroy(&thread->mutex);
    pthread_cond_destroy(&thread->cond);
    fok_free(thread);
}

FokThread *fok_thread_alloc(FokThreadRunable entry, void *info, const char *name)
{
    FokThread *t = fok_malloc(sizeof(FokThread));
    t->runable = entry;
    t->data = info;
    strcpy(t->name, name);
    int ret = pthread_create(&t->tid, NULL, fok_thread_entry, t);
    if (ret) {
        goto fail;
    }
    ret = pthread_mutex_init(&t->mutex, NULL);
    if (ret) {
        goto fail;
    }
    ret = pthread_cond_init(&t->cond, NULL);
    if (ret) {
        goto fail;
    }
    return t;
fail:
    fok_thread_free(t);
    return NULL;
}

int fok_thread_wait_timeout(FokThread *thread, long ms)
{
    if (!thread)
        return -1;
    
    thread->waiting = 1;
    pthread_mutex_lock(&thread->mutex);
    
    int retval;
    struct timeval delta;
    struct timespec abstime;
    gettimeofday(&delta, NULL);

    abstime.tv_sec = delta.tv_sec + (ms / 1000);
    abstime.tv_nsec = (delta.tv_usec + (ms % 1000) * 1000) * 1000;
    //改变之后的值如果大于1s，则进1s，-1ns
    if (abstime.tv_nsec > 1000000000) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }

    while (1) {
        retval = pthread_cond_timedwait(&thread->cond, &thread->mutex, &abstime);
        if (retval == 0)
            return 0;
        else if (retval == EINTR)
            continue;
        else if (retval == ETIMEDOUT)
            return -2;
        else
            break;
    }
    
    pthread_mutex_unlock(&thread->mutex);
    return retval;
}


void fok_thread_wait(FokThread *thread)
{
    if (!thread)
        return;
    thread->waiting = 1;
    pthread_mutex_lock(&thread->mutex);
    pthread_cond_wait(&thread->cond, &thread->mutex);
    pthread_mutex_unlock(&thread->mutex);
}

void fok_thread_singal(FokThread *thread)
{
    if (!thread)
        return;
    thread->waiting = 0;
    pthread_cond_signal(&thread->cond);
}

void fok_thread_join(FokThread *thread)
{
    if (!thread)
        return;
    pthread_join(thread->tid, NULL);
}

void fok_thread_detach(FokThread *thread)
{
    if (!thread)
        return;
    pthread_detach(thread->tid);
}

void fok_thread_destory(FokThread *thread)
{
    if (!thread)
        return;
    thread->abort = 1;
    pthread_cond_broadcast(&thread->cond);
    fok_log("thread:%s pthread_cond_broadcast", thread->name);
}
