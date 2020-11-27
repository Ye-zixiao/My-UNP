﻿#include "MyUNP.h"

#define SERV_PORT 12000

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc != 2)
		err_sys("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	//任何人只要知道这个UDP的IP地址和端口号则都可以向其发送内容
	dg_cli(sockfd, stdin, (const struct sockaddr*)&svaddr, sizeof(svaddr));

	exit(EXIT_SUCCESS);
}
