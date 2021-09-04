//
//  gmsgqueue.h
//  MoviePlayer
//
//  Created by zuer on 2021/3/2.
//  Copyright © 2021 liu. All rights reserved.
//

#ifndef gmsgqueue_h
#define gmsgqueue_h

#include <stdio.h>
#include "gthread.h"
#include "gmutex.h"
#include "gqueue.h"

typedef struct GMessage GMessage;
typedef struct GMessageQueue GMessageQueue;

struct GMessage {
    int msgtype;
    void *argv;
};

struct GMessageQueue {
    GThread *thread;
    GMutex *mutex;
    GCond *cond;
    GQueue *q;
    int runing;
    int waiting;
    int abort;
    GPointer ctx;
    void (*on_message)(GMessageQueue *q, GMessage *msg);
    void (*on_resume)(GMessageQueue *q);
    void (*on_suspend)(GMessageQueue *q);
};

GMessage *g_msg_alloc(void);

void g_msg_release(GMessage *m);

GMessageQueue *g_msg_queue_alloc(const char *name, GPointer ctx);

void g_msg_queue_release(GMessageQueue *q);

void g_msg_queue_push(GMessageQueue *q, GMessage *msg);



#endif /* gmsgqueue_h */
