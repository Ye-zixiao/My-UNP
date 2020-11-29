#include "MyUNP.h"


/**
 * 传入一个struct in_addr的数组，并根据指定的字符串提取出一个或者
 * 多个IPv4地址存放在上述数组中
 **/
int gethost(struct in_addr* addrArr, int n, const char* name) {
	struct hostent* hptr;
	struct in_addr addr;
	char** pptr;
	int i;

	if ((hptr = gethostbyname(name)) == NULL) {
		if (inet_aton(name, &addr) == -1)
			return -1;
		addrArr[0] = addr;
		i = 1;
	}
	else {
		pptr = hptr->h_addr_list;
		for (i = 0; pptr[i] != NULL; ++i) {
			if (i >= n)return -2;
			memcpy(&addrArr[i], *pptr, sizeof(struct in_addr));
		}
	}
	return i;
}