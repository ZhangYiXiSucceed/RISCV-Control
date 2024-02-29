#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <getopt.h>
#include "link.h"
#include "net.h"


static struct option long_options[] = {
        {"device", required_argument, 0, 'd'},
        {"speed", required_argument, 0, 's'},
        {"byte", required_argument, 0, 'b'},
        {"net", required_argument, 0, 'n'},
        {"send", required_argument, 0, 'e'},
        {0, 0, 0, 0}
    };

int main(int argc, char* argv[])
{

    char* device = NULL;
    int index = 0;
    int op_type = 0;
    int byte_num;
    int res;
    int fd;
    while ((res = getopt_long(argc, argv, "d:s:b:n:e", long_options, &index)) != -1) {
        switch (res) {
            case 'd':
                device = optarg;
                printf("device=%s\r\n",device);
                break;
            case 's':
                op_type = atoi(optarg);;
                break;
            case 'b':
                byte_num = atoi(optarg);
                printf("byte_num=%d\r\n", byte_num);
                break;
            case 'n':
                fd = net_init("192.168.0.141", 51230);
                printf("fd=%d\r\n",fd);
                net_send(fd, "192.168.0.141", sizeof("192.168.0.141"));
                break;
            case 'e':
                
                break;
            case '?':
                if (optopt == 'd' || optopt == 's') {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                } else {
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                }
                return 1;
            default:
                abort();
        }
    }

    switch(op_type)
    {
      case 1:
      {
        printf("test_type=%d\r\n", op_type);
        test_usb_speed(device, byte_num);
      }
      case 2:
      {
        test_get_capacity(device);
      }
      break;
      case 3:
      {
        test_get_device_description(device);
      }
      break;
      default:
    }
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