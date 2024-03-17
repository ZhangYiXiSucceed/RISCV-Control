#include "thread.h"
#include <stdlib.h>
 
int thread_create(thread_handle_t *thread, thread_attr_t* attr)
{	
	thread->attr = *attr;

	memset((char*)&thread->pthread,0x00,sizeof(thread->pthread));
	int res = pthread_create(&thread->pthread, NULL, thread->attr.func, thread->attr.arg);  
	if(res!=0)
	{
		printf("thread created failed!\n");
	}
	else
	{
		int id = pthread_self();
		thread->thread_id = id;
		printf("thread id = %d\n",id);
	}

	return res;
}
void thread_stop(thread_handle_t *thread)
{
  	if(thread->pthread!=0)
  	{
  		pthread_join(thread->pthread, NULL);
  		printf("thread is over!\n");
  	}
	else
	{
		printf("thread is null!\n");
		exit(-1);
	}
}


