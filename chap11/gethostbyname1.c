#include "MyUNP.h"


static const char*
Inet_ntop(int af, const void* addr, char* buf, socklen_t len) {
	if (inet_ntop(af, addr, buf, len) == NULL)
		err_sys("inet_ntop error");
	return buf;
}


int main(int argc, char* argv[])
{
	char buf[INET_ADDRSTRLEN];
	struct hostent* hptr;
	char** pptr;

	for (++argv; *argv != NULL; ++argv) {
		if ((hptr = gethostbyname(*argv)) == NULL) {
			err_msg("gethostbyname error for host: %s: %s",
				*argv, hstrerror(h_errno));
			continue;
		}

		printf("official name: %s\n", hptr->h_name);
		for (pptr = hptr->h_aliases; *pptr != NULL; ++pptr)
			printf("\talias: %s\n", *pptr);
		switch (hptr->h_addrtype) {
		case AF_INET:
			for (pptr = hptr->h_addr_list; *pptr != NULL; ++pptr) {
				printf("\taddress: %s", Inet_ntop(AF_INET, *pptr, buf, sizeof(buf)));

				hptr = gethostbyaddr((const void*)*pptr, sizeof(struct in_addr), AF_INET);
				if (hptr) printf("  hostname: %s", hptr->h_name);
				putchar('\n');
			}
			break;
		default:
			err_ret("unkown address type"); break;
		}
	}

	exit(EXIT_SUCCESS);
}
