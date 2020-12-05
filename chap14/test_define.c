#include "MyUNP.h"
#include <netinet/in.h>

int main(int argc, char* argv[])
{
#ifdef IPPROTO_IP
	printf("define IPPROTO_IP\n");
#else 
	printf("not define IPPROTO_IP\n");
#endif
#ifdef IPPROTO_IPV6
	printf("define IPPROTO_IPV6\n");
#else 
	printf("not define IPPROTO_IPV6\n");
#endif
#ifdef SOL_SOCKET
	printf("define SOL_SOCKET\n");
#else
	printf("not define SOL_SOCKET\n");
#endif

#ifdef IP_RECVDSTADDR
	printf("define IP_RECVDSTADDR\n");
#else
	printf("not define IP_RECVDSTADDR\n");
#endif
#ifdef IP_RECVIF
	printf("define IP_RECVIF\n");
#else
	printf("not define IP_RECVIF\n");
#endif
#ifdef IPV6_DSTOPTS
	printf("define IPV6_DSTOPTS\n");
#else
	printf("not define IPV6_DSTOPTS\n");
#endif
#ifdef IPV6_HOPLIMIT
	printf("define IPV6_HOPLIMIT\n");
#else
	printf("not define IPV6_HOPLIMIT\n");
#endif
#ifdef IPV6_NEXTHOP
	printf("define IPV6_NEXTHOP\n");
#else
	printf("not define IPV6_NEXTHOP\n");
#endif
#ifdef IPV6_PKTINFO
	printf("define IPV6_PKTINFO\n");
#else
	printf("not define IPV6_PKTINFO\n");
#endif
#ifdef IPV6_RTHDR
	printf("define IPV6_RTHDR\n");
#else
	printf("not define IPV6_RTHDR\n");
#endif
#ifdef IPV6_TCLASS
	printf("define IPV6_TCLASS\n");
#else
	printf("not define IPV6_TCLASS\n");
#endif
#ifdef SCM_RIGHTS
	printf("define IPV6_TCLASS\n");
#endif
#if defined(SCM_CREDS)
	printf("define SCM_CREDS\n");
#elif defined(SCM_CREDENTIALS)
	printf("define SCM_CREDENTIALS\n");
#else
	printf("not define SCM_CREDS or SCM_CREDENTIALS\n");
#endif
	return 0;
}