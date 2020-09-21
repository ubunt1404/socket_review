/****************************************************************
*	Author       : guhaiming 
*	Last modified: 2020-03-17 21:53
*	Email        : 2427779305@qq.com
*	blog         : https://blog.csdn.net/go_home_look
*	Filename     : server_fifo.c
*	Description  : 一开始的思路不对一直在server端写两个管道，一个读一个写，结果发现根本不行 
*				因为read、write在读写的时候会阻塞，根本无法同时进行，除非用多路复用但是那个 *				架子太大了,最后只能写这种一条命名管道的半双工通信
****************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#define FIFO	"fifo"

//int mkfifo(const char *pathname, mode_t mode);

int main(int argc ,char **argv)
{
	int			mkf_rt=0;
	int			open_rt=0;
	char			buf[30];
	int			sev_wr_rt;
	int			sev_rd_rt;
	int			mode;

	if(access(FIFO,F_OK))//确定目录下fifo文件已经存在，免得每次都要手动删除再运行
	{
		mkf_rt=mkfifo(FIFO,0777);//创建管道文件
		if(mkf_rt<0)
		{	printf("create server fifo failure!\n");
			return -1;
		}
	}
	mode=atoi(argv[1]);
	printf("mode is :%d\n",mode);
	if(mode==0)
	{
		//mode 0表示读
		//这里的open函数打开会阻塞这次是真的见识到了
		open_rt=open(FIFO,O_NONBLOCK|O_RDONLY);//服务器端只读，形势打开管道文件
		if(open_rt<0)
		{
			printf("server open FIFO  failture in line 35!\n");
			return -1;
		}
		printf("open file successfully!\n");
	
		memset(buf,0,sizeof(buf));//这里清空一下免得在cline端打印出来的东西是乱码
		while(1)
		{
			sev_rd_rt=read(open_rt,buf,sizeof(buf));
			if(sev_rd_rt<0)
			{
				printf("read I am cline failture in line 43!\n");	
				printf("%s\n",strerror(errno));
				return -1;
			}
			printf("server read data from cline is:%s\n",buf);
			sleep(3);
		}
	}

	if(mode==1)
	{
		//mode 1 表示写
		open_rt=open(FIFO,O_NONBLOCK|O_WRONLY);//客户端只写，前面open的只写和这里open的只读对应命名管道的两头
		if(open_rt<0)
		{
			printf("server open cline failture!\n");
			printf("%s\n",strerror(errno));
			return -1;
		}
		printf("open fifo no problem!\n");
		while(1)
		{
			sev_wr_rt=write(open_rt,"server",20);
			if(sev_wr_rt<0)
			{
				printf("write I am server failture in line 75!\n");
				printf("%s\n",strerror(errno));
				return -1;
			}
			printf("I am successfully writing data to cline \n");
			sleep(3);
		}
	
	}
	close(open_rt);//关闭客户端管道文件
	unlink(FIFO);//删除管道
	return 0;
}
