#include "MyUNP.h"

/* 从客户进程中读取文本行并简单回射 */
void str_echo(int sockfd) {
	ssize_t nread;
	char buf[MAXLINE];

again:
	while ((nread = readline(sockfd, buf, MAXLINE)) > 0) {
		if (write(sockfd, buf, nread) != nread)
			err_sys("write error");
	}
	if (nread < 0 && errno == EINTR)
		goto again;
	else if (nread < 0)
		err_sys("readline error");
}


/* 将用户输入的文本行发送给服务进程然后读取回射的内容 */
void str_cli(int sockfd, FILE* fp) {
	char sendline[MAXLINE], recvline[MAXLINE];
	size_t len; ssize_t nread;

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		len = strlen(sendline);
		if (write(sockfd, sendline, len) != len)
			err_sys("write error");

		if ((nread = readline(sockfd, recvline, MAXLINE)) == -1)
			err_sys("readline error");
		/* 若客户收到了EOF，则说明是服务器执行了主动关闭的工作，
			显然这是不正常的。执行主动关闭的应该是客户端 */
		else if (nread == 0)
			err_quit("str_cli: server terminated prematurely");

		fputs(recvline, stdout);
		if (ferror(stdout))
			err_sys("ferror error");
	}
	if (ferror(fp))
		err_sys("fgets error");
}