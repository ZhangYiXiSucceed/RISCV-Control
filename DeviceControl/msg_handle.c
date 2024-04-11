#include <unistd.h>
#include <stdio.h>
#include "msg_handle.h"
#include "thread.h"
#include "net.h"

int usb_speed_test(usb_device_handle_t *usb_device);
void test_get_capacity(usb_device_handle_t *usb_device, int *total_blcok, int *block_size);
void test_get_device_description(usb_device_handle_t *usb_device);
void control_lcd_show_cmd_process();
usb_info_t usb_device_info_g;
usb_handle_t  usb_handle_g;
void*  usb_msg_process(void* attr)
{
    thread_handle_t *usb_thread = (thread_handle_t *)attr;
    usb_handle_g.device = (usb_device_handle_t*)usb_thread->attr.internal_data;
    while(1)
    {
        usb_cmd_handle();
    }
}

void usb_cmd_handle()
{
    switch(usb_handle_g.cmd)
    {
        case USB_GET_INFO_CMD:
        {
            test_get_capacity(usb_handle_g.device, &usb_handle_g.info.block_num, &usb_handle_g.info.block_size);
        }break;
         case USB_SPEED_CMD:
        {
            usb_handle_g.info.usb_speed = usb_speed_test(usb_handle_g.device);
        }break;
         case USB_SHOW_CMD:
        {
            control_lcd_show_cmd_process();
        }break;
        default:
    }
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
        }
        count++;
    }
    gettimeofday(&end, NULL);   // 获取结束时间
    // 计算代码执行时间
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

    int usb_speed = times*512/mtime*1000;
    printf("%ld ms,speed=%d B/S\n", mtime, usb_speed);
    return usb_speed;
}

void test_get_capacity(usb_device_handle_t *usb_device, int *total_blcok, int *block_size)
{
    u8 data[512]= {0};

    usb_rx_data(usb_device, 0x25, data, 8);
    *total_blcok =  data[0]<<24 | data[1]<<16 |data[2]<<8 |data[3];
    *block_size =   data[4]<<24 | data[5]<<16 |data[6]<<8 |data[7];

    printf("total_blcok=%d  block_size=%d\r\n", *total_blcok + 1, *block_size);
}

void test_get_device_description(usb_device_handle_t *usb_device)
{
    u8 data[512]= {0};
    u8 cdb[16] ={0x12, 0x00,0x00,0x00,0x00,0x00};

    usb_rx_data_low_level(usb_device, cdb, data, 36);
    printf("Vendor=%s Product=%s Version=%s\r\n", &data[8], &data[16], &data[32]);
}


void control_lcd_show_cmd_process()
{

}

u32 CalCheckSum(uint8_t* Data, uint32_t len)
{
    int CheckSum = 0;
    for(int i=0;i<len;i++)
    {
        CheckSum+=Data[i];
    }
    return CheckSum;
}

void msg_rsp_packet_and_send(net_device_handle_t *net_device, u16 cmd, u8 err_code)
{
	u8 cmd_data[sizeof(cmd_msg_frame_t) + 1 + 4];
	cmd_msg_frame_t* msg = (cmd_msg_frame_t*)cmd_data;

	msg->header = MSG_FRAME_HEADER;
	msg->cmd = cmd;
	msg->device_addr = 0x00;
	msg->seq = 0x00;
	msg->data_len = 0x01;

	u8* temp_err_code = (u8*)&cmd_data[sizeof(cmd_msg_frame_t)];
	*temp_err_code = err_code;

	u32* check_sum = (u32*)&cmd_data[sizeof(cmd_msg_frame_t) + 1 ];
	*check_sum = CalCheckSum(cmd_data,sizeof(cmd_msg_frame_t) + 1);

	net_send(net_device, cmd_data,sizeof(cmd_msg_frame_t) + 1 + 4);
}

int net_cmd_handle(net_device_handle_t *net_device, u8* packet, u32 len)
{
    cmd_msg_frame_t *cmd_msg_frame = (cmd_msg_frame_t *)packet;
	cmd_process_errcode_e res = MSG_OK;
	if( MSG_FRAME_HEADER != cmd_msg_frame->header)
	{
		rt_kprintf("frame header err,0x%x\r\n", cmd_msg_frame->header);
		res = MSG_HEADER_ERR;
		goto err;
	}
	if(APP_DEVICE_ADDR != cmd_msg_frame->device_addr)
	{
		rt_kprintf("device addr err,0x%x\r\n", cmd_msg_frame->device_addr);
		res = MSG_ADDR_ERR;
		goto err;
	}
    rt_kprintf("cmd=%x\r\n",cmd_msg_frame->cmd );
	switch(cmd_msg_frame->cmd)
	{
		case GET_USB_INFO_CMD:
        {
            u8 cmd_data[sizeof(cmd_msg_frame_t) + sizeof(usb_info_t) + 4];
			cmd_msg_frame_t* msg = (cmd_msg_frame_t*)cmd_data;

			msg->header = MSG_FRAME_HEADER;
			msg->cmd = GET_USB_INFO_CMD;
			msg->device_addr = APP_DEVICE_ADDR;
			msg->seq = 0x00;
			msg->data_len = sizeof(usb_info_t);

			u8* msg_data = (u8*)(msg + 1);
			usb_info_t *usb_info = (usb_info_t *)msg_data;
			usb_info->usb_speed = usb_device_info_g.usb_speed;
			usb_info->block_num = usb_device_info_g.block_num;
			usb_info->block_size = usb_device_info_g.block_size;
			usb_info->rsv = 0x0;

			u32* check_sum = (u32*)&cmd_data[sizeof(cmd_msg_frame_t) + sizeof(usb_info_t)];
			*check_sum = CalCheckSum(cmd_data,sizeof(cmd_msg_frame_t) + sizeof(usb_info_t));

			net_send(net_device, cmd_data,sizeof(cmd_msg_frame_t) + sizeof(usb_info_t) + 4);
            rt_kprintf("len=%d\r\n",sizeof(cmd_data));
        }break;
        case SET_PICTURE_CMD:
        {

        }break;
        default:
        break;
    }
	
	return MSG_OK;
err:
	msg_rsp_packet_and_send(net_device, cmd_msg_frame->cmd,res);
	return res;
}

void* net_msg_process(void* attr)
{
    thread_handle_t *net_thread = (thread_handle_t *)attr;
    net_device_handle_t *net_device = (net_device_handle_t*)net_thread->attr.internal_data;
    int net_recv_res;
    u8 buf[64];
    while(1)
    {
        int net_recv_len = net_rev(net_device, buf, 64);
        if(net_recv_len>=0)
        {
            net_cmd_handle(net_device, buf, net_recv_len);
        }
        printf("hello world, net msg=%d\r\n", net_recv_len);
    }
}