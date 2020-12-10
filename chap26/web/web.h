#ifndef WEBS42X_H_
#define WEBS42X_H_

#include "MyUNP.h"


#define SERV	 "80"
#define MAXFILES 20
#define GET_CMD "GET %s HTTP/1.0\r\n\r\n"

/* 这些状态常量是按照二进制位设置的🙃！ */
#define F_CONNECTING 1
#define F_READING    2
#define F_DONE       4
#define F_JOINED     8

struct file {
	char* f_name;
	char* f_host;
	int f_fd;
	int f_flags;
	pthread_t f_tid;
};

extern struct file files[MAXFILES];
extern int nconn, nfiles, nlefttoconn, nlefttoread, ndone;
extern pthread_mutex_t ndone_lock;
extern pthread_cond_t ndone_cond;


void* do_get_read(void*);
void home_page(const char* host, const char* fname);
void write_get_cmd(struct file* fptr);



#endif // !WEBS42X_H_