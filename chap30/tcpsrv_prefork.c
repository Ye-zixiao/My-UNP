#include "MyUNP.h"

/**
 * TCP多进程并发服务器（预先分配进程池）
 */

#define DEBUG

static int nchildren;
static pid_t* pids;


/* 用来终止服务器和进程池中的所有进程 */
void sig_int(int signo) {
	for (int i = 0; i < nchildren; ++i)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0);
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


/* 子进程处理真正的客户请求事务，在这里accpet函数是由服务子进程
	自己调用的，而不是父进程调用的（虽然listenfd是各个子进程在
	共用，但accept操作应该是原子的） */
void child_main(int i, int listenfd, socklen_t addrlen) {
	struct sockaddr* cliaddr;
	socklen_t clilen;
	int connfd;

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
#ifdef DEBUG
	printf("child %d starting\n", getpid());
#endif
	
	for (;;) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		printf("%s: pid(%d): new connection from %s\n", currtime("%T"), getpid(),
			sock_ntop(cliaddr, clilen));

		web_child(connfd);
		if (close(connfd) == -1)
			err_sys("close error");
	}
}


/* 预先创建指定数量的进程池 */
pid_t child_make(int i, int listenfd, socklen_t addrlen) {
	pid_t pid;

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)
		child_main(i, listenfd, addrlen);
	return pid;//子进程却不会到这
}



int main(int argc, char* argv[])
{
	socklen_t addrlen;
	int listenfd;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <serv/port> <#nchildren>", argv[0]);

	nchildren = atoi(argv[argc - 1]);
	if ((pids = calloc(nchildren, sizeof(pid_t))) == NULL)
		err_sys("calloc error");
	for (int i = 0; i < nchildren; ++i)
		pids[i] = child_make(i, listenfd, addrlen);

	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	for (;;) 
		pause();
}