#include "MyUNP.h"

#define MAXN 16384

/**
 * TCP客户测试程序：创建指定数量的子进程，每一子进程创建
 * 指定数量的TCP套接字，它们每一个与TCP服务器进行连接，然后
 * 发送请求数据，并读取回复数据
 */


int main(int argc, char* argv[])
{
	int sockfd, nchildren, nloops, nbytes;
	char request[MAXLINE], reply[MAXN];
	pid_t pid;

	if (argc != 6)
		err_quit("usage: client <host/ip> <serv/port> <#children>"
			" <#loops/child> <#bytes/request>");
	nchildren = atoi(argv[3]);
	nloops = atoi(argv[4]);
	nbytes = atoi(argv[5]);
	snprintf(request, sizeof(request), "%d\n", nbytes);

	for (int i = 0; i < nchildren; ++i) {
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			for (int j = 0; j < nloops; ++j) {
				sockfd = tcp_connect(argv[1], argv[2]);

				if (write(sockfd, request, strlen(request)) != strlen(request))
					err_sys("write error");
				if (readn(sockfd, reply, nbytes) != nbytes)
					err_sys("readn error");
				if (close(sockfd) == -1)
					err_sys("close error");
			}
			printf("child %2d done\n", i);
			exit(EXIT_SUCCESS);
		}
	}

	while (wait(NULL) > 0);
	if (errno != ECHILD)
		err_sys("wait error");
	exit(EXIT_SUCCESS);
}
