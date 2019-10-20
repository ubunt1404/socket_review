#!/bin/bash
gcc pthread.c -o test -lpthread -lsqlite3
rm -rf *.o a.out
