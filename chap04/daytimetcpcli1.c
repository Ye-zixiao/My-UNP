#include "../include/MyUNP.h"

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd, nread, cnt = 0;
	char buf[MAXLINE];

	if (argc != 2)
		err_quit("usage: %s <IPAddr>", argv[0]);

	/* 创建连接套接字并与服务进程建立tcp连接 */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_quit("inet_pton can't convert string(%s) to ipaddr", argv[1]);
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(struct sockaddr_in)) == -1)
		err_sys("connect error");

	//从服务进程中获取响应数据
	while ((nread = read(sockfd, buf, MAXLINE)) > 0) {
		cnt++;
		buf[nread] = '\0';
		if (fputs(buf, stdout) == EOF)
			err_sys("fputs error");
	}
	if (nread < 0)
		err_sys("read error");
//	printf("client read %d times from connection\n", cnt);
	exit(EXIT_SUCCESS);
}
