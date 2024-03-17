#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C"{
#endif

#define NET_DEVICE_NAME_LEN 16
typedef struct net_device_handle_struct{
    int  fd;
    int  port;
    char ip[NET_DEVICE_NAME_LEN];
}net_device_handle_t;


int net_init(net_device_handle_t* net_device, char* ip, int port);
void net_send(net_device_handle_t* net_device, unsigned char* message, int len);
#ifdef __cplusplus
};
#endif

#endif