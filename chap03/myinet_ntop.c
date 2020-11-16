#include "MyUNP.h"


/**
 * 简单实现inet_pton的IPv4版本
 */
int myinet_pton(int family, const char* str, void* addrptr) {
	if (family == AF_INET) {
		struct in_addr in_val;
		
		if (inet_aton(str, &in_val)) {
			memcpy(addrptr, &in_val, sizeof(struct in_addr));
			return 1;
		}
		return 0;
	}
	errno = EAFNOSUPPORT;
	return -1;
}

/**
 * 简单实现inet_ntop的IPv6版本
 */
char * myinet_ntop(int family, void* addrptr, char* str, size_t slen) {
	const unsigned char* p = addrptr;

	if (family == AF_INET) {
		char buf[INET_ADDRSTRLEN];

		snprintf(buf, sizeof(buf), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
		if (slen < sizeof(buf)) {
			errno = ENOSPC;
			return NULL;
		}
		memcpy(str, buf, sizeof(buf));
		return str;
	}
	errno = EAFNOSUPPORT;
	return NULL;
}


int main(int argc, char* argv[])
{
	char buf[INET_ADDRSTRLEN];
	struct in_addr in;

	in.s_addr = INADDR_ANY;
	printf("INET_ADDRANY: %s\n", 
		myinet_ntop(AF_INET, &in, buf, INET_ADDRSTRLEN));
	exit(EXIT_SUCCESS);
}
