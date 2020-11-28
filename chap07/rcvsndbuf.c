#include "MyUNP.h"

int main(int argc, char* argv[])
{
	int udpfd, tcpfd, val;
	socklen_t len;

	if ((tcpfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		err_sys("socket error");
	if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		err_sys("socket error");

	len = sizeof(int);
	printf("tcp: ");
	if (getsockopt(tcpfd, SOL_SOCKET, SO_RCVBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	printf("recvbuf %d bytes, ", val);
	if (getsockopt(tcpfd, SOL_SOCKET, SO_SNDBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	printf("sendbuf %d bytes\n", val);

	len = sizeof(int);
	printf("udp: ");
	if (getsockopt(udpfd, SOL_SOCKET, SO_RCVBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	printf("recvbuf %d bytes, ", val);
	if (getsockopt(udpfd, SOL_SOCKET, SO_SNDBUF, &val, &len) == -1)
		err_sys("getsockopt error");
	printf("sendbuf %d bytes\n", val);

	exit(EXIT_SUCCESS);
}