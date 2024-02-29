#include "scsi.h"
#include "errno.h"

int __handle_sense_info(u8 *sense_buf)
{
    u8 sense_key = sense_buf[2]& 0xf;
    u8 asc = sense_buf[12];
    u8 ascq = sense_buf[13];
}

int scsi_excete(handle fd, scsi_enum direction, const void *cdb, u8 cdb_len, void* buf, u32 buff_len, void* sense, u32 sense_len, u32 timeout, int retries)
{
    int error;
    int loops;
    int data_direction;

    struct sg_io_hdr io_hdr;
    u8 sense_buf[SENSE_LENTH];

    switch (direction)
    {
        case SCSI_NONE:
            data_direction = SG_DXFER_NONE;
            break;
        case SCSI_TO_DEVICE:
            data_direction = SG_DXFER_TO_DEV;
            break;
        case SCSI_FROM_DEVICE:
            data_direction = SG_DXFER_FROM_DEV;
            break;
        default:
            error = -1;
            break;
    }

    if (retries < 0)
    {
        retries = 0;
    }

    for(loops=retries; loops>=0; loops--)
    {
        memset(sense_buf, 0, sizeof(sense_buf));
        memset(&io_hdr, 0, sizeof(io_hdr));
        
        io_hdr.interface_id = 'S';
        io_hdr.dxfer_direction = data_direction;

        io_hdr.cmdp = (u8*)cdb;
        io_hdr.cmd_len = __min(cdb_len, MAX_SCSI_CDBLEN);

        io_hdr.sbp = sense_buf;
        io_hdr.mx_sb_len = sizeof(sense_buf);

        io_hdr.dxferp = buf;
        io_hdr.dxfer_len = buf ? buff_len : 0;

        io_hdr.timeout = timeout*1000;

        error = ioctl(fd, SG_IO, &io_hdr);
        if(-1 == error)
        {
            printf("error=%d, errno=%s\r\n", error,strerror(errno));
        }
        if(io_hdr.status &&  io_hdr.status != SG_CHECK_CONDITION)
        {
            error = -1;
            printf("1\r\n");
            continue;
        }

        if(io_hdr.host_status)
        {
            error = -1;
             printf("2\r\n");
            continue;
        }

        if(io_hdr.driver_status && io_hdr.driver_status!= SG_DRIVER_SENSE)
        {
            error = -1;
             printf("3\r\n");
            continue;
        }

        if(io_hdr.driver_status != SG_DRIVER_SENSE && __handle_sense_info(sense_buf))
        {
            error = -1;
             printf("4\r\n");
            continue;
        }
        else if(io_hdr.driver_status == SG_DRIVER_SENSE && io_hdr.sb_len_wr > 10 && (sense_buf[0] !=0x72 \
                                                                                    || sense_buf[7] < 0x14 \
                                                                                    || sense_buf[8] != 0x09 \
                                                                                    || sense_buf[9] < 0x0c \
                                                                                    ))
        {
            error = -1;
             printf("5 key=%x asc=%x ascq=%x\r\n", sense_buf[2],sense_buf[12],sense_buf[13]);
            continue;
        }

    }
    
    if(sense && sense_len && io_hdr.sb_len_wr)
    {
        memcpy(sense, sense_buf, __min(io_hdr.sb_len_wr, __min(sense_len, sizeof(sense_buf))));
    }
    return error;
}