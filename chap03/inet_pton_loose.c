#include "MyUNP.h"

#define IPV4LEN sizeof(struct in_addr)
#define IPV6LEN sizeof(struct in6_addr)
#define IPV4_IN6FLEN 2

//IPv4映射的IPv6地址中的IPv4地址的起始位置
#define IPV4_IN6START(addr) \
	((addr) + IPV6LEN - IPV4LEN)

//IPv4映射的IPv6地址中的FFFF部分的起始位置
#define IPV4_IN6FSTART(addr) \
	((addr) + IPV6LEN - IPV4LEN - IPV4_IN6FLEN)

//获取套接字地址结构中存储IP地址的成员地址
#define SOCKADDR(af,ss)                         \
	(af==AF_INET?                                   \
		(void*)&(((struct sockaddr_in*)&ss)->sin_addr):	    \
		(void*)&(((struct sockaddr_in6*)&ss)->sin6_addr))



/* IPv4/IPv6表达形式地址字符串（允许非规范型）
	转换为二进制地址形式 */
int inet_pton_loose(int af, const char* src, void* dst) {
	int err;

	if ((err = inet_pton(af, src, dst)) < 0)
		return -1;
	else if (err == 0) {
		if (af == AF_INET) {
			if (inet_aton(src, (struct in_addr*)dst) == 0)
				return -2;
		}
		else if (af == AF_INET6) {
			/* 在IPv4映射的IPv6地址中，其128位高80位都为0，次16位全为1，
				低32位为IPv4地址。且需要注意二进制地址是网络字节序的 */
			memset(dst, 0, IPV6LEN);
			if (inet_aton(src, (struct in_addr*)IPV4_IN6START(dst)) == 0)
				return -2;
			memset(IPV4_IN6FSTART(dst), 0xFF, IPV4_IN6FLEN);
		}
		else {
			errno = EAFNOSUPPORT;
			return -1;
		}
	}
	return 0;
}


int main(int argc, char* argv[])
{
	struct sockaddr_storage ss;
	char buf[INET6_ADDRSTRLEN];
	int af_family = AF_INET6;
	int opt, t;

	opterr = 0;
	while ((opt = getopt(argc, argv, "hi:")) != -1) {
		switch (opt) {
		case 'h':
			err_quit("usage: %s [-i inet_type] ip-address\n"
				"          [-h]\n"
				"i inet-type, can be 4 or 6, default 6\n"
				"h for help\n", argv[0]);
		case 'i':
			af_family = ((t = atoi(optarg)) == 4 ? AF_INET :
				t == 6 ? AF_INET6 : AF_UNSPEC);
			break;
		case '?':
			err_quit("wrong option: -%c\n", optopt);
		}
	}

	if ((t = inet_pton_loose(af_family, argv[argc - 1],
			SOCKADDR(af_family, ss))) < 0) {
		if (t == -2)
			err_quit("No input ip-address");
		err_sys("inet_pton_loose error");
	}
	if (inet_ntop(af_family, SOCKADDR(af_family, ss), buf, sizeof(buf)) <= 0)
		err_sys("inet_ntop error");
	printf("standard address: %s\n", buf);
	exit(EXIT_SUCCESS);
}



/*
实验结果：

# yexinhai @ Localhost in ~/projects/UNP [21:16:22]
$ ./main -i4 0xe
standard address: 0.0.0.14

# yexinhai @ Localhost in ~/projects/UNP [21:16:29]
$ ./main -i6 192.168.1.223
standard address: ::ffff:192.168.1.223

*/
