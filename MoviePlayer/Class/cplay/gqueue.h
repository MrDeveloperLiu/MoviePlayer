//
//  gqueue.h
//  MoviePlayer
//
//  Created by zuer on 2021/2/24.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gqueue_h
#define gqueue_h

#include <stdio.h>
#include "gbase.h"
#include "gmutex.h"

typedef struct GQueueTask GQueueTask;
typedef struct GQueue GQueue;

typedef GPointer(*GQueueTaskRetain)(const GPointer);
typedef void(*GQueueTaskRelease)(GPointer);

struct GQueueTask {
    long index;
    GPointer data;
    GQueueTaskRetain retain;
    GQueueTaskRelease release;
    GQueueTask *next;
};

//
void g_queue_task_ref(GQueueTask *t, GQueueTaskRetain retain, GQueueTaskRelease release, GPointer data);

GQueueTask *g_queue_task_alloc(void);

void g_queue_task_release(GQueueTask *t);



struct GQueue {
    int cap;
    int size;
    long serial;
    GQueueTask *head;
    GQueueTask *trail;
    GMutex *lock;
};

GQueue *g_queue_alloc(int cap);

void g_queue_release(GQueue *q);

void g_queue_reset(GQueue *q);

int g_queue_full(GQueue *q);

//return -2 means: queue has full
int g_queue_enqueue(GQueue *q, GPointer d);

int g_queue_enqueue_task(GQueue *q, GQueueTask *task);

GQueueTask *g_queue_dequeue(GQueue *q);

#endif /* gqueue_h */
