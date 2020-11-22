#include "MyUNP.h"


void sig_child(int signo) {
	if (wait(NULL) == -1)
		err_sys("wait error");
}


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr, cliaddr;
	int listenfd, connfd;
	char buf[MAXLINE];
	socklen_t len;
	int val = 1;
	pid_t pid;

	if (signal(SIGUSR1, sig_child) == SIG_ERR)
		err_sys("signal error");
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
		err_sys("setsockeopt error");
	bzero(&svaddr, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	if (listen(listenfd, 5) == -1)
		err_sys("listen error");

	for (;;) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
			err_sys("accept error");

		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, buf, sizeof(buf)),
				ntohs(cliaddr.sin_port));

			snprintf(buf, sizeof(buf), "%s\r\n", currtime(NULL));
			if (write(connfd, buf, strlen(buf)) != strlen(buf))
				err_sys("write error");
			if (close(connfd) == -1)
				err_sys("close error");
			if (kill(getppid(), SIGUSR1) == -1)
				err_sys("kill error");
			exit(EXIT_SUCCESS);
			/* 但由于父进程不调用waitpid去获取子进程的终止状态，会很容易导致
				僵尸进程的产生 */
		}
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
