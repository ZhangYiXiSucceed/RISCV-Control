#ifndef _MSG_MANAGER_H__
#define _MSG_MANAGER_H__
#include "basic_type.h"
#define MSG_QUEUE_MAX_NUM   32

typedef struct msg_queue_header_item_struct
{
   
    u16 header_cnt;
    u16 tail_cnt;
}msg_queue_header_item_t;

typedef struct msg_queue_data_item_struct
{
    
    void* data_buf;
}msg_queue_data_item_t;

typedef struct msg_queue_manager_list_struct
{
    msg_queue_header_item_t header_item;
    msg_queue_data_item_t data_item[MSG_QUEUE_MAX_NUM];
}msg_queue_manager_list_t;

typedef struct msg_queue_struct
{
    msg_queue_manager_list_t data_list;
    msg_queue_manager_list_t free_list;
    u32 msq_queue_num;
    u32 msg_queue_one_size;
}msg_queue_t;



#endif

