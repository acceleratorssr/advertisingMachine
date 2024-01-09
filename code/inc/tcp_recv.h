#ifndef _TCP_RECV_H_
#define _TCP_RECV_H_

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

extern int st;
// 客户接收
void *client_recv(void *arg);

#endif