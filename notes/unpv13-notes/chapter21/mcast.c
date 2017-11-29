#include "../Gnet.h"

int main(int argc, const char* argv[])
{
    int sendfd, recvfd;
    const int on = 1;
    //const int off = 0;
    struct sockaddr_in sockaddr;
    socklen_t addrlen;
    struct ip_mreq mreq;
    int nrecv;
    char buf[MAX_LINE];
    char addrstr[INET_ADDRSTRLEN];

    if(argc != 2)
        perr_exit("usage: mcast <IP-multicast-address>");

    sendfd = Socket(AF_INET, SOCK_DGRAM, 0);
    recvfd = Socket(AF_INET, SOCK_DGRAM, 0);

    if(setsockopt(recvfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        perr_exit("setsockopt error. 22");

    sockaddr.sin_family = AF_INET;
    //sockaddr.sin_port = htons(SERVER_PORT);
    if(inet_pton(AF_INET, argv[1], &sockaddr.sin_addr) < 0)
        perr_exit("inet_pton error.27");
    addrlen = sizeof(sockaddr);
    Bind(recvfd, (struct sockaddr*)&sockaddr, addrlen);//接收套接字绑定多播地址

    mreq.imr_multiaddr = sockaddr.sin_addr;
    //inet_pton(AF_INET, "192.168.17.129", &mreq.imr_interface.s_addr);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if(setsockopt(recvfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) <0)//接收套接字加入多播组
        perr_exit("setsockopt error.35");
    //setsockopt(sendfd, IPPROTO_IP, IP_MULTICAST_LOOP, &off, sizeof(off));
    if(fork() > 0)
    {
        while(1)
        {
            nrecv = recvfrom(recvfd, buf, MAX_LINE, 0, (struct sockaddr*)&sockaddr, &addrlen); 
            printf("from %s: %s\n", 
                   inet_ntop(AF_INET, &sockaddr, addrstr, sizeof(addrstr)), buf);
        }
    }
    else
    {
        while(1)
        {
            sendto(sendfd, "test data", strlen("test data")+1, 0, (struct sockaddr*)&sockaddr, addrlen);
            sleep(2);
        }
    }

    return 0;
}
