#include "MyUNP.h"

#define SERV_PORT 12000

int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
	int sockfd;

	if (argc != 2)
		err_sys("usage: %s <IPAddress>", argv[0]);

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	if (inet_pton(AF_INET, argv[1], &svaddr.sin_addr) <= 0)
		err_sys("inet_pton error");
	dg_clix(sockfd, stdin, (const struct sockaddr*)&svaddr, sizeof(svaddr));

	exit(EXIT_SUCCESS);
}


/**
 我通过将UDP的接收缓冲区设置为10×1024×sizeof(int)大小，以此来模拟UDP套接字接收区
 被淹没的情景，然后执行如下的命令：

# yexinhai @ Localhost in ~/My-UNP/chap08 on git:master x [21:29:43]
$ ./udpsrvx &
[1] 11823

# yexinhai @ Localhost in ~/My-UNP/chap08 on git:master x [21:29:49]
$ netstat -s -u |grep buffer
    3950 receive buffer errors
    0 send buffer errors

# yexinhai @ Localhost in ~/My-UNP/chap08 on git:master x [21:29:51]
$ ./udpclix 192.168.1.103

# yexinhai @ Localhost in ~/My-UNP/chap08 on git:master x [21:29:56]
$ fg
[1]  + 11823 running    ./udpsrvx
^C
received 1987 datagrams

# yexinhai @ Localhost in ~/My-UNP/chap08 on git:master x [21:29:58]
$ netstat -s -u |grep buffer
    3963 receive buffer errors
    0 send buffer errors
从上面的结果可以看到，接收UDP因为数据报的淹没而最终导致丢弃了13个数据包


 */
