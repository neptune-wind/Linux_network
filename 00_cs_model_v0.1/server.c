#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "wrap.h"

#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

int main(void)
{
    pid_t pid;
    int sfd, cfd;
    int len, i;
    char buf[BUFSIZ], clie_IP[BUFSIZ];

    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    sfd = Socket(AF_INET, SOCK_STREAM, 0);

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
	}
	close(cfd);
    }

    Close(sfd);

    return 0;
}
