#ifndef __GNET_H__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <sys/select.h>
#include <poll.h>
#include <sys/un.h>
#include <fcntl.h>
#include <net/if.h>//ifreq
#include <sys/ioctl.h>

#define SERVER_PORT (49152)
#define LISTENQ (5)
#define MAX_LINE (256)
#define OPEN_MAX (1024)
#define LOCAL_STRPATH ("localtest")
#define MAX(a,b) ((a)>(b) ? (a) : (b))

void perr_exit(const char* str);
int Accept(int fd, struct sockaddr* sa, socklen_t* salenptr);
int Bind(int fd, const struct sockaddr* sa, socklen_t salen);
int Connect(int fd, const struct sockaddr* sa, socklen_t salen);
int Listen(int fd, int backlog);
int Socket(int family, int type, int protocol);
ssize_t Read(int fd, void* ptr, size_t nbytes);
ssize_t Write(int fd, const void* ptr, size_t nbytes);
int Close(int fd);

ssize_t Readn(int fd, void* vptr, size_t n);
ssize_t Writen(int fd, const void* vptr, size_t n);
//ssize_t my_read(int fd, char* ptr);
ssize_t Readline(int fd, void* vptr, size_t maxlen);

#endif
