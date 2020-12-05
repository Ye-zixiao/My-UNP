#include "MyUNP.h"


/* 从客户进程中读取文本行并简单回射 */
void str_echo(int sockfd) {
	ssize_t nread;
	char buf[MAXLINE];

again:
	while ((nread = readline(sockfd, buf, MAXLINE)) > 0) {
		if (write(sockfd, buf, nread) != nread)
			err_sys("write error");
	}
	if (nread < 0 && errno == EINTR)
		goto again;
	else if (nread < 0)
		err_sys("readline error");
}


/* str_echo1的再版，但这里直接使用系统调用read/write实现读写，
	不再基于文本行协议 */
void str_echo1(int sockfd) {
	ssize_t nread;
	char buf[MAXLINE];

	for (;;) {
		if ((nread = read(sockfd, buf, MAXLINE)) == -1) {
			if (errno == EINTR)continue;
			err_sys("read error");
		}
		else if (nread == 0)break;
		if (write(sockfd, buf, nread) != nread)
			err_sys("write error");
	}
}