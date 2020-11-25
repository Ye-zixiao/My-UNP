/**
 * 这里简单的测试下OPEN_MAX的值就差不多得了😋
 */
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#ifdef OPEN_MAX
long openmax = OPEN_MAX;
#else 
long openmax = 0;
#endif

#define OPEN_MAX_GUESS 256

static long open_max(void) {
	if (openmax == 0) {
		errno = 0;
		if ((openmax = sysconf(_SC_OPEN_MAX)) == -1) {
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;
			else return -1;
		}
	}
	return openmax;
}


int main(void)
{
	printf("OPEN_MAX: %ld\n", open_max());
	return 0;
}
