#include "link.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#define sleep_ms(x)  usleep((x)*1000)
usb_device_handle_t usb_device_g;

int  test_usb_speed(const char* device, int times)
{
    u8 data[512]= {0};
    for(int i=0;i<512;i++)
    {
      data[i]=i;
    }
    struct timeval start, end;
    long mtime, seconds, useconds;
    int count = 0;

    int res = usb_open(&usb_device_g,device);
    while(res != -1)
    {
      gettimeofday(&start, NULL); // 获取开始时间
      count = 0;
      while(count<times)
      {
          int res = usb_tx_data(&usb_device_g, 0xF1, data,512);
          //printf("res=%d\r\n",res);
          while(1)
          {
            res = usb_has_data(&usb_device_g, 0xF1);
            //printf("has data res=%d\r\n",res);
            if(res)
            { 
              break;
            }
          }
          if(res)
          {
              //memset(data,0x00,sizeof(data));
              res = usb_rx_data(&usb_device_g, 0xF1, data, 512);

            

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

      printf("Elapsed time: %ld milliseconds,speed=%f B/S\n", mtime, (float)(times*512/mtime*1000));
    }
  
    //printf("hello world\r\n");
    return 0;
}

usb_device_handle_t  udisk_g;
void test_get_capacity(const char* device)
{
     u8 data[512]= {0};
    int res = usb_open(&udisk_g,device);
    if(res != -1)
    {
        res = usb_rx_data(&udisk_g, 0x25, data, 8);
        int total_blcok =  data[0]<<24 | data[1]<<16 |data[2]<<8 |data[3];
        int block_size =   data[4]<<24 | data[5]<<16 |data[6]<<8 |data[7];

        printf("total_blcok=%d  block_size=%d\r\n", total_blcok + 1, block_size);
    }
}

void test_get_device_description(const char* device)
{
    u8 data[512]= {0};
    u8 cdb[16] ={0x12, 0x00,0x00,0x00,0x00,0x00};
    int res = usb_open(&udisk_g,device);
    if(res != -1)
    {
        res = usb_rx_data_low_level(&udisk_g, cdb, data, 36);
        printf("Vendor=%s Product=%s Version=%s\r\n", &data[8], &data[16], &data[32]);
    }
}