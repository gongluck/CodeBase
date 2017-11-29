#include "../Gnet.h"

int fd;

void sig_io(int sig)
{
    char buf[MAX_LINE];
    int nread;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    printf("in sig_io\n");
    nread = recvfrom(fd, buf, MAX_LINE, 0,(struct sockaddr*)&client_addr, &client_addr_len);
    if(nread > 0)
        sendto(fd, buf, nread, 0, (struct sockaddr*)&client_addr, client_addr_len);
    else
        printf("nread < 0\n");

    printf("out sig_io\n");
}

void do_server(int udpfd)
{
    const int on = 1;
    sigset_t old, new;

    fd = udpfd;

    signal(SIGIO, sig_io);//注册信号处理函数
    if(fcntl(udpfd, F_SETOWN, getpid()) < 0)
        perr_exit("fcntl F_SETOWN error.27");//设置套接字属主
    if(ioctl(udpfd, FIONBIO, &on) < 0)
        perr_exit("ioctl FIONBIO error.29*");//非阻塞IO
    if(ioctl(udpfd, FIOASYNC, &on) < 0)
        perr_exit("ioctl FIOASYNC error.31");//信号驱动IO

    sigemptyset(&new);
    sigaddset(&new, SIGIO);
    sigprocmask(SIG_UNBLOCK, &new, &old);
}

int main(int argc, const char* argv[])
{
    int udpfd;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    udpfd = Socket(AF_INET, SOCK_DGRAM, 0);
    Bind(udpfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    printf("waiting for connecting.\n");

    do_server(udpfd);
    while(1)
        ;

    return 0;
}
