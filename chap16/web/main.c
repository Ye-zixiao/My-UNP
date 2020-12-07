#include "web.h"

struct file file[MAXFILES];

int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
fd_set rset, wset;


int main(int argc, char* argv[])
{
	int i, fd, n, maxnconn, flags, error;
	char buf[MAXLINE];
	fd_set rs, ws;

	if (argc < 4)
		err_quit("usage: web <conns> <hostname> <homepage> <file1> ...");
	maxnconn = atoi(argv[1]);//最大试图建立并发数
	nfiles = MIN(argc - 4, MAXFILES);
	for (i = 0; i < nfiles; ++i) {
		file[i].f_name = argv[i + 4];
		file[i].f_host = argv[2];
		file[i].f_flags = 0;
	}
	printf("nfiles = %d\n", nfiles);
	home_page(argv[2], argv[3]);


	FD_ZERO(&rset);
	FD_ZERO(&wset);
	maxfd = -1;
	nlefttoread = nlefttoconn = nfiles;
	nconn = 0;

	while (nlefttoread > 0) {
		/* 若现处于F_CONNECTING的连接数未达到最大并发数，
			且还有请求没有执行TCP连接 */
		while (nconn < maxnconn && nlefttoconn > 0) {
			for (i = 0; i < nfiles; ++i)
				if (file[i].f_flags == 0)break;
			if (i == nfiles)
				err_quit("nlefttoconn = %d but nothing found", nlefttoconn);
			start_connect(&file[i]);
			nconn++;
			nlefttoconn--;
		}

		/* 使用select监控相关处于连接待建立、欲读取、
			欲发送的套接字描述符 */
		rs = rset; ws = wset;
		if ((n = select(maxfd + 1, &rs, &ws, NULL, NULL)) == -1)
			err_sys("select error");

		for (i = 0; i < nfiles; ++i) {
			flags = file[i].f_flags;
			if (flags == 0 || flags & F_DONE)
				continue;

			fd = file[i].f_fd;
			/* 当前套接字处于连接待建立状态（但有可能发生错误，故需要获取待处理错误） */
			if (flags & F_CONNECTING &&
				(FD_ISSET(fd, &rs) || FD_ISSET(fd, &ws))) {
				n = sizeof(error);
				if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&n) == -1 || error != 0)
					err_ret("nonblocking connect failed for %s", file[i].f_name);

				printf("connection established for %s\n", file[i].f_name);
				FD_CLR(fd, &wset);
				write_get_cmd(&file[i]);
			}
			/* 当前套接字处理待读取状态 */
			else if (flags & F_READING && FD_ISSET(fd, &rs)) {
				if ((n = read(fd, buf, MAXLINE)) == -1)
					err_sys("read error");
				else if (n == 0) {
					printf("end-of-file on %s\n", file[i].f_name);
					if (close(fd) == -1)
						err_sys("close error");
					file[i].f_flags = F_DONE;
					FD_CLR(fd, &rset);
					nconn--;
					nlefttoread--;
				}
				else
					printf("read %d bytes from %s\n", n, file[i].f_name);
			}
		}
	}

	exit(EXIT_SUCCESS);
}
