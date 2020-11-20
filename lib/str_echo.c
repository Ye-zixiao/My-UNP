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
