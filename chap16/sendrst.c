#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	struct linger ling;
	int sockfd;

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	ling.l_onoff = 1;
	ling.l_linger = 0;
	if (setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) == -1)
		err_sys("setsockopt error");
	if (close(sockfd) == -1)
		err_sys("close error");
		
	exit(EXIT_SUCCESS);
}
