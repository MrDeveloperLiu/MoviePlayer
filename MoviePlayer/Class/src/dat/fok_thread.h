//
//  fok_thread.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/18.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_thread_h
#define fok_thread_h

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

typedef void (*FokThreadRunable)(FokThread *thread, void *info);

struct FokThread {
    pthread_t tid;
    pthread_mutex_t mutex;
    pthread_cond_t cond;    
    
    void *data;
    char name[256];
    
    int lock;
    int waiting;
    int abort;
    
    FokThreadRunable runable;
};

FokThread *fok_thread_alloc(FokThreadRunable entry, void *info, const char *name);

void fok_thread_free(FokThread *thread);

int fok_thread_wait_timeout(FokThread *thread, long ms);

void fok_thread_wait(FokThread *thread);

void fok_thread_singal(FokThread *thread);

void fok_thread_join(FokThread *thread);

void fok_thread_detach(FokThread *thread);

void fok_thread_destory(FokThread *thread);

#endif /* fok_thread_h */
