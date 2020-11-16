#include "MyUNP.h"

int main(int argc, char* argv[])
{
	struct sockaddr_in sin;
	struct sockaddr_in6 sin6;

	bzero(&sin, sizeof(sin));
	bzero(&sin6, sizeof(sin6));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(32);
	sin.sin_addr.s_addr = INADDR_ANY;
	sin6.sin6_family = AF_INET6;
	sin6.sin6_port = htons(54);
	sin6.sin6_addr = in6addr_any;

	printf("address(IPv4): %s\n", sock_ntop((struct sockaddr*)&sin, 
		sizeof(struct sockaddr)));
	printf("address(IPv6): %s\n", sock_ntop((struct sockaddr*)&sin6,
		sizeof(struct sockaddr_in6)));
	exit(EXIT_SUCCESS);
}