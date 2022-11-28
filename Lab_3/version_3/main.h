#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

#define STACK_SIZE 1024  //���ջ�ռ�
#define THR_TASKS 32     //����̸߳���
#define MAX_TIMER_NUM 50 //����ʱ������


typedef enum{
THREAD_STATUS_RUNNING,  //�̴߳��ڿɵ��ȶ��У���ʱ��û�õ�cpu��Դ
THREAD_STATUS_SLEEP,    //�߳�˯�ߣ���ʱ�̲߳��ɱ�cpu����
THREAD_STATUS_READY,    //�߳���������
THREAD_STATUS_EXIT,     //�߳��˳�
THREAD_STATUS_STOP,     //�߳��ܵ����أ�ֹͣ����
THREAD_STATUS_BLOCK     //�߳���Ϊ�ȴ�ĳ��ʱ����������ʱ���ɱ�cpu����
}thread_state;


struct thread_struct {
  int id;                                       //�̱߳�ʶ��
  void (*thread_func)();                        //ָ���̹߳��̺����ĺ���ָ�룬������¼�߳�ִ�еĺ���
  int esp;                                      //stack��ջ��ָ��
  unsigned int wakeuptime;                     //�̻߳���ʱ��
  int status;                                   //�߳�״̬
  int counter;                                  //ʱ��Ƭ����
  int priority;                                 //�߳����ȼ�
  int stack[STACK_SIZE];                        //�߳�����ջ
  int tickets;                                  //��Ʊ����
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
void start(struct thread_struct *tsk);                                      //start������
int make_context(int *stack,struct thread_struct *tsk,void (*start_routine)());    //��ʼ��ջ�ռ�
int thread_create(int *tid, void (*start_routine)());                       //�����߳�
int thread_join(int tid);                                                    //����ʽ�����߳�
void detach(int tid);                                                        //����ʽ�����߳�
void switch_to_next();                                                       //�������л�����
void wait_thread(struct thread_struct *tsk);                                //�����̣߳���״̬����ΪTHREAD_STATUS_BLOCK
void exit_thread(struct thread_struct *tsk) ;                               //�˳��̣߳���״̬����ΪTHREAD_STATUS_EXIT
/*-------- schedule -------*/
void closealarm();                                                           //ģ����ж�
void openalarm();                                                            //ģ�⿪�ж�
void schedule();                                                             //�л����Ⱥ���
static unsigned int getmstime();					                          //Get_time
void mysleep(int seconds);                                                   //ʱ���жϺ�������״̬����ΪTHREAD_STATUS_SLEEP
void wake_thread(struct thread_struct *tsk);                                 //���Ѻ�������״̬����ΪTHREAD_STATUS_RUNNING
/*-------- timer -------*/
static void sig_func(int signo);
struct timer_list *create_timer_list(int count);                            //����timer����
int  add_timer(int interval, callback cb, void *user_data, int len);        //����timer
int  del_timer(int timer_id);                                                //ɾ��timer
int destroy_timer(struct timer_list *list);                                 //����timer����
