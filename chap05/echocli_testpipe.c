/**
 * 测试客户端向一个已收到RST分节的套接字继续写数据
 * 而引发SIGPIPE信号的情况
 */

#include "MyUNP.h"

static void str_cli_testpipe(int sockfd, FILE* fp) {
	char sendline[MAXLINE], recvline[MAXLINE];
	ssize_t nread; size_t len;

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		len = strlen(sendline);
		if (write(sockfd, sendline, 1) != 1)
			err_sys("write error");
		sleep(1);
		if (write(sockfd, sendline + 1, len - 1) != (len - 1))
			err_sys("write error");
		if ((nread = readline(sockfd, recvline, MAXLINE)) == -1)
			err_sys("readline error");
		else if (nread == 0)
			err_quit("str_cli_testpipe error: server prematurely terminated");
		
		fputs(recvline, stdout);
		if (ferror(stdout))
			err_sys("fputs error");
	}
	if (ferror(fp))
		err_sys("ferror");
}


int main(int argc, char* argv[])
{
	struct sockaddr_in svaddr;
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

	str_cli_testpipe(sockfd, stdin);
	exit(EXIT_SUCCESS);
}


/*
实验结果：
$ ./main 192.168.137.215
fdjk
fdjk
jsfd

# yexinhai @ Localhost in ~/projects/UNP [21:33:51] C:141

从上面可以看到shell返回了141（即SIGPIPE(13)+128）

*/
