#include <stdio.h>
#include "crc-itu-t.h"
#define MAGIC_CRC           0x1E50
#define SUCCESS             1
#define FAULT               -1
#if 0
int main()
{
	unsigned short				crc_rt;
	unsigned char				buf[2]={'a','2'};
	unsigned int				length=2;
	crc_rt=crc_itu_t(MAGIC_CRC,buf,1);//计算crc值
	printf("is one :%d\n",crc_rt);

	crc_rt=crc_itu_t(MAGIC_CRC,buf,2);//计算crc值
	printf("is two:%d\n",crc_rt);
	return 0;
}
#endif

/*函数设计的大概功能：将发送过来的crc与计算出的crc进行校验*/
//现在有一个问题就是我这里的方法是只能因对一帧的情况，如果发过来的帧数据是两帧连续过来，我应该改这里还是在收到的时候做数据缓冲呢？
unsigned short  crc_check(unsigned short magic_crc,char *client_buf)
{
	unsigned short				crc_rt;
    unsigned short              client_crc;
	unsigned short				*client_crc_temp;
	unsigned short				msg_length;
    unsigned short              offest=0;
    unsigned short              crc_position;

    offest+=2;//获取msg_length
    msg_length=(unsigned short)(client_buf[offest]-2);//得到用于校验的length值
    crc_position=client_buf[offest]-2;//crc value in buf position
    memcpy(client_crc_temp,&client_buf[crc_position],sizeof(client_crc));
    client_crc=client_crc_temp[0];//获取client端数据包中的crc值
    printf("check the client_crc value is:%d\n",client_crc);

	crc_rt=crc_itu_t(MAGIC_CRC,cline_buf,msg_length);//计算crc值
	printf("crc_rt is:%d\n",crc_rt);
    
	if(crc_rt!=cline_crc->crc)//计算值crc与读取的crc进行对比
	{
		printf("the data is bad value!\n");
        return FAULT;
	}
	else
		printf("crc check successful!\n");
    return SUCCESS;
}
