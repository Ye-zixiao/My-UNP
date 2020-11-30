#include "MyUNP.h"

/**
 * 根据主机+服务创建一个获取服务器协议地址，然后创建一个服务UDP
 * 套接字，使得客户进程（先要发送一个任意信息，使得服务进程获取其
 * 地址）可以获取日期时间
 **/

int main(int argc, char* argv[])
{
	struct sockaddr_storage cliaddr;
	char buf[MAXLINE];
	ssize_t nrecv;
	socklen_t len;
	int sockfd;

	if (argc == 2)
		sockfd = udp_server(NULL, argv[1], &len);
	else if (argc == 3)
		sockfd = udp_server(argv[1], argv[2], &len);
	else
		err_quit("usage: %s [host/IPAddress] <server/port>", argv[0]);

	for (;;) {
		len = sizeof(cliaddr);
		if ((nrecv = recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len)) == -1)
			err_sys("recvfrom error");
		printf("received datagram from %s\n", sock_ntop((struct sockaddr*)&cliaddr, len));

		snprintf(buf, MAXLINE, "%s", currtime(NULL));
		if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&cliaddr, len) == -1)
			err_sys("sendto error");
	}
}