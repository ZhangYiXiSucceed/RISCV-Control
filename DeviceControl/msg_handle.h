#ifndef __MSG_HANDLE_H__
#define __MSG_HANDLE_H__

typedef struct cmd_msg_frame_struct
{
    u32 header;
	u16 device_addr;
	u16  cmd;
	u16  seq;
	u16 data_len;
}cmd_msg_frame_t;

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
void* usb_msg_process(void* attr);
void* net_msg_process(void* attr);


#endif