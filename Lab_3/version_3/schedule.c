#include "main.h"
extern struct thread_struct *current;
extern struct thread_struct *task[THR_TASKS];
static struct timer_list *timer_list = NULL;


static struct thread_struct *FCFS_pick() {
    int current_id  = current->id;
    int i;
    struct task_struct *next = NULL;
repeat:
    for (i = 0; i < THR_TASKS; ++i) {
        if (task[i] && task[i]->status == THREAD_STATUS_SLEEP) {
            if (getmstime() > task[i]->wakeuptime)
            task[i]->status = THREAD_STATUS_RUNNING;
        }
    }
    i = current_id;
    while(1) {
    i = (i + 1) % THR_TASKS;
    if (i == current_id) {
        goto repeat;
    }
    if (task[i] && task[i]->status == THREAD_STATUS_RUNNING) {
        next = task[i];
        break;
    }
    }
  return next;
}

//pick�����������Ǵ�task��������ѡһ�����ʵ��̲߳��������߳̽ṹ��ָ��
//ÿ��ִ��pick����������ѡһ��ʱ��Ƭֵ�����̲߳�����
static struct thread_struct *RR_pick()
{
    int i, next, c;
    for (i = 0; i < THR_TASKS; ++i)
    {
        if (task[i] && task[i]->status != THREAD_STATUS_EXIT && getmstime() > task[i]->wakeuptime)
        {
            task[i]->status = THREAD_STATUS_RUNNING;
        }
    }
    while (1)
    {
        c = -1;
        next = 0;
        for (i = 0; i < THR_TASKS; ++i)
        {
            if (!task[i])
                continue;
            if (task[i]->status == THREAD_STATUS_RUNNING && task[i]->counter > c)
            {
                c = task[i]->counter;
                next = i;
            }
        }
        if (c)
            break;
        if (c == 0)
        {
            for (i = 0; i < THR_TASKS; ++i)
            {
                if (task[i])
                {
                    task[i]->counter = task[i]->priority + (task[i]->counter >> 1);
                }
            }
        }
    }

    return task[next];
}

static struct thread_struct *Lottery_pick(){
    int counter_ticket=0;
    int winner=50;    //��Ʊwinner
    int current_id  = current->id;
    int i;
    struct task_struct *next = NULL;
repeat:
    for (i = 0; i < THR_TASKS; ++i) {
        if (task[i] && task[i]->status == THREAD_STATUS_SLEEP) {
            if (getmstime() > task[i]->wakeuptime)
            task[i]->status = THREAD_STATUS_RUNNING;
        }
    }
    i = current_id;
    while(1) {
    i = (i + 1) % THR_TASKS;
    if (i == current_id) {
        goto repeat;
    }
    if (task[i] && task[i]->status == THREAD_STATUS_RUNNING) {
	counter_ticket += task[i]->tickets;
	//printf("counter_ticket is %d",counter_ticket);
        if(counter_ticket>=winner){
            next = task[i];
            break;}
	//next = task[i];
        break;
    //break;
    }
    }
  return next;
}

void closealarm()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0)
    {
        perror("sigprocmask BLOCK");
    }
}


void openalarm()
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0)
    {
        perror("sigprocmask BLOCK");
    }
}

//�������Ⱥ���schedule
//�������̾���pick��һ���߳̽ṹ��ָ�룬����ʹ��switch_to�����л�������
void schedule()
{
    struct thread_struct *next = Lottery_pick();
    //if(next->id!=0){
    //printf("<===There is schedule===>\n");
    //printf("the next thread is %d,the next esp is %d,it's counter is %d\n",(*next).id,(*next).esp,(*next).counter);
    //for(int i=1;i<12;i++){
	//printf("stack[%d] is %d\n",12-i,(*next).stack[STACK_SIZE-(12-i)]);
    //}
    //printf("<=======================>\n");
    //}
    if (next)
    {
        switch_to_next(next);
    }
}

//getmstime���������mysleepһ��ʹ�õģ���Ҫ��������ú��뾫�ȵ�ʱ��
static unsigned int getmstime()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
    {
        perror("gettimeofday");
        exit(-1);
    }
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//sleep��������������wakeuptime�Լ��߳�״̬�����������߳��л������Կ����Ƕ�schedule�����л����Ż���
void mysleep(int seconds)
{
    current->wakeuptime = getmstime() + 1000 * seconds;
    current->status = THREAD_STATUS_SLEEP;
    schedule();
}

void wake_thread(struct thread_struct *tsk){
    if(tsk->status == THREAD_STATUS_SLEEP){
        tsk->status = THREAD_STATUS_RUNNING;
    schedule();
    }
}

//ʱ���жϴ�������ϵͳ��ÿ������в���һ��ʱ���жϲ�����ʱ���жϴ�����do_timer����do_timer�����̵߳�ʱ��Ƭֵ��1ֱ��0��ִ��schedule����
static void do_timer()
{
    if (--current->counter > 0){
    //printf("<==thread_%d remain %d counter==>\n",current->id,current->counter);
    return;
    }
    current->counter = 0;
    schedule();
}


//����ʱ���жϵĵ���
//����˼����ϵͳ��ÿ������в���һ��ʱ���жϲ�����ʱ���жϴ�����do_timer����do_timer�����̵߳�ʱ��Ƭֵ��1ֱ��0��ִ��schedule����
//�˴�����ʹ��linux���źŻ��ƣ�ʹ�ú���setitimerÿ���10ms���˴���һ�����Ϊ10ms������һ���ź�SIGALRMȻ��׽���źż���
__attribute__((constructor))   //���Σ�ʹ�øú����ܹ���main����֮ǰִ��
static void init()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;                      //it_valueΪ��ʱʱ����1000us����1ms
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;                   //it_intervalΪ��ʱ���.1000*10us����10ms
    value.it_interval.tv_usec = 1000 * 10;
    if (setitimer(ITIMER_REAL, &value, NULL) < 0)
    {
        perror("setitimer");
    }
    signal(SIGALRM, do_timer);
}

//��������ʹ���Զ����timer�滻�����Ǹ��򵥵��źŻ��ƣ�ʱ������Ȼ����Ϊ10ms
void timer_func(){
    int timer_id = -1;
	int count = 0;
	timer_list = create_timer_list(10);
	timer_id = add_timer(10, do_timer, &count, 50);
	while(count++ < 20)
		sleep(1);
	del_timer(timer_id);
	destroy_timer(timer_list);
}
