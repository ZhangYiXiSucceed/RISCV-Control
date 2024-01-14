#include "link.h"

#define  SCSI_WRITE_DATA 1
#define  SCSI_READ_LEN   2
#define  SCSI_READ_DATA  3
#define  SCSI_HAS_DATA_TIMEOUT 500
#define  SCSI_TX_DATA_TIMEOUT  500
#define  SCSI_RX_DATA_TIMEOUT  30

#define BSWAP_32(x)  \
    (u32)\
    ( \
        (((u32)(x) & 0xFF000000) >> 24) | \
        (((u32)(x) & 0x00FF0000) >> 24) | \
        (((u32)(x) & 0x0000FF00) >> 24) | \
        (((u32)(x) & 0x000000FF) >> 24) \
    )

#define __cpu_to_be32(x)  BSWAP_32((x))

typedef struct scsi_cmd_struct
{
    u8 prj_id;
    u8 data_bype;
    u8 resv[10];
    u32 length;
}scsi_cmd_t;

handle open_device(const char* device)
{
    handle fd;
    printf("start open device\r\n");
    fd = open(device, O_RDWR);
    if(fd == invalid_handle)
    {
        fd = open(device, O_RDWR | O_NONBLOCK);
    }
    if(fd == invalid_handle)
    {
        printf("failed open\r\n");
    }
    return fd;
}

int close_device(handle fd)
{
    printf("close device\r\n");
    int err=close(fd);
    return err;
}

int usb_open(usb_device_handle_t* usb, const char* device)
{
    usb->fd = open_device(device);
    printf("fd=%d\r\n", usb->fd);
    int device_len = strlen(device);
    device_len = (device_len>DEVICE_NAME_LEN)?DEVICE_NAME_LEN:device_len;
    memcpy(usb->device, device, device_len);
    return usb->fd;
}


int usb_has_data(usb_device_handle_t* device, u8 opcode)
{
    u32 nbytes = 4;
    scsi_cmd_t cdb = { opcode, SCSI_READ_LEN, {0}, __cpu_to_be32(nbytes)};
    u32 data = 0;
    device->opcode = opcode;
    if(0 != scsi_excete(device->fd, SCSI_FROM_DEVICE, &cdb, sizeof(cdb), &data, nbytes, NULL, 0, SCSI_HAS_DATA_TIMEOUT, 0))
    {
        return 0;
    }
    if(data !=0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


int usb_tx_data(usb_device_handle_t* device, u8 opcode, void* data, u32 len)
{
    scsi_cmd_t cdb = { opcode, SCSI_WRITE_DATA, {0}, __cpu_to_be32(len)};
    device->opcode = opcode;
    device->data = data;
    device->data_len = len;
    int tx_len = ((0 == scsi_excete(device->fd, SCSI_TO_DEVICE, &cdb, sizeof(cdb), data, len, NULL, 0, SCSI_TX_DATA_TIMEOUT, 0))?len:0);
    return tx_len;
}


int usb_rx_data(usb_device_handle_t* device, u8 opcode, void* data, u32 len)
{
    scsi_cmd_t cdb = { opcode, SCSI_READ_DATA, {0}, __cpu_to_be32(len)};
    device->opcode = opcode;
    device->data = data;
    device->data_len = len;
    int rx_len = ((0 == scsi_excete(device->fd, SCSI_FROM_DEVICE, &cdb, sizeof(cdb), data, len, NULL, 0, SCSI_RX_DATA_TIMEOUT, 0))?len:0);
    return rx_len;
}

int usb_rx_data_low_level(usb_device_handle_t* device, u8* cbd_data, void* data, u32 len)
{
    device->opcode = cbd_data[0];
    device->data = data;
    device->data_len = len;
    int rx_len = ((0 == scsi_excete(device->fd, SCSI_FROM_DEVICE, cbd_data, 16, data, len, NULL, 0, SCSI_RX_DATA_TIMEOUT, 0))?len:0);
    return rx_len;
}


