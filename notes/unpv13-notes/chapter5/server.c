#include "../Gnet.h"

void do_server(int connfd)
{
    ssize_t nread;
    char buf[MAX_LINE];

    while((nread = Read(connfd, buf, MAX_LINE)) > 0)
        Write(connfd, buf, nread);
}

void sig_child(int signo)
{
    pid_t pid;
    int stat;

    printf("in sig_child.\n");
    while((pid = waitpid(-1,&stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    printf("out sig_child.\n");
}

int main(int argc, const char* argv[])
{
    int lfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;
    pid_t child_id;

    struct sigaction sigaction_set, sigaction_get;
    sigaction_set.sa_handler = sig_child;
    sigemptyset(&sigaction_set.sa_mask);
    sigaction_set.sa_flags = 0;
    if(sigaction(SIGCHLD, &sigaction_set, &sigaction_get) <0)
        printf("sigaction(SIGCHLD) error!\n");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    lfd = Socket(AF_INET, SOCK_STREAM, 0);
    Bind(lfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    Listen(lfd, LISTENQ);
    printf("waiting for connecting.\n");

    memset(&client_addr, 0, sizeof(client_addr));
    while(1)
    {
        client_addr_len = sizeof(client_addr_len);
        connfd = Accept(lfd, (struct sockaddr*)&client_addr, &client_addr_len);

        if((child_id = fork()) == 0)//子进程
        {
            Close(lfd);
            do_server(connfd);
            Close(connfd);
            exit(0);
        }
        else//父进程
        {
            printf("child %d connected\n", child_id);
            Close(connfd);
        }
    }

    return 0;
}
