#!/bin/bash
gcc -c server_fork.c -o server_fork.o
gcc -o test server_fork.o -lsqlite3
rm -rf *.o

