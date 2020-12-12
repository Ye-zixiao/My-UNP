#include "MyUNP.h"

#define REQSIZE 16
#define MAXN 16384

/**
 * TCP迭代式服务器
 */


int main(int argc, char* argv[])
{
	char buf[MAXN], reqstr[REQSIZE];
	int listenfd, connfd, nreq;
	struct sockaddr* cliaddr;
	socklen_t addrlen, len;
	
	if (argc != 2)
		err_quit("usage: %s <serv/port>", argv[0]);

	listenfd = tcp_listen(NULL, argv[1], &addrlen);
	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	
	for (;;) {
		len = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("%s: new conntion from %s\n", currtime("%T"),
			sock_ntop(cliaddr, len));

		if (read(connfd, reqstr, sizeof(REQSIZE)) == -1)
			err_sys("read error");
		nreq = atoi(reqstr);
		if (writen(connfd, buf, nreq) != nreq)
			err_sys("writen error");
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
