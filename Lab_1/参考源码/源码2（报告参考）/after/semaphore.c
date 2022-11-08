#include"semaphore.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

extern struct task_struct *current;
void schedule();
#define SEM_COUNT 32

sem_t* semaphores[SEM_COUNT];

//队列相关的操作
void init_queue(sem_queue *q) {
	q->front = q->rear = 0;
}

int is_empty(sem_queue *q) {
	return q->front == q->rear ? 1 : 0;
}

int is_full(sem_queue *q) {
	return (q->rear + 1) % QUE_LEN == q->front ? 1 : 0; 
}

//获得队列头的第一个任务
struct task_struct* get_task(sem_queue *q) {
	if(is_empty(q)) {
		printf("Queue is empty\n");
		return NULL;
	}

	struct task_struct* tsk = q->wait_tasks[q->front];
	q->front = (q->front + 1) % QUE_LEN;
	return tsk;
}

//将线程插入队尾
int insert_task(struct task_struct* p,sem_t* sem) {
	sem_queue *q = &sem->wait_queue;
	printf("thread %d insert to %s\n",p->id,sem->name);
	if(is_full(q)) {
		printf("Queue is full\n");
		return -1;
	}
	q->wait_tasks[q->rear] = p;
	q->rear = (q->rear + 1) % QUE_LEN;
	return 1;
}

//检查是否存在信号量，是则返回位置
int sem_location(const char* name) {
	int i;
	for(i = 0; i < SEM_COUNT; ++i) {
		if(semaphores[i] == NULL) continue;
		if(strcmp(name,semaphores[i]->name) == 0)
		  return i;
	}
	
	return -1;
}

//打开信号量
sem_t* sem_open(const char* name,unsigned int value) {
	
	char tmp[16];
	char c;
	int i;
	for(i = 0; i < 16; i++) {
		c= name[i];
		tmp[i] = c;
		if(c == '\0') break;
	}
	
	if(c >= 16) {
		printf("Semaphore name is too long\n");
		return NULL;
	}

	if((i = sem_location(tmp)) != -1) {
		return semaphores[i];
	}
	
	sem_t* sem = (sem_t*)malloc(sizeof(sem_t));
	atomic_set(&sem->value,value);
	strcpy(sem->name,tmp);
	init_queue(&sem->wait_queue);
	for(i = 0; i < SEM_COUNT; ++i) {
		if(semaphores[i] == NULL) {
			semaphores[i] = sem;
			return semaphores[i];
		}
	}
	
	printf("Number of semaphores are limited\n");
	return NULL;
}

//P操作
int sem_wait(sem_t* sem) {
	atomic_sub(1,&sem->value);
	
	//0 - 1 = -1,资源不足,睡眠
	if(atomic_read(&sem->value) < 0) {
		current->status = THREAD_UNINTERRUPTIBLE;
		insert_task(current,sem);
		mysleep(0.1);
	}

	return 0;
}

//V操作
int sem_post(sem_t* sem) {
	
	atomic_add(1,&sem->value);
	if(atomic_read(&sem->value) <= 0) {
		struct task_struct* p = get_task(&sem->wait_queue);
		if(p != NULL) p->status = THREAD_RUNNING;
	}
	return 0;
}

int sem_unlink(const char* name) {
	
	char tmp[16];
	char c;
	int i,ret;
	for(i = 0; i < 16; i++) {
		c= name[i];
		tmp[i] = c;
		if(c == '\0') break;
	}
	
	if(c >= 16) {
		printf("Semaphore name is too long\n");
		return -1;
	}
	
	ret = sem_location(tmp);
	if(ret != -1) {
		free(semaphores[ret]);
		semaphores[ret] = NULL;
		return 0;
	}
	return -1;
}
