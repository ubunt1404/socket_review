#include <sys/types.h>
#include <dirent.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "temperature.h"
/*int get_temperature(char temp_string[50]);
int main()
{
	char                    temp_string[50];
	if(get_temperature(temp_string)<0)
	{
		printf("get temperature failure!\n");
		return -1;
	}
	printf("ds18b20 temperature is:%s\n",temp_string);
	return 0;
}*/

/*获取温度以字符串形式返回*/
int get_temperature(char temp_string[50])
{
	char                 buf[150];
	char                 path_name[50];
	char                 *p;
 	int                  fd=-1;
        int                  found=0;
        float                temp;
        char    	     path[150]="/sys/bus/w1/devices/";
        DIR                  *openfile;
        struct dirent        *readfile;
	//char                 temp_string[50];
	
	/*获取path子目录下的所由文件和目录的列表，如果path是个文件则返回值为NULL,
	 * 如果成功返回一个指向该目录的指针*/
	openfile=opendir(path);
	if(openfile==NULL)
	{
		printf("opendir error!\n");
		return -1;
	}

	/*struct dirent *readdir(DIR *dirp)*/
	readfile=readdir(openfile);//将openfile下的所有文件读取一遍放入readfile中
	if(readfile==NULL)
	{
	        printf("readfile error:%s\n",strerror(errno));
		return   -2;
	}

	while((readfile=readdir(openfile))!=NULL)
	{
		/*strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。
		 * 如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL
		 */
		if(strstr(readfile->d_name,"28-"))//读取成功就会将指针指向首部
		{
			
			printf("%s\n",strstr(readfile->d_name,"28-"));
			/*打印一下strstr()返回的指针（字符串首地址即该字符串）*/
			
			printf("%s\n",readfile->d_name);
			/*经过strstr()函数的作用已经将指针由d_name数组首地址移向28-的首部*/	

			/* char *strcpy(char *dest, const char *src) 
			 * 把 src 所指向的字符串复制到 dest。
	                 */
			strcpy(path_name,readfile->d_name);
			found=1;
			break;
		}
	}
	
	closedir(openfile);
	strncat(path,path_name,sizeof(path_name)-strlen(path_name));
        strncat(path,"/w1_slave",sizeof(path_name)-strlen(path_name));
	printf("%s\n",path);

	/*以只读形式打开path路径下的文件*/
	fd=open(path,O_RDONLY);
	if(fd<0)
	{
		printf("open error:%s\n",strerror(errno));
		return -3;
	}
	
	/* read函数从filedes指定的已打开文件中读取nbytes字节到buf中*/
	read(fd,buf,sizeof(buf));
	p=strstr(buf,"t=");
        p+=2;
//	temp=atof(p);
	
	strcpy(temp_string,p);
	printf("temp_string value is:%s\n",temp_string);
	
//	*temperature=temp/1000.0;

	if(!found)
	{
		return -1;
	}
	close(fd);
        return 0;
}











