#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

//       int msgget(key_t key, int msgflg);

//       key_t ftok(const char *pathname, int proj_id);

//思路：创建键值和消息队列号，发送消息，关闭消息队列号
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
	data			buf[3]={{1,"suprise"},{2,"amazing"},{3,"depress"}};

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
	
	for(int i=0;i<3;i++)
	{
		msgsnd_rt=msgsnd(msgget_rt,(void *)&buf[i],sizeof(buf[i].mtext),IPC_NOWAIT);
		if(msgsnd_rt<0)
		{
			perror("msgsnd failture!");
			return -1;
		}
		sleep(2);
	}
	printf("send data over!\n");

	msgctl(msgget_rt,IPC_RMID,NULL);
	return 0;
}
