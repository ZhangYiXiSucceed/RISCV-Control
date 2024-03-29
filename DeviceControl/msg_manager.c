#include "msg_manager.h"

int msg_queue_create(msg_queue_t* msg_queue, void* mem_buf, u32 mem_size, u32 one_data_size)
{
    msg_queue = (msg_queue_t*)mem_buf;
    if(mem_size < sizeof(msg_queue_t))
    {
        return -1;
    }

    u32 msg_queue_num = mem_size/one_data_size;
    if(msg_queue_num > MSG_QUEUE_MAX_NUM)
    {
        msg_queue_num = MSG_QUEUE_MAX_NUM;
    }
    msg_queue->msq_queue_num = msg_queue_num;
    msg_queue->msg_queu_one_size = one_data_size;
    
    msg_queue->data_list.header_item.header_cnt = 0;
    msg_queue->data_list.header_item.tail_cnt = 0;
    int i;
    for(i=0;i<msg_queue_num;i++)
    {
        msg_queue->data_list.data_item[i].data_buf = NULL;
    }

    msg_queue->free_list.header_item.header_cnt = 0;
    msg_queue->free_list.header_item.tail_cnt = msg_queue_num;
    int i;

    u8* buf_addr = (u8*)mem_buf + sizeof(msg_queue_t);
    for(i=0;i<msg_queue_num;i++)
    {
        msg_queue->free_list.data_item[i].data_buf = (void*)buf_addr;
        buf_addr += one_data_size;
    }


    return 0;
}




void* msg_queue_get_free(msg_queue_t* msg_queue)
{
    u8* buf_addr = NULL;

    if(msg_queue->free_list.header_item.header_cnt <  msg_queue->free_list.header_item.tail_cnt)
    {   
        buf_addr =  msg_queue->free_list.data_item[msg_queue->free_list.header_item.header_cnt%msg_queue->msq_queue_num].data_buf;
        msg_queue->free_list.header_item.header_cnt ++;
    }
    return buf_addr;
}



void msg_queue_put_free(msg_queue_t* msg_queue)
{

}


void* msg_queue_get_data(msg_queue_t* msg_queue)
{

}


void* msg_queu_put_data(msg_queue_t* msg_queue)
{

}