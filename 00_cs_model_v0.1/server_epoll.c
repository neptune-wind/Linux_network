#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <strings.h>
#include <sys/epoll.h>
#include <errno.h>
#include "wrap.h"

#define OPEN_MAX	1024
#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

int main(void)
{
    int listenfd, connfd, ret, nready, len;
    int opt, epfd, i, j;
    char buf[BUFSIZ], str[BUFSIZ];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    struct epoll_event evt;
    struct epoll_event ev[10];
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(listenfd, 256);

    epfd = epoll_create(10);
    if(epfd < 0)
    {
	perror("epoll create.");
	exit(1);
    }

    evt.events = EPOLLIN;
    evt.data.fd = listenfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &evt);
    if(ret < 0)
    {
	perror("epoll_ctl fail.");
	exit(1);
    }
    while(1)
    {
	nready = epoll_wait(epfd, ev, 10, -1);
	if(nready != 0) 
	{   
	    //若有新的客户端连接，加入监听中，若有客户端发来数据，则处理
	    for(i = 0; i < nready; i++)
	    {
		if(!ev[i].events & EPOLLIN)
		    continue;
		if(ev[i].data.fd == listenfd)
		{
		    clie_addr_len = sizeof(clie_addr);
		    connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
		    printf("received from %s at PORT %d\n", inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
		        ntohs(clie_addr.sin_port));
		    evt.events = EPOLLIN;
		    evt.data.fd = connfd;
		    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &evt);
		    if(ret < 0)
		    {
		        perror("epoll_ctl fail.");
		        exit(1);
		    }
		}
		else
		{
		    len = Read(ev[i].data.fd, buf, sizeof(buf));
		    if(len < 0)
		    {
			if(errno == ECONNRESET) //客户端断开
			    printf("client fd %d is aborted.\n", ev[i].data.fd);
			else
			{
			    perror("read error");
			    exit(1);
			}
		    }
		    else if(len == 0) //Read返回0，表示socket关闭
		    {
		        printf("client fd %d is closed.\n", ev[i].data.fd);
		        epoll_ctl(epfd, EPOLL_CTL_DEL, ev[i].data.fd, NULL);
			Close(ev[i].data.fd);
		    }
		    else if(len > 0)
		    {
		        for(j = 0; j < len; j++)
			    buf[j] = toupper(buf[j]);
			Write(ev[i].data.fd, buf, len);
			Write(STDOUT_FILENO, buf, len);
		    }		
		}
	    }  
	}		
    }
    Close(listenfd);

    return 0;
}
