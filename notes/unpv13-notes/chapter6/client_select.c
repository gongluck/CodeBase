#include "../Gnet.h"

void do_client(int connfd)
{
    char buf[MAX_LINE];
    int maxfd;
    fd_set rset;
    int n;
    int closewr;

    closewr = 0;
    while(1)
    {
        FD_ZERO(&rset);
        if(closewr == 0)
            FD_SET(STDIN_FILENO, &rset);
        FD_SET(connfd, &rset);
        maxfd = STDIN_FILENO > connfd ? STDIN_FILENO : connfd;
        if(select(maxfd+1, &rset, NULL, NULL, NULL) < 0)
            perr_exit("select error.");
        if(FD_ISSET(connfd, &rset))
        {
            if((n = Read(connfd, buf, MAX_LINE)) == 0)
            {
                if(closewr == 1)
                    return;
                else
                    perr_exit("server terminated.");
            }
            Write(STDOUT_FILENO, buf, n);
        }
        if(FD_ISSET(STDIN_FILENO, &rset))
        {
            if((n = Read(STDIN_FILENO, buf, MAX_LINE)) == 0)
            {
                shutdown(connfd, SHUT_WR);
                FD_CLR(STDIN_FILENO, &rset);
                closewr = 1;
            }
            else
                Write(connfd, buf, n);
        }
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
