#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define		SIZE	512
//       key_t ftok(const char *pathname, int proj_id);
//        int shmget(key_t key, size_t size, int shmflg);

//思路：很简单就是创建共享内存然后调用操作函数，使进程关联共享内存或者分离共享内存，其实只要你前面的消息队列通了后面的都是类似的，就是设计的机制不同，工具都是IPC对象
typedef struct 
{
	char			reader[50];
}*point;

int main()
{
	int				shm_g_rt=0;
	key_t			ftok_rt=0;
	point			pointer;
	
    ftok_rt=ftok("test", 1);//创建键值
	if(ftok_rt<0)
	{
		printf("create key value failture!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}

    shm_g_rt=shmget(ftok_rt,SIZE,IPC_CREAT|0666);//创建共享内存标识符 
	if(shm_g_rt<0)
	{
		printf("shared memory create failtore!\n");
		printf("%s\n",strerror(errno));
		return -1;
	}

	//void *shmat(int shmid, const void *shmaddr, int shmflg);返回一个不确定类型的指针
	pointer=shmat(shm_g_rt, NULL, 0);
	if((void *) -1==pointer)
	{
		printf("shmat function on error return value !\n");
		printf("%s\n",strerror(errno));
	}

	printf("%s\n",pointer->reader);	//从共享内存中读取数据

	shmdt(pointer);					//分离共享内存
	
	//int shmctl(int shmid, int cmd, struct shmid_ds *buf);
	shmctl(shm_g_rt,IPC_RMID,NULL);	//删除共享内存
	
	return 0;

}
