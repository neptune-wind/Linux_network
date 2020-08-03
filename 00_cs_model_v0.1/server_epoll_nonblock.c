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

#define OPEN_MAX    1024
#define SERV_PORT   8888
#define SERV_IP	    "127.0.0.1"
#define MAXLEN	    10

int main(void)
{
    int listenfd, connfd, ret, len;
    int opt, epfd;
    char buf[BUFSIZ];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;
    struct epoll_event evl;
    struct epoll_event evc;
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(listenfd, 256);

    connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);

    epfd = epoll_create(10);
    if(epfd < 0)
    {
	perror("epoll create.");
	exit(1);
    }

    evl.events = EPOLLIN | EPOLLET;
    evl.data.fd = connfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &evl);
    if(ret < 0)
    {
	perror("epoll_ctl fail.");
	exit(1);
    }
    while(1)
    {
	epoll_wait(epfd, &evc, 1, -1);
	if(evc.events == EPOLLIN && evc.data.fd == connfd)  
	{
	    while((len = Read(connfd, buf, MAXLEN/2)) > 0)
	    {
		write(STDOUT_FILENO, buf, len);
	    }
	}
    }
    Close(listenfd);
    Close(connfd);
    Close(epfd);

    return 0;
}

