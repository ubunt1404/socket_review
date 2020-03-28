#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <libgen.h> 
#include <sys/types.h>   
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <poll.h>
#define ARRAY_SIZE(x)       (sizeof(x)/sizeof(x[0]))

static inline void print_usage(char *progname);
int socket_server_init(char *listen_ip, int listen_port);

int main(int argc, char **argv)
{
    int                       listenfd, connfd;
    int                       serv_port = 0;
    int                       daemon_run = 0;
    char                     *progname = NULL;
    int                       opt;
    int                       rv;
    int                       i, j;
    int                       found;
    int                       max;
    char                      buf[1024];
    struct pollfd             fds_array[1024];
    float                     temp;
    sqlite3*                  sqt_db;
    int                       sqt_id=0;
    
    struct option             long_options[] = 
    {   
        {"daemon", no_argument, NULL, 'b'},
        {"port", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };  

    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "bp:h", long_options, NULL)) != -1) 
    {   
        switch (opt)
        {   
            case 'b':
				daemon_run=1;
                break;

            case 'p':
                serv_port = atoi(optarg);
                break;

            case 'h':  /*  Get help information */
                print_usage(progname);
                return EXIT_SUCCESS;

            default:
                break;
        }   
    }   

    if( !serv_port ) 
    {   
        print_usage(progname);
        return -1; 
    }

    if( (listenfd=socket_server_init(NULL, serv_port)) < 0 )
    {
        printf("ERROR: %s server listen on port %d failure\n", argv[0],serv_port);
        return -2;
    }
    printf("%s server start to listen on port %d\n", argv[0],serv_port);


    /* set program running on background */
    if( daemon_run ) 
    {
        daemon(0, 0);
    }
    
    /*与select相比就是将数组改成了结构体数组来存储文件描述符*/
    for(i=0; i<ARRAY_SIZE(fds_array) ; i++)
    {
	    fds_array[i].fd=-1;
    }

    /*
     * struct pollfd
     * {
     * 		int fd;  //文件描述符 
     * 		short events;  //等待的事件：通过这个值告诉内核我们关心的是每个描述符的哪些事件
     * 		short revents; // 从内核中返回文件描述符实际发生了的事件 
     * };
     */
    fds_array[0].fd = listenfd;
    fds_array[0].events = POLLIN;/*普通或者优先级带数据可读*/

    max = 0;

    for ( ; ; ) 
    {
		/* 参数：
		* 	参1：指向pollfd型结构体的数组
		* 	参2：数组中监听元素的个数
		* 	参3: 我们愿意等待多长时间：
		* 				==-1：永不超时
		* 				==0：不等待
		* 				>0:等待毫秒级 
		*/

		rv = poll(fds_array, max+1, -1);
        if(rv < 0)
        {
            printf("poll failure: %s\n", strerror(errno));
            break;
        }
        else if(rv == 0)
        {
            printf("poll get timeout\n");
            continue;
        }

		/* listen socket get event means new client start connect now */
		if (fds_array[0].revents & POLLIN)
        {
            if( (connfd=accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0)
            {
                printf("accept new client failure: %s\n", strerror(errno));
                continue;
            }

	    found = 0;
	    for(i=1; i<ARRAY_SIZE(fds_array) ; i++)
	    {
	        if( fds_array[i].fd < 0 )
			{
				printf("accept new client[%d] and add it into array\n", connfd );
				fds_array[i].fd = connfd;
				fds_array[i].events = POLLIN;
				found = 1;
				break;
			}
	    }

	    if( !found )
	    {
			printf("accept new client[%d] but full, so refuse it\n", connfd);
			close(connfd);
			continue;
	    }
	   
	    max = i>max ? i : max;
	    if (--rv <= 0)
		    continue;
        }
        else /* data arrive from already connected client */
        {
			for(i=1; i<ARRAY_SIZE(fds_array); i++)
			{
				if( fds_array[i].fd < 0 )
					continue;
				
				if(fds_array[i].revents & POLLIN)//连上的客户端被监听到有事件响应
				{
					rv=read( fds_array[i].fd, buf, sizeof(buf) );
					if( (rv=read(fds_array[i].fd, buf, sizeof(buf))) <= 0)
					{
						printf("socket[%d] read failure or get disconncet.\n", fds_array[i].fd);
						close(fds_array[i].fd);
						fds_array[i].fd = -1;
					}
					printf("socket[%d] read get %d bytes data successful!\n", fds_array[i].fd, rv);
		    
					/*数据库操作*/
					temp=(atoi(buf))/1000.0;
					printf("receive the temperature from cline is:%f\n",temp);
					sqlite3_open("temperature.db",&sqt_db);
					sqlite3_exec(sqt_db,"create table poll_temp(sqt_id int,temperature float)",NULL,NULL,NULL);
					sqt_id++;

					/*sqlite3中插入的值为变量要将变量转化为字符*/
					char* Sql=sqlite3_mprintf("insert into poll_temp values('%d','%f')",sqt_id,temp);
					sqlite3_exec(sqt_db,Sql,NULL,NULL,NULL);
					sqlite3_close(sqt_db);
				
				    else if( write(fds_array[i].fd, buf, rv) < 0 )
					{
						printf("socket[%d] write failure: %s\n", fds_array[i].fd, strerror(errno));
						close(fds_array[i].fd);
						fds_array[i].fd = -1;
					}
				}
			}
		}
	}

CleanUp:
    close(listenfd);
    return 0;
}


static inline void print_usage(char *progname)
{ 
    printf("Usage: %s [OPTION]...\n", progname);
    
    printf(" %s is a socket server program, which used to verify client and echo back string from it\n", progname); 
    printf("\nMandatory arguments to long options are mandatory for short options too:\n"); 
    
    printf(" -b[daemon ]  set program running on background\n");
    printf(" -p[port    ]  Socket server port address\n");
    printf(" -h[help    ]  Display this help information\n");
    

    printf("\nExample: %s -b -p 8900\n", progname);
    return ;
}


int socket_server_init(char *listen_ip, int listen_port)
{
    struct sockaddr_in        servaddr;
    int                       rv = 0;
    int                       on = 1;
    int                       listenfd;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Use socket() to create a TCP socket failure: %s\n", strerror(errno));
        return -1;
    }

    /* Set socket port reuseable, fix 'Address already in use' bug when socket server restart */
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;   
    servaddr.sin_port = htons(listen_port); 

    if( !listen_ip )  /* Listen all the local IP address */
    {
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  
    }
    else /* listen the specified IP address  */
    {
        if (inet_pton(AF_INET, listen_ip, &servaddr.sin_addr) <= 0)
        {
            printf("inet_pton() set listen IP address failure.\n");
		    rv = -2;
            goto CleanUp;
        }
    }


    if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
    {
        printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
        rv = -3;
        goto CleanUp;
    }

    if(listen(listenfd, 13) < 0)
    {
        printf("Use bind() to bind the TCP socket failure: %s\n", strerror(errno));
        rv = -4;
        goto CleanUp;
    }

CleanUp:
    if(rv<0)
        close(listenfd);
    else
        rv = listenfd;

    return rv;
}

