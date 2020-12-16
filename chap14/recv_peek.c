#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_storage ss;
	char buf[MAXLINE + 1];
	int sockfd, npend;
	ssize_t nrecv;
	socklen_t len;

	if (argc != 3)
		err_quit("usage: %s <host/ip> <serv/port>", argv[0]);

	sockfd = tcp_connect(argv[1], argv[2]);
	len = sizeof(ss);
	if (getpeername(sockfd, (struct sockaddr*)&ss, &len) == -1)
		err_sys("getpeername error");
	printf("connection to %s\n", sock_ntop((struct sockaddr*)&ss, len));

	for (;;) {
		if ((nrecv = recv(sockfd, buf, MAXLINE, MSG_PEEK)) == -1)
			err_sys("recv error");
		else if (nrecv == 0) break;
		if (ioctl(sockfd, FIONREAD, &npend) == -1)
			err_sys("ioctl error");
		printf("%ld bytes from PEEK, %d bytes pending\n", nrecv, npend);

		if ((nrecv = read(sockfd, buf, MAXLINE)) == -1)
			err_sys("read error");
		buf[nrecv] = 0;
		if (fputs(buf, stdout) == EOF)
			err_sys("fputs error");
	}
}