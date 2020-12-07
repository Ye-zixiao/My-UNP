#include "web.h"


/**
 * 对指定的主机web服务器请求一个主页，实际上用这个函数可以请求这个
 * web服务器中的任意文件。收到文件后仅显示接受数据量
 */
void home_page(const char* host, const char* fname) {
	char line[MAXLINE];
	int sockfd, n;

	sockfd = tcp_connect(host, SERV);
	n = snprintf(line, MAXLINE, GET_CMD, fname);
	if (writen(sockfd, line, n) != n)
		err_sys("write error");

	for (;;) {
		if ((n = read(sockfd, line, MAXLINE)) == -1)
			err_sys("read error");
		else if (n == 0)break;
		printf("read %d bytes of home pages\n", n);
	}
	printf("end-of-file on home page\n");
	if (close(sockfd) == -1)
		err_sys("close error");
}