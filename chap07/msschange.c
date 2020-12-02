#include "MyUNP.h"
#include <netinet/tcp.h>

#define SERV_PORT 12000

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	char buf[MAXLINE];
	int sockfd, val;
	socklen_t len;
	ssize_t nread;

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");

	//connect之前查看接收/发送缓冲区大小
	len = sizeof(int);
	fprintf(stderr, "before connect: ");
	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	fprintf(stderr, "recvbuf %d bytes, ", val);
	if (getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &val, &len) == -1)
		err_sys("getsockopt error");
	fprintf(stderr, "mss %d bytes\n", val);

	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	//connnect之后查看接收/发送缓冲区大小
	len = sizeof(int);
	fprintf(stderr, "after connect: ");
	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	fprintf(stderr, "recvbuf %d bytes, ", val);
	if (getsockopt(sockfd, IPPROTO_TCP, TCP_MAXSEG, &val, &len) == -1)
		err_sys("getsockopt error");
	fprintf(stderr, "mss %d bytes\n", val);

	while ((nread = read(sockfd, buf, MAXLINE)) > 0)
		if (write(STDOUT_FILENO, buf, nread) != nread)
			err_sys("write error");
	if (nread < 0)
		err_sys("read error");
	exit(EXIT_SUCCESS);
}
