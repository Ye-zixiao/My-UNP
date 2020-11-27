#include "../include/MyUNP.h"

#define SERV_PORT 12000

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr, myaddr;
	socklen_t len;
	int sockfd;

	if(argc != 2)
		err_quit("usage: $s <IPAddress>");

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if(inet_pton(AF_INET, argv[1], &svaddr.sin_addr) == -1)
		err_sys("inet_pton error");
	if(connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	len = sizeof(myaddr);
	if(getsockname(sockfd, (struct sockaddr*)&myaddr, &len) == -1)
		err_sys("getsockname error");
	printf("local address: %s\n",sock_ntop((struct sockaddr*)&myaddr, len));
	exit(EXIT_SUCCESS);
}
