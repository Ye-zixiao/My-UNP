#include "MyUNP.h"

#define MAXN 16384


/* 接收来自客户的数据请求，并返回给它指定数量的数据 */
void web_child(int sockfd) {
	char line[MAXLINE], result[MAXN];
	int ntowrite; ssize_t nread;

	for (;;) {
		if ((nread = readline(sockfd, line, MAXLINE)) == -1)
			err_sys("readline error");
		else if (nread == 0)return;

		ntowrite = atoi(line);
		if (ntowrite <= 0 || ntowrite > MAXN)
			err_quit("client request for %d bytes", ntowrite);
		if (writen(sockfd, result, ntowrite) != ntowrite)
			err_sys("writen error");
	}
}