#include "MyUNP.h"

/**
 * 这版的daytimecli:
 * 使用简单的gethostbyname()+inet_aton()组合获取
 * 服务器地址,使用getservbyname()+直接转换组合获取
 * 端口号
 **/

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	struct in_addr addr[10];
	char buf[MAXLINE + 1];
	struct servent* sptr;
	int sockfd, nread;
	int i, n;

	if (argc != 3)
		err_quit("usage: %s <ip/name> <port/server>", argv[0]);

	/* 获取地址和端口号 */
	if ((n = gethost(addr, 10, argv[1])) < 0)
		err_sys("gethost error");
	if ((sptr = getserv(argv[2], "tcp")) == NULL)
		err_sys("getserv error");

	/* 对得到的每一个地址进行尝试直到成功或全失败 */
	for (i = 0; i < n; ++i) {
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			err_sys("socket error");
		bzero(&svaddr, sizeof(svaddr));
		svaddr.sin_family = AF_INET;
		svaddr.sin_port = sptr->s_port;
		svaddr.sin_addr = addr[i];
		printf("trying %s...\n", sock_ntop((struct sockaddr*)&svaddr, sizeof(svaddr)));

		if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == 0) {
			printf("connect success\n\n"); break;
		}
		err_ret("connect error");
		close(sockfd);
	}
	if (i == n) err_quit("unable to connect");

	while ((nread = read(sockfd, buf, MAXLINE)) > 0) {
		buf[nread] = '\0';
		if (fputs(buf, stdout) == EOF)
			err_sys("fputs error");
	}

	exit(EXIT_SUCCESS);
}
