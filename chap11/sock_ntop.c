#include "MyUNP.h"


/**
 * 使用getnameinfo函数实现的sock_ntop，可以达到协议无关性的
 * IP地址+端口号信息输出
 */
static char *
mysock_ntop(const struct sockaddr* addr, socklen_t addrlen) {
	char hostname[NI_MAXHOST], servname[NI_MAXSERV];
	static char result[MAXLINE];

	if (getnameinfo(addr, addrlen, hostname, sizeof(hostname),
		servname, sizeof(servname), NI_NUMERICHOST | NI_NUMERICSERV) != 0)
		return NULL;
	switch (addr->sa_family) {
	case AF_INET:
		snprintf(result, MAXLINE, "%s:%s", hostname, servname);
		break;
	case AF_INET6:
		snprintf(result, MAXLINE, "[%s]:%s", hostname, servname);
		break;
	default:
		return NULL;
	}
	return result;
}


int main(int argc, char* argv[])
{
	struct sockaddr* cliaddr;
	socklen_t addrlen, len;
	int listenfd, connfd;
	char buf[MAXLINE];

	if (argc == 2)
		listenfd = tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: %s [IPAddress/Host] <Server/Port>\n"
				 "this program aims to test sock_ntop(using getnameinfo)", argv[0]);

	if ((cliaddr = malloc(addrlen)) == NULL)
		err_sys("malloc error");
	for (;;) {
		len = addrlen;
		if ((connfd = accept(listenfd, cliaddr, &len)) == -1)
			err_sys("accept error");
		printf("connection from %s\n", mysock_ntop(cliaddr, len));
		
		snprintf(buf, MAXLINE, "%s", currtime(NULL));
		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			err_sys("write error");
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
