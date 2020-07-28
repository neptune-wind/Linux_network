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
#include <pthread.h>

#include "wrap.h"

#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

struct clie_info
{
    int cfd;
    struct sockaddr_in caddr;
};

void* work(void* arg)
{
    int len, i;
    char buf[BUFSIZ];
    struct clie_info* p = (struct clie_info*)arg;
    while(1)
    {
	len = Read(p->cfd, buf, sizeof(buf));
	for(i = 0; i < len; i++)
        {
	    buf[i] = toupper(buf[i]);
        }
	Write(p->cfd, buf, len);
	Write(STDOUT_FILENO, buf, len); //服务器端将客户端的输入转换后打印到屏幕
    }
    Close(p->cfd);
    return (void*)0;
}

int main(void)
{
    pthread_t tid;
    int sfd, cfd;
    int i = 0, opt;
    char clie_IP[BUFSIZ];

    struct clie_info ci[256];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    sfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));   //enable port muiltiplex

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

	ci[i].cfd = cfd;
	ci[i].caddr = clie_addr;
	pthread_create(&tid, NULL, work, (void*)&ci[i]);
	pthread_detach(tid);
	
	i++;
    }

    Close(sfd);

    return 0;
}
