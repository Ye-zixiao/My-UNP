#include "MyUNP.h"
#include <sys/mman.h>

/**
 * TCP多进程并发服务器（预先分配进程池）：
 * 加入了文件锁，保障accept的原子性。但实际上Linux的accept本来就是原子的
 */

#define DEBUG

static int lock_fd = -1;
static int nchildren;
static pid_t* pids;
static long* cptr;


static void sig_int(int signo);
pid_t child_make(int i, int listenfd, socklen_t addrlen);
void child_main(int i, int listenfd, socklen_t addrlen);
long* mmap_make(int ncs);

void mylock_init(char* pathname);
void mylock_wait(void);
void mylock_release(void);


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
	mylock_init("/tmp/lock.XXXXXX");
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

	if ((ptr = mmap(0, sizeof(long) * ncs, PROT_READ | PROT_WRITE,
			MAP_ANON | MAP_SHARED, -1, 0)) == MAP_FAILED)
		err_sys("mmap error");
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
		mylock_wait();
		if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		mylock_release();
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


/* 创建一个临时文件 */
void mylock_init(char* pathname) {
	char lock_file[1024];

	strncpy(lock_file, pathname, sizeof(lock_file));
	if ((lock_fd = mkstemp(lock_file)) == -1)
		err_sys("mkstemp error");
	if (unlink(lock_file) == -1)
		err_sys("unlink error");
}


void mylock_wait(void) {
	while (writew_lock(lock_fd, 0, SEEK_SET, 0) == -1) {
		if (errno == EINTR) continue;
		err_sys("writew_lock error");
	}
}


void mylock_release(void) {
	while (unlock(lock_fd, 0, SEEK_SET, 0) == -1) {
		if (errno == EINTR) continue;
		err_sys("unlock error");
	}
}