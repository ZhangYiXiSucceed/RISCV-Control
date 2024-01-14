#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "net.h"

int net_init(char* ip, int port)
{
    int client_socket = 0;
    struct sockaddr_in server_addr;
    char *message = "Hello from client";
    char buffer[1024] = {0};

    // 创建socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 将IP地址从点分十进制转换为二进制
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    return client_socket;
}

void net_send(int client_socket, unsigned char* message, int len)
{
    // 发送数据
    send(client_socket , message , len, 0);
    printf("Message sent\n");
}


void net_rev(int client_socket, unsigned char*buffer)
{
    // 接收数据
    read(client_socket, buffer, 1024);
    printf("Server: %s\n",buffer);
}