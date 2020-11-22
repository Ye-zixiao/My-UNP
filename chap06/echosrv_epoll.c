/**
 * 照着poll例子仿照出的使用epoll回射服务器（采用默认的水平触发）
 */
#include "MyUNP.h"

#define SERV_PORT 12000
#define MAX_EVENTS 10

int main(int argc, char* argv[])
{
	int nfds, listenfd, connfd, sockfd;
	struct epoll_event ev, events[MAX_EVENTS];
	struct sockaddr_in svaddr, cliaddr;
	int epfd, val = 1;
	char buf[MAXLINE];
	socklen_t len;
	ssize_t nread;


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


	/* 创建epoll对象，并将监听套接字加入到epoll的感兴趣列表之中 */
	if ((epfd = epoll_create1(0)) == -1)
		err_sys("epoll_create1 error");
	ev.data.fd = listenfd;
	ev.events = EPOLLIN;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
		err_sys("epoll_ctl error");

	for (;;) {
		if ((nfds = epoll_wait(epfd, events, MAX_EVENTS, -1)) == -1)
			err_sys("epoll_wait error");

		for (int i = 0; i < nfds; ++i) {
			sockfd = events[i].data.fd;

			/* 若epoll返回的struct epoll_event数组指出监听套接字描述符变得可用，
				则说明有新的连接需要服务进程接受 */
			if (sockfd == listenfd) {
				len = sizeof(cliaddr);
				if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1)
					err_sys("accept error");
				printf("%s: connection from %s\n", currtime("%Y-%m-%d %T"),
					sock_ntop((struct sockaddr*)&cliaddr, len));

				ev.data.fd = connfd;
				ev.events = EPOLLIN;//
				if (epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev) == -1)
					err_sys("epoll_ctl1 error");
			}
			/* 否则处理的就是客户相关的连接套接字描述符变得可用，此时对它们
				做出响应 */
			else {
				if ((nread = read(sockfd, buf, MAXLINE)) == -1) {
					if (errno = ECONNRESET) {
						if (epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL) == -1)
							err_sys("epoll_ctl2 error");
						if (close(sockfd) == -1)
							err_sys("close error");
					}
					else
						err_sys("read error");
				}
				else if (nread == 0) {
					if (epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL) == -1)
						err_sys("epoll_ctl3 error");
					if (close(sockfd) == -1)
						err_sys("close error");
				}
				else {
					if (write(sockfd, buf, nread) != nread)
						err_sys("write error");
				}
			}
		}
	}
}