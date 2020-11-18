#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr, cliaddr;
	int listenfd, connfd, val = 1;
	char addrbuf[INET_ADDRSTRLEN];
	socklen_t len;
	pid_t pid;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
		err_sys("setsockopt error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	svaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(listenfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("listen error");
	if (listen(listenfd, 5) == -1)
		err_sys("listen error");

	for (;;) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
			err_sys("accept error");
		
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			if (close(listenfd) == -1)
				err_sys("close error");

			printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, addrbuf, sizeof(addrbuf)),
				ntohs(cliaddr.sin_port));
			str_echo(connfd);
			exit(EXIT_SUCCESS);
		}
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
