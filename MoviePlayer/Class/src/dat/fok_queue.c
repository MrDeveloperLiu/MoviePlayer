//
//  fok_queue.c
//  MoviePlayer
//
//  Created by zuer on 2020/12/2.
//  Copyright © 2020 liu. All rights reserved.
//

#include "fok_queue.h"

FokQueueTask *fok_queue_task_alloc(void);

FokQueue *fok_queue_alloc(enum FokQueueType type)
{
    FokQueue *q = fok_malloc(sizeof(FokQueue));
    q->type = type;
    return q;
}

void fok_queue_free(FokQueue *queue)
{
    if (!queue)
        return;
    fok_free(queue);
}

void fok_queue_enqueue(FokQueue *queue, void *data, size_t size)
{
    fok_queue_enqueue_func(queue, data, size, NULL, NULL);
}

void fok_queue_enqueue_func(FokQueue *queue, void *data, size_t size, FokTaskCopyFunc copy, FokTaskFreeFunc free)
{
    if (!queue || !data || queue->abort == 1) {
        return;
    }
    FokQueueTask *task = fok_queue_task_alloc();
    task->size = size;
    task->task_idx = queue->count;
    task->copy = copy;
    task->free = free;
    if (task->copy) {
        task->data = task->copy(data);
    }else{
        task->data = data;
    }
    
    if (!queue->head)//none
    {
        queue->head = task;
        queue->trail = task;
    }
    else//one or more
    {
        queue->trail->next = task;
        task->prev = queue->trail;
        queue->trail = task;
    }
    queue->count ++;
    queue->size += task->size;
}

FokQueueTask *fok_queue_task_dequeue(FokQueue *queue)
{
    if (!queue || queue->count == 0 || queue->abort == 1) {
        return NULL;
    }
    FokQueueTask *task;
    if (queue->head == queue->trail)//one
    {
        task = queue->head;
        queue->head = queue->trail = NULL;
    }
    else//more
    {
        if (queue->type == FokQueueFIFO)
        {
            task = queue->head;
            queue->head = task->next;
        }
        else
        {
            task = queue->trail;
            queue->trail = task->prev;
        }
    }
    queue->count --;
    queue->size -= task->size;
    queue->cur = task;
    return task;
}

FokQueueTask *fok_queue_task_alloc(void)
{
    FokQueueTask *t = fok_malloc(sizeof(FokQueueTask));
    return t;
}

void fok_queue_task_free(FokQueue *queue, FokQueueTask *task)
{
    if (!task)
        return;
    if (task->free) {
        task->free(task->data);
    }
    fok_free(task);
}

void fok_queue_task_next(FokQueue *queue)
{
    if (!queue || queue->abort == 1)
        return;
    
    fok_queue_task_free(queue, queue->cur);
    queue->cur = NULL;
}

void fok_queue_abort(FokQueue *queue)
{
    if (!queue)
        return;
    queue->abort = 1;
}

void fok_queue_free_all_task(FokQueue *queue)
{
    if (!queue)
        return;

    FokQueueTask *current = queue->head;
    FokQueueTask *temp;
    while (current)
    {
        temp = current;
        current = current->next;
        fok_queue_task_free(queue, temp);
    }
}
