#ifndef SCSI_H__
#define SCSI_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <asm/byteorder.h>
#include <linux/types.h>
#include <linux/hdreg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define SENSE_LENTH  32
#define MAX_SCSI_CDBLEN 16

#define SG_CHECK_CONDITION 0x02
#define SG_DRIVER_SENSE 0x08

typedef   signed  char         int8_t;  //  标准表达方式 signed char 被等同于 int8_t；
typedef   signed short int     int16_t;
typedef   signed  int          int32_t;//在32位环境里，int代表4个字节32位！！
 
typedef   unsigned char         uint8_t;
typedef   unsigned short int    uint16_t;
typedef   unsigned int          uint32_t;
 
typedef   uint32_t   u32;///32位
typedef   uint16_t   u16;///16位
typedef   uint8_t     u8;///8位

typedef int handle;
#define invalid_handle ((handle)(-1))

typedef enum scsi_enum {
    SCSI_NONE = 0,
    SCSI_TO_DEVICE = 1,
    SCSI_FROM_DEVICE = 2,
}scsi_enum;
#define __min(a,b) ((a)<(b)?(a):(b))

int scsi_excete(handle fd, scsi_enum direction, const void *cdb, u8 cdblen, void* buf, u32 buff_len, void* sense, u32 sense_len, u32 timeout, int retries);

#ifdef __cplusplus
};
#endif

#endif