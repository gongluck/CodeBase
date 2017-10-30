#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include "wrap.h"

#define MAXLINE 80
#define PORT 5555

int main(int argc, const char* argv[])
{   
    int lfd, connfd, maxfd;
    int client[FD_SETSIZE];
    struct sockaddr_in serveraddr,connaddr;
    int addrlen;
    char str[INET_ADDRSTRLEN];
    fd_set allset, rset;
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

    for(int i=0; i<FD_SETSIZE; ++i)
    {
        client[i] = -1;
    }
    maxfd = lfd;
    FD_ZERO(&allset);
    FD_SET(lfd,&allset);

    while(1)
    {
        rset = allset;
        nready = select(maxfd+1,&rset,NULL,NULL,NULL);
        if(nready < 0)
        {
            perr_exit("select error");
        }
        if(FD_ISSET(lfd,&rset))
        {
            addrlen = sizeof(connaddr);
            connfd = Accept(lfd,(struct sockaddr *)&connaddr,&addrlen);
            printf("client %s::%d connected.\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port));
            for(int i=0; i<FD_SETSIZE; ++i)
            {
                if(client[i] < 0)
                {
                    client[i] = connfd;
                    FD_SET(connfd,&allset);
                    if(connfd > maxfd)
                        maxfd = connfd;
                    break;
                }
                else if(i == FD_SETSIZE-1)
                {
                    printf("client connect max.\n");
                    printf("client %s::%d disconnected.\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port));
                    Close(connfd);
                }
            }
            if(--nready == 0)
                continue;
        }
        for(int i=0; i<FD_SETSIZE; ++i)
        {
            if(client[i] < 0)
                continue;
            if(FD_ISSET(client[i],&rset))
            {
                memset(buf,0,sizeof(buf));
                rnum = Read(client[i],buf,sizeof(buf));
                if(rnum == 0)
                {
                    printf("client[%d] close.\n",i);
                    Close(client[i]);
                    FD_CLR(client[i],&allset);
                    client[i] = -1;
                }
                else
                {
                    printf("client[%d] says : %s\n",i,buf);
                    Write(client[i],buf,rnum);
                }
                if(--nready == 0)
                    break;
            }
        }

    }

    Close(lfd);
    return 0;
}
