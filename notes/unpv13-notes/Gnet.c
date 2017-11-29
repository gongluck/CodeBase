#include "Gnet.h"

void perr_exit(const char* str)
{
    perror(str);
    exit(-1);
}

int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr)
{
   int afd;

AGAIN:
   if((afd = accept(fd, sa, salenptr)) <0 )
   {
       if((errno == ECONNABORTED) || (errno == EINTR))
           goto AGAIN;
       else
           perr_exit("accept error");
   }

   return afd;
}

int Bind(int fd, const struct sockaddr* sa, socklen_t salen)
{
    int bfd;

    if((bfd = bind(fd, sa, salen)) < 0)
        perr_exit("bind error");

    return bfd;
}

int Connect(int fd, const struct sockaddr* sa, socklen_t salen)
{
    int cfd;

    if((cfd = connect(fd, sa, salen)) < 0)
        perr_exit("connect error");

    return cfd;
}

int Listen(int fd, int backlog)
{
    int res;

    if((res = listen(fd, backlog)) < 0)
        perr_exit("listen error");

    return res;
}

int Socket(int family, int type, int protocol)
{
    int sfd;

    if((sfd = socket(family, type, protocol)) < 0)
        perr_exit("socket error");

    return sfd;
}

ssize_t Read(int fd, void* ptr, size_t nbytes)
{
    ssize_t n;

AGAIN:
    if((n = read(fd, ptr, nbytes)) == -1)
    {
        if(errno == EINTR)
            goto AGAIN;
        else if(errno == ECONNRESET)
            Close(fd);
        else
            return -1;
    }

    return n;
}

ssize_t Write(int fd, const void* ptr, size_t nbytes)
{
    ssize_t n;

AGAIN:
    if((n = write(fd, ptr, nbytes)) == -1)
    {
        if(errno == EINTR)
            goto AGAIN;
        else
            return -1;
    }

    return n;
}

int Close(int fd)
{
    int res;

    if((res = close(fd)) == -1)
        perr_exit("close error");

    return res;
}

ssize_t Readn(int fd, void* vptr, size_t n)
{
    ssize_t nleft;
    ssize_t nread;
    char* ptr;

    ptr = (char*)vptr;
    nleft = n;

    while(nleft > 0)
    {
        if((nread = read(fd, ptr, nleft)) < 0)
        {
            if(errno == EINTR)
                nread = 0;
            else
                return -1;
        }
        else if(nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
    }       

    return n-nleft;
}

ssize_t Writen(int fd, const void* vptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char* ptr;

    ptr = (const char*)vptr;
    nleft = n;

    while(nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0 && errno == EINTR)
                nwritten = 0;
            else
                return -1;
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return n - nleft;
}

static ssize_t my_read(int fd, char* ptr)
{
    static int read_cnt;
    static char* read_ptr;
    static char read_buf[MAX_LINE];

    if(read_cnt <= 0)
    {
AGAIN:
        if((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
        {
            if(errno == EINTR)
                goto AGAIN;
            else
                return -1;
        }
        else if(read_cnt == 0)
            return 0;

        read_ptr = read_buf;
    }

    --read_cnt;
    *ptr = *read_ptr++;

    return 1;
}

ssize_t Readline(int fd, void* vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = (char*)vptr;

    for(n = 1; (size_t)n < maxlen; ++n)
    {
        if((rc = my_read(fd, &c)) == 1)
        {
            *ptr++ = c;
            if(c == '\n')
                break;
        }
        else if(rc == 0)
        {
            *ptr = '\0';
            return n-1;
        }
        else
            return -1;
    }

    *ptr = '\0';

    return n;
}
