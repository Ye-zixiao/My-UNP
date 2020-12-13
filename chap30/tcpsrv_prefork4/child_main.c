#include "child.h"


/* 子进程从父进程中读取到传递给它的套接字描述符，然后处理这个网络事务 */
void child_main(int i, int listenfd, socklen_t addrlen) {
	ssize_t nread;
	int connfd;
	char c;

	printf("child %d starting\n", getpid());
	for (;;) {
		if ((nread = recv_fd(STDERR_FILENO, &c, 1, &connfd)) == -1)
			err_sys("recv_fd error");
		else if (nread == 0)
			err_quit("recv_fd return 0");
		if (connfd == -1)
			err_quit("no descriptor from recv_fd");

		web_child(connfd);
		if (close(connfd) == -1)
			err_sys("close error");
		if (write(STDERR_FILENO, "", 1) != 1)
			err_sys("write error");
	}
}