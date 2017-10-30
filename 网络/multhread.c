#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include "wrap.h"

#define MAXARGS 1
#define MAXLINE 80
#define PORT 5555

struct Args
{
    int fd;
    struct sockaddr_in addr;
    int used;
}args[MAXARGS];

void* work_thread(void* arg)
{
    char buf[MAXLINE],str[INET_ADDRSTRLEN];
    int rnum;
    struct Args* args = (struct Args *)arg;

    pthread_detach(pthread_self());
    printf("threadid : %d\n",(int)pthread_self());
    while(1)
    {
        memset(buf,0,MAXLINE);
        rnum = Read(args->fd,buf,sizeof(buf));
        if(rnum == 0)
        {  
           printf("client %s::%d close\n",inet_ntop(AF_INET,&args->addr.sin_addr,str,sizeof(str)),ntohs(args->addr.sin_port));
           break;
        }
        printf("client %s::%d says : %s\n",inet_ntop(AF_INET,&args->addr.sin_addr,str,sizeof(str)),ntohs(args->addr.sin_port),buf);
        Write(args->fd,buf,rnum);
    }
    Close(args->fd);
    args->used = 0;

    return NULL;
}

int main(int argc, const char* argv[])
{   
    int lfd, connfd;
    struct sockaddr_in serveraddr,connaddr;
    int addrlen;
    int i;
    pthread_t tid;

    lfd = Socket(AF_INET,SOCK_STREAM,0);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    Bind(lfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    Listen(lfd,20);
    printf("waiting for connect ...\n");
    memset(args,0,sizeof(args));

    while(1)
    {
        addrlen = sizeof(connaddr);
        connfd = Accept(lfd,(struct sockaddr *)&connaddr,&addrlen);
        for(i=0; i<MAXARGS; ++i)
        {
            if(args[i].used == 0)
            {
                args[i].fd = connfd;
                args[i].addr = connaddr;
                args[i].used = 1;
                pthread_create(&tid,NULL,work_thread,&args[i]);
                break;
            }
        }
        if(i == MAXARGS)
        {
            Close(connfd);
            printf("Max threads !\n");
        }
    }

    Close(lfd);
    return 0;
}
