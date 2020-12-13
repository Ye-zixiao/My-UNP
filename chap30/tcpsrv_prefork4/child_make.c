#include "child.h"


/* 创建一个未命名UNIX域套接字，然后创建子进程，通过这个UNIX
	域套接字连通子进程和父进程。其中一些无用的描述符关闭 */
pid_t child_make(int i, int listenfd, socklen_t addrlen) {
	int sockfds[2];
	pid_t pid;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfds) == -1)
		err_sys("socketpair error");
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) {
		if (mysignal(SIGINT, SIG_DFL) == SIG_ERR)
			err_sys("mysignal error");

		if (dup2(sockfds[1], STDERR_FILENO) == -1)
			err_sys("dup2 error");
		if (close(sockfds[0]) == -1)
			err_sys("close error");
		if (close(sockfds[1]) == -1)
			err_sys("close error");
		if (close(listenfd) == -1)
			err_sys("close error");

		child_main(i, listenfd, addrlen);
	}
	if (close(sockfds[1]) == -1)
		err_sys("close error");
	cptr[i].child_pid = pid;
	cptr[i].child_pipefd = sockfds[0];
	cptr[i].child_status = 0;
	return pid;
}