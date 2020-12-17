#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdlib.h>
#include <stddef.h>

typedef int Comp(const void*, const void*);

struct priority_queue {
	size_t nmem, memsz;
	int iput;
	Comp* pf;
	void* arr;
};


struct priority_queue* pq_create(size_t nmem, size_t memsz, Comp* pf);
void pq_destroy(struct priority_queue* pq);
int pq_size(const struct priority_queue* pq);
int pq_empty(const struct priority_queue* pq);
int pq_push(struct priority_queue* pq, const void* val, size_t memsz);
int pq_pop(struct priority_queue* pq, void* val, size_t memsz);
int pq_top(const struct priority_queue* pq, void* val, size_t memsz);



#endif // !PRIORITY_QUEUE_H
