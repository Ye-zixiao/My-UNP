#include "../include/MyUNP.h"

int main(void)
{
	int sockfd, recvbuf;
	socklen_t len;

	len = sizeof(int);
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuf, &len) == -1)
		err_sys("getsockopt error");
	printf("recvbuf size: %d\n", recvbuf);
	exit(EXIT_SUCCESS);
}
