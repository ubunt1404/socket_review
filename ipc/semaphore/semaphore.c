#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>


//思路：fork之后在子进程中用V操作唤醒后面用P操作进行等待的父进程
int main()
{
	int					p_id;
	key_t				key;
	int					semid;
	struct sembuf		sembuf1;
	struct sembuf		sembuf2;
	int					semop_v_rt;
	int					semop_p_rt;

	key=ftok("test",1);
	/*创建键值,注意这里的test文件必须要存在，你没有创建的话他不会给你自动创建的
	这是由他的原理决定的，key值的创建是根据已存在文件的索引节点号来创建的，所以要从原
	理上区别open,不要产生误解*/
	
	if(key<0)
	{
		printf("child process create key value failture !\n");
		printf("%s\n",strerror(errno));
		return -1;
	}
	semid=semget(key,1,IPC_CREAT|0666);
	/*设置成互斥锁，当信号量不存在时就创建新的信号量，创建成功后将返回信号量的标示符，
	* 这里要注意下的是0666这里是给的权限如果还是不行，就直接sudo权限执行c文件*/

	if(semid<0)
	{
		printf("create semaphore failture you will can't get semid !\n");
		printf("%s\n",strerror(errno));
		return -1;
	}
	printf("create semaphore successfully!\n");
	
	p_id=fork();
	if(p_id<0)
	{
		printf("create child process failture!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}
	if(p_id==0)//子进程
	{
		printf("I am child!\n");
		sembuf1.sem_num = 0;
		sembuf1.sem_op = 1;//V操作,释放资源信号量加1,V操作后唤醒父进程，一定是子进程运行完了父进程再接过来
		sembuf1.sem_flg = SEM_UNDO; 
		semop_v_rt=semop(semid,&sembuf1,1);//V操作只对信号量操作一次
		if(semop_v_rt<0)
		{

			printf("child process operate semaphore failture in line 66 will return -1!\n");
		    printf("%s\n",strerror(errno));
			return -1;
		}

		printf("child process start runing!\n");
		printf("Dad! I am using critical resource don't make trouble!\n");
		printf("Dad! I've run out ,Here you are !\n");
		semctl(semid,0,IPC_RMID);//删除信号量集中标号为1的信号量，也就是第一个信号量,标号从0开始
	}
	//p操作等着子进程唤醒
	
	printf("I am parent!\n");
	sembuf2.sem_num=0;
	sembuf2.sem_op=-1;
	sembuf2.sem_flg=SEM_UNDO;
	semop_p_rt=semop(semid,&sembuf2,1);
	if(semop_p_rt<0)
	{
		printf("parent process operate semaphore failture in line 78!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}

	printf("parent process start runing!\n");
	printf("Oh! thank you my child! dad will using the critical resource!\n");
	printf("I've run out ,True relaxation!\n");
	semctl(semid,0,IPC_RMID);//删除信号量集中标号为1的信号量，也就是第一个信号量,标号从0开始
	
	return 0;
}
