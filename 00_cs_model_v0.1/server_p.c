#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "wrap.h"

#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

void wait_child(int signo)
{
    while(waitpid(0, NULL, WNOHANG) > 0);   //考虑多个子进程同时结束的情况
    return;
}

int main(void)
{
    pid_t pid;
    int sfd, cfd;
    int len, i, opt;
    char buf[BUFSIZ], clie_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    sfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(sfd, 3);
    
    while(1)
    {
	cfd = Accept(sfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
	
	printf("client IP: %s port: %d\n", 
	inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP)),
	    ntohs(clie_addr.sin_port));
	
	pid = fork();
	if(pid < 0)
	{
	    perror("fork error.");
	    exit(1);
	}
	else if(pid == 0)	//子进程
	{
	    break;
	}
	else	//父进程
	{
	    close(cfd);
	    signal(SIGCHLD, wait_child);
	}
    }
    if(pid == 0)
    {
	while(1)
	{
	    close(sfd);
	    len = Read(cfd, buf, sizeof(buf));
	    for(i = 0; i < len; i++)
	    {
		buf[i] = toupper(buf[i]);
	    }
	    Write(cfd, buf, len);
	    Write(STDOUT_FILENO, buf, len); //服务器端将客户端的输入转换后打印到屏幕
	}
	close(cfd);
    }

    Close(sfd);

    return 0;
}
