#!/bin/bash
gcc -c poll.c -o poll.o
gcc -o test poll.o
rm -rf *.o a.out
