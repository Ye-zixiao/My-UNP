#include "MyUNP.h"

/*
 * Test program for getaddrinfo() and getnameinfo().
 * 我这里相比于官方的程序去掉了测试错误码的部分
 */

 /* function prototypes for internal functions */
static int	do_onetest(char*, char*, struct addrinfo*, int);
static const char* str_fam(int);
static const char* str_sock(int);
static void	usage(const char*);
static char* Sock_ntop1(const struct sockaddr*, socklen_t);


/* globals */
int		vflag;


int
main(int argc, char** argv)
{
	int				loopcount = 1;
	int				c, i;
	char* host = NULL;
	char			hostbuf[NI_MAXHOST];
	char* serv = NULL;
	char			servbuf[NI_MAXSERV];
	struct protoent* proto;
	struct addrinfo	hints;		/* set by command-line options */

	if (argc < 2)
		usage("");

	memset(&hints, 0, sizeof(struct addrinfo));

	opterr = 0;		/* don't want getopt() writing to stderr */
	while ((c = getopt(argc, argv, "cef:h:l:pr:s:t:v")) != -1) {
		switch (c) {
		case 'c':
			hints.ai_flags |= AI_CANONNAME;
			break;

		case 'f':			/* 地址族 */
			if (strcmp(optarg, "inet") == 0) {
				hints.ai_family = AF_INET;
				break;
			}
			if (strcmp(optarg, "inet6") == 0) {
				hints.ai_family = AF_INET6;
				break;
			}
			if (strcmp(optarg, "unix") == 0) {
				hints.ai_family = AF_UNIX;
				break;
			}
			usage("invalid -f option");

		case 'h':			/* 主机名 */
			strncpy(hostbuf, optarg, NI_MAXHOST - 1);
			host = hostbuf;
			break;

		case 'l':			/* 循环次数 */
			loopcount = atoi(optarg);
			break;

		case 'p':
			hints.ai_flags |= AI_PASSIVE;
			break;

		case 'r':			/* 协议类型 */
			if ((proto = getprotobyname(optarg)) == NULL) {
				hints.ai_protocol = atoi(optarg);
			}
			else {
				hints.ai_protocol = proto->p_proto;
			}
			break;

		case 's':			/* 服务类型 */
			strncpy(servbuf, optarg, NI_MAXSERV - 1);
			serv = servbuf;
			break;

		case 't':			/* 套接字类型 */
			if (strcmp(optarg, "stream") == 0) {
				hints.ai_socktype = SOCK_STREAM;
				break;
			}

			if (strcmp(optarg, "dgram") == 0) {
				hints.ai_socktype = SOCK_DGRAM;
				break;
			}

			if (strcmp(optarg, "raw") == 0) {
				hints.ai_socktype = SOCK_RAW;
				break;
			}

			if (strcmp(optarg, "rdm") == 0) {
				hints.ai_socktype = SOCK_RDM;
				break;
			}

			if (strcmp(optarg, "seqpacket") == 0) {
				hints.ai_socktype = SOCK_SEQPACKET;
				break;
			}
			usage("invalid -t option");

		case 'v':		/* 详细信息 */
			vflag = 1;
			break;

		case '?':
			usage("unrecognized option");
		}
	}
	if (optind < argc) {
		usage("extra args");
	}

	for (i = 1; i <= loopcount; i++) {
		if (do_onetest(host, serv, &hints, i) > 0)
			exit(1);

		if (i % 1000 == 0) {
			printf(" %d", i);
			fflush(stdout);
		}
	}

	exit(0);
}


/* 真正负责对用户输入的命令行信息进行解析，并输出相应获取到的协议地址 */
static int
do_onetest(char* host, char* serv, struct addrinfo* hints, int iteration)
{
	int				rc, fd, verbose;
	struct addrinfo* res, * rescopy;
	char			rhost[NI_MAXHOST], rserv[NI_MAXSERV];

	verbose = vflag && (iteration == 1);	/* only first time */

	if (host != NULL && verbose)
		printf("host = %s\n", host);
	if (serv != NULL && verbose)
		printf("serv = %s\n", serv);

	rc = getaddrinfo(host, serv, hints, &res);
	if (rc != 0) {
		printf("getaddrinfo return code = %d (%s)\n", rc, gai_strerror(rc));
		return(1);
	}

	rescopy = res;
	do {
		if (iteration == 1) {	/* 在第一次中输出结果信息，后面不再输出 */
			printf("\nsocket(%s, %s, %d)", str_fam(res->ai_family),
				str_sock(res->ai_socktype), res->ai_protocol);

			/* canonname should be set only in first addrinfo{} */
			if (hints->ai_flags & AI_CANONNAME) {
				if (res->ai_canonname)
					printf(", ai_canonname = %s", res->ai_canonname);
			}
			printf("\n");

			printf("\taddress: %s\n",
				Sock_ntop1(res->ai_addr, res->ai_addrlen));
		}

		//尝试进行套接字创建
		/* Call socket() to make sure return values are valid */
		fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (fd < 0)
			printf("call to socket() failed!\n");
		else
			close(fd);

		/*
		 * 测试反向映射
		 * Call getnameinfo() to check the reverse mapping.
		 */
		rc = getnameinfo(res->ai_addr, res->ai_addrlen,
			rhost, NI_MAXHOST, rserv, NI_MAXSERV,
			(res->ai_socktype == SOCK_DGRAM) ? NI_DGRAM : 0);
		if (rc == 0) {
			if (verbose)
				printf("\tgetnameinfo: host = %s, serv = %s\n",
					rhost, rserv);
		}
		else
			printf("getnameinfo returned %d (%s)\n", rc, gai_strerror(rc));

	} while ((res = res->ai_next) != NULL);

	freeaddrinfo(rescopy);
	return(0);
}


static void usage(const char* msg)
{
	printf(
		"usage: testaddrinfo [ options ]\n"
		"options: -h <host>    (can be hostname or address string)\n"
		"         -s <service> (can be service name or decimal port number)\n"
		"         -c    AI_CANONICAL flag\n"
		"         -p    AI_PASSIVE flag\n"
		"         -l N  loop N times (check for memory leaks with ps)\n"
		"         -f X  address family, X = inet, inet6, unix\n"
		"         -r X  protocol, X = tcp, udp, ... or number e.g. 6, 17, ...\n"
		"         -t X  socket type, X = stream, dgram, raw, rdm, seqpacket\n"
		"         -v    verbose\n"
	);

	if (msg[0] != 0)
		printf("%s\n", msg);
	exit(1);
}


/* 返回地址族字符串 */
static const char* str_fam(int family)
{
	if (family == AF_INET)
		return("AF_INET");
	if (family == AF_INET6)
		return("AF_INET6");
	if (family == AF_LOCAL)
		return("AF_LOCAL");
	return("<unknown family>");
}


/* 返回套接字类型字符串 */
static const char* str_sock(int socktype)
{
	switch (socktype) {
	case SOCK_STREAM:	return "SOCK_STREAM";
	case SOCK_DGRAM:	return "SOCK_DGRAM";
	case SOCK_RAW:		return "SOCK_RAW";
#ifdef SOCK_RDM
	case SOCK_RDM:		return "SOCK_RDM";
#endif
#ifdef SOCK_SEQPACKET
	case SOCK_SEQPACKET:return "SOCK_SEQPACKET";
#endif
	default:		return "<unknown socktype>";
	}
}


/* 转换成易读地址字符串 */
static char*
sock_ntop1(const struct sockaddr* sa, socklen_t salen)
{
	char		portstr[8];
	static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in* sin = (struct sockaddr_in*)sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}
				/* end sock_ntop */

	case AF_INET6: {
		struct sockaddr_in6* sin6 = (struct sockaddr_in6*)sa;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}

	case AF_UNIX: {
		struct sockaddr_un* unp = (struct sockaddr_un*)sa;

		/* OK to have no pathname bound to the socket: happens on
		   every connect() unless client calls bind() first. */
		if (unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}

	default:
		snprintf(str, sizeof(str), "sock_ntop1: unknown AF_xxx: %d, len %d",
			sa->sa_family, salen);
		return(str);
	}
	return (NULL);
}


static char* Sock_ntop1(const struct sockaddr* sa, socklen_t salen)
{
	char* ptr;

	if ((ptr = sock_ntop1(sa, salen)) == NULL)
		err_sys("sock_ntop1 error");	/* inet_ntop() sets errno */
	return(ptr);
}