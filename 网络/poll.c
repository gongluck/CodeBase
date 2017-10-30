#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include "wrap.h"

#define MAXPOLL 1024
#define MAXLINE 80
#define PORT 5555

int main(int argc, const char* argv[])
{   
    int lfd, connfd, maxi;
    struct sockaddr_in serveraddr,connaddr;
    int addrlen;
    char str[INET_ADDRSTRLEN];
    struct pollfd client[MAXPOLL];
    int nready;
    int rnum;
    char buf[MAXLINE];

    lfd = Socket(AF_INET,SOCK_STREAM,0);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    Bind(lfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    Listen(lfd,20);
    printf("waiting for connect ...\n");

    client[0].fd = lfd;
    client[0].events = POLLRDNORM;
    for(int i=1; i<MAXPOLL; ++i)
    {
        client[i].fd = -1;
    }
    maxi = 0;

    while(1)
    {
        nready = poll(client,maxi+1,-1);
        if(nready < 0)
        {
            perr_exit("poll error");
        }
        if(client[0].revents & POLLRDNORM)
        {
            addrlen = sizeof(connaddr);
            connfd = Accept(lfd,(struct sockaddr *)&connaddr,&addrlen);
            printf("client %s::%d connected.\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port));
            for(int i=1; i<MAXPOLL; ++i)
            {
                if(client[i].fd < 0)
                {
                    client[i].fd = connfd;
                    client[i].events = POLLRDNORM;
                    if(i > maxi)
                        maxi = i;
                    break;
                }
                else if(i == MAXPOLL-1)
                {
                    printf("client connect max.\n");
                    printf("client %s::%d disconnected.\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port));
                    Close(connfd);
                }
            }
            if(--nready == 0)
                continue;
        }
        for(int i=1; i<MAXPOLL; ++i)
        {
            if(client[i].fd < 0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR))
            {
                memset(buf,0,sizeof(buf));
                rnum = Read(client[i].fd,buf,sizeof(buf));
                if(rnum == 0)
                {
                    printf("client[%d] close.\n",i);
                    Close(client[i].fd);
                    client[i].fd = -1;
                }
                else
                {
                    printf("client[%d] says : %s\n",i,buf);
                    Write(client[i].fd,buf,rnum);
                }
                if(--nready == 0)
                    break;
            }
        }

    }

    Close(lfd);
    return 0;
}
