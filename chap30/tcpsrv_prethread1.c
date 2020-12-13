#include "MyUNP.h"

/**
 * TCP多线程并发服务器：
 * 预设线程池，由各个非主线程自己调用accept的方式接受网络连接请求
 * (各线程调用accept时不再设置互斥量，因此这种方式把accpet的原子性
 * 保障交给了内核，且会引起惊群效应！)
 */

struct Thread {
	pthread_t thread_tid;
	long thread_count;
};

struct Thread* tptr;
int listenfd, nthreads;
socklen_t addrlen;


static void sig_int(int signo);
void* thread_main(void* arg);
void thread_make(int i);


int main(int argc, char* argv[])
{
	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <serv/port> <#nthread>", argv[0]);

	nthreads = atoi(argv[argc - 1]);
	if ((tptr = calloc(nthreads, sizeof(struct Thread))) == NULL)
		err_sys("calloc error");
	for (int i = 0; i < nthreads; ++i)
		thread_make(i);

	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");

	for (;;)
		pause();
}



static void sig_int(int signo) {
	putchar('\n');
	for (int i = 0; i < nthreads; ++i)
		printf("thread %2d: %ld\n", i, tptr[i].thread_count);
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


void thread_make(int i) {
	int err;

	if ((err = pthread_create_detached(&tptr[i].thread_tid, thread_main, (void*)i)) != 0)
		err_exit(err, "pthread_create_detached error");
	return;
}


void* thread_main(void* arg) {
	struct sockaddr* cliaddr;
	socklen_t clilen;
	int connfd;

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	printf("thread %2d starting\n", (int)arg);
	
	for (;;) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		printf("thread %2d: %s: new connection from %s\n", (int)arg,
			currtime("%T"), sock_ntop(cliaddr, clilen));
		tptr[(int)arg].thread_count++;

		web_child_r1(connfd);
		if (close(connfd) == -1)
			err_sys("close error");
	}
}