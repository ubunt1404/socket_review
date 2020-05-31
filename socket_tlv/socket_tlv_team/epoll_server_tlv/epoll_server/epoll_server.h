#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H 
int socket_server_init(char *listen_ip,int listen_port);
int getopt_server(int argc,char *argv[]);//返回解析出的端口号
#endif
