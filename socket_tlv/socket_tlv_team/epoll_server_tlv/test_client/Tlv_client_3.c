/*********************************************************************************
 *      Copyright:  (C) 2020 Qiuliang<comeonqiuliang@163.com>
 *                  All rights reserved.
 *
 *       Filename:  client_1.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(2020年03月05日)
 *         Author:  Qiuliang <comeonqiuliang@163.com>
 *      ChangeLog:  1, Release initial version on "2020年03月05日 21时41分30秒"
 *                  2, 添加参数解析函数, setsockopt函数    
 *                  3, 添加发送时间函数           
 ********************************************************************************/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include <sys/types.h>          /*  See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include "crc-itu-t.h"

#define                 HEAD            0XFD
#define                 BUF_SIZE        128
#define                 MAGIC_CRC       0x1E50
#define                 TLV_FIXED_SIZE  5//H+T+L+crc1+crc2=5
#define                 TLV_MINI_SIZE   (TLV_FIXED_SIZE+1)
#define                 SUCCESS        1
#define                 RESEND         2

typedef struct send_ack_or_nack
{
    char        head;
    char        tag;
    char        length;
    char        value;
}ack_tlv;

void dump_buf(char *type, char *data, int len);
void get_time(char *time_now);
int  get_temperature(float *temp);
int  pack_temp_Tlv(unsigned char *buf, int size, float temp);
int  pack_time_Tlv(unsigned char *buf, int size, char *time_now);
int ack_unpack(ack_tlv *unpack);

int main(int argc, char **argv)
{
    int                      cli_fd;
    int                      rv;
    int                      cmd;
    struct sockaddr_in       cliaddr;
    unsigned char            buf[BUF_SIZE];
    int                      rv_tlv;
    int                      port =0;
    int                      on = 1;
    char                     time_now[128];
    //char                     send_buf[512];
    float        		     temp;
    ack_tlv                  ack;

    struct option           opts[]={
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while((cmd=getopt_long(argc, argv, "p:h", opts, NULL)) != -1)
    {
        switch(cmd)
        {
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
                printf("请以此格式执行程序： ./a.out -p XXXX\n");
                printf("\t\t-p: 指定绑定端口。\n");
                printf("\t\txxxx 为所选端口。\n");
                break;

        }

    }

    if( !port ) 
    {
        
                printf("请以此格式执行程序： ./a.out -p XXXX\n");
                printf("\t\t-p: 指定绑定端口。\n");
                printf("\t\txxxx 为所选端口。\n");
                return -1;
    }




    if((cli_fd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("SOCKET 失败， 原因是：%s.\n", strerror(errno));
        return -1;
    }
    printf("SOCKET 成功， 进入BIND.\n");

    setsockopt(cli_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    memset(&cliaddr, 0, sizeof(cliaddr));

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(port);
    inet_aton("127.0.0.1", &cliaddr.sin_addr);

    if((connect(cli_fd, (struct sockaddr *)&cliaddr, sizeof(cliaddr))) <  0)
    {
        printf("CONNECT失败， 原因是：%s.\n", strerror(errno));
        return -1;
    }
    printf("CONNECT 成功。\n");
   
/**************************************************************************************************/
    while(1)
    {
        if(get_temperature(&temp) < 0)
        {
            printf("获取温度失败， 原因是：%s。\n", strerror(errno));
            return -1;
        }

        memset(buf, 0, sizeof(buf));
        rv_tlv = pack_temp_Tlv(buf, sizeof(buf), temp);
        dump_buf("temperature:", buf, rv_tlv);

        if(write(cli_fd, buf, sizeof(buf)) < 0)
        {
            printf("温度发送失败， 原因是%s.\n",strerror(errno));
            close(cli_fd);
        }
        else 
        {
            printf("温度发送成功, 继续发送时间。\n");
        }

        get_time(time_now);
        memset(buf, 0, sizeof(buf));
        rv_tlv = pack_time_Tlv(buf, sizeof(buf), time_now);
        dump_buf("time:", buf, rv_tlv);
        printf("buf is %s。\n", buf+3);

        if(write(cli_fd, buf, sizeof(buf)) < 0)
        {
            printf("时间发送失败， 原因是%s.\n",strerror(errno));
            close(cli_fd);
        }
        else 
        {
            printf("时间发送成功, 5秒后继续发送。\n");
        }
        rv=0;
        rv=read(cli_fd,buf,sizeof(buf));
        printf("already read buf for ack unpack!\n");
        if(rv<=0)
        {
            printf("no receive data from server!\n");
        }
        printf("read yes\n");
        memcpy(&ack,buf,sizeof(ack));
        ack_unpack(&ack);
      sleep(10);
    }

/**************************************************************************************************/
    close(cli_fd);

}


int  pack_temp_Tlv(unsigned char *buf, int size, float temp)
{
        unsigned int             pack_len = 0;
        int                      ofset = 0;//它这里每次从0开始装，有问题啊
        int                      value_len=0;
        unsigned short           crc_rt=0;

        if( !buf || size<TLV_MINI_SIZE || temp<0 || temp > 40 )
        {
            printf("TLV封包输入无效参数\n");
            return 0;
        }

        buf[ofset] = HEAD;//Head
        ofset += 1;

        buf[ofset] = 1;//Tag
        ofset += 1;

        value_len = sizeof(temp);
        pack_len=value_len+TLV_FIXED_SIZE;
        buf[ofset]=pack_len;//Length
        ofset+=1;

        memcpy(buf+ofset, &temp, sizeof(temp));//value
        ofset+=sizeof(temp);

        crc_rt=crc_itu_t(MAGIC_CRC,buf,pack_len-2);
        memcpy(buf+ofset,&crc_rt,sizeof(crc_rt));
        return pack_len;

}


int  pack_time_Tlv(unsigned char *buf, int size, char *time_now)
{
        unsigned int             pack_len = 0;
        int                      data_len = 0; 
        int                      ofset = 0;
        unsigned short           crc_rt=0;

        if( !buf || size<TLV_MINI_SIZE || !time_now )
        {
            printf("TLV封包输入无效参数\n");
            return 0;
        }

        buf[ofset] = HEAD;//head
        ofset += 1;

        buf[ofset] = 2;//tag
        ofset += 1;

        if(strlen(time_now) < size-TLV_FIXED_SIZE)
            data_len = strlen(time_now);
        else 
            data_len = size - TLV_FIXED_SIZE;

        pack_len = data_len + TLV_FIXED_SIZE;
        buf[ofset] = pack_len;//length
        ofset += 1;

        memcpy(&buf[ofset], time_now, strlen(time_now));//value 
        ofset += data_len;
        
        crc_rt=crc_itu_t(MAGIC_CRC,buf,pack_len-2);//crc
        memcpy(buf+ofset,&crc_rt,sizeof(crc_rt));
        return ofset;
}

void dump_buf(char *type, char *data, int len)
{
    if(type)
    {
        printf("%s:\n", type);
    }

    int i;

    for(i=0; i<len; i++)
    {
        printf("0x%02x", data[i]);
    }

    printf("\n");
}

void get_time(char *time_now)
{
    time_t                      sec;
    struct tm                   *t1;
    time(&sec);
    t1 = localtime(&sec);

    sprintf(time_now, "%d#%d#%d#%d", t1->tm_mday, t1->tm_hour, t1->tm_min, t1->tm_sec);
   // printf("time_now is %s。\n",  time_now);
}

int get_temperature(float *temp)
{
	char 					way_1[50]="/sys/bus/w1/devices/";
	char 					way_2[50];
	char 					buf[128];
	char 					*ptr = NULL;
	int 					found = -1, rv = -1, fd = -1;
	DIR					*dir_1;
	struct dirent 				*dirent_1;

	if((dir_1 = opendir(way_1)) == NULL)
	{
		printf("无法打开文件夹[%s]\n", way_1);
		return -1;
	}

	while((dirent_1 = readdir(dir_1)) != NULL)
	{
		if(strstr(dirent_1->d_name, "28-"))
		{
			strcpy(way_2, dirent_1->d_name);
			found  = 1;
			break;
		}
	}

	if( !found )
	{
		printf("在文件夹内找不到28-的子文件夹。\n");
		return -1;
	}

	strncat(way_1, way_2, sizeof(way_1)-strlen(way_1));
	strncat(way_1, "/w1_slave", sizeof(way_1)-strlen(way_1));

	if((fd = open(way_1, O_RDONLY)) < 0)
	{
		printf("不能打开文件[%s]。\n", way_1);
		return -1;
	}

	if(rv = read(fd, buf, sizeof(buf)) < 0)
	{
		printf("不能读取文件[%s].\n", way_1);
		return -1;
	}

	ptr = strstr(buf, "t=");
	if(!ptr)
	{
		printf("在文件内找不到内容t=\n");
		return -1;
	}

	ptr += 2;
	*temp = atof(ptr)/1000.0;
	close(fd);


}

int ack_unpack(ack_tlv *unpack)//这里的函数只是最开始第一帧的解析，第一帧后面的就不行了，记得改
{
    if(unpack->head==0xfd && (unpack->tag==0x04 || unpack->tag==0x05))
    {
        printf("start unpack ack!\n");
        if(unpack->tag==0x04 && unpack->value==0x01)
        {
            printf("舒服，对方成功接受！\n");
            return SUCCESS;
        }
        if(unpack->tag==0x05 && unpack->value==0x02)
        {
            printf("翻车了要重传！\n");
            return RESEND;
        }
    }
}
