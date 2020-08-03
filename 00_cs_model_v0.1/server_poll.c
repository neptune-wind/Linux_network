#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <strings.h>
#include <poll.h>
#include <errno.h>
#include "wrap.h"

#define OPEN_MAX	1024
#define SERV_PORT	8888
#define SERV_IP    	"127.0.0.1"

int main(void)
{
    int listenfd, connfd, nready;
    int len, i, j, maxi, opt;
    char buf[BUFSIZ], str[BUFSIZ];
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in serv_addr, clie_addr;
    socklen_t clie_addr_len;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);

    Bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    Listen(listenfd, 256);
    
    for(i = 0; i < OPEN_MAX; i++)
	client[i].fd = -1;
    
    maxi = 0;	//maxi表示需监听读数据的客户端个数

    client[0].fd = listenfd;	//listenfd用于监听新的客户端连接请求
    client[0].events = POLLRDNORM;
    
    while(1)
    {	
	nready = poll(client, maxi + 1, -1); //maxi+1的1表示listenfd   
	if(nready < 0)
	{
	    perror("poll error:");
	    exit(1);
	}

	if(client[0].revents & POLLRDNORM)	//为true表示有新的客户端连接请求
	{
	    clie_addr_len = sizeof(clie_addr);
	    connfd = Accept(listenfd, (struct sockaddr*)&clie_addr, &clie_addr_len); //不会阻塞
	    printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
                    ntohs(clie_addr.sin_port));
	    
	    for(i = 1; i < OPEN_MAX; i++)
	    {
		if(client[i].fd == -1)
		{
		    client[i].fd = connfd;	    
		    break;
		}
	    }
	    
	    if (i == OPEN_MAX) {                 /* 达到poll能监控的文件个数上限 1024 */
                fputs("too many clients\n", stderr);
                exit(1);
            }

	    if(i > maxi)
		maxi = i;
	    
	    client[i].events = POLLRDNORM;

	    if(--nready <= 0)	
		continue; 
	}

	for(i = 1; i <= maxi; i++)
	{
	    if(client[i].fd == -1)
		continue;
	    if(client[i].revents & (POLLRDNORM | POLLERR))
	    {
		len = Read(client[i].fd, buf, sizeof(buf));
		if(len < 0)
		{
		    if(errno == ECONNRESET) //客户端断开
		    {
			printf("client fd %d is aborted.\n", client[i].fd);
			Close(client[i].fd);
			client[i].fd = -1; //置-1表示不再监听	
		    }
		    else
		    {
			perror("read error");
			exit(1);
		    }
		}
		else if(len == 0) //Read返回0，表示socket关闭
		{
		    printf("client fd %d is closed.\n", client[i].fd);
		    Close(client[i].fd);
		    client[i].fd = -1;
		}
		else if(len > 0)
		{
		    for(j = 0; j < len; j++)
		    {
			buf[j] = toupper(buf[j]);	
		    }
		    Write(client[i].fd, buf, len);
		}
		if(--nready <= 0)
		    break;
	    }
	}
    }
    Close(listenfd);

    return 0;
}
