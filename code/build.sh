#!/bin/bash

cd server
gcc -o server tcp_send.c
cd ..

clean
make