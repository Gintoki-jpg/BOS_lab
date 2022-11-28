#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

#define STACK_SIZE 1024  //最大栈空间
#define THR_TASKS 32     //最大线程个数
#define MAX_TIMER_NUM 50 //最大计时器个数


typedef enum{
THREAD_STATUS_RUNNING,  //线程处于可调度队列，暂时还没得到cpu资源
THREAD_STATUS_SLEEP,    //线程睡眠，此时线程不可被cpu调度
THREAD_STATUS_READY,    //线程正在运行
THREAD_STATUS_EXIT,     //线程退出
THREAD_STATUS_STOP,     //线程受到调控，停止运行
THREAD_STATUS_BLOCK     //线程因为等待某个时间阻塞，此时不可被cpu调度
}thread_state;


struct thread_struct {
  int id;                                       //线程标识符
  void (*thread_func)();                        //指向线程过程函数的函数指针，用来记录线程执行的函数
  int esp;                                      //stack的栈顶指针
  unsigned int wakeuptime;                     //线程唤醒时间
  int status;                                   //线程状态
  int counter;                                  //时间片数量
  int priority;                                 //线程优先级
  int stack[STACK_SIZE];                        //线程运行栈
  int tickets;                                  //彩票数量
};

typedef void (*callback)(int id, void *data, int len);
typedef void (*SIG_FUNC)(int signo);

typedef struct timer{
	struct list_head node;
	int interval; /*timer interval(second)*/
	int elapse;   /*timer count*/

	callback cb; /*call back function*/
	void *user;  /*user data*/
	int len;
	int id; /*timerid*/
}timer_node_t;

struct timer_list{
	struct list_head head;
	int num;
	int size;
	void (*sighandler_old)(int);
	void (*sighandler)(int);
	struct itimerval ovalue;
	struct itimerval value;
};


/*-------- thread -------*/
void start(struct thread_struct *tsk);                                      //start管理函数
int make_context(int *stack,struct thread_struct *tsk,void (*start_routine)());    //初始化栈空间
int thread_create(int *tid, void (*start_routine)());                       //创建线程
int thread_join(int tid);                                                    //阻塞式启动线程
void detach(int tid);                                                        //分离式启动线程
void switch_to_next();                                                       //上下文切换函数
void wait_thread(struct thread_struct *tsk);                                //阻塞线程，将状态设置为THREAD_STATUS_BLOCK
void exit_thread(struct thread_struct *tsk) ;                               //退出线程，将状态设置为THREAD_STATUS_EXIT
/*-------- schedule -------*/
void closealarm();                                                           //模拟关中断
void openalarm();                                                            //模拟开中断
void schedule();                                                             //切换调度函数
static unsigned int getmstime();					                          //Get_time
void mysleep(int seconds);                                                   //时间中断函数，将状态设置为THREAD_STATUS_SLEEP
void wake_thread(struct thread_struct *tsk);                                 //唤醒函数，将状态设置为THREAD_STATUS_RUNNING
/*-------- timer -------*/
static void sig_func(int signo);
struct timer_list *create_timer_list(int count);                            //创建timer队列
int  add_timer(int interval, callback cb, void *user_data, int len);        //加入timer
int  del_timer(int timer_id);                                                //删除timer
int destroy_timer(struct timer_list *list);                                 //销毁timer队列
