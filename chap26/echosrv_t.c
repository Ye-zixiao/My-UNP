#include "MyUNP.h"


static void* thread_func(void* arg) {
	int fd = *(int*)arg;
	str_echo1(fd);
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

		if ((err = pthread_create_detached(&tid, thread_func, (void*)&connfd)) != 0)
			err_exit(err, "pthread_create_detached error");
	}
}
