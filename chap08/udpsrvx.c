﻿/** 
 * 由于自己的局域网内没有什么慢速的unix设备,所以实际上按照书中的程序
 * 很难出现什么问题
 */
#include "MyUNP.h"

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr, cliaddr;
	int  sockfd;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(12000);
	svaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	dg_echox(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
}
