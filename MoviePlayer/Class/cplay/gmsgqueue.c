//
//  gmsgqueue.c
//  MoviePlayer
//
//  Created by zuer on 2021/3/2.
//  Copyright © 2021 liu. All rights reserved.
//

#include "gmsgqueue.h"

void g_msg_queue_thread_processing(GThread *t, GPointer data)
{
    GMessageQueue *q = data;
    
    while (1)
    {
        if (q->abort) {
            goto exit; break;
        }
            
        g_mutex_lock(q->mutex);
        GQueueTask *t = g_queue_dequeue(q->q);
        if (t) {
            GFnCall(q->on_message, q, t->data);
            g_queue_task_release(t);
        }else{
            GFnCall(q->on_suspend, q);
            q->waiting = 1;
            g_cond_wait(q->cond, q->mutex);
        }
        g_mutex_unlock(q->mutex);
    }
    
exit:
    q->runing = 0;
    gprintf("msg queue:%s exit!", q->thread->name);
    g_cond_release(q->cond);
    g_mutex_release(q->mutex);
    g_thread_release(q->thread);
}

GMessage *g_msg_alloc(void)
{
    GMessage *m = (GMessage *)gmalloc(sizeof(GMessage));
    return m;
}

void g_msg_release(GMessage *m)
{
    gfree(m);
}

GMessageQueue *g_msg_queue_alloc(const char *name, GPointer ctx)
{
    GMessageQueue *q = (GMessageQueue *)gmalloc(sizeof(GMessageQueue));
    if (q) {
        q->cond = g_cond_alloc();
        q->mutex = g_mutex_alloc();
        q->thread = g_thread_alloc(name, g_msg_queue_thread_processing, q);
        q->q = g_queue_alloc(0);
        q->ctx = ctx;
        q->runing = 1;
    }
    return q;
}

void g_msg_queue_release(GMessageQueue *q)
{
    if (q) {
        q->abort = 1;
        g_cond_broadcast(q->cond);
        g_thread_join(q->thread);
        g_queue_reset(q->q);
    }
    gfree(q);
}

void g_msg_queue_push(GMessageQueue *q, GMessage *msg)
{
    if (!q)
        return;
    
    g_mutex_lock(q->mutex);
    GQueueTask *t = g_queue_task_alloc();
    g_queue_task_ref(t, NULL, (GQueueTaskRelease)g_msg_release, msg);
    g_queue_enqueue_task(q->q, t);
    g_mutex_unlock(q->mutex);
    
    GFnCall(q->on_resume, q);
    q->waiting = 0;
    g_cond_signal(q->cond);
}
