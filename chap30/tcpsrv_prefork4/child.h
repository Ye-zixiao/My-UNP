#ifndef CHILDFD3X_H_
#define CHILDFD3X_H_

#include "MyUNP.h"

struct Child {
	pid_t child_pid;
	int child_pipefd;
	int child_status;
	long child_count;
};


extern struct Child* cptr;

pid_t child_make(int i, int listenfd, socklen_t addrlen);
void child_main(int i, int listenfd, socklen_t addrlen);



#endif // !CHILDFD3X_H_