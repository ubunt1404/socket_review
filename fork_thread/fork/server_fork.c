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
	char		              buf[150];	
	pid_t                         pid=0;/*fork()函数返回的进程号*/
	struct sockaddr_in            serv_addr;
        struct sockaddr_in            cli_addr;
        socklen_t                     cliaddr_len;
	int                           c=0;
	int 			      reuse=0;
	int                           port=-1;
	int                           rt_bind=-1;
	float 			      temp=0;
	int                           id_sqt=0;
	sqlite3*		      sqt_db;
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
	
	/*命令行解析参数*/
	static struct option longopts[]=
  	{
		{"port",  required_argument,0,'p'},
		{"help",  no_argument      ,0,'h'},
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
				printf("端口：--p  [args] \n帮助: h\n");
		 		break;
		}
		break;
	}
	if(c<0)
	{
		printf("error:-p 没加！\n");
		return 0;
	}

	/*指定与哪个套接字sockfd绑定地址通过指明IPV4/6、IP和端口来完成*/
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,(const char*)& reuse,sizeof(reuse));/*设置端口复用*/
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port = htons(port);//对端口号进行处理      
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	rt_bind=bind(socket_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	printf("%d\n",rt_bind);
        if(rt_bind<0)
	{
	        	printf("bind erro:%s\n",strerror(errno));
			return -2;
	}
	else 
		        printf("bind successfully!\n");

       	/*监听来自客户端的tcp socket的连接请求*/
	listen(socket_fd,BACKLOG);
	printf("start listen port...\n");
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
	     if(pid<0)/*小于0出错*/
	     {
		        printf("fork error:%s\n",strerror(errno));
		        close(acpt_fd);
		        continue;
	       	        return -4;
	     }
	     else if(pid>0)/*大于0时是返回给父进程的进程号*/
	     {
		        printf("The is parents process:%s\n",strerror(errno));
		        close(acpt_fd);
		        continue;
		        return -5;
	     }
	     else if(pid==0)/*等于0时是返回给子进程的进程号*/
	     {
		        printf("build pid successfully:%s\n",strerror(errno));
          	        close(socket_fd);/*关闭监听套接字*/
		        memset(buf,0,sizeof(buf));
		while(1)
		{
			/*处理客户端请求*/
		        rv=read(acpt_fd,buf,sizeof(buf));
			if(rv<0)//出错
			{
				printf("read erro:%s\n",strerror(errno));
				close(acpt_fd);
				exit(0);
		       	}
			else if(rv==0)//文件读完（已到底部）
			{
				printf("read break!");
				close(acpt_fd);
				exit(0);
			}
			temp=(atoi(buf))/1000.0;
			printf("content of buf is:%s\n",buf);
			printf("receive the temperature from cline is:%f\n",temp);
			
			/*对收到的温度做数据库操作*/
			sqlite3_open("temp_fork.db",&sqt_db);
			sqlite3_exec(sqt_db,"create table temperature(id_sqt int,temperature float)",NULL,NULL,NULL);
			char* sqt_laug=sqlite3_mprintf("insert into temperature values('%d','%f')",id_sqt,temp);
			sqlite3_exec(sqt_db,sqt_laug,NULL,NULL,NULL);
			sqlite3_close(sqt_db);
		}
      		close(acpt_fd);/*关闭子进程中的客户端请求*/
	     }
	     close(acpt_fd);/*关闭父进程的客户端请求*/
	}
	return 0;    
}


