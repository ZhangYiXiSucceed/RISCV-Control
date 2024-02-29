#ifndef SCSI_H__
#define SCSI_H__

#ifdef __cplusplus
extern "C"{
#endif

int net_init(char* ip, int port);
void net_send(int client_socket, unsigned char* message, int len);

#ifdef __cplusplus
};
#endif

#endif
