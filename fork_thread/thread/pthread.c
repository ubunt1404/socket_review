#include <getopt.h>
#include<stdio.h>
#include<errno.h>
#include<sys/types.h>          
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>

#define  BACKLOG   13
void* work_func(void* arg);
int  main(int   argc,char   **argv)
{

	int 	                      socket_fd=-1;
       	int                           acpt_fd=-1;
	int                           rv=-1;
	char		              buf[150];	
	pthread_t                     tid;//子线程创建成功时会返回自己的进程ID
	pthread_attr_t                thread_attr;//线程属性变量用于设置子线程属性
	struct sockaddr_in            serv_addr;
        struct sockaddr_in            cli_addr;
        socklen_t                     cliaddr_len;
	int                           port=-1;
	int                           rt_bind=-1;
   	int                           c=0;
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

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(port);
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
        listen(socket_fd,BACKLOG);
	printf("waiting cline...\n");
	while(1)
	{
             acpt_fd=accept(socket_fd,(struct sockaddr *)&cli_addr,&cliaddr_len);
	     if(acpt_fd<1)
	     {
		        printf("accept error:%s\n",strerror(errno));
			break;
			//return 1;
	     }
	     /*对thread_attr线程属性变量进行初始化*/
	     if( pthread_attr_init(&thread_attr) )
	     {
		     printf("pthread_attr_init() failure: %s\n", strerror(errno));
		     goto CleanUp;
	     }

	     /*设置子线程堆栈大小*/
	     if( pthread_attr_setstacksize(&thread_attr, 120*1024) )
	     {
		     printf("pthrread_attr_setstacksize() failure: %s\n", strerror(errno));
		     goto CleanUp;
	     }

	     /*设置线程启动为分离状态
	      * 注解：不同的状态对应线程资源释放情况不同
	      * 分离状态（detached）：线程资源随着子线程的结束由系统自动释放
	      * 可会和状态（joinable）：原有的线程等待创建的线程结束，只有当pthread_join函数返回时，
	      * 创建的线程才算终止，释放自己占用的系统资源*/
	     if( pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) )
	     {
		     printf("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
		     goto CleanUp;
	     }

	     /*创建子线程：
	      * 参数：
	      * 1.返回线程ID 2.按照线程变量中的内容对子线程属性进行配置 
	      * 3.处理客户请求 4.客户端套接字，通过这个标识来为相应客户创建相应字线程*/
	      pthread_create(&tid,&thread_attr,work_func,(void *)acpt_fd);
	}
	close(socket_fd);
CleanUp:
	/*线程属性变量在使用完之后，我们应该调用pthread_attr_destroy 把他摧毁释放。*/
	pthread_attr_destroy(&thread_attr);
	return 0;    
}

void* work_func(void* arg)
{
	char                      buf[1024];
	int                       acpt_fd;
	int                       rv=-1;
	
	acpt_fd=(int)arg;
	memset(&buf,0,sizeof(buf));
	while(1)
	{
		rv=read(acpt_fd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("read error:%s\n",strerror(errno));
		        continue;
		}
		else if(rv==0)
		{
			printf("read break!\n");
		        close(acpt_fd);
			pthread_exit(NULL);
		}
		rv=write(acpt_fd,buf,sizeof(buf));
		if(rv<0)
		{
			printf("write error:%s\n",strerror(errno));
	                close(acpt_fd);
			pthread_exit(NULL);
		}
		printf("%s\n",buf);
	}

}
