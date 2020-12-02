/**
 * 与服务进程创建多个连接，但只正常使用一个连接，其他连接
 * 仅仅是为了让服务进程fork多个子进程而已
 */
#include "MyUNP.h"

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfds[5];

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);
	
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	for (int i = 0; i < 5; i++) {
		if ((sockfds[i] = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			err_sys("socket error");
		if (connect(sockfds[i], (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
			err_sys("connect error");
	}
	str_cli(sockfds[0], stdin);
	exit(EXIT_SUCCESS);
}
