#!/bin/bash 
gcc -c message_queue_send.c -o send.o
gcc -o send send.o

gcc -c message_queue_receive.c -o receive.o
gcc -o receive receive.o

rm rf *.o
