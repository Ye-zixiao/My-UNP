#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd, val;
	socklen_t len;

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

	len = sizeof(int);
	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	printf("listen socket's receive buffer size: %d\n", val);
	exit(EXIT_SUCCESS);
}