#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "wrap.h"

#define MAXLINE 80
#define PORT 5555

void sigchild(int arg)
{
    while(waitpid(0,NULL,WNOHANG) > 0)
        ;
}

int main(int argc, const char* argv[])
{   
    int lfd, connfd;
    struct sockaddr_in serveraddr,connaddr;
    int addrlen, rnum;
    char buf[MAXLINE],str[INET_ADDRSTRLEN];
    pid_t pid;

    struct sigaction sigact;
    sigact.sa_handler = sigchild;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGCHLD,&sigact,NULL);

    lfd = Socket(AF_INET,SOCK_STREAM,0);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(PORT);
    Bind(lfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
    Listen(lfd,20);
    printf("waiting for connect ...\n");

    while(1)
    {
        addrlen = sizeof(connaddr);
        connfd = Accept(lfd,(struct sockaddr *)&connaddr,&addrlen);
        pid = fork();
        if(pid == 0)
        {
            Close(lfd);
            while(1)
            {
                rnum = Read(connfd, buf, sizeof(buf));
                if(rnum == 0)
                {
                    printf("Client %s::%d closed .\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port));
                    Close(connfd);
                    return 0;
                }
                else
                {
                    printf("client %s::%d says : %s\n",inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),ntohs(connaddr.sin_port),buf);
                    Write(connfd,buf,rnum);
                }
            }
        }
        else if(pid > 0)
        {
            Close(connfd);
        }
        else
        {
            perr_exit("fork");
        }
    }

    Close(lfd);
    return 0;
}
