#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>


//思路：创建键值和消息队列号，接收消息，关闭消息队列号
typedef struct
{
	long	mtypes;
	char	mtext[50];
}data;

int main()
{
	key_t			ftok_rt;
	int				msgget_rt=0;
	int				msgsnd_rt=0;
	data			buf[3];

	ftok_rt=ftok("test",1);
	if(ftok_rt<0)
	{
		printf("create key value failture!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}

	msgget_rt=msgget(ftok_rt,IPC_CREAT|0666);//创建消息序列号
	if(msgget_rt<0)
	{
		printf("create qid failture!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}
	
	for(int i=0;i<3;i++)//把buf[]中的数据全部发出去
	{
		msgsnd_rt=msgrcv(msgget_rt,(void *)&buf[i],sizeof(buf[i].mtext),i,IPC_NOWAIT);
		if(msgsnd_rt<0)
		{
			perror("msgsnd failture!");
			return -1;
		}
		printf("type %d data is %s\n",buf[i].mtypes,buf[i].mtext);
		sleep(3);//这里我是在两个终端来做测试的所以有点来不急就在这里延时一下
	}

	printf("receive data over!\n");

	msgctl(msgget_rt,IPC_RMID,NULL);//关闭消息队列 
	return 0;
}
