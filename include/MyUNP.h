#ifndef MY_UNPXFE34_H_
#define MY_UNPXFE34_H_

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


#define BUFSIZE 4096
#define MAXLINE 4096

#define LISTENQ 1024 //最大客户排队连接数



/**
 * 错误例程
 */
void err_ret(const char* fmt, ...);
void err_sys(const char* fmt, ...);
void err_dump(const char* fmt, ...);
void err_msg(const char* fmt, ...);
void err_quit(const char* fmt, ...);
void debug(void);



#endif
