#include "MyUNP.h"

/**
 * 根据指定的主机+服务查询服务器协议地址，然后向其获取
 * 时间日期（采用UDP协议）
 **/

int main(int argc, char* argv[])
{
	char buf[MAXLINE];
	int sockfd, nread;

	if (argc != 3)
		err_quit("usage: %s <host/IPAddress> <server/port>", argv[0]);

	sockfd = udp_connect(argv[1], argv[2]);
	if (write(sockfd, "", 1) != 1)
		err_sys("write error");
	if ((nread = read(sockfd, buf, MAXLINE)) == -1)
		err_sys("recvfrom error");
	buf[nread] = 0;
	if (fputs(buf, stdout) == EOF)
		err_sys("fputs error");
	exit(EXIT_SUCCESS);
}