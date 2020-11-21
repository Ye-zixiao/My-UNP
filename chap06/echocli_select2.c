#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc < 2)
		err_quit("usage: %s <ip-address>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton convert string %s to address error", argv[1]);
	if (connect(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("connect error");

	/* 此时的str_cli2不仅放弃了以文本行为中心的规定，
	   而且取消了缓冲IO的使用，以支持批量输入 */
	str_cli2(sockfd, stdin);
	if (close(sockfd) == -1)
		err_sys("close error");
	exit(EXIT_SUCCESS);
}
