#include "web.h"

struct file files[MAXFILES];
int nconn, nfiles, nlefttoconn, nlefttoread, ndone;
pthread_mutex_t ndone_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ndone_cond = PTHREAD_COND_INITIALIZER;


int main(int argc, char* argv[])
{
	int i, err, maxnconn;
	struct file* fptr;
	pthread_t tid;

	if (argc < 4)
		err_quit("usage: %s <conn> <host> <homepage> file1 ...", argv[0]);
	maxnconn = atoi(argv[1]);

	//初始化file结构数组
	nfiles = MIN(argc - 4, MAXFILES);
	printf("nfiles = %d\n", nfiles);
	for (i = 0; i < nfiles; ++i) {
		files[i].f_name = argv[i + 4];
		files[i].f_host = argv[2];
		files[i].f_flags = 0;
	}

	home_page(argv[2], argv[3]); putchar('\n');
	nlefttoconn = nlefttoread = nfiles;
	nconn = 0;
	while (nlefttoread > 0) {
		/* 为相应的资源请求创建新线程 */
		while (nconn < maxnconn && nlefttoconn > 0) {
			for (i = 0; i < nfiles; i++)
				if (files[i].f_flags == 0)
					break;
			if (i == nfiles)
				err_quit("nlefttoconn = %d but nothing found", nlefttoconn);

			files[i].f_flags = F_CONNECTING;
			if ((err = pthread_create(&tid, NULL, do_get_read, (void*)&files[i])) != 0)
				err_exit(err, "pthread_create error");
			files[i].f_tid = tid;
			nconn++;
			nlefttoconn--;
		}

		/* 当负责处理非主页资源的线程完成了连接、请求发送、数据接收之后，就会激活
			ndone_cond条件变量，然后唤醒主线程获取终止线程的返回状态等信息 */
		if ((err = pthread_mutex_lock(&ndone_lock)) != 0)
			err_exit(err, "pthread_mutex_lock error");
		while (ndone == 0)
			if ((err = pthread_cond_wait(&ndone_cond, &ndone_lock)) != 0)
				err_exit(err, "pthread_cond_wait error");

		/* 若请求的文件多了, 遍历整个files数组才能知道是哪一个线程失效,显得有些低效 */
		for (i = 0; i < nfiles; ++i) {
			if (files[i].f_flags & F_DONE) {
				if ((err = pthread_join(files[i].f_tid, (void**)&fptr)) != 0)
					err_exit(err, "pthread_join error");

				if (&files[i] != fptr)
					err_quit("files[i] != fptr");
				fptr->f_flags = F_JOINED;
				--ndone;
				--nconn;
				--nlefttoread;
				printf("thread 0x%lx for %s done\n", fptr->f_tid, fptr->f_name);
			}
		}
		if ((err = pthread_mutex_unlock(&ndone_lock)) != 0)
			err_exit(err, "pthread_mutex_unlock error");
	}

	exit(EXIT_SUCCESS);
}
