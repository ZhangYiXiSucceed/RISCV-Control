#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__

#include "pthread.h"
#include "string.h"
#include "stdio.h"
#include "link.h"

typedef void* (*callback)(void*);
typedef struct thread_attr_struct
{
    callback func;
    void* arg;
    void* internal_data;
}thread_attr_t;

typedef struct thread_handle_struct
{
    thread_attr_t attr;
    pthread_t pthread;
    int thread_id;
}thread_handle_t;



int thread_create(thread_handle_t *thread, thread_attr_t* attr);
void thread_stop(thread_handle_t *pthread);
#endif


