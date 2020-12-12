#include "MyUNP.h"
#include <sys/mman.h>

/**
 * TCP多进程并发服务器（预先分配进程池）
 * 通过共享内存机制辅助记录accep连接在各个进程之间的分布性
 */

#define DEBUG

static int nchildren;
static pid_t* pids;
static long* cptr;


static void sig_int(int signo);
pid_t child_make(int i, int listenfd, socklen_t addrlen);
void child_main(int i, int listenfd, socklen_t addrlen);
long* mmap_make(int ncs);


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
	cptr = mmap_make(nchildren);
	if ((pids = calloc(nchildren, sizeof(pid_t))) == NULL)
		err_sys("calloc error");
	for (int i = 0; i < nchildren; ++i)
		pids[i] = child_make(i, listenfd, addrlen);

	if (mysignal(SIGINT, sig_int) == SIG_ERR)
		err_sys("mysignal error");
	for (;;) 
		pause();
}



/* 用来终止服务器和进程池中的所有进程 */
static void sig_int(int signo) {
	for (int i = 0; i < nchildren; ++i)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0);
	if (errno != ECHILD)
		err_sys("wait error");

	putchar('\n');
	for (int i = 0; i < nchildren; ++i)
		printf("child(%d): %ld\n", pids[i], cptr[i]);
	pr_cpu_time();
	exit(EXIT_SUCCESS);
}


/* 服务父进程用此函数来创建一个共享存储区，并子进程继承这个存储区并在从
	accept返回的时候递增其中的计数，用来测试连接接收在各个子进程中的分布性 */
long* mmap_make(int ncs) {
	long* ptr;

#ifdef MAP_ANON
	if ((ptr = mmap(0, sizeof(long) * ncs, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
		err_sys("mmap error");
#else 
	int fd;

	if ((fd = open("/dev/zero", O_RDWR, 0)) == -1)
		err_sys("open error");
	if ((ptr = mmap(0, ncs * sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
		err_sys("mmap error");
	if (close(fd) == -1)
		err_sys("close error");
#endif
	return ptr;
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
		++cptr[i];

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
	return pid;//子进程不会到这
}
