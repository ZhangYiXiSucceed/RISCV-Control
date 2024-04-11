#ifndef __MSG_HANDLE_H__
#define __MSG_HANDLE_H__

#include "basic_type.h"
#include "link.h"

#define APP_DEVICE_ADDR  0
#define MSG_FRAME_HEADER 0x676A737A
#define OTA_ONE_PACKAGE_SIZE   1024

#define rt_kprintf printf

typedef enum cmd_process_errcode_enum
{
	MSG_OK,
	MSG_HEADER_ERR,
	MSG_CMD_ERR,
	MSG_LEN_ERR,
	MSG_CRC_ERR,
	MSG_ADDR_ERR,
	UPDATE_HEADER_ERR,
	
}cmd_process_errcode_e;

enum 
{
	IDLE_CMD,
	HEART_CMD,
	PICTURE_CMD,
	START_UPDATE,
	UPDATE_DATA,
	UPDATE_END,
	JUMP_CMD,
	CONNECT_CMD,
	RESET_CMD,
	VERSION_CMD,
	CONNECT_MODE_CMD,
	OLED_SHOW_MODE_CMD,
	SET_TIME_DATE,

	GET_USB_INFO_CMD = 0x20,
	SET_PICTURE_CMD,
};

enum{
	USB_GET_INFO_CMD = 0x1,
	USB_SPEED_CMD,
	USB_SHOW_CMD,
};
typedef struct cmd_msg_frame_struct
{
    u32 header;
	u16 device_addr;
	u16  cmd;
	u16  seq;
	u16 data_len;
}cmd_msg_frame_t;

typedef struct usb_info_struct
{
	int usb_speed;
	int block_num;
	int block_size;
	int rsv;
}usb_info_t;

typedef struct usb_handle_struct{
	usb_device_handle_t *device;
	usb_info_t info;
	u8 cmd;
}usb_handle_t;

void* usb_msg_process(void* attr);
void* net_msg_process(void* attr);


#endif