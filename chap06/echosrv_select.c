#include "MyUNP.h"

#define SERV_PORT 12000


int main(int argc, char* argv[])
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	struct sockaddr_in svaddr, cliaddr;
	int nready, client[FD_SETSIZE];
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t len;
	ssize_t nread;
	int val = 1;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) == -1)
		err_sys("setsockopt error");
	bzero(&svaddr, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(SERV_PORT);
	svaddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(listenfd, (struct sockaddr*)&svaddr, sizeof(svaddr)) == -1)
		err_sys("bind error");
	if (listen(listenfd, 5) == -1)
		err_sys("listen error");

	/* 初始化allset文件描述符集和client数组，注意maxfd和maxi并不是
		指maxfdp1和最大client开辟(不知道用什么词)总数 */
	maxfd = listenfd; maxi = -1;
	for (int i = 0; i < FD_SETSIZE; ++i)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for (;;) {
		rset = allset;
		if ((nready = select(maxfd + 1, &rset, NULL, NULL, NULL)) == -1)
			err_sys("select error");

		//若接收到内核中的已完成连接队列非空，则让服务进程接受新连接
		if (FD_ISSET(listenfd, &rset)) {
			len = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, 
					&len)) == -1)
				err_sys("accept error");
			printf("connection from %s\n",
					sock_ntop((struct sockaddr*)&cliaddr, sizeof(cliaddr)));

			/* 将新连接套接字描述符加入到client数组和allset描述符集中，并
				更新maxfd和maxi */
			for (i = 0; i < FD_SETSIZE; ++i)
				if (client[i] < 0) {
					client[i] = connfd;
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many client");
			
			FD_SET(connfd, &allset);
			if (connfd > maxfd) maxfd = connfd;
			if (i > maxi) maxi = i;
			if (--nready <= 0) continue;
		}

		/* 若有连接套接字变得可用，则遍历client数组，以知晓哪个套接字描述符
			可用并向客户进程回射 */
		for (i = 0; i <= maxi; ++i) {
			if ((sockfd = client[i]) < 0 || !FD_ISSET(sockfd, &rset))
				continue;
			else {
				if ((nread = read(sockfd, buf, sizeof(buf))) == -1)
					err_sys("read error");
				else if (nread == 0) {
					if (close(sockfd) == -1)
						err_sys("close error");
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else {
					if (write(sockfd, buf, nread) != nread)
						err_sys("write error");
				}
				
				if (--nready <= 0) break;
			}
		}
	}
}
