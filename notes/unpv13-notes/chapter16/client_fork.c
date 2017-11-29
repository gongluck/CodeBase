#include "../Gnet.h"

void do_client(int connfd)
{
    char buf[MAX_LINE];
    pid_t pid;

    if((pid = fork()) == 0)
    {
        while(Readline(connfd, buf, MAX_LINE) > 0)
            fputs(buf, stdout);

        kill(getppid(), SIGTERM);
        exit(0);
    }
    else
    {
        while(fgets(buf, MAX_LINE, stdin) != NULL)
            Write(connfd, buf, strlen(buf));

        shutdown(connfd, SHUT_WR);
        pause();
    }
}

int main(int argc, const char* argv[])
{
    int connfd;
    struct sockaddr_in server_addr;

    if(argc < 2)
        perr_exit("usage : client <IPaddress>");

    connfd = Socket(AF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    Connect(connfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    do_client(connfd);

    return 0;
}
