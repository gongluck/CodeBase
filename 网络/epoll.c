#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/epoll.h>
#include "wrap.h"

#define MAXEPOLL 1
#define MAXLINE 80
#define PORT 5555

int main(int argc, const char* argv[])
{   
    int lfd, connfd, maxi, efd, res;
    struct sockaddr_in serveraddr,connaddr;
    int addrlen;
    char str[INET_ADDRSTRLEN];
    int client[MAXEPOLL];
    struct epoll_event tep, ep[MAXEPOLL];
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

    for(int i=0; i<MAXEPOLL; ++i)
    {
        client[i] = -1;
    }

    efd = epoll_create(MAXEPOLL);
    if(efd == -1)
    {
        perr_exit("epoll_create");
    }
    tep.events = EPOLLIN;
    tep.data.fd = lfd;
    res = epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &tep);
    if(res == -1)
        perr_exit("epoll_ctl");

    while(1)
    {
        nready = epoll_wait(efd, ep, MAXEPOLL, -1);
        if(nready == -1)
        {
            perr_exit("epoll_wait error");
        }
        
        for(int i=0; i<nready; ++i)
        {
            if(!(ep[i].events & EPOLLIN))
                continue;
            if(ep[i].data.fd == lfd)
            {
                addrlen = sizeof(connaddr);
                connfd = Accept(lfd,(struct sockaddr *)&connaddr,&addrlen);
                printf("client %s::%d connected.\n",
                       inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),
                       ntohs(connaddr.sin_port));
                for(int i=0; i<MAXEPOLL; ++i)
                {
                    if(client[i] == -1)
                    {
                        client[i] = connfd;
                        tep.events = EPOLLIN;
                        tep.data.fd = connfd;
                        epoll_ctl(efd,EPOLL_CTL_ADD,connfd,&tep);
                        break;
                    }
                    if(i == MAXEPOLL-1)
                    {
                        printf("EPOLL MAX.\n");
                        Close(connfd);
                    }
                }
            }
            else
            {
                memset(buf,0,sizeof(buf));
                rnum = Read(ep[i].data.fd,buf,sizeof(buf));
                if(rnum == 0)
                {
                    epoll_ctl(efd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);
                    Close(ep[i].data.fd);
                    client[i] = -1;
                    printf("client[%d] clsoed.\n",i);
                }
                else
                {
                    printf("client[%d] : %s\n.",i,buf);
                    Write(ep[i].data.fd,buf,rnum);
                }

            }
        }
    }

    Close(lfd);
    Close(efd);
    return 0;
}
