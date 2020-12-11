#include "MyUNP.h"

/**
 * 支持IPv4或者IPv6协议（通过不同形式的IP地址完成）和端口号
 * 可指定的时间获取服务器
 **/

int main(int argc, char* argv[])
{
	struct sockaddr_storage cliaddr;
	int listenfd, connfd;
	char buf[MAXLINE];
	socklen_t len;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &len);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &len);
	else
		err_quit("usage: %s [host] <server/port>", argv[0]);

	for (;;) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("%s: connection from %s\n", currtime(NULL),
			sock_ntop((struct sockaddr*)&cliaddr, len));

		snprintf(buf, MAXLINE, "%s", currtime(NULL));
		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			err_sys("write error");
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
