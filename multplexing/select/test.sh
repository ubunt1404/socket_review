#!/bin/bash
gcc -c select.c -o select.o
gcc -o test select.o -lsqlite3
rm -rf *.o a.out
