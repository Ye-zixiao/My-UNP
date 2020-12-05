#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc < 2)
		err_quit("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	
	dg_clit2(sockfd, stdin, (const struct sockaddr*)&svaddr, sizeof(svaddr));
	exit(EXIT_SUCCESS);
}
