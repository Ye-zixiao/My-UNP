#include "MyUNP.h"


/**
 * 从指定套接字读取指定字节数的数据
 */
ssize_t readn(int fd, void* buf, size_t nbytes) {
	size_t nleft;
	ssize_t nread;
	char* ptr;

	ptr = buf;
	nleft = nbytes;
	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else return -1;
		}
		else if (nread == 0)
			break;

		nleft -= nread;
		ptr += nread;
	}
	return nbytes - nleft;
}


/**
 * 向指定套接字写入指定字节数的数据
 */
ssize_t writen(int fd, const void* buf, size_t nbytes) {
	size_t nleft;
	ssize_t nwrite;
	const char* ptr;

	ptr = buf;
	nleft = nbytes;
	while (nleft > 0) {
		if ((nwrite = write(fd, ptr, nleft)) <= 0) {
			if (nwrite < 0 && errno == EINTR)
				nwrite = 0;
			else return -1;
		}

		nleft -= nwrite;
		ptr += nwrite;
	}
	return nbytes - nleft;
}


/**
 * 从指定文件描述符中读取一行数据，若没有遇到换行符则该函数会自动
 * 对最后一个字符进行填充'\n'。不过这个函数执行效率非常低
 */
ssize_t readline(int fd, void* buf, size_t maxlen) {
	ssize_t nread, n;
	char c, * ptr;

	ptr = buf;
	/* i从1计数是为了预留一个字符以在buf不能容纳的时候 
		在最后面放置一个'\n' */
	for (n = 1; n < maxlen; ++n) {
again:
		if ((nread = read(fd, &c, 1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		//遇到文件结束符EOF
		else if (nread == 0) {
			*ptr = 0;
			return n - 1;
		}
		else {
			if (errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = '\0';
	return n;
}