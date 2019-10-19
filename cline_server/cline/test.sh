#!/bin/bash
#此脚本用于客户端的编译链接生成test可执行文件

gcc -c temperature.c cline_01.c
gcc -o test temperature.o cline_01.o
rm -rf *.o 

