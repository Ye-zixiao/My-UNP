#include "MyUNP.h"


int main(int argc, char* argv[])
{
	int listenfd, connfd;
	socklen_t addrlen, clilen;
	struct sockaddr* cliaddr;
	char buf[MAXLINE];

	if (argc < 2 || argc>3)
		err_quit("usage: %s [host/ip] <serv/port>", basename(argv[0]));
	daemon_init(argv[0], 0);
	
	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");

	for (;;) {
		clilen = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &clilen)) == -1)
			err_sys("accept error");
		err_msg("connection from %s", sock_ntop(cliaddr, clilen));

		snprintf(buf, MAXLINE, "%s\n", currtime(NULL));
		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			err_sys("write error");
		if (close(connfd) == -1)
			err_sys("close error");
	}
}