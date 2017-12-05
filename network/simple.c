#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

#define MAXLINE 80
#define PORT 5555

void perr_exit(const char* str)
{
    perror(str);
    exit(1);
}

int main(int argc, const char* argv[])
{
    int res;
    int lfd,connfd;
    struct sockaddr_in servaddr, connaddr;
    int connaddrlen;
    int rnum;
    char buf[MAXLINE],str[INET_ADDRSTRLEN];

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd < 0)
        perr_exit("socket error : ");

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    res = bind(lfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    if(res < 0)
        perr_exit("bind error : ");

    res = listen(lfd,20);
    if(res < 0)
        perr_exit("listen error : ");

    printf("waiting for client connect ...\n");
    while(1)
    {
        connaddrlen = sizeof(connaddr);
        connfd = accept(lfd,(struct sockaddr *)&connaddr, &connaddrlen);
        rnum = read(connfd,buf,MAXLINE);
        printf("%s::%d said : %s", 
               inet_ntop(AF_INET,&connaddr.sin_addr,str,sizeof(str)),
               ntohs(connaddr.sin_port),
               buf);
        write(connfd,buf,rnum);
        close(connfd);
    }


    close(lfd);

    return 0;
}
