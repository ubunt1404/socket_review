#include<getopt.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>          
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<sqlite3.h>
#define  BACKLOG   13
int  main(int   argc,char   **argv)
{

	int 	                      socket_fd=-1;
       	int                           acpt_fd=-1;
	int                           rv=-1;
	char		              buf_rv[150];	
	pid_t                         pid=0;
	struct sockaddr_in            serv_addr;
        struct sockaddr_in            cli_addr;
        socklen_t                     cliaddr_len;
	int                           c=0;
	int 			      reuse=0;
	int                           port=-1;
	int                           b_rv;
	float                         temp;
	sqlite3*		      sqt_db;     
	int                           sqt_id=0;	
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

	/*设置命令行参数*/	
	static struct option longopts[]=
  	{
		{"port",  required_argument,0,'p'},
		{"help",  no_argument      ,0,'h'},
		{0,0,0,0}
	};
	while((c=getopt_long(argc,argv,"p:h",longopts,NULL))!=-1)
	{
		switch(c)
  		{
			case 'p':
				port=atoi(optarg);
				printf("%d\n",port);
				break;
			case 'h':
				printf("端口：--p  [args] \nIP地址：--i [args]\n帮助 h\n");
					break;
		}
	}

	if(port<0)
	{
		printf("请加参数：-p\n");
		return 0;
	}

	/*指定与哪个套接字sockfd绑定地址通过指明IPV4/6、IP和端口来完成*/
	memset(&serv_addr,0,sizeof(serv_addr));

	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(const char*)& reuse,sizeof(int)); /*设置端口复用*/
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	b_rv=bind(socket_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));//bind 返回值: =0:成功 ；=-1失败
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
		
	     while(1)
	     {
	     	     rv=read(acpt_fd,buf_rv,sizeof(buf_rv));//<0:出错 ==0：读到文件底部了 >:返回读取的字节数
	     	     if(rv<0)//出错
	     	     {
	     		     printf("read erro:%s\n",strerror(errno));
			     close(acpt_fd);
     			     exit(0);
		     }
		     else if(rv==0)//读到文件底部
		     {
			     printf("read break!\n");
			     close(acpt_fd);
			     exit(0);
		     }
		     temp=(atoi(buf_rv))/1000;
		     
		     /*数据库操作：
		      * 创建库、创建表、插入数据
		      * */
		     sqlite3_open("temperature.db",&sqt_db);
		     sqlite3_exec(sqt_db,"create table socket_server(id int,temperature float)",NULL,NULL,NULL);
		     sqt_id++;
		     /*sqlite3中插入的值为变量*/
		     char* Sql=sqlite3_mprintf("insert into socket_server values('%d','%f')",sqt_id,temp);
		     sqlite3_exec(sqt_db,Sql,NULL,NULL,NULL);
		     sqlite3_close(sqt_db);
		     printf("ds18b20 temperature is:%f\n",temp);
		     if(write(acpt_fd,"server successfully receive!",rv)<0)
		     {
	       		     printf("write erro:%s",strerror(errno));
       			     close(acpt_fd);
		     }
     	     }
	     close(acpt_fd);
	}
	return 0;    
}


