#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include <fcntl.h>
#include <syslog.h>
#include <termios.h>
#include <time.h>
#include <sys/stat.h>   //获取一些文件相关的信息
#include <errno.h>      //错误号定义
#include "net.h"

int net_init(net_device_handle_t* net_device, char* ip, int port)
{
    int client_socket = 0;
    struct sockaddr_in server_addr;

    memcpy(net_device->ip, ip, strlen(ip));
    net_device->port = port;
    // create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // convert ip from ten to the binary
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // connect server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    net_device->fd = client_socket;
    return client_socket;
}

void net_send(net_device_handle_t* net_device, unsigned char* message, int len)
{
    // send data
    signal(SIGPIPE, SIG_IGN);   //ignore the quit signal 
    if(net_device->fd > 0)
    {
        if((message != NULL) && (len > 0))
        {
            int len = send(net_device->fd, message, len, 0);
            if(len==-1)
            {
                printf("Failed to Send Message!\n");
                close(net_device->fd);
                net_device->fd=-1;
            }
        }
    }
    else
    {
        printf("net is disconnect\n");
        net_init(net_device, net_device->ip, net_device->port);
    }
}


void net_rev(net_device_handle_t* net_device, unsigned char*buffer, u16 data_len)
{
    fd_set rfds_Socket;
    FD_ZERO(&rfds_Socket);
    FD_SET(net_device->fd, &rfds_Socket);
    int retval=select(net_device->fd + 1, &rfds_Socket, NULL, NULL, NULL);
                                    //tv控制选择的时间，若规定时间内没有收到数据
                                    //则不监控该rfds。则若为NULL，一直等到接收到数据
                                    //再继续程序执行。
    printf("retval=%d--rfds_Socket=%d*****Sockfd_CCU=%d\n",retval,rfds_Socket,net_device->fd);
    if(retval==0)
    {
        printf("select error\n");
        return;
    }

    if(FD_ISSET(net_device->fd, &rfds_Socket)) //接收到数据了
    {
        //bzero(RecBuff_VST, MaxLength);
        int  len = recv(net_device->fd, buffer, data_len, 0);
        if(len<=0)
        {
            close(net_device->fd);
            net_device->fd=-1;
        }
    }
    else
    {
        printf("dont recieve data\n");
        sleep(1);
    }
}