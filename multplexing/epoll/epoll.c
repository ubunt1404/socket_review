#include <stdio.h>
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
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sqlite3.h>

#define MAX_EVENTS          512
#define ARRAY_SIZE(x)       (sizeof(x)/sizeof(x[0]))

static inline void print_usage(char *progname);
int socket_server_init(char *listen_ip, int listen_port);
void set_socket_rlimit(void);

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
    char                      buf[1024];

    int                       epollfd;
    struct epoll_event        event;
    struct epoll_event        event_array[MAX_EVENTS];
    int                       events;
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

    set_socket_rlimit(); /* set max open socket count */

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

    /*创建epoll对象实例，size指定要通过epoll实例来检查的文件描述符个数*/
    if( (epollfd=epoll_create(MAX_EVENTS)) < 0 )
    {
	printf("epoll_create() failure: %s\n", strerror(errno));
	return -3;
    }

    //event.events = EPOLLIN|EPOLLET;
    event.events = EPOLLIN;//可读取非高优先级数据
    event.data.fd = listenfd;

    /*作用：修改由文件描述符epfd所代表的epoll实例中的兴趣列表
     * 将listenfd添加（EPOLL_CTL_ADD）到epoll对象实例的兴趣列表中
     * 并指定listenfd文件描述符上我们感兴趣的事件(event.events=EPOLLIN)*/
    if( epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) < 0)
    {
	printf("epoll add listen socket failure: %s\n", strerror(errno));
	return -4;
    }


    for ( ; ; ) 
    {
	/*作用：返回epoll实例中处于就绪态的文件描述符信息 
	 * 通过event_array指向的结构体返回就绪态文件描述符的信息 
	 * MAX_EVENTS：数组中包含的元素个数
	 * 指定timeout等于-1，调用将一直阻塞，直到兴趣列表中的文件描述符上有事件产生或者直到捕获到一个信	  号为止。
	 * 成功：返回数组中元素的个数 */
 	events = epoll_wait(epollfd, event_array, MAX_EVENTS, -1);
        if(events < 0)
        {
            printf("epoll failure: %s\n", strerror(errno));
            break;
        }
        else if(events == 0)
        {
            printf("epoll get timeout\n");
            continue;
        }

	/* rv>0 is the active events count */
	for(i=0; i<events; i++)
	{
		/*发生错误或出现挂断*/
	    if ( (event_array[i].events&EPOLLERR) || (event_array[i].events&EPOLLHUP) )
	    {
		printf("epoll_wait get error on fd[%d]: %s\n", event_array[i].data.fd, strerror(errno));
		epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
		close(event_array[i].data.fd);
	    }

	    /* 新客户连接 */
	    if( event_array[i].data.fd == listenfd )
	    { 
		if( (connfd=accept(listenfd, (struct sockaddr *)NULL, NULL)) < 0)
		{
		    printf("accept new client failure: %s\n", strerror(errno));
		    continue;
		}

		event.data.fd = connfd;
		event.events =  EPOLLIN|EPOLLET;//可读取非优先级数据、边沿触发
		//event.events =  EPOLLIN;
		if( epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) < 0 )
		{
		    printf("epoll add client socket failure: %s\n", strerror(errno));
		    close(event_array[i].data.fd);//关闭就绪态文件描述符
		    continue;
		}
	       	printf("epoll add new client socket[%d] ok.\n", connfd);
	    }
	    else /* 已经连接的客户有发数据来 */
	    {
                if( (rv=read(event_array[i].data.fd, buf, sizeof(buf))) <= 0)
		{
                    printf("socket[%d] read failure or get disconncet and will be removed.\n", event_array[i].data.fd);
		    epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);//将相应文件描述符从兴趣列表删除
		    close(event_array[i].data.fd);
		    continue;
		}
		else
		{
		    printf("socket[%d] read get %d bytes data\n", event_array[i].data.fd, rv);
		    
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

                    if( write(event_array[i].data.fd, buf, rv) < 0 )
		    {
		        printf("socket[%d] write failure: %s\n", event_array[i].data.fd, strerror(errno));
		       	epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd, NULL);
		       	close(event_array[i].data.fd);
		    } 
		} 
	    } 
	} /* for(i=0; i<rv; i++) */
    } /* while(1) */

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

/* Set open file description count to max */
void set_socket_rlimit(void)
{
     struct rlimit limit = {0};

     getrlimit(RLIMIT_NOFILE, &limit );
     limit.rlim_cur  = limit.rlim_max;
     setrlimit(RLIMIT_NOFILE, &limit );

     printf("set socket open fd max count to %d\n", limit.rlim_max);
}

