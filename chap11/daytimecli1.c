#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	char recvline[MAXLINE + 1];
	struct hostent* hptr;
	struct servent* sptr;
	ssize_t nread;
	int i, sockfd;
	char** pptr;

	if (argc != 3)
		err_quit("usage: %s <IPAddress/Host> <Server/Port>", argv[0]);

	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	if ((hptr = gethost1(argv[1])) == NULL)
		err_sys("gethost1 error");
	if ((sptr = getserv(argv[2], "tcp")) == NULL)
		err_sys("getserv error");
	
	pptr = hptr->h_addr_list;
	for (i = 0; pptr[i] != NULL; ++i) {
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			err_sys("socket error");
		svaddr.sin_port = sptr->s_port;
		memcpy(&svaddr.sin_addr, pptr[i], sizeof(svaddr.sin_addr));
		printf("trying %s...\n", sock_ntop((struct sockaddr*)&svaddr, sizeof(svaddr)));

		if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == 0)
			break;
		err_ret("connect error");
		if (close(sockfd) == -1)
			err_sys("close error");
	}
	if (pptr[i] == NULL) err_quit("unable to connect");

	while ((nread = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[nread] = '\0';
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	exit(EXIT_SUCCESS);
}