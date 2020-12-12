#include "MyUNP.h"

/**
 * TCP多进程并发服务器
 */

static void sig_int(int signo) {
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


static void sig_chld(int signo) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
	return;
}


int main(int argc, char* argv[])
{
	struct sockaddr* cliaddr;
	socklen_t addrlen, len;
	int listenfd, connfd;
	pid_t pid;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <server/port>", argv[0]);
	
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	if (mysignal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("mysignal error");

	for (;;) {
		len = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &len)) == -1) {
			if (errno == EINTR) continue;
			err_sys("accept error");
		}
		printf("%s: new connetion from %s\n", currtime("%T"),
			sock_ntop(cliaddr, len));

		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			if (close(listenfd) == -1)
				err_sys("close error");
			web_child(connfd);
			if (close(connfd) == -1)
				err_sys("close error");
			exit(EXIT_SUCCESS);
		}

		if (close(connfd) == -1)
			err_sys("close error");
	}
}
