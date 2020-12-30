//
//  fok_queue.h
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#ifndef fok_queue_h
#define fok_queue_h

#include <stdio.h>

typedef struct FokQueueTask FokQueueTask;
typedef struct FokQueue FokQueue;

typedef void *(*FokTaskCopyFunc)(void *i);
typedef void (*FokTaskFreeFunc)(void *i);

enum FokQueueType{
    FokQueueFIFO,
    FokQueueLIFO
};

struct FokQueueTask {
    void            *data;
    size_t           size;
    int              task_idx;
    FokQueueTask     *prev;
    FokQueueTask     *next;
    FokTaskCopyFunc  copy;
    FokTaskFreeFunc  free;
};

struct FokQueue {
    enum FokQueueType type;
    int               count;
    size_t            size;
    FokQueueTask     *head;
    FokQueueTask     *trail;
    FokQueueTask     *cur;
    int              abort;
};


FokQueue *fok_queue_alloc(enum FokQueueType type);

void fok_queue_free(FokQueue *queue);

void fok_queue_enqueue(FokQueue *queue, void *data, size_t size);

void fok_queue_enqueue_func(FokQueue *queue, void *data, size_t size, FokTaskCopyFunc copy, FokTaskFreeFunc free);

void fok_queue_free_all_task(FokQueue *queue);

FokQueueTask *fok_queue_task_dequeue(FokQueue *queue);

void fok_queue_abort(FokQueue *queue);

void fok_queue_task_next(FokQueue *queue);


#endif /* fok_queue_h */
