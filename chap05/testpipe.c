#include "MyUNP.h"

void sig_pipe(int signo) {	
	printf("caught SIGPIPE\n");
	exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc < 2)
		err_quit("usage: %s <ip-address>", argv[0]);
	if (mysignal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("mysignal error");

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(9999);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton convert string %s to address error", argv[1]);
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");
	
	sleep(2);
	write(sockfd, "f", 1);
	sleep(2);
	write(sockfd, "f", 1);

	if (close(sockfd) == -1)
		err_sys("close error");
	exit(EXIT_SUCCESS);
}
