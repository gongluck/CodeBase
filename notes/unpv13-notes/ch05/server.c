#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);

    return;
}

int main(int argc, char* argv[])
{
    int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenfd == -1)
    {
        perror("function socket error :");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));
    if(bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("function bind error : ");
        exit(-1);
    }

    if(listen(listenfd, 5) == -1)
    {
        perror("function listen error : ");
        exit(-1);
    }

    struct sigaction act, oact;
    act.sa_handler = sig_chld;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(sigaction(SIGCHLD, &act, &oact) == -1)
    {
        perror("function sigaction error : ");
        exit(-1);
    }

    int connfd;
    struct sockaddr_in clientaddr;
    socklen_t clientlen;
    while(1)
    {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        if(connfd == -1)
        {
            if(errno == EINTR)
                continue;
            perror("function accept error : ");
            exit(-1);
        }

        if(fork() == 0)
        {
            if(close(listenfd) == -1)
            {
                perror("function close error : ");
                exit(-1);
            }
            
            char buf[1024];
            int len;
        AGAIN:
            while((len = read(connfd, buf, 1024)) > 0)
                write(connfd, buf, len);
            if(len < 0 && errno == EINTR)
                goto AGAIN;
            else if(len < 0)
            {
                perror("function read error : ");
                exit(1);
            }
        }
        else
        {
            if(close(connfd) == -1)
            {
                perror("function close error : ");
                exit(-1);
            }
        }
    }
    return 0;
}
