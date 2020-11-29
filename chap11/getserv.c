/**
 * 既支持使用端口号对服务名进行查询，又支持使用服务名
 * 对端口号进行反向查询
 */
#include "MyUNP.h"


int main(int argc, char* argv[])
{
	struct servent* sptr;

	if (argc < 2)
		err_quit("usage: %s <IPAddress> [protocol]", argv[0]);
	
	if ((sptr = getserv(argv[1], argv[2])) == NULL)
		err_quit("getserv error for %s", argv[1]);
	printf("server: %s\n", sptr->s_name);
	printf("port  : %d\n", ntohs(sptr->s_port));
	printf("proto : %s\n", sptr->s_proto);
	exit(EXIT_SUCCESS);
}
