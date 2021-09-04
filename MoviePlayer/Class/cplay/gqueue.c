//
//  gqueue.c
//  MoviePlayer
//
//  Created by zuer on 2021/2/24.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gqueue.h"


void g_queue_task_ref(GQueueTask *t, GQueueTaskRetain retain, GQueueTaskRelease release, GPointer data)
{
    if (!t)
        return;
    
    t->retain = retain;
    t->release = release;
    
    if (t->retain)
        t->data = t->retain(data);
    else
        t->data = data;
}


GQueueTask *g_queue_task_alloc(void)
{
    GQueueTask *t = (GQueueTask *)gmalloc(sizeof(GQueueTask));
    return t;
}

void g_queue_task_release(GQueueTask *t)
{
    if (t) {
        if (t->release)
            t->release(t->data);
        t->retain = NULL;
        t->release = NULL;
    }
    gfree(t);
}


GQueue *g_queue_alloc(int cap)
{
    GQueue *q = (GQueue *)gmalloc(sizeof(GQueue));
    if (q) {
        q->lock = g_mutex_alloc();
        if (q->cap <= 0) {
            q->cap = INT16_MAX;
        }else{
            q->cap = cap;
        }
    }
    return q;
}

void g_queue_release(GQueue *q)
{
    if (q) {
        g_mutex_release(q->lock);
        g_queue_reset(q);
    }
    gfree(q);
}

int g_queue_full(GQueue *q)
{
    if (q->cap > 0 && q->size >= q->cap)
        return 1;
    return 0;
}

void g_queue_reset(GQueue *q)
{
    if (!q)
        return;
    
    GQueueTask *t = q->head;
    q->head = q->trail = NULL;
    
    while (t) {
        g_queue_task_release(t);
        t = t->next;
    }
}

int g_queue_enqueue(GQueue *q, GPointer d)
{
    if (!q)
        return -1;
    if (g_queue_full(q))
        return -2;
    
    GQueueTask *task = g_queue_task_alloc();
    g_queue_task_ref(task, NULL, NULL, d);
    return g_queue_enqueue_task(q, task);
}

int g_queue_enqueue_task(GQueue *q, GQueueTask *task)
{
    if (!q)
        return -1;
    if (g_queue_full(q))
        return -2;
    
    g_mutex_lock(q->lock);
    task->index = q->serial;
    
    if (!q->head)
    {
        q->head = task;
    }
    else if (!q->trail)
    {
        q->head->next = task;
        q->trail = task;
    }
    else
    {
        q->trail->next = task;
        q->trail = task;
    }        
    q->size++;
    if (q->serial >= INT32_MAX) {
        q->serial = 0;
    }else{
        q->serial++;
    }
    g_mutex_unlock(q->lock);
    return 0;
}

GQueueTask *g_queue_dequeue(GQueue *q)
{
    if (!q)
        return NULL;

    if (!q->head) {
        return NULL;
    }
    
    g_mutex_lock(q->lock);
    GQueueTask *t = q->head;
    if (!q->trail) {
        q->head = NULL;
    }else if (q->head->next == q->trail) {
        q->head = q->trail;
        q->trail = NULL;
    }else{
        q->head = t->next;
    }
    q->size--;
    g_mutex_unlock(q->lock);
    return t;
}
