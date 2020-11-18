#ifndef MY_UNPXFE34_H_
#define MY_UNPXFE34_H_

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


#define BUFSIZE 4096
#define MAXLINE 4096

#define LISTENQ 1024 //最大客户排队连接数



/**
 * 错误例程 */
void err_ret(const char* fmt, ...);
void err_sys(const char* fmt, ...);
void err_dump(const char* fmt, ...);
void err_msg(const char* fmt, ...);
void err_quit(const char* fmt, ...);
void debug(void);


/**
 * 网络地址辅助函数  */
char* sock_ntop(const struct sockaddr* sockaddr, socklen_t addrlen);

//下面的暂不实现
int sock_bind_wild(int sockfd, int family);
int sock_cmp_addr(const struct sockaddr* sockaddr1, 
				const struct sockaddr* sockaddr2, socklen_t addrlen);
int sock_cmp_port(const struct sockaddr* sockaddr1,
				const struct sockaddr* sockaddr2, socklen_t addrlen);
int sock_get_port(const struct sockaddr* sockaddr, socklen_t addrlen);
void sock_set_addr(struct sockaddr* sockaddr, socklen_t addrlen, void* ptr);
void sock_set_port(struct sockaddr* sockaddr, socklen_t addrlen, in_port_t port);
void sock_set_wild(struct sockaddr* sockaddr, socklen_t addrlen);


/**
 * 流式套接字指定字节的读写 */
ssize_t readn(int fd, void* buf, size_t nbytes);
ssize_t writen(int fd, const void* buf, size_t nbytes);
ssize_t readline(int fd, void* buf, size_t maxlen);
ssize_t readline1(int fd, void* buf, size_t maxlen);
ssize_t readlinebuf(void** cptrptr);


/**
 * 时间状态函数 */
const char* currTime(const char* fmt);
char* currTime_r(char* buf, size_t maxlen, const char* fmt);


/**
 * 自定义信号处理程序安装 */
typedef void Sigfunc(int);
Sigfunc* mysignal(int signo, Sigfunc* func);


/**
 * 回射服务器辅助函数 */
void str_echo(int sockfd);
void sum_echo1(int sockfd);
void sum_echo2(int sockfd);
void str_cli(int sockfd, FILE* fp);
void sum_cli2(int sockfd, FILE* fp);



#endif //!MY_UNPXFE34_H_