#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__

#include "pthread.h"
#include "string.h"
#include "stdio.h"

typedef void (*callback)(void*);

int thread_create(pthread_t *thread, void*(*start_rountine)(void*), void *arg);
void thread_stop(pthread_t *pthread);
#endif


