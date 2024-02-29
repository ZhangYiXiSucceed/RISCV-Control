#include "link.h"
#include <unistd.h>
#define sleep_ms(x)  usleep((x)*1000)
u8 data[1024];
int main()
{
    usb_open("/dev/sdb");
    while(1)
    {
         u8 data[16]= {1,2,3,4,5,6,7,8};
        int res = usb_tx_data(data,16);
        printf("res=%d\r\n",res);
        sleep_ms(3000);
        res = usb_has_data();
        printf("has data res=%d\r\n",res);
        if(res)
        {
            res = usb_rx_data(data, 16);
            printf("rx data res=%d\r\n",res);
            for(int i=0;i<16;i++)
            {
                printf("%d ",data[i]);
            }
            printf("\r\n");
            sleep_ms(3000);
        }
    }
    printf("hello world\r\n");
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <sys/ioctl.h>

// #define MY_IOCTL_CMD _IOR('k', 1, int)  // 定义自定义ioctl命令

// int main() {
//     int fd;
//     const char *device = "/dev/sda";  // 替换为您的设备路径

//     // 打开设备文件
//     fd = open(device, O_RDWR);
//     if (fd == -1) {
//         perror("Failed to open device");
//         exit(EXIT_FAILURE);
//     }

//     // 发送ioctl命令
//     int arg;
//     if (ioctl(fd, MY_IOCTL_CMD, &arg) == -1) {
//         perror("Failed to send ioctl command");
//         exit(EXIT_FAILURE);
//     }

//     printf("Result: %d\n", arg);

//     // 关闭设备文件
//     close(fd);

//     return 0;
// }
