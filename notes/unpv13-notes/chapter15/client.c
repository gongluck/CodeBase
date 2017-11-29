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
    struct sockaddr_un server_addr;

    connfd = Socket(AF_LOCAL, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, LOCAL_STRPATH);
    Connect(connfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    do_client(connfd);

    return 0;
}
