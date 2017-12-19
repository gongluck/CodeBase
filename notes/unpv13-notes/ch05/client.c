#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd == -1)
    {
        perror("function socket error : ");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    switch(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr))
    {
    case 0:
        perror("function inet_pton error (input invalid) : ");
        exit(-1);
        break;
    case 1:
        //successed
        break;
    default:
        perror("function inet_pton error : ");
        exit(-1);
        break;
    }
    serveraddr.sin_port = htons(atoi(argv[2]));
    connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    char buf[1024];
    while(fgets(buf, 1024, stdin) != NULL)
    {
        write(sockfd, buf, strlen(buf));
        if(read(sockfd, buf, 1024) == 0)
        {
            perror("server terminated");
            exit(0);
        }
        fputs(buf, stdout);
    }

    return 0;
}
