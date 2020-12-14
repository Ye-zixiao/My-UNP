#include "MyUNP.h"
#include "array_queue.h"

/**
 * TCP多线程并发服务器：
 * 预设线程池，主线程控制新TCP连接的接受，并将相应的描述符加入到
 * 等待处理队列中。而其他工作线程通过加锁的方式从队列中取出需要
 * 处理的套接字描述符，然后处理网络事务
 */

#define MAXNCLI 32

struct Thread {
	pthread_t thread_tid;
	long thread_count;
};


static int nthreads;
static struct Thread* tptr;
static struct arrqueue* aq;		//等待处理连接套接字描述符队列
static pthread_mutex_t aq_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t aq_accept_cond = PTHREAD_COND_INITIALIZER;	//需要主线程去接受新的TCP连接
static pthread_cond_t aq_avail_cond = PTHREAD_COND_INITIALIZER;		//需要事务处理线程去处理新的网络事务

static void sig_int(int signo);
void thread_make(int i);
void* thread_main(void* arg);


int main(int argc, char* argv[])
{
	struct sockaddr* cliaddr;
	socklen_t addrlen, clilen;
	int err, connfd, listenfd;

	if (argc == 3)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host/ip] <serv/port> <#threads>", argv[0]);

	nthreads = atoi(argv[argc - 1]);
	if ((tptr = calloc(nthreads, sizeof(struct Thread))) == NULL)
		err_sys("calloc error");
	if ((aq = arrq_create(MAXNCLI, sizeof(int))) == NULL)
		err_sys("arrq_create error");
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");

	for (int i = 0; i < nthreads; ++i)
		thread_make(i);
	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");

	for (;;) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		printf("%s: new connection from %s\n", currtime("%T"),
			sock_ntop(cliaddr, clilen));

		/* 将已连接套接字描述符加入到队列中，若队列满则先阻塞主线程，待队列不满
			(条件满足)后再操作。当描述符成功加入到队列后通过条件变量通知工作线程
			进行处理 */
		if ((err = pthread_mutex_lock(&aq_lock)) != 0)
			err_exit(err, "ptherad_mutex_lock error");
		while (arrq_full(aq))
			if ((err = pthread_cond_wait(&aq_accept_cond, &aq_lock)) != 0)
				err_sys("pthread_cond_wait error");
		arrq_enqueue(aq, &connfd, sizeof(int));
		if ((err = pthread_mutex_unlock(&aq_lock)) != 0)
			err_exit(err, "pthread_mutex_unlock error");
		if ((err = pthread_cond_signal(&aq_avail_cond)) != 0)
			err_exit(err, "pthread_cond_signal error");
	}
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
	int i, err, connfd;

	i = (int)arg;
	printf("thread %2d starting\n", i);
	for (;;) {
		/* 从队列中取出一个已连接套接字。若队列空则阻塞工作线程，等待主线程
			通知它后重新检查到队列为空，就将从中取出一个描述符然后通知主线程
			可以继续接受新的连接，而自己则接着处理网络事务 */
		if ((err = pthread_mutex_lock(&aq_lock)) != 0)
			err_exit(err, "pthread_mutex_lock error");
		while (arrq_empty(aq))
			if ((err = pthread_cond_wait(&aq_avail_cond, &aq_lock)) != 0)
				err_exit(err, "pthread_cond_wait error");
		arrq_dequeue(aq, &connfd, sizeof(int));
		if ((err = pthread_mutex_unlock(&aq_lock)) != 0)
			err_exit(err, "pthread_mutex_unlock error");
		if ((err = pthread_cond_signal(&aq_accept_cond)) != 0)
			err_exit(err, "pthread_cond_signal error");
		
		tptr[i].thread_count++;
		web_child_r1(connfd);
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
