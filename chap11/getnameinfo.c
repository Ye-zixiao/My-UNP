#include "MyUNP.h"

/**
 * 使用getnameinfo()进行反向解析IPv4地址和端口号，返回
 * 对应的主机名+服务名
 */

int main(int argc, char* argv[])
{
	struct sockaddr_in testaddr;
	char host[MAXLINE], server[MAXLINE];
	int gerr;

	if (argc != 3)
		err_quit("usage: %s <IPAddress> <port>", argv[0]);
	
	bzero(&testaddr, sizeof(testaddr));
	testaddr.sin_family = AF_INET;
	testaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &testaddr.sin_addr) <= 0)
		err_sys("inet_pton error");

	if ((gerr = getnameinfo((const struct sockaddr*)&testaddr, sizeof(testaddr),
		host, MAXLINE, server, MAXLINE, 0)) != 0)
		err_quit("getnameinfo error");
	printf("hostname: %s\n", host);
	printf("server  : %s\n", server);
	exit(EXIT_SUCCESS);
}