#include "../Gnet.h"

void do_client(int connfd)
{
    char buf[MAX_LINE];

    while(fgets(buf, MAX_LINE, stdin) != NULL)
    {
        Write(connfd, buf, strlen(buf));
        memset(buf, 0, MAX_LINE);
        if(Read(connfd, buf, MAX_LINE) == 0)
            perr_exit("server terminated.\n");
        fputs(buf, stdout);
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
