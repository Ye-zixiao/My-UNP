#ifndef WEBDKX_H_
#define WEBDKX_H_

#include "MyUNP.h"

#define MAXFILES 20
#define SERV "80"


struct file {
	char* f_name;
	char* f_host;
	int f_fd;
	int f_flags;
};

extern struct file file[MAXFILES];


#define F_CONNECTING 1
#define F_READING    2
#define F_DONE       4

#define GET_CMD "GET %s HTTP/1.0 \r\n\r\n"


/* UNP原书中的定义方式是有问题，会造成变量的多重定义 */
extern int nconn, nfiles, nlefttoconn, nlefttoread, maxfd;
extern fd_set rset, wset;


void home_page(const char* host, const char* fname);
void start_connect(struct file* fptr);
void write_get_cmd(struct file* fptr);



#endif // WEBRDKX_H_
