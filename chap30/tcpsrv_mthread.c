#include "MyUNP.h"

/**
 * TCP多线程并发服务器：
 * 为每一个客户临时创建一个可分离线程，以处理网络事务
 */

static void sig_int(int signo) {
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


static void* thread_func(void* arg) {
	int fd = *(int*)arg;
	free(arg);

	web_child_r1(fd);
	if (close(fd) == -1)
		err_sys("close error");
	return (void*)NULL;
}


int main(int argc, char* argv[])
{
	int listenfd, * connfdp, err;
	struct sockaddr* cliaddr;
	socklen_t addrlen, clilen;
	pthread_t tid;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <serv/port>", argv[0]);

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	
	for (;;) {
		clilen = addrlen;
		if ((connfdp = malloc(sizeof(int))) == NULL)
			err_sys("malloc error");
		if ((*connfdp = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		printf("%s: new connection from %s\n", currtime("%T"),
			sock_ntop(cliaddr, clilen));

		if ((err = pthread_create_detached(&tid, thread_func, (void*)connfdp)) != 0)
			err_exit(err, "pthread_create_detached error");
	}
}