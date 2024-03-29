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
#include "basic_type.h"

#define SENSE_LENTH  32
#define MAX_SCSI_CDBLEN 16

#define SG_CHECK_CONDITION 0x02
#define SG_DRIVER_SENSE 0x08

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