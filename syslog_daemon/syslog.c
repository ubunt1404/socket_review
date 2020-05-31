/****************************************************************
*	Author       : guhaiming 
*	Last modified: 2020-04-19 20:39
*	Email        : 2427779305@qq.com
*	blog         : https://blog.csdn.net/go_home_look
*	Filename     : syslog.c
*	Description  : syslog 的另一种理解是查看守护进程这种在后台什么都不输出的进程的运行状态，
*	查看日志的命令是：tail -2 + 日志存放路径
*	日志存放的路径是：以我的电脑为例 /var/log/syslog 
*	例如：tail -2 /var/log/syslog 
*	google@ubuntu1604:/var/log$ tail -2 /var/log/syslog
*	Apr 19 21:26:20 ubuntu1604 gnome-session[2127]: message repeated 3 times: [ MainControlle*   r::SogouImePinYinShow]
*	Apr 19 21:26:49 ubuntu1604 syslog: progarm test starting...
*	但是真的系统日志会被冲刷掉，前面我测试的时候记录的结果，现在我去看真的不见了
****************************************************************/
#include <stdio.h>
#include <syslog.h>
int main(int argc ,char *argv[])
{
	syslog(LOG_ERR,"progarm %s start runing...",argv[0]);//这的argc[0]相当于printf函数后面的%s%d 对应的参数,与%s 对应
	printf("hello world!\n");
	return 0;
}
