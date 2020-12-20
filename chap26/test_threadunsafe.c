#include "MyUNP.h"

/* 用来测试当服务进程使用非线程安全版本的readline()函数
所造成的错误，我在同一个目录中提供了一个文本tale.txt来进行测试 */

int main(int argc, char* argv[])
{
	int sockfd, infd, outfd, count;
	char tempfile[SBUFSIZE];
	char* infile;
	pid_t pid;
	
	if (argc < 5)
		err_quit("usage: %s <host/ip> <serv/port> <file> <count>", argv[0]);

	count = atoi(argv[argc - 1]);
	infile = argv[argc - 2];
	for (int i = 0; i < count; ++i) {
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid == 0) {
			sockfd = tcp_connect(argv[1], argv[2]);

			if ((infd = open(infile, O_RDONLY)) == -1)
				err_sys("open error");
			strcpy(tempfile, infile);
			strcat(tempfile, ".XXXXXX");
			if ((outfd = mkstemp(tempfile)) == -1)
				err_sys("mkstemp error");

			str_cli3(sockfd, fdopen(infd, "r"), fdopen(outfd, "w"));
			printf("child %d finished\n", i);
			exit(EXIT_SUCCESS);
		}
	}

	exit(EXIT_SUCCESS);
}