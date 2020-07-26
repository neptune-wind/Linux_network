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

#define SERV_PORT   6666
#define SERV_IP     "127.0.0.1"

int main(void)
{
    int cfd;
    int n;
    char buf[BUFSIZ];
    struct sockaddr_in serv_addr;


    cfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
    
    Connect(cfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while(1)
    {
        fgets(buf, sizeof(buf), stdin);
        Write(cfd, buf, strlen(buf));
        n = Read(cfd, buf, sizeof(buf));
        Write(STDOUT_FILENO, buf, n);
    }

    Close(cfd);

    return 0;
}
