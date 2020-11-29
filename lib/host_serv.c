#include "MyUNP.h"


/**
 * ���κ��û���ȡЭ���ַʹ��.
 * ʹ������+���񷵻�addrinfo�ṹ����ָ�룬��������ָ����ַ��
 * ���׽������ͣ���ʧ�ܷ���NULL
 **/
struct addrinfo*
host_serv(const char* host, const char* serv, int family, int socktype) {
	struct addrinfo hint, * res;

	bzero(&hint, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = family;
	hint.ai_socktype = socktype;
	if (getaddrinfo(host, serv, &hint, &res) != 0)
		return NULL;
	return res;
}