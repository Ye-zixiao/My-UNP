/**
 * 感觉除了本地/etc/hosts文件中的地址，什么也查不了？🤔
 */
#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct hostent* hptr;
	struct in_addr addr;

	if (argc != 2)
		err_quit("usage: %s <IPAddress>", argv[0]);
	
	if (inet_pton(AF_INET, argv[1], &addr) <= 0)
		err_sys("inet_pton error for addr: %s", argv[1]);
	if ((hptr = gethostbyaddr(&addr, 4, AF_INET)) == NULL)
		err_quit("gethostbyaddr error for host: %s : %s",
			argv[1], hstrerror(h_errno));
	printf("official name: %s\n", hptr->h_name);
	exit(EXIT_SUCCESS);
}