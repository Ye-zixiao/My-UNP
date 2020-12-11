#include "MyUNP.h"

#define NLOOP 5000

int counter;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void* thread_func(void* arg) {
	int i, val, err;
	for (i = 0; i < NLOOP; ++i) {
		if ((err = pthread_mutex_lock(&lock)) != 0)
			err_exit(err, "pthread_mutex_lock error");

		val = counter;
		printf("0x%lx: %d\n", pthread_self(), val + 1);
		counter = val + 1;

		if ((err = pthread_mutex_unlock(&lock)) != 0)
			err_exit(err, "pthread_mutex_unlock error");
	}
	return (void*)NULL;
}


int main(int argc, char* argv[])
{
	pthread_t tid1, tid2;
	int err;

	if ((err = pthread_create(&tid1, NULL, thread_func, (void*)NULL)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_create(&tid2, NULL, thread_func, (void*)NULL)) != 0)
		err_exit(err, "pthread_create error");
	if ((err = pthread_join(tid1, (void**)NULL)) != 0)
		err_exit(err, "pthread_join error");
	if ((err = pthread_join(tid2, (void**)NULL)) != 0)
		err_exit(err, "pthread_join error");
	exit(EXIT_SUCCESS);
}