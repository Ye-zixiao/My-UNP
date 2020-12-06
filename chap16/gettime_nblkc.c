#include "MyUNP.h"

#define SERV_PORT 12000


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	char buf[MAXLINE];
	ssize_t nread;
	int sockfd;

	if (argc < 2)
		err_quit("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	if (connect_nblk(sockfd, (const struct sockaddr*)&svaddr, sizeof(svaddr), 5) == -1)
		err_sys("connect_nblk error");

	while ((nread = read(sockfd, buf, MAXLINE)) > 0)
		if (write(STDOUT_FILENO, buf, nread) != nread)
			err_sys("write error");
	if (nread < 0)
		err_sys("read error");

	exit(EXIT_SUCCESS);
}