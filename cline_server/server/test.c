#include<getopt.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>          
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>

#define  BACKLOG   13
int  main(int   argc,char   **argv)
{

	int 	                      socket_fd=-1;
       	int                           acpt_fd=-1;
	int                           rv=-1;
	char		              buf[150];	
	pid_t                         pid=0;
	struct sockaddr_in            serv_addr;
        struct sockaddr_in            cli_addr;
        socklen_t                     cliaddr_len;
	int                           c=0;
	int 			      reuse=0;
	int                           port=0;
	int                           b_rv;

	/*通信协议（ipv 4/6）、通信类型(TCP/UDP)、协议编号。
	 * 成功：返回文件描述符。失败：返回-1*/
	socket_fd=socket(AF_INET,SOCK_STREAM,0);
	if(socket_fd<0)
	{
	               printf("socket create error:",strerror(errno));
		       return   -1;
	}
	else 
		       printf("socket successfully!\n");
        memset(&serv_addr,0,sizeof(serv_addr));
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(const char*)& reuse,sizeof(int));  

	//serv_addr.sin_family=AF_INET;
	static struct option longopts[]=
  	{
		{"port",  required_argument,0,'p'},
		{"help",  no_argument      ,0,'h'},
	};
	while(1)
	{
		c=getopt_long(argc,argv,"p:h",longopts,NULL);
		if(c==-1)
		
			break;

		switch(c)
  		{
			case 'p':
				//port=atoi(optarg);
  				//serv_addr.sin_port = htons(atoi(optarg));//对端口号进行处理 htons(atoi(optarg)) 会出问题自己试           
				serv_addr.sin_family=AF_INET;
				//serv_addr.sin_port=htons(port);
				serv_addr.sin_port=htons(atoi(optarg));
				serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
				printf("%d\n",port);
				printf("%d\n",serv_addr.sin_port );
				break;
			case 'h':
				printf("端口：--p  [args] \nIP地址：--i [args]\n帮助 h\n");
		 		break;
		}
	}

	//printf("%d\n",port);
	/*指定与哪个套接字sockfd绑定地址通过指明IPV4/6、IP和端口来完成*/
	b_rv=bind(socket_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	//bind 返回值: =0:成功 ；=-1失败
	
	printf("b_rv is:%d\n",b_rv);
	if(b_rv<0)
        {
	        	printf("bind erro:%s\n",strerror(errno));
	        	printf("%d\n",bind(socket_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)));
			return -2;
	}

	else
		        printf("bind successfully!\n");

       	/*监听来自客户端的tcp socket的连接请求*/
	listen(socket_fd,BACKLOG);
	printf("start listening port...\n");
	while(1)
	{
             /*接收从客户端发来的请求信息*/
	     acpt_fd=accept(socket_fd,(struct sockaddr *)&cli_addr,&cliaddr_len);
	     if(acpt_fd<1)
	     {
		        printf("accept error:%s\n",strerror(errno));
		        break;
			//return 1;
	     }
	     pid=fork();
	     if(pid<0)
	     {
		        printf("fork error:%s\n",strerror(errno));
		        close(acpt_fd);
		        continue;
	       	        return -4;
	     }
	     else if(pid>0)
	     {
		        printf("The is parents process:%s\n",strerror(errno));
		        close(acpt_fd);
		        continue;
		        return -5;
	     }
	     else if(pid==0)
	     {
		        printf("build pid successfully:%s\n",strerror(errno));
          	        close(socket_fd);
		        memset(buf,0,sizeof(buf));
		while(1)
		{
		        rv=read(acpt_fd,buf,sizeof(buf));
			if(rv<0)
			{
				printf("read erro:%s\n",strerror(errno));
				close(acpt_fd);
				exit(0);
		       	}
			else if(rv==0)
			{
				printf("read break!");
				close(acpt_fd);
				exit(0);
			}
			printf("%s\n",buf);
			/*if(write(acpt_fd,buf,rv)<0)
			 * {
			 * printf("write erro:%s",strerror(errno));
			 * close(acpt_fd);
			 * }*/
			printf("%s\n",buf);
		}
      		close(acpt_fd);
	     }
	     close(acpt_fd);
	}
	return 0;    
}


