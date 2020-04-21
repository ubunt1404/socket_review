/****************************************************************
*	Author       : guhaiming 
*	Last modified: 2020-04-19 15:45
*	Email        : 2427779305@qq.com
*	blog         : https://blog.csdn.net/go_home_look
*	Filename     : basename.c
*	Description  : 说明一下basename命令和函数的使用。
*	命令：basename + 文件路径名=路径下最后一个文件名 
*	举个例子：
*	google@ubuntu1604:~$ basename /workspace/socket_review/syslog_daemon/test.txt
*	test.txt
*	google@ubuntu1604:~$ basename /workspace/socket_review/syslog_daemon
*	syslog_daemon
*	google@ubuntu1604:~$ basename /workspace/socket_review
*	socket_review
*	可以看出basename 命令就是用来提取一条路径下的最后一个名字
*	下面看basename()函数的使用和运行结果：
****************************************************************/
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
int main(int argc ,char *argv[])
{
	char			 *print_index;
	
	daemon(0,0);
	while(1)
	{
		print_index=basename(argv[0]);
		printf("the argumnet of file is:%s\n",print_index);
	}
	return 0;
}

/* 运行结果：
 * google@ubuntu1604:~/workspace/socket_review/syslog_daemon$ gcc basename.c -o basename
 * google@ubuntu1604:~/workspace/socket_review/syslog_daemon$ ./basename 
 * the argumnet of file is:basename 
 * 从运行结果可以看出：basename函数获取的是运行程序时"./"后面的内容
 */

/* 这里借机解释一波daemon(),daemon就是让程序在后台运行而且是那种即使，系统不停我不停的那种，  * 如果父进程退出太任然让那个程序在后台执行，你只用ps命令是看不到在后台运行的进程的
 * 查看后台运行程序命令：
 * ps -aux | grep +程序名
 * ps aux | grep +程序名
 * ps -ef | grep +程序名
 * 看看效果：
 * google@ubuntu1604:~/workspace/socket_review/syslog_daemon$ ps aux | grep basename
 * google     7882 44.4  0.0   4356    76 ?        Rs   16:42   4:18 ./basename
 * google     7943 34.6  0.0   4356    76 ?        Rs   16:44   2:49 ./basename
 * google     7962 32.8  0.0   4356    76 ?        Rs   16:45   2:22 ./basename
 * google     8271  0.0  0.0  21312   940 pts/4    S+   16:52   0:00 grep --color=auto basena * me
 * google@ubuntu1604:~/workspace/socket_review/syslog_daemon$ ps -ef | grep basename
 * google     7882   1895 34 16:42 ?        00:41:12 ./basename
 * google     7943   1895 33 16:44 ?        00:39:44 ./basename
 * google     7962   1895 33 16:45 ?        00:39:16 ./basename
 * google    10133   2098  0 18:43 pts/4    00:00:00 grep --color=auto basename
 * google@ubuntu1604:~/workspace/socket_review/syslog_daemon$ ps -aux | grep basename
 * google     7882 34.0  0.0   4356    76 ?        Rs   16:42  41:28 ./basename
 * google     7943 33.2  0.0   4356    76 ?        Rs   16:44  39:59 ./basename
 * google     7962 33.1  0.0   4356    76 ?        Rs   16:45  39:32 ./basename
 * google    10144  0.0  0.0  21312   960 pts/4    S+   18:44   0:00 grep --color=auto basena * me
 * */
