/**
 * 实验方法:向一个主机A发送数据报,然后让主机B发送给当前
 * 服务器一个udp数据报,那么这个数据报就会被忽略
 **/
#include "MyUNP.h"

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
	dg_cli1(sockfd, stdin, (const struct sockaddr*)&svaddr, sizeof(svaddr));

	exit(EXIT_SUCCESS);
}
