#include "MyUNP.h"

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc != 2)
		err_sys("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(19);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	if(shutdown(sockfd, SHUT_RD) == -1)
		err_sys("shutdown error");
	//这里使用wireshark进行抓包
	pause();
	exit(0);
}
