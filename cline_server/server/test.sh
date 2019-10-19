#!bin/bash/
gcc -c socket.c  -o socket.o
gcc -o test socket.o -lsqlite3
rm -rf *.o
