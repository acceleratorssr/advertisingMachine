#!/bin/bash

gcc -o client fileclient.c
gcc -o server fileserver.c
./server