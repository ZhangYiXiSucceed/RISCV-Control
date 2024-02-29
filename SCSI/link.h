#ifndef LINK_H__
#define LINK_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "scsi.h"
#include "link.h"

#define   DEVICE_NAME_LEN 64 

typedef struct usb_device_handle_struct{
    handle fd;
    u8* data;
    u32 data_len;
    char device[DEVICE_NAME_LEN];
    u8 opcode;
}usb_device_handle_t;

int usb_open(usb_device_handle_t* usb, const char* device);
int usb_has_data(usb_device_handle_t* device, u8 opcode);
int usb_tx_data(usb_device_handle_t* device, u8 opcode, void* data, u32 len);
int usb_rx_data(usb_device_handle_t* device, u8 opcode, void* data, u32 len);
int usb_rx_data_low_level(usb_device_handle_t* device, u8* cbd_data, void* data, u32 len);
#ifdef __cplusplus
};
#endif

#endif
