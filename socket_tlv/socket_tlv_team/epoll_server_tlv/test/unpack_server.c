#include "unpack_server.h"
#include <stdio.h>
void unpack_server(char* unpack_buf)
{
    int             buf_ofset=0;
    int             tlv_head=0xfd;
    int             value_length=0; //每一帧报文中数据占的长度
    int             length=0;
    int             current_loction_buf=0;
    char            temperature_buf[12];//目前只有温度，后面会加sn,time,所以就先设大点
    //crc_check(CRC_MAGIC,unpack_buf,value_length);//目前先不加入，等客户端完成再进行修改
    if(unpack_buf[buf_ofset]==tlv_head)
    {
        printf("cline data start!\n");
    }
    //马上去完善没有找到head的应对方法 

    buf_ofset+=2;//读取TLV中的length
    length=unpack_buf[buf_ofset];//在求亮的代码中这里是value的长度而不是整个报文的长度
    //value_length=length-3;//获得value长度以便以字节为单位读取value值
    value_length=length;
    current_loction_buf=buf_ofset+1;//读取value的开始位置
    printf("the unpack temperature is:\n");
    for(int i=0;i<value_length;i++)
    {
            temperature_buf[i]=unpack_buf[current_loction_buf+i];
            //printf("%d",temperature_buf[i]);
            //printf("0x%02X",temperature_buf[i]);
            //memcpy();
    }
          printf("0x%02x\n",temperature_buf[0]);
    //current_loction_buf+=value_length;//移动当前位置到下一帧数据

}
