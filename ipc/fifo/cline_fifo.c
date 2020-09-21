/****************************************************************
*	Author       : guhaiming 
*	Last modified: 2020-03-15 23:42
*	Email        : 2427779305@qq.com
*	blog         : https://blog.csdn.net/go_home_look
*	Filename     : fifo_cline.c
*	Description  : 复习fifo的知识 
****************************************************************/
//思路：用1个管道（抽象的一种模型），实现两个进程间半双工通信，同一时间只能发或只能收
//管1：通过管道实现服务器端读，客户端写
//
#include <stdio.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define FIFO	"fifo"


//int mkfifo(const char *pathname, mode_t mode);

int main(int argc,char **argv)
{
	int			cln_rd_rt=0;
	int			cln_wr_rt=0;
	int			mkf_rt=0;
	int			open_rt=0;
	char			buf[30];
	int			mode;

	if(access(FIFO,F_OK))//判断一下文件，免得每次./test都要手动删除两个文件
	{
		mkf_rt=mkfifo(FIFO,0777);//创建客户端管道文件
		if(mkf_rt<0)
		{
			printf("cline fifo create filture!\n");
			return -1;
		}
	}
	mode=atoi(argv[1]);
	if(mode==0)
	{

		//open_cln_rt=open("cline.c",O_NONBLOCK );//只读形势打开管道文件
	
		//mode 0表示读
		open_rt=open(FIFO,O_NONBLOCK);//只写不阻塞形势打开管道文件
		if(open_rt<0)
		{
			printf("cline open cline.c failture!\n");
			return -1;
		}
		printf("50 line is blocking ?\n");
		memset(buf,0,sizeof(buf));
		while(1)
		{
			cln_rd_rt=read(open_rt,buf,sizeof(buf));
			if(cln_rd_rt<0)
			{
				printf("cline read hello failture in line 55 \n");
				printf("%s\n",strerror(errno));
				return -1;
			}
			printf("read data from server is:%s\n",buf);
			sleep(3);
		}

	}

	if(mode==1)
	{
		//mode 1表示写
		open_rt=open(FIFO,O_NONBLOCK);//设置服务器端读，实际就是一个模拟原理图的过程
		if(open_rt<0)
		{
			printf("cline open server.c failture !\n");
			return -1;
		}
		cln_wr_rt=write(open_rt,"I am cline !",14);
		if(cln_wr_rt<0)
		{
			printf("clien write I am clien failture in line 70\n");
			printf("%s\n",strerror(errno));
			return -1;
		}

	}

	sleep(3);
	close(open_rt);//关闭管道文件
	unlink(FIFO);//删除管道 

	return 0;
}

