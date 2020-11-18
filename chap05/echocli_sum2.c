/**
 * 将用户输入的两个的二进制形式数值直接发送给服务器进程，
 * 然后从其获取相加的结果
 */

#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");
	
	sum_cli2(sockfd, stdin);
	exit(EXIT_SUCCESS);
}

//执行的时候结果有问题,明天检查吧
