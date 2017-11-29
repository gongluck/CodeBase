#include "../Gnet.h"

int main(int argc, const char* argv[])
{
    int lfd, connfd;
    int maxi, findi;
    int nready;
    int nread;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;    
    char buf[MAX_LINE];
    struct pollfd client[OPEN_MAX];

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(lfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    Listen(lfd, LISTENQ);

    client[0].fd = lfd;
    client[0].events = POLLRDNORM;
    for(int i=1; i<OPEN_MAX; ++i)
        client[i].fd = -1;
    maxi = 0;

    printf("waiting for connecting.\n");

    while(1)
    {
        nready = poll(client, maxi+1, -1);
        if(nready < 0)
            perr_exit("poll error.");

        if(client[0].revents & POLLRDNORM)
        {
            client_addr_len = sizeof(client_addr);
            connfd = Accept(lfd, (struct sockaddr*)&client_addr, &client_addr_len);
            for(findi=1; findi<OPEN_MAX; ++findi)
            {
                if(client[findi].fd < 0)
                {
                    client[findi].fd = connfd;
                    break;
                }
            }
            if(findi == OPEN_MAX)
                Close(connfd);
            else
            {
                if(findi > maxi)
                    maxi = findi;
                client[findi].events = POLLRDNORM;

                if(--nready <= 0)
                    continue;
            }
        }

        for(int i =1; i <= maxi; ++i)
        {
            if(client[i].fd < 0)
                continue;
            if(client[i].revents & (POLLRDNORM | POLLERR))
            {
                if((nread = Read(client[i].fd, buf, MAX_LINE)) < 0)
                {
                    client[i].fd = -1;
                    printf("read error.");
                }
                else if(nread == 0)
                {
                    Close(client[i].fd);
                    client[i].fd = -1;
                }
                else
                    Write(client[i].fd, buf, nread);

                if(--nready <= 0)
                    break;
            }
        }
    }

    return 0;
}
