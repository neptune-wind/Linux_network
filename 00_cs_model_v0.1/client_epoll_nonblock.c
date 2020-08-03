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

#define SERV_PORT   8888
#define SERV_IP     "127.0.0.1"
#define MAXLEN	    10

int main(void)
{
    int cfd;
    int i;
    char buf[MAXLEN];
    char ch = 'a';
    struct sockaddr_in serv_addr;


    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
    
    Connect(cfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while(1)
    {
        for(i = 0; i < MAXLEN/2; i++)
        {
            buf[i] = ch;
        }
        buf[i - 1] = '\n';
        ch++;
        for(; i < MAXLEN; i++)
        {
            buf[i] = ch;
        }
        buf[i - 1] = '\n';
        ch++;
        write(cfd, buf, MAXLEN);
	sleep(2);
    }

    Close(cfd);

    return 0;
}

