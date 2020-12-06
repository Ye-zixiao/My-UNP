#include "MyUNP.h"


/**
 * 使用select+非阻塞套接字实现connect_timeo()函数相同的功能，使得用户
 * 可以自定义超时的时间，而不是使用connect默认超时时间，从而避免长时间
 * （可能无效）的连接建立等待
 */
int connect_nblk(int sockfd, 
		const struct sockaddr* svaddr, socklen_t svlen, time_t nsec) {
	int flags, n, error;
	struct timeval tv;
	fd_set rset, wset;
	socklen_t len;

	/* 将套接字设置为非阻塞模式 */
	if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1)
		err_sys("fcntl error");
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
		err_sys("fcntl error");

	error = 0;
	if ((n = connect(sockfd, svaddr, svlen)) == -1) {
		if (errno != EINPROGRESS)
			return -1;
	}
	//若TCP立即完成建立，则跳到goto
	else if (n == 0) goto done;


	/* 使用select函数来完成对TCP连接建立的超时记录，并检查返回时
		是否有错误，若有待处理错误，取之设置errno */
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tv.tv_sec = nsec;
	tv.tv_usec = 0;
	if ((n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tv : NULL)) == 0) {
		close(sockfd);
		errno = ETIMEDOUT;
		return -1;
	}
	if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
			return -1;
	}
	else
		err_quit("select error: sockfd not set");

done:
	if (fcntl(sockfd, F_SETFL, flags) == -1)
		err_sys("fcntl error");
	if (error) {
		close(sockfd);
		errno = error;
		return -1;
	}
	return 0;
}