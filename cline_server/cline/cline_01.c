#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "temperature.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc,char **argv)
{
	char                             temp[50];
	int                              sockfd=-1;
	int                              conect_rt=0;
	char                             buf_s[50];
	struct sockaddr_in               serv_addr;
	int                              wrt_rt=0;
	int                              read_rt=0;
	char		                 buf_r[50];
	int                              ip_port;
	int                              port;
	char*                            ip;
	/*socket指明大反面通信协议IPV4/6、小方面通信类型TCP/UDP*/
	sockfd =socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
	{
		printf("socket failure:%s\n",strerror(errno));
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	 /* int inet_aton(const char *cp, struct in_addr *inp);
	  * 将一个字符串IP地址转换为一个32位的网络序列IP地址
	  * inet_aton( SERVER_ADDR, &serv_addr.sin_addr );
	  */
	
	const struct option longopts[]=
	{
		{"port",  required_argument,0,'p'},
		{"ip"  ,  required_argument,0,'i'},
		{"help",  no_argument      ,0,'h'},
		{NULL,0,NULL,0}
	};
	int                                c=0;

	while((c=getopt_long(argc,argv,"i:p:h",longopts,NULL))!=-1)
	{
		/*c必须放在while循环内*/
		switch(c)
		{
			case 'p':
				port=atoi(optarg);
				break;
			case 'i':
				ip=optarg;
				break;
			case 'h':
				printf("端口：--p  [args] \nIP地址：--i [args]\n帮助 h\n");
				return 0;
		}
	}
	ip_port=((!ip) ||(!port));
	if(ip_port)
	{
		printf("请ip 、port同时输入！\n");
		return 0;
	}
	printf("错误在这里吗？73\n");
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);//对端口号进行处理
	inet_aton( ip, &serv_addr.sin_addr );//对IP进行操作
	/*connect指定大的通信协议IPV4/6、通信端口、IP*/
	printf("in line 79?\n");
	conect_rt =connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
	printf("%d\n",conect_rt);
	if(conect_rt<0)
	{
		printf("connect failure:%s\n",strerror(errno));
		return -2;
	}
	else
		printf("connect successfully!\n");
	
	if(get_temperature(temp)<0)
	{
	 	printf("get_temperature error:%s\n",strerror(errno));
    		return -1;
 	}
	printf("DS18B20 temperature is:%s\n",temp);
	
	wrt_rt=write(sockfd,temp,sizeof(temp));
	if(wrt_rt<0)
	{
		printf("write data failure:%s\n",strerror(errno));
		return -3;
	}
	
	read_rt=read(sockfd,buf_r,sizeof(buf_r));
	if(read_rt<0)
	{
		printf("read failure!\n");
		return -5;
	}
		printf("%s\n",buf_r);
	sleep(3);
	close(sockfd);
   return 0;
}
