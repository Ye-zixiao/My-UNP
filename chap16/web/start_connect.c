#include "web.h"


/**
 * 根据fptr指向的内容（主机+请求文件名）向web服务器发起TCP连接请求。
 * 若调用connect后连接立即建立则调用write_get_cmd向服务器发送GET请
 * 求；否则加入select监控文件描述符集
 */
void start_connect(struct file* fptr) {
	struct addrinfo* ai;
	int sockfd, n;

	if ((ai = host_serv(fptr->f_host, SERV, AF_INET/*0*/, SOCK_STREAM)) == NULL)
		err_sys("host_serv error");

	if ((sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1)
		err_sys("socket error");
	fptr->f_fd = sockfd;
	printf("start_connect for %s, fd %d\n", fptr->f_name, sockfd);

	if (set_fl(sockfd, O_NONBLOCK) == -1)
		err_sys("set_fl error");

	if ((n = connect(sockfd, ai->ai_addr, ai->ai_addrlen)) == -1) {
		if (errno != EINPROGRESS)
			err_sys("nonblocking connect error");
		//若连接现没有建立，则使用select进行监控
		fptr->f_flags = F_CONNECTING;
		FD_SET(sockfd, &rset);
		FD_SET(sockfd, &wset);
		if (sockfd > maxfd) maxfd = sockfd;
	}
	else if (n >= 0)
		write_get_cmd(fptr);
}
