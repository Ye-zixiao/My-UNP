#include "MyUNP.h"

/**
 * 这版的daytimecli:
 * 完全使用getaddrinfo()函数获取协议地址,而不再使用
 * gethostbyname或者getservbyname等旧式函数或者直接获取
 * 的方式获取协议地址
 **/


int main(int argc, char* argv[])
{
	struct sockaddr_storage ss;
	char buf[MAXLINE];
	ssize_t nread;
	socklen_t len;
	int sockfd;

	if (argc != 3)
		err_quit("usage: %s <IPaddress/hostname> <server/port>", argv[0]);

	len = sizeof(ss);
	sockfd = tcp_connect(argv[1], argv[2]);
	if (getpeername(sockfd, (struct sockaddr*)&ss, &len) == -1)
		err_sys("getpeername error");
	printf("connect to %s\n", sock_ntop((const struct sockaddr*)&ss, len));
	
	while ((nread = read(sockfd, buf, MAXLINE)) > 0)
		if (write(STDOUT_FILENO, buf, nread) != nread)
			err_sys("write error");
	if (nread < 0)
		err_sys("read error");
	exit(EXIT_SUCCESS);
}
