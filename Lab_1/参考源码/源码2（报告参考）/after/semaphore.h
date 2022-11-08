#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include"thread.h"
#include"atomic.h"
#define QUE_LEN 16
#define SEM_FAILED 0

struct semaphore_queue {
	int front;
	int rear;
	struct task_struct *wait_tasks[QUE_LEN];
};
typedef struct semaphore_queue sem_queue;

struct semaphore_t {
	atomic_t value;	
	char name[16];
	struct semaphore_queue wait_queue;
};

typedef struct semaphore_t sem_t;

sem_t* sem_open(const char* name,unsigned int value);
int sem_wait(sem_t* sem);
int sem_post(sem_t* sem);
int sem_unlink(const char* name);
#endif
