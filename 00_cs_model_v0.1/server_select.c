#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <strings.h>
#include "wrap.h"

#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

int main(void)
{
    int listenfd, connfd, nready;
    int len, i, j, maxfd, maxi;
    char buf[BUFSIZ], str[BUFSIZ];
    int client[FD_SETSIZE];
    fd_set rset, allset;
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(listenfd, 256);
    
    for(i = 0; i < FD_SETSIZE; i++)
	client[i] = -1;	//client集合与allset同步，保存与客户端通信的fd，用于接收数据（不包含建立连接的listenfd）
    
    maxi = -1;
    maxfd = listenfd;
    
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1)
    {	
	rset = allset; //allset为设置server监听的fd集合，rset为select传出的集合，包含已经发生事件的fd集合
	nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    
	if(nready < 0)
	{
	    perror("select error:");
	    exit(1);
	}
	if(FD_ISSET(listenfd, &rset))
	{
	    clie_addr_len = sizeof(clie_addr);
	    connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len);
	    printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
                    ntohs(clie_addr.sin_port));
	    
	    for(i = 0; i < FD_SETSIZE; i++)
	    {
		if(client[i] == -1)
		{
		    client[i] = connfd;
		    break;
		}
	    }
	    
	    if (i == FD_SETSIZE) {                              /* 达到select能监控的文件个数上限 1024 */
                fputs("too many clients\n", stderr);
                exit(1);
            }
	   
	    FD_SET(connfd, &allset);
 
	    if(connfd > maxfd)
		maxfd = connfd;

	    if(i > maxi)
		maxi = i;
	    
	    if(--nready == 0)	
		continue; 
	}

	for(i = 0; i <= maxi; i++)
	{
	    if(client[i] == -1)
		continue;
	    if(FD_ISSET(client[i], &rset))
	    {
		len = Read(client[i], buf, sizeof(buf));
		if(len == 0) //Read返回0，表示socket关闭
		{
		    Close(client[i]);
		    FD_CLR(client[i], &allset);
		    client[i] = -1;
		}
		else if(len > 0)
		{
		    for(j = 0; j < len; j++)
		    {
			buf[j] = toupper(buf[j]);	
		    }
		    Write(client[i], buf, len);
		}
		if(--nready == 0)
		    break;
	    }
	}
    }
    Close(listenfd);

    return 0;
}
