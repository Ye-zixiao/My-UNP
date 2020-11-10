#include "MyUNP.h"
#include <stdarg.h>
#include <syslog.h>

int daemon_proc;


/**
 * 处理真正的错误消息输出的功能
 * @param errnoflag 是否输出errno消息
 * @param level     syslog消息级别
 * @param fmt       格式化字符串
 * @param ap        可变参数列表
 */
static void 
err_doit(int errnoflag, int level, const char* fmt, va_list ap) {
	int errno_save, slen;
	char buf[MAXLINE + 1];

	errno_save = errno;
	vsnprintf(buf, MAXLINE, fmt, ap);
	slen = strlen(buf);
	if (errnoflag)
		snprintf(buf + slen, MAXLINE - slen, ": %s", strerror(errno_save));
	strcat(buf, "\n");

	//根据情况输出到syslog设施或者stderr中
	if (daemon_proc)
		syslog(level, buf);
	else {
		fflush(stdout);
		fputs(buf, stdout);
		fflush(stderr);
	}
}


/* 与系统调用相关的非致命错误，打印errno */
void err_ret(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}


/* 与系统调用相关的致命错误 */
void err_sys(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/* 与系统调用相关的致命错误，产生dump core */
void err_dump(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	abort();
	exit(EXIT_FAILURE);
}


/* 与系统调用无关的非致命错误，不打印errno */
void err_msg(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_INFO, fmt, ap);
	va_end(ap);
	return;
}


/* 与系统调用无关的致命错误，不打印errno */
void err_quit(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}


/* 自定义调试函数 */
void debug(void) {
	static int cnt = 0;
	fprintf(stderr, "get here(%d)?\n", cnt++);
}
