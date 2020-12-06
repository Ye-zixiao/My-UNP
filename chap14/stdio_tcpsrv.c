#include "MyUNP.h"


int main(void)
{
	struct sockaddr_in svaddr;
	int sockfd, connfd;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	svaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	if (listen(sockfd, 20) == -1)
		err_sys("listen error");

	for (;;) {
		if ((connfd = accept(sockfd, NULL, NULL)) == -1)
			err_sys("accept error");

		str_echo2(connfd);
		if (close(connfd) == -1)
			err_sys("close error");
	}
}
