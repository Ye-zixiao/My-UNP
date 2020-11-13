#include "MyUNP.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
		err_quit("usage: %s <message>", argv[0]);

	if (writen(STDOUT_FILENO, argv[1], strlen(argv[1]) + 1) != strlen(argv[1]) + 1)
		err_sys("writen error");
	putchar('\n');
	exit(EXIT_SUCCESS);
}
