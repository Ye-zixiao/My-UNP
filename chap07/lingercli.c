#include "MyUNP.h"

/**
 * 对套接字开启SO_LINGER选项，检查在close之后客户留下的套接字是否进入TIME_WAIT状态；
 * 同时若开启了该套接字选项，服务进程必然接收到来自客户TCP发送的RST分节，那么对于我们
 * 的python运行程序会抛出ConnectionResetError异常，我们对其进行捕获并提示打印而不让其
 * 终止
 */

#define SERV_PORT 12000

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	struct linger mylinger;
	int sockfd;

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	str_cli(sockfd, stdin);
	mylinger.l_onoff = 1;
	mylinger.l_linger = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &mylinger, sizeof(mylinger)) == -1)
		err_sys("setsockopt error");
	exit(EXIT_SUCCESS);
}
