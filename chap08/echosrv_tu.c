/**
 * 一个能够同时处理TCP和UDP的多进程并发回射服务器
 **/
#include "MyUNP.h"

#define SERV_PORT 12000

static void sig_chld(int signo) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
	return;
}

static void sig_int(int signo) {
	printf("\nclosing server...\n");
	exit(EXIT_SUCCESS);
}


int main(int argc, char* argv[])
{
	int udpfd, listenfd, connfd, maxfdp1, nready;
	struct sockaddr_in svaddr, cliaddr;
	char buf[MAXLINE];
	const int on = 1;
	socklen_t len;
	fd_set rset;
	ssize_t nrcv;
	pid_t pid;

	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	svaddr.sin_addr.s_addr = INADDR_ANY;

	//创建TCP监听套接字
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) == -1)
		err_sys("setsockopt error");
	if (bind(listenfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	if (listen(listenfd, 20) == -1)
		err_sys("listen error");

	//创建UDP套接字
	if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	if (bind(udpfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");

	if (mysignal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("mysignal error");
	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	FD_ZERO(&rset);
	maxfdp1 = MAX(listenfd, udpfd) + 1;
	for (;;) {
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) == -1) {
			if (errno == EINTR) continue;
			err_sys("select error");
		}

		//处理TCP事务
		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
				err_sys("accept error");
			printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
				sock_ntop((const struct sockaddr*)&cliaddr, sizeof(cliaddr)));
			
			if ((pid = fork()) < 0)
				err_sys("fork error");
			else if (pid == 0) {
				if (close(listenfd) == -1)
					err_sys("listenfd error");
				str_echo1(connfd);
				exit(EXIT_SUCCESS);
			}
			if (close(connfd) == -1)
				err_sys("close error");
		}

		//处理UDP事务
		if (FD_ISSET(udpfd, &rset)) {
			len = sizeof(len);
			if ((nrcv = recvfrom(udpfd, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len)) == -1)
				err_sys("recvfrom error");
			printf("%s: received new datagram from %s\n", currtime("%Y-%m-%d %T"),
				sock_ntop((const struct sockaddr*)&cliaddr, sizeof(cliaddr)));
			if (sendto(udpfd, buf, nrcv, 0, (struct sockaddr*)&cliaddr, len) == -1)
				err_sys("sendto error");
		}
	}
}
