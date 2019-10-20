gcc -c epoll.c -o epoll.o
gcc -o test epoll.o -lsqlite3
rm -rf *.o a.out
