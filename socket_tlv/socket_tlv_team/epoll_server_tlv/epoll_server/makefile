epoll_tlv_test: socket_server_init.o getopt_server.o epoll_server.o
	gcc -o epoll_tlv_test -l sqlite3 socket_server_init.o getopt_server.o epoll_server.o
epoll_server.o: epoll_server.c
	gcc -c epoll_server.c -o epoll_server.o
socket_server_init.o:socket_server_init.c
	gcc -c socket_server_init.c -o socket_server_init.o
getopt_server.o:getopt_server.c
	gcc -c getopt_server.c -o getopt_server.o
.PHONY:clean
clean:
	-rm -rf *.o epoll_tlv_test test a.out

