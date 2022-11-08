#include"thread.h"
#include<stdlib.h>
#include<stdio.h>
#include<sys/time.h>
#include<signal.h>

extern struct task_struct *current;
extern struct task_struct *task[NR_TASKS];

static struct task_struct *pick();
void switch_to(struct task_struct *next);
void schedule();
void mysleep(int seconds);
static unsigned int getmstime() {
	struct timeval tv;
	/*
		struct timeval {
			long int tv_sec;// s
			long int tv_usec;// ms
		}
	*/
	if(gettimeofday(&tv,NULL) < 0) {//get the time begin in 1970-01-01 00:00:00
		perror("gettimeofday");
		exit(-1);
	}
	
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;//固定形式返回微秒
}
static struct task_struct *pick() {
	int i,next,c;
	
	//make sleep to running status
	for(i = 0; i < NR_TASKS; ++i) {
		if(task[i] && task[i]->status != THREAD_EXIT &&
			getmstime() > task[i]->wakeuptime) {
			task[i]->status = THREAD_RUNNING;
		}
	}

	//
	while(1) {
		c = -1;
		next = 0;

		//find the running and the thread with  biggest counter
		for(i = 0; i < NR_TASKS; ++i) {
			if(!task[i]) continue;
			if(task[i]->status == THREAD_RUNNING && task[i]->counter > c) {
				c = task[i]->counter;
				next = i;
			}
		}

		//found or not
		/*
		   c == 0 all the counter == 0
		   c == -1 all status is sleep or counter == 0,now next is 0,so return to init_task
		   c == default  found the next thread
		*/
		if(c) break;
		
		//all the counter == 0
		if(c == 0) {
			for(i = 0; i < NR_TASKS; ++i) {
				if(task[i]) {//make it bigger,or distribute timer

					task[i]->counter = task[i]->priority + (task[i]->counter>>1);
				}
			}
		}
	}
	return task[next];
}

//屏蔽时钟信号
void closealarm() {
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGALRM);
	if(sigprocmask(SIG_BLOCK,&mask,NULL) < 0) {//第一个参数表示当前信号集和mask信号集的并集，mask包含希望阻塞的信号
		perror("sigprocmask BLOCK");
	}
}

//解除屏蔽时钟信号
void openalarm() {
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGALRM);
	if(sigprocmask(SIG_UNBLOCK,&mask,NULL) < 0) {//第一个参数表示当前信号集和mask信号集的补集的交集，mask包含希望阻塞的信号
		perror("sigprocmask BLOCK");
	}
	
}
void schedule() {
	struct task_struct *next = pick();
	if(next) {
		switch_to(next);
	}
}

void mysleep(int seconds) {
	current->wakeuptime = getmstime() + 1000 * seconds;//转化为微秒
	current->status = THREAD_SLEEP;

	schedule();
}

static void do_timer() {
	if(--current->counter > 0) return;
	current->counter = 0;
	schedule();
}

//第一行表示 被标记的函数在 main 函数前执行
__attribute__((constructor))
static void init() {
	struct itimerval value;
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 1000;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 1000 * 10;
	if(setitimer(ITIMER_REAL,&value,NULL) < 0) {
		perror("setitimer");
	}
	signal(SIGALRM,do_timer);//每10ms发一次信号并调用时间片函数
}
