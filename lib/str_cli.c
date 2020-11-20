#include "MyUNP.h"


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


/**
 * 使用select函数防止客户进程阻塞在某一个特定的描述符上(特别指
 * 阻塞在STDIN_FILENO上)，从而避免向一个因特殊原因终止的服务子
 * 进程继续发送数据
 */
void str_cli1(int sockfd, FILE* fp) {
	char sendline[MAXLINE], recvline[MAXLINE];
	ssize_t nread;
	fd_set rset;
	int maxfdp1;
	size_t len;

	FD_ZERO(&rset);
	for (;;) {
		FD_SET(fileno(stdin), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(stdin), sockfd) + 1;
		if (select(maxfdp1, &rset, NULL, NULL, NULL) == -1)
			err_sys("select error");

		if (FD_ISSET(sockfd, &rset)) {
			if ((nread = readline(sockfd, recvline, MAXLINE)) < 0)
				err_sys("readline error");
			/* 若服务进程在客户没有发出主动关闭的前提下就向客户进程发送了
				FIN分节，这说明服务进程异常的终止了，此时我们不应该继续向
				其发送数据（客户进程可以正常地终止也可以向这里一样异常退出
				） */
			else if (nread == 0)
				err_quit("str_cli: server terminated prematurely");
			if (fputs(recvline, stdout) == EOF)
				err_sys("fputs error");
		}
		if (FD_ISSET(fileno(stdin), &rset)) {
			if (fgets(sendline, MAXLINE, stdin) == NULL)
				return;
			len = strlen(sendline);
			if (write(sockfd, sendline, len) != len)
				err_sys("write error");
		}
	}
}
