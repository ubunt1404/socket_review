#!/bin/bash
gcc -c select.c -o select.o
gcc -o test select.o
rm -rf *.o a.out
