#include "MyUNP.h"


static void* thread_func(void* arg) {
	int fd = (int)arg;
	str_echo(fd);
	if (close(fd) == -1)
		err_sys("close error");
	return (void*)NULL;
}


int main(int argc, char* argv[])
{
	int listenfd, connfd, err;
	struct sockaddr* cliaddr;
	socklen_t addrlen, len;
	pthread_t tid;

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [host] <server/port>", argv[0]);

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	for (;;) {
		len = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
			sock_ntop(cliaddr, len));

		/* 若这里pthread_create的最后一个参数不是直接传递connfd的值，那么
		   当新线程创建之后并没有及时执行：	int fd=*(int*)arg;  然后就让
		   发生了上下文切换，控制权又交给了主线程，而主线程又调用了accept
		   返回了一个新的套接字描述符覆写了connfd。那么此时前面的线程就无法
		   真正用到属于自己的套接字描述符了！这便是静态条件问题（线程没有同步） */
		if ((err = pthread_create_detached(&tid, thread_func, (void*)connfd)) != 0)
			err_exit(err, "pthread_create_detached error");
	}
}