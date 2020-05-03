VARABLE= unpack_server.o epoll_server.o getopt_server.o socket_server_init.o send_ack_or_nack.o
test:$(VARABLE)
	gcc -o test $(VARABLE) -lsqlite3
%.o:%.c
	gcc -c $< -o $@
#$<=所有c文件 $@=所有目标文件
.PHONY:clean
clean:
	rm -rf *.o a.out test 
