#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C"{
#endif

int net_init(char* ip, int port);
int server_accept_client(int *listenfd);

#ifdef __cplusplus
};
#endif

#endif