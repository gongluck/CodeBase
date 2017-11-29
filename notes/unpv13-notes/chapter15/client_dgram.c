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
    int sockfd;
    struct sockaddr_un server_addr, client_addr;

    sockfd = Socket(AF_LOCAL, SOCK_DGRAM, 0);
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_LOCAL;
    strcpy(client_addr.sun_path, tmpnam(NULL));
    Bind(sockfd, (struct sockaddr*)&client_addr, sizeof(client_addr));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strcpy(server_addr.sun_path, LOCAL_STRPATH);

    do_client(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    return 0;
}
