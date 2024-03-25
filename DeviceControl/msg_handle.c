#include <unistd.h>
#include <stdio.h>
#include "msg_handle.h"
#include "thread.h"
#include "net.h"

int usb_speed_test(usb_device_handle_t *usb_device);
void test_get_capacity(usb_device_handle_t *usb_device);
void test_get_device_description(usb_device_handle_t *usb_device);

void*  usb_msg_process(void* attr)
{
    thread_handle_t *usb_thread = (thread_handle_t *)attr;
    usb_device_handle_t *usb_device = (usb_device_handle_t*)usb_thread->attr.internal_data;
    while(1)
    {
        usb_speed_test(usb_device);
        test_get_capacity(usb_device);
    }
}

void usb_cmd_handle()
{

}

int usb_speed_test(usb_device_handle_t *usb_device)
{
    u8 data[512]= {0};
    for(int i=0;i<512;i++)
    {
      data[i]=i;
    }
    struct timeval start, end;
    long mtime, seconds, useconds;
    int times = 1000;
    gettimeofday(&start, NULL); // 获取开始时间
    int count = 0;
    while(count<times)
    {
        int res = usb_tx_data(usb_device, 0xF1, data,512);
        //printf("res=%d\r\n",res);
        while(1)
        {
            res = usb_has_data(usb_device, 0xF1);
            //printf("has data res=%d\r\n",res);
            if(res)
            { 
                break;
            }
        }
        if(res)
        {
            //memset(data,0x00,sizeof(data));
            res = usb_rx_data(usb_device, 0xF1, data, 512);

            // printf("rx data res=%d\r\n",res);
            // for(int i=0;i<512;i++)
            // {
            //     if(0 == (i%16))
            //     {
            //       printf("\r\n");
            //     }
            //     printf("%04d ",data[i]);
            // }
            // printf("\r\n");
        }
        count++;
    }
    gettimeofday(&end, NULL);   // 获取结束时间
    // 计算代码执行时间
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

    int usb_speed = times*512/mtime*1000
    printf("%ld ms,speed=%d B/S\n", mtime, usb_speed);
    return usb_speed;
}

void test_get_capacity(usb_device_handle_t *usb_device)
{
    u8 data[512]= {0};

    usb_rx_data(usb_device, 0x25, data, 8);
    int total_blcok =  data[0]<<24 | data[1]<<16 |data[2]<<8 |data[3];
    int block_size =   data[4]<<24 | data[5]<<16 |data[6]<<8 |data[7];

    printf("total_blcok=%d  block_size=%d\r\n", total_blcok + 1, block_size);
}

void test_get_device_description(usb_device_handle_t *usb_device)
{
    u8 data[512]= {0};
    u8 cdb[16] ={0x12, 0x00,0x00,0x00,0x00,0x00};

    usb_rx_data_low_level(usb_device, cdb, data, 36);
    printf("Vendor=%s Product=%s Version=%s\r\n", &data[8], &data[16], &data[32]);
}

void* net_msg_process(void* attr)
{
    thread_handle_t *net_thread = (thread_handle_t *)attr;
    net_device_handle_t *net_device = (net_device_handle_t*)net_thread->attr.internal_data;
    while(1)
    {
        net_send(net_device, "hello world, net msg\r\n", 20);
        usleep(1000000);
    }
}

void net_cmd_handle(u8* packet, u32 len)
{
    cmd_msg_frame_t *cmd_msg_frame = (cmd_msg_frame_t *)packet;
	cmd_process_errcode_e res = MSG_OK;
	if(APP_DEVICE_ADDR  != cmd_msg_frame->header)
	{
		rt_kprintf("frame header err,0x%x\r\n", cmd_msg_frame->header);
		res = MSG_HEADER_ERR;
		goto err
	}
	if(MSG_ADDR_ERR != cmd_msg_frame->device_addr)
	{
		rt_kprintf("device addr err,0x%x\r\n", cmd_msg_frame->device_addr);
		res = MSG_ADDR_ERR;
		goto err
	}
	switch(cmd_msg_frame->cmd)
	{
		case GET_USB_INFO_CMD:
        {

        }break;
        default:
        break;
    }
    	if((HEART_CMD != cmd_msg_frame->cmd) && (VERSION_CMD != cmd_msg_frame->cmd) && (PICTURE_CMD != cmd_msg_frame->cmd))
	{
		msg_rsp_packet_and_send(cmd_msg_frame->cmd,res);
	}
	
	return MSG_OK;
err:
	msg_rsp_packet_and_send(cmd_msg_frame->cmd,res);
	return res;
}