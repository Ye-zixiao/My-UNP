#include "MyUNP.h"

int main(void)
{
	char buf[MAXLINE];
	ssize_t nread;

	if ((nread = readline1(STDIN_FILENO, buf, MAXLINE)) < 0)
		err_sys("readline1 error");
	printf("readline1 read %ld chars from stdin:\n", nread);
	printf("%s", buf);
	exit(EXIT_SUCCESS);
}