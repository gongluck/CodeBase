#include "../Gnet.h"

void do_client(int connfd)
{
    char to[MAX_LINE], fr[MAX_LINE];
    char *toiptr, *tooptr, *friptr, *froptr;
    int val, stdineof, maxfd, n, nwritten;
    fd_set rset, wset;

    val = fcntl(connfd, F_GETFL, 0);
    fcntl(connfd, F_SETFL, val|O_NONBLOCK);
    val = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, val|O_NONBLOCK);
    val = fcntl(STDOUT_FILENO, F_GETFL, 0);
    fcntl(STDOUT_FILENO, F_SETFL, val|O_NONBLOCK);

    toiptr = tooptr = to;
    friptr = froptr = fr;
    stdineof = 0;

    maxfd = MAX(MAX(STDIN_FILENO, STDOUT_FILENO), connfd);
    while(1)
    {
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if(stdineof==0 && toiptr<&to[MAX_LINE])//标准输入->套接字 还有数据要发送&&缓冲区还有容量
            FD_SET(STDIN_FILENO, &rset);
        if(friptr<&fr[MAX_LINE])//套接字->标准输出 缓冲区还有空闲
            FD_SET(connfd, &rset);
        if(tooptr!=toiptr)
            FD_SET(connfd, &wset);
        if(froptr!=friptr)
            FD_SET(STDOUT_FILENO, &wset);

        select(maxfd+1,&rset,&wset,NULL,NULL);

        if(FD_ISSET(STDIN_FILENO, &rset))
        {
            if((n = read(STDIN_FILENO, toiptr, &to[MAX_LINE]-toiptr)) < 0)
            {
                if(errno != EWOULDBLOCK)
                    perr_exit("read error on stdin");
            }
            else if(n == 0)
            {
                fprintf(stderr,"EOF on stdin\n");
                stdineof = 1;
                if(tooptr == toiptr)
                    shutdown(connfd, SHUT_WR);
            }
            else
            {
                fprintf(stderr, "read %d bytes from stdin\n", n);
                toiptr += n;
                FD_SET(connfd, &wset);
            }
        }

        if(FD_ISSET(connfd, &rset))
        {
            if((n = read(connfd, friptr, &fr[MAX_LINE]-friptr)) < 0)
            {
                if(errno != EWOULDBLOCK)
                    perr_exit("read error on socker");
            }
            else if(n == 0)
            {
                fprintf(stderr, "EOF on socket\n");
                if(stdineof)
                    return;
                else
                    perr_exit("server terminated prematurely");
            }
            else
            {
                fprintf(stderr, "read %d bytes from socket\n", n);
                friptr += n;
                FD_SET(STDOUT_FILENO, &wset);
            }
        }

        if(FD_ISSET(STDOUT_FILENO, &wset) && ((n = friptr-froptr) > 0 ))
        {
            if((nwritten = write(STDOUT_FILENO, froptr, n)) < 0)
            {
                if(errno != EWOULDBLOCK)
                    perr_exit("write error to stdout");
            }
            else
            {
                fprintf(stderr, "wrote %d bytes to stdout\n", nwritten);
                froptr += nwritten;
                if(froptr == friptr)
                    froptr = friptr = fr;
            }
        }

        if(FD_ISSET(connfd, &wset) && ((n = toiptr-tooptr) > 0))
        {
            if((nwritten = write(connfd, tooptr, n)) < 0)
            {
                if(errno != EWOULDBLOCK)
                    perr_exit("write error to socket");
            }
            else
            {
                fprintf(stderr, "wrote %d bytes to socket\n", nwritten);
                tooptr += nwritten;
                if(tooptr == toiptr)
                {
                    toiptr = tooptr = to;
                    if(stdineof)
                        shutdown(connfd, SHUT_WR);
                }
            }
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
