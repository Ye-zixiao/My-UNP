#include "web.h"


/**
 * 向web服务器发送连接GET请求，发送完毕后将相应描述符加入到对读
 * 感兴趣的文件描述符集中。向自己刚创建的套接字写不太可能阻塞
 */
void write_get_cmd(struct file* fptr) {
	char line[MAXLINE];
	int n;

	n = snprintf(line, MAXLINE, GET_CMD, fptr->f_name);
	if (writen(fptr->f_fd, line, n) != n)
		err_sys("write error");
	printf("wrote %d bytes for %s\n", n, fptr->f_name);
	fptr->f_flags = F_READING;

	FD_SET(fptr->f_fd, &rset);
	if (fptr->f_fd > maxfd) maxfd = fptr->f_fd;
}