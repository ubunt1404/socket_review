#!/bin/bash
gcc -c poll.c -o poll.o
gcc -o test poll.o -lsqlite3
rm -rf *.o a.out
