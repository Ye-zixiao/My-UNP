#include "MyUNP.h"


/**
 * 向服务进程发送UDP数据报并试图获取回射数据 */
void dg_cli(int sockfd, FILE* fp, 
		const struct sockaddr* svaddr, socklen_t svlen) {
	char sendline[MAXLINE], recvline[MAXLINE];
	ssize_t nrecv;

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		if (sendto(sockfd, sendline, strlen(sendline), 0, svaddr, svlen) == -1)
			err_sys("sendto error");
		if ((nrecv = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) == -1)
			err_sys("recvfrom error");
		recvline[nrecv] = 0;
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (ferror(fp))
		err_sys("fgets error");
}


/* 具有验证功能的dg_cli()函数，只接收来自服务器协议地址的数据报 */
void dg_cli1(int sockfd, FILE* fp,
		const struct sockaddr* svaddr, socklen_t svlen) {
	char sendline[MAXLINE], recvline[MAXLINE];
	struct sockaddr* replyaddr;
	ssize_t nrecv;
	socklen_t len;

	if ((replyaddr = malloc(svlen)) == NULL)
		err_sys("malloc error");
	while (fgets(sendline, MAXLINE, fp) != NULL) {
		len = svlen;
		if (sendto(sockfd, sendline, strlen(sendline), 0, svaddr, len) == -1)
			err_sys("sendto error");
		if ((nrecv = recvfrom(sockfd, recvline, MAXLINE, 0, replyaddr, &len)) == -1)
			err_sys("recvfrom error");
		if (len != svlen || memcmp(replyaddr, svaddr, len) != 0) {
			printf("reply from %s (ignored)\n", sock_ntop(replyaddr, len));
			continue;
		}
		recvline[nrecv] = '\0';
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
}


/* 使用已连接UDP套接字执行发送并接收回射数据的功能 */
void dg_cli2(int sockfd, FILE* fp, 
		const struct sockaddr* svaddr, socklen_t svlen) {
	char sendline[MAXLINE], recvline[MAXLINE];
	ssize_t nrecv;

	if (connect(sockfd, svaddr, svlen) == -1)
		err_sys("connect error");
	while (fgets(sendline, MAXLINE, fp) != NULL) {
		if (write(sockfd, sendline, strlen(sendline)) == -1)
			err_sys("read error");
		if ((nrecv = read(sockfd, recvline, MAXLINE)) == -1)
			err_sys("read error");
		recvline[nrecv] = 0;
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
}


#define NDG 2000
#define DGLEN 1400

void dg_clix(int sockfd, FILE* fp,
	const struct sockaddr* svaddr, socklen_t svlen) {
	char sendline[DGLEN];

	for (int i = 0; i < NDG; ++i)
		if (sendto(sockfd, sendline, DGLEN, 0, svaddr, svlen) == -1)
			err_sys("sendto error");
}