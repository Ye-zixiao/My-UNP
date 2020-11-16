/**
 * 判断主机字节序（小端/大端）
 */
#include "MyUNP.h"
#include <sys/utsname.h>

union endian {
	short sval;
	char ch[sizeof(short)];
};

int main(int argc, char* argv[])
{
	struct utsname myuname;
	union endian un;

	un.sval = 0x0102;
	if (uname(&myuname) == -1)
		err_sys("uname error");
	printf("System arch: %s %s\n", myuname.sysname, myuname.machine);
	if (sizeof(short) == 2) {
		if (un.ch[0] == 1 && un.ch[1] == 2)
			printf("big endian\n");
		else if (un.ch[0] == 2 && un.ch[1] == 1)
			printf("little-endian\n");
		else
			printf("unkown\n");
	}
	else {
		printf("sizeof(short): %ld\n", sizeof(short));
	}

	exit(EXIT_SUCCESS);
}