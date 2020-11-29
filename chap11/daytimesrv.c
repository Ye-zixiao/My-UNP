#include "MyUNP.h"

/**
 * 这版的日期时间获取服务器:
 * 使用tcp_listen()函数获取指定地址并完成socket()-
 * bind()-listen()一条龙服务不再单兵作战
 **/

int main(int argc, char* argv[])
{
	struct sockaddr_storage cliaddr;
	int listenfd, connfd;
	char buf[MAXLINE];
	socklen_t len;

	if (argc != 2)
		err_quit("usage: %s <server/port>", argv[0]);

	listenfd = tcp_listen(NULL, argv[1], &len);
	for (;;) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
			sock_ntop((struct sockaddr*)&cliaddr, len));

		snprintf(buf, MAXLINE, "%s", currtime(NULL));
		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			err_sys("write error");
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
