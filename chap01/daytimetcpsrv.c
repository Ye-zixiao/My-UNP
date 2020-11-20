#include "../include/MyUNP.h"

int main(int argc, char* argv[])
{
	int listenfd, connfd, val = 1;
	struct sockaddr_in svaddr;
	char buf[MAXLINE];
	time_t currTime;

	/* 完成socket创建-地址绑定-监听一条龙操作 */
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val,
			sizeof(int)) == -1)
		err_sys("setsockopt error");
	bzero(&svaddr, sizeof(struct sockaddr_in));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(9999);
	svaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listenfd, (struct sockaddr*)&svaddr,
			sizeof(struct sockaddr_in)) == -1)
		err_sys("bind error");
	if (listen(listenfd, LISTENQ) == -1)
		err_sys("listen error");

	for (;;) {
		if ((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1)
			err_sys("accept error");

		currTime = time(NULL);
		snprintf(buf, MAXLINE, "%.24s\r\n", ctime(&currTime));
		if (write(connfd, buf, strlen(buf)) != strlen(buf))
			err_sys("write error");
		//逐字节传送给客户进程
		/*for (int i = 0; i < strlen(buf); ++i)
			if (write(connfd, &buf[i], 1) != 1)
				err_sys("write error");
		*/
		//在这个服务器中执行主动关闭的是服务进程
		if (close(connfd) == -1)
			err_sys("close connfd error");
	}
}
