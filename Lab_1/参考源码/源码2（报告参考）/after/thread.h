#ifndef __THREAD_h__
#define __THREAD_H__

#define NR_TASKS 16
#define STACK_SIZE 1024

#define THREAD_READY 0
#define THREAD_RUNNING 1
#define THREAD_SLEEP 2
#define THREAD_EXIT 3
#define THREAD_UNINTERRUPTIBLE 4
struct task_struct {
	int id;//thread id
	void (*th_fn)();//func of thread
	int esp;//the ptr of stack top
	unsigned int wakeuptime;
	int status;
	int counter;
	int priority;
	int stack[STACK_SIZE];//stack
};

int thread_create(int* tid,void(*start_routine)());
int thread_join(int tid);
void mysleep(int seconds);
int gettid();
#endif
