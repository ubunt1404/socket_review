#include "unpack_server.h"
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "crc-itu-t.h"
#define MAGIC_CRC           0x1E50
#define SUCCESS             1
#define FAULT               -1
#define FIXED_SIZE          5

#define TEMPERATURE         1
#define TIME                2
#define SN                  3
//ACK=4,NACK=5
#if 1
/*用memcpy()解析数据*/
int unpack_server(unsigned char* unpack_buf,sqlite3* sqt_db;)
{
    int             offset=0;
    int             tlv_head=0xfd;
    int             tlv_tag=0;

    float           temperature=0;//进数据库准备的变量
    char            sn_temp[30];
    char            time_temp[30];
    int             sqt_id=0;
    char            *ptr_temperature;
    char            *ptr_sn;
    char            *ptr_time;
    char            *name_temp="temperature";
    char            *name_time="time";
    char            *name_sn="sn";


    int             current_loction_buf=0;
    char            temperature_buf[12];//目前只有温度，后面会加sn,time,所以就先设大点
    int             crc_check_rt=0;
    int             value_length;

    if(unpack_buf[offset]==tlv_head)
    {
        printf("cline data start!\n");
    }
    //马上去完善没有找到head的应对方法 
    crc_check_rt=crc_check(MAGIC_CRC,unpack_buf);
    if(crc_check_rt>0)
    {
        offset+=1;                //读取TLV中的tag
        tlv_tag=unpack_buf[offset];
        offset+=1;                //读取pack_length
        value_length=unpack_buf[offset]-FIXED_SIZE;
        offset+=1;                //get value

        if(tlv_tag==TEMPERATURE)
        {
            /*unpack temperature*/
            memcpy(&temperature,unpack_buf[offset],value_length);
            sprintf(ptr_temperature,"%s",temperature);
            char* Sql_tmp=sqlite3_mprintf("insert into device_temp values(null,'%s','%s')",   
                    sqt_id,name_temp,ptr_temperature);
            sqlite3_exec(sqt_db,Sql_tmp,NULL,NULL,NULL);
        }
        else if(tlv_tag==TIME)
        {
            /* unpack time */
            memcpy(time_temp,unpack_buf[offset],value_length);
            ptr_time=time_temp;
            char* Sql_tim=sqlite3_mprintf("insert into device_temp values(null,'%s','%s')",
                    sqt_id,name_time,ptr_time);
            sqlite3_exec(sqt_db,Sql_tim,NULL,NULL,NULL);
        }
        else if(tlv_tag==SN)
        {
            /* unpack SN */
            memcpy(sn_temp,unpack_buf[offset],value_length);
            ptr_sn=sn_temp;
            char* Sql_sn=sqlite3_mprintf("insert into device_temp values(null,'%s','%s')",
                    sqt_id,name_sn,ptr_sn);
            sqlite3_exec(sqt_db,Sql_sn,NULL,NULL,NULL);
        }

        //current_loction_buf=offset+1;//读取value的开始位置
        //printf("the unpack temperature is:\n");
        //current_loction_buf+=value_length;//移动当前位置到下一帧数据
        return SUCCESS;
    }
    else
        return FAULT;
}

#endif

#if 0
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

    buf_ofset+=2;                //读取TLV中的length
    length=unpack_buf[buf_ofset];
    //value_length=length-3;     //获得value长度以便以字节为单位读取value值
    value_length=length;
    current_loction_buf=buf_ofset+1;//读取value的开始位置
    printf("the unpack temperature is:\n");
    for(int i=0;i<value_length;i++)
    {
        temperature_buf[i]=unpack_buf[current_loction_buf+i];
        printf("0x%02X",temperature_buf[i]);
    }
    //current_loction_buf+=value_length;//移动当前位置到下一帧数据

}
#endif

//现在这里的函数只能做解析一帧数据的逻辑，如果过来的数据是连续的多帧一起过来怎么办？
unsigned short  crc_check(unsigned short magic_crc,unsigned char *client_buf)
{
    unsigned short              crc_rt;
    unsigned short              client_crc;
    unsigned short              client_crc_temp[4];
    unsigned int                pack_length;
    unsigned short              offest=0;
    unsigned short              crc_position;

    offest+=2;//获取msg_length
    pack_length=(unsigned int)(client_buf[offest]);//得到用于校验的length值
    crc_rt=crc_itu_t(MAGIC_CRC,client_buf,pack_length-2);//计算crc值

    crc_position=(unsigned short)(client_buf[offest])-2;//crc value in buf position
    memcpy(client_crc_temp,&client_buf[crc_position],sizeof(client_crc));
    client_crc=client_crc_temp[0];//获取client端数据包中的crc值

    if(crc_rt!=client_crc)//计算值crc与读取的crc进行对比
    {
        printf("the data is bad value!\n");
        return FAULT;
    }
    else
        printf("congratulation they are equals,crc check successful!\n");
    return SUCCESS;
}
