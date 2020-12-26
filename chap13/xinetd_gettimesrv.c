#include "MyUNP.h"

/**
 * 一个由xinetd守护进程启动的getdaytime服务程序
 */


int main(int argc, char* argv[])
{
	struct sockaddr* cliaddr;
	char buf[MAXLINE];
	socklen_t len;

	daemon_inetd(argv[0], 0);
	if ((cliaddr = malloc(sizeof(struct sockaddr_storage))) == NULL)
		err_sys("malloc error");
	len = sizeof(struct sockaddr_storage);
	
	if (getpeername(0, cliaddr, &len) == -1)
		err_sys("getpeername error");
	err_msg("connection from %s", 
		sock_ntop((const struct sockaddr*)cliaddr, len));

	snprintf(buf, MAXLINE, "%s", currtime(NULL));
	if (write(0, buf, strlen(buf)) != strlen(buf))
		err_sys("write error");

	if (close(0) == -1)
		err_sys("close error");
	exit(EXIT_SUCCESS);
}