#include "../Gnet.h"

void do_client(int udpfd, struct sockaddr* pserver_addr, socklen_t server_addr_len)
{
    char buf[MAX_LINE];
    int nread;

    while(fgets(buf, MAX_LINE, stdin) != NULL)
    {
        sendto(udpfd, buf, strlen(buf), 0, pserver_addr, server_addr_len);
        nread = recvfrom(udpfd, buf, MAX_LINE, 0, NULL, NULL);
        fputs(buf, stdout);
    }
}

int main(int argc, const char* argv[])
{
    int udpfd;
    struct sockaddr_in server_addr;

    if(argc < 2)
        perr_exit("usage : client <IPaddress>");

    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    do_client(udpfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    return 0;
}
