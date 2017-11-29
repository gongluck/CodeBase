#include "../Gnet.h"

int main(int argc, const char* argv[])
{
    int lfd, connfd;
    int maxfd;
    int client[FD_SETSIZE], maxi, findi;
    int nready;
    int nread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    fd_set allset, rset;
    char buf[MAX_LINE];

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(lfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    Listen(lfd, LISTENQ);
    maxfd = lfd;
    maxi = -1;
    for(int i=0; i<FD_SETSIZE; ++i)
        client[i] = -1;
    FD_ZERO(&allset);
    FD_SET(lfd, &allset);
    printf("waiting for connecting.\n");

    while(1)
    {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(nready < 0)
            perr_exit("select error.");
        if(FD_ISSET(lfd, &rset))
        {
            client_addr_len = sizeof(client_addr);
            connfd = Accept(lfd, (struct sockaddr*)&client_addr, &client_addr_len);
            for(findi = 0; findi < FD_SETSIZE; ++findi)
            {
                if(client[findi] < 0)
                {
                    client[findi] = connfd;
                    break;
                }
            }
            if(findi == FD_SETSIZE)
                Close(connfd);
            else
            {
                FD_SET(connfd, &allset);
                if(connfd > maxfd)
                    maxfd = connfd;
                if(findi > maxi)
                    maxi = findi;
                if(--nready <= 0)
                    continue;
            }
        }
        for(int i=0; i <= maxi; ++i)
        {        
            if(client[i] < 0)
                continue;
            if(FD_ISSET(client[i], &rset))
            {
                if((nread = Read(client[i], buf, MAX_LINE)) == 0)
                {
                    Close(client[i]);
                    FD_CLR(client[i], &allset);
                    client[i] = -1;
                }
                else
                    Write(client[i], buf, nread);

                if(--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}
