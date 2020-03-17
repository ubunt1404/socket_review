/****************************************************************
*	Author       : guhaiming 
*	Last modified: 2020-03-14 21:31
*	Email        : 2427779305@qq.com
*	blog         : https://blog.csdn.net/go_home_look
*	Filename     : pipe.c
*	Description  : 用管道实现父子进程间的通信，实现的原理在博客上 
****************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
//int pipe(int pipefd[2]);
int main()
{
	//思路：先pipe()再fork(),再根据数据流向关闭两端相应端口

	pid_t				pid; 
	int					pipe_fd[2];
	char				buf[30];
	int					rd;

	if(pipe(pipe_fd)<0)//先创建pipe
	{
		printf("pipe create failure!\n");
		return -1;
	}

	pid=fork();
	if(pid<0)//出错
	{
		printf("create child fork  failure!\n");
		return -2;
	}

	if(pid==0)//子进程创建成功，根据数据的流向方向，开始对子进程的进出端口操作 
	{
		close(pipe_fd[1]);//关写端口
		rd=read(pipe_fd[0],buf,sizeof(buf));//从读端口读数据
        if(rd<0)
		{
			printf("read data failture!\n");
			return	-3;
		}
		printf("I am you child process ,received data is:%s\n",buf);
	}

	//sleep(3);
	close(pipe_fd[0]);//根据数据的流向方向，操作父进程的进出端口
	write(pipe_fd[1],"hello",strlen("hello"));
	sleep(1);
	return 0;
	
}
