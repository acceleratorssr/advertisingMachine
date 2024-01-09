#ifndef _TCP_RECV_H_
#define _TCP_RECV_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// 客户接收
void *clientRecv(void *arg);
extern int st;

#endif