/**
 * 从客户进程中读入两个字符串形式的数值，然后将其相加回射给
 * 客户进程
 */

#include "MyUNP.h"
#define DEBUG

void sig_chld(int signo) {
	pid_t pid;
	int stat;

	/* 正确处理子进程SIGCHLD信号的方式，这样可以避免当多个
	   子进程被终止时几乎同时发送SIGCHLD信号却没有被父进程
	   完全处理的情况 */
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
#ifdef DEBUG
		fprintf(stderr, "child %d terminated\n", pid);
#else
		;
#endif
	return;
}


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr, cliaddr;
	int listenfd, connfd, val = 1;
	char addrbuf[INET_ADDRSTRLEN];
	socklen_t len;
	pid_t pid;

	if (mysignal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("mysignal error");

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
		err_sys("setsockopt error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	svaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(listenfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	if (listen(listenfd, 5) == -1)
		err_sys("listen error");

	for (;;) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1) {
			/* 防止accept被信号中断而异常返回，不过实际上
				在Linux中默认确实是可以重启动的 */
			if (errno = EINTR)
				continue;
			else err_sys("accept error");
		}

		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			if (close(listenfd) == -1)
				err_sys("close error");

			printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, addrbuf, sizeof(addrbuf)),
				ntohs(cliaddr.sin_port));
			//str_echo(connfd);
			sum_echo1(connfd);
			exit(EXIT_SUCCESS);
		}
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
