#!/bin/bash
gcc -c cline_fifo.c  server_fifo.c
gcc -o cline cline_fifo.o  
gcc -o server server_fifo.o
rm -rf *.o a.out

