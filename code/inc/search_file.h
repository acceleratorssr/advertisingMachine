#ifndef _SEARCH_FILE_H_
#define _SEARCH_FILE_H_
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "singleLinkedList.h"
void search_file(char *path, LinkList L, char *front, int len);

#endif