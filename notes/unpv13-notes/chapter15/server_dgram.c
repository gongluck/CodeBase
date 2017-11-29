#include "../Gnet.h"

void do_server(int udpfd)
{
    ssize_t nread;
    char buf[MAX_LINE];
    struct sockaddr_un client_addr;
    socklen_t client_addr_len;

    while(1)
    {
        client_addr_len = sizeof(client_addr);
        nread = recvfrom(udpfd, buf, MAX_LINE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        sendto(udpfd, buf, nread, 0, (struct sockaddr*)&client_addr, client_addr_len);
    }
}

int main(int argc, const char* argv[])
{
    int udpfd;
    struct sockaddr_un server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, LOCAL_STRPATH);

    udpfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
    unlink(LOCAL_STRPATH);
    Bind(udpfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("waiting for connecting.\n");

    do_server(udpfd);

    return 0;
}
