#include "MyUNP.h"

#define TIMEBUFLEN 64


/**
 * 以指定字符串格式获取当前时间，非线程安全 
 */
const char* currTime(const char* fmt) {
	static char timebuf[TIMEBUFLEN];
	struct tm* ptm;
	time_t ct;

	time(&ct);
	if ((ptm = localtime(&ct)) == NULL)
		return NULL;
	if (strftime(timebuf, TIMEBUFLEN,
			fmt == NULL ? "%c" : fmt, ptm) == 0)
		return NULL;
	return timebuf;
}


/**
 * 以指定字符串格式获取当前时间，线程安全
 */
char* currTime_r(char* buf, size_t maxlen, const char* fmt) {
	struct tm* ptm;
	time_t ct;

	time(&ct);
	if ((ptm = localtime(&ct)) == NULL)
		return NULL;
	if (strftime(buf, maxlen, fmt == NULL ? "%c" : fmt, ptm) == 0)
		return NULL;
	return buf;
}