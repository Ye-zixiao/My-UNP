#include "MyUNP.h"

#define SERV_PORT 12000


int main(int argc, char* argv[])
{
	int i, maxi, listenfd, connfd, sockfd;
	struct sockaddr_in svaddr, cliaddr;
	struct pollfd client[OPEN_MAX];
	int nready, val = 1;
	char buf[MAXLINE];
	ssize_t nread;
	socklen_t len;

	
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
	if (listen(listenfd, 20) == -1)
		err_sys("listen error");

	//初始化pollfd数组
	client[0].fd = listenfd;
	client[0].events = POLLIN;
	for (int i = 1; i < OPEN_MAX; ++i)
		client[i].fd = -1;
	maxi = 0;

	for (;;) {
		if ((nready = poll(client, maxi + 1, -1)) == -1)
			err_sys("poll error");

		/* 接受客户的连接请求，并将新生成的连接套接字加入的client数组
			之中，方便后续让poll进行监控 */
		if (client[0].revents & POLLIN) {
			len = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1) {
				if (errno == EINTR) continue;
				err_sys("accept error");
			}
			printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
				sock_ntop((struct sockaddr*)&cliaddr, sizeof(cliaddr)));

			for (i = 0; i < OPEN_MAX; ++i)
				if (client[i].fd < 0) {
					client[i].fd = connfd;
					client[i].events = POLLIN;
					break;
				}
			if (i == OPEN_MAX)err_quit("too many client");

			if (i > maxi) maxi = i;
			if (--nready <= 0) continue;
		}

		/* 遍历小于索引maxi下的所有pollfd数组，检查是否有与客户相关的连接
		   套接字描述符变得可用，若在可用时做出相关处理。
		       注意：maxi在执行的过程中只增不减 */
		for (i = 1; i <= maxi; ++i) {
			if ((sockfd = client[i].fd) < 0 ||
				!(client[i].revents & (POLLIN | POLLERR)))
				continue;
			else {
				if ((nread = read(sockfd, buf, MAXLINE)) == -1) {
					//若客户发送了RST分节，则移除掉这个连接套接字
					if (errno = ECONNRESET) {
						if (close(sockfd) == -1)
							err_sys("close error");
						client[i].fd = -1;
					}
					else err_sys("read error");
				}
				else if (nread == 0) {
					if (close(sockfd) == -1)
						err_sys("close error");
					client[i].fd = -1;
				}
				else {
					if (write(sockfd, buf, nread) != nread)
						err_sys("write error");
				}

				if (--nready <= 0)
					break;
			}
		}
	}

}
