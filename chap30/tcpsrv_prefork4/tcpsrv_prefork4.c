#include "child.h"

/**
 * TCP多进程并发服务器：
 * 在这版的服务器中accept由父进程调用，当成功返回之后通过未命名UNIX域套接字
 * 发送给它的子进程，这样可以避免考虑accept原子性的问题。但新的问题在于这种
 * 服务器可能效率是所有多进程并发服务器中最低的，而且连接处理任务在子进程中
 * 的分布也不是很均衡，完全取决于同一时刻到来的连接数量
 */

#define AVAILCHILD 0
#define BUSYCHILD  1


struct Child* cptr;
static int nchildren;


static void sig_int(int signo) {
	for (int i = 0; i < nchildren; ++i)
		kill(cptr[i].child_pid, SIGTERM);
	while (wait(NULL) > 0);
	if (errno != ECHILD) err_sys("wait error");
	
	putchar('\n');
	for (int i = 0; i < nchildren; ++i)
		printf("child(%d): %ld\n", cptr[i].child_pid, cptr[i].child_count);
	pr_cpu_time();
#ifdef DEBUG
	printf("progress %d print cpu time\n", getpid());
#endif
	exit(EXIT_SUCCESS);
}


static void sig_chld(int signo) {
	while(waitpid(-1, NULL, WNOHANG) > 0);
	return;
}


int main(int argc, char* argv[])
{
	int i, rc, navail, nsel, listenfd, connfd, maxfd;
	struct sockaddr* cliaddr;
	socklen_t addrlen, clilen;
	fd_set rset, allset;
	ssize_t n;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <serv/port> <#nchild>", argv[0]);

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	maxfd = listenfd;
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");

	/* 创建进程池 */
	nchildren = atoi(argv[argc - 1]);
	navail = nchildren;
	if ((cptr = calloc(nchildren, sizeof(struct Child))) == NULL)
		err_sys("calloc error");
	for (int i = 0; i < nchildren; ++i) {
		child_make(i, listenfd, addrlen);
		FD_SET(cptr[i].child_pipefd, &allset);
		maxfd = MAX(maxfd, cptr[i].child_pipefd);
	}

	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	if (mysignal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("mysignal error");

	for (;;) {
		rset = allset;
		if (navail <= 0)//若没有多余的子进程可用，则先不接受新的连接
			FD_CLR(listenfd, &rset);
		if ((nsel = select(maxfd + 1, &rset, NULL, NULL, NULL)) == -1)
			err_sys("select error");

		/* 接受新的连接，并将套接字描述符发送给子进程 */
		if (FD_ISSET(listenfd, &rset)) {
			clilen = addrlen;
			if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
				err_sys("accept error");
			printf("%s: new connection from %s\n", currtime("%T"),
				sock_ntop(cliaddr, clilen));

			for (i = 0; i < nchildren; ++i)
				if (cptr[i].child_status == AVAILCHILD) break;
			if (i == nchildren)
				err_quit("no available children");
			cptr[i].child_status = BUSYCHILD;
			cptr[i].child_count++;
			navail--;

			if ((n = send_fd(cptr[i].child_pipefd, "", 1, connfd)) == -1)
				err_sys("send_fd error");
			if (close(connfd) == -1)
				err_sys("close error");
			if (--nsel == 0) continue;
		}
		
		/* 处理子进程发送过来的回复信息，若正确收到表示其处理好了一个网络任务，
			此时将这个子进程的状态设置为空闲状态 */
		for (i = 0; i < nchildren; ++i) {
			if (FD_ISSET(cptr[i].child_pipefd, &rset)) {
				if ((n = read(cptr[i].child_pipefd, &rc, 1)) == -1)
					err_sys("read error");
				else if (n == 0) {
					// err_quit("child %d terminated unexpectedly", i);

					/* 若子进程异常终止，则父进程需要重新派生出一个子进程作为补充。这里不仅
					   需要处理SIGCHLD信号的问题，以及新建子进程继承了SIGCHLD信号处理程序
					   等一些问题 */
					FD_CLR(cptr[i].child_pipefd, &allset);
					if(close(cptr[i].child_pipefd) == -1)
						err_sys("close error");
					child_make(i, listenfd, addrlen);
					FD_SET(cptr[i].child_pipefd, &allset);
				}
				else {
					cptr[i].child_status = AVAILCHILD;
					navail++;
				}
				if (--nsel == 0) break;
			}
		}
	}
}