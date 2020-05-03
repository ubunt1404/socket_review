#include <stdio.h>
#include "epoll_server.h"
#define     ACK             4
#define     NACK            5
#define     FIXED_SIZE      4
/*head(1)+tag(1)+length(1)+value(1)*/
void send_ack_or_nack(ack_tlv* send_ack ,int send_msg)
{
    send_ack->head=0xfd;
    if(send_msg==4)
    {
        send_ack->tag=ACK;
        send_ack->value=0x01;
    }
    if(send_msg==5)
    { 
        send_ack->tag=NACK;
        send_ack->value=0x02;
    }
    send_ack->length=FIXED_SIZE;
}
