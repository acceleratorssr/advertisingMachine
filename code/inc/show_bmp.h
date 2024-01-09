#ifndef _SHOW_BMP_H_
#define _SHOW_BMP_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include "tcp_recv.h"

int show_bmp(char *bmp_path);
int show_bmp_loop();

#endif