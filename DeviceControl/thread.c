#include "thread.h"
 
int thread_create(pthread_t *thread, callback func, void *arg)
{
	memset(thread,0x00,sizeof(pthread_t));
	int TempReturnValue;

	res = pthread_create(thread,NULL,func,arg);  
	if(TempReturnValue!=0)
	{
		printf("thread created failed!\n");
	}
	else
	{
		int id = pthread_self();
		printf("thread id = %d\n",id);
	}
	return id;
}
void thread_stop(pthread_t *pthread)
{
  	if(*pthread!=0)
  	{
  		pthread_join(*pthread, NULL);
  		printf("thread is over!\n");
  	}
	else
	{
		printf("thread is null!\n");
		exit(-1);
	}
}


