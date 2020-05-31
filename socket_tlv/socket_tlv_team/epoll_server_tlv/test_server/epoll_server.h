#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H 
typedef struct send_ack_or_nack
{
     char        head;
     char        tag;//tag =4表示ack tag=5 表示nack
     char        length;//不加crc这里算4个字节
     char        value;
}ack_tlv;
int socket_server_init(char *listen_ip,int listen_port);
int getopt_server(int argc,char *argv[]);//返回解析出的端口号
void send_ack_or_nack(ack_tlv* send_ack ,int send_msg);
#endif
