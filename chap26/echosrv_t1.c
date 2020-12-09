#include "MyUNP.h"


static void sig_intr(int signo) {
	fprintf(stderr, "\nclosing server...\n");
	exit(EXIT_SUCCESS);
}


static void* thread_func(void* arg) {
	int fd = *(int*)arg;
	free(arg);

	str_echo1(fd);
	if (close(fd) == -1)
		err_sys("close error");
	return (void*)NULL;
}


int main(int argc, char* argv[])
{
	int listenfd, *connfdp, err;
	struct sockaddr* cliaddr;
	socklen_t addrlen, len;
	pthread_t tid;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host] <server/port>", argv[0]);

	if (mysignal(SIGINT, sig_intr) == SIG_ERR)
		err_sys("mysignal error");
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	for (;;) {
		len = addrlen;
		/* 通过为每一个线程动态分配一个int大小的空间来存储accept
		   新返回的连接套接字描述符，从而来规避线程间对原来connfd
		   这个唯一变量的竞态条件  */
		if ((connfdp = malloc(sizeof(int))) == NULL)
			err_sys("malloc error");

		if ((*connfdp = accept(listenfd, cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
			sock_ntop(cliaddr, len));

		if ((err = pthread_create_detached(&tid, thread_func, (void*)connfdp)) != 0)
			err_exit(err, "pthread_create_detached error");
	}
}