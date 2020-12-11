#include "MyUNP.h"

#define SERVLEN 8

static void sig_alrm(int signo) {
	errno = ETIMEDOUT; return;
}


static int 
getnameinfo_timeo(const struct sockaddr* addr, socklen_t addrlen,
		char* host, socklen_t hostlen, char* serv, socklen_t servlen,
		int flags,time_t nsec) {
	int err;

	if (mysignal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("mysignal error");
	if (alarm(nsec) > 0)
		printf("old alarm closed\n");
	
	err = getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
	alarm(0);
	if (err != 0 && !(flags & NI_NAMEREQD)) {
		err = getnameinfo(addr, addrlen, host, hostlen,
			serv, servlen, NI_NUMERICHOST | NI_NUMERICSERV);
	}
	if (mysignal(SIGALRM, SIG_DFL) == SIG_ERR)
		err_sys("mysignal error");
	return err;
}


int main(int argc, char* argv[])
{
	char host[INET_ADDRSTRLEN], serv[SERVLEN];
	struct sockaddr_in testaddr;
	int err;

	if (argc < 3)
		err_quit("usage: %s <IPAddress> <port>", argv[0]);
	bzero(&testaddr, sizeof(testaddr));
	testaddr.sin_family = AF_INET;
	testaddr.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &testaddr.sin_addr) <= 0)
		err_sys("inet_pton error");

	if ((err = getnameinfo_timeo((const struct sockaddr*)&testaddr, sizeof(testaddr),
		host, sizeof(host), serv, sizeof(serv), 0, 3)) != 0)
		err_exit(err, "getnameinfo error");
	printf("host: %s, serv: %s\n", host, serv);

	exit(EXIT_SUCCESS);
}