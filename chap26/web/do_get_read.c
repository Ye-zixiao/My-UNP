#include "web.h"


/* web客户进程试图与服务器进行连接，并在建立TCP连接之后
	发送GET请求，最后打印读取到的数据量信息 */
void* do_get_read(void* arg) {
	char line[MAXLINE];
	struct file* fptr;
	int n, fd, err;

	/* 试图建立TCP连接，并发送GET请求 */
	fptr = (struct file*)arg;
	fd = tcp_connect(fptr->f_host, SERV);
	fptr->f_fd = fd;
	printf("do_get_read for %s, fd %d, thread 0x%lx\n",
		fptr->f_name, fd, pthread_self());
	write_get_cmd(fptr);

	/* 读取返回的信息并打印数据量信息 */
	for (;;) {
		if ((n = read(fd, line, MAXLINE)) == -1)
			err_sys("read error");
		else if (n == 0) break;
		printf("read %d bytes from %s\n", n, fptr->f_name);
	}
	printf("end-of-file on %s\n", fptr->f_name);
	if (close(fd) == -1)
		err_sys("close error");

	/* 递增全局变量ndone，激活条件变量并唤醒主线程 */
	if ((err = pthread_mutex_lock(&ndone_lock)) != 0)
		err_exit(err, "pthread_mutex_lock error");
	fptr->f_flags = F_DONE;
	++ndone;
	if ((err = pthread_cond_signal(&ndone_cond)) != 0)
		err_exit(err, "pthread_cond_signal error");
	if ((err = pthread_mutex_unlock(&ndone_lock)) != 0)
		err_exit(err, "pthread_mutex_unlock error");

	return fptr;
}
