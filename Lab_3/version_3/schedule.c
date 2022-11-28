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

//pick函数的任务是从task数组中挑选一个合适的线程并返回其线程结构体指针
//每次执行pick函数都会挑选一个时间片值最大的线程并返回
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
    int winner=50;    //彩票winner
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

//主动调度函数schedule
//基本流程就是pick下一个线程结构体指针，接着使用switch_to函数切换上下文
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

//getmstime这个函数和mysleep一起使用的，主要是用来获得毫秒精度的时间
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

//sleep函数，用于设置wakeuptime以及线程状态，主动将该线程切换（可以看作是对schedule主动切换的优化）
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

//时钟中断处理函数，系统在每个嘀嗒中产生一个时钟中断并进入时钟中断处理函数do_timer，在do_timer中让线程的时间片值减1直到0，执行schedule函数
static void do_timer()
{
    if (--current->counter > 0){
    //printf("<==thread_%d remain %d counter==>\n",current->id,current->counter);
    return;
    }
    current->counter = 0;
    schedule();
}


//基于时间中断的调度
//基本思想是系统在每个嘀嗒中产生一个时钟中断并进入时钟中断处理函数do_timer，在do_timer中让线程的时间片值减1直到0，执行schedule函数
//此处可以使用linux的信号机制，使用函数setitimer每间隔10ms（此处设一个嘀嗒为10ms）发送一次信号SIGALRM然后捕捉该信号即可
__attribute__((constructor))   //修饰，使得该函数能够在main函数之前执行
static void init()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;                      //it_value为延时时长，1000us等于1ms
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;                   //it_interval为计时间隔.1000*10us等于10ms
    value.it_interval.tv_usec = 1000 * 10;
    if (setitimer(ITIMER_REAL, &value, NULL) < 0)
    {
        perror("setitimer");
    }
    signal(SIGALRM, do_timer);
}

//这里我们使用自定义的timer替换上面那个简单的信号机制，时间间隔仍然设置为10ms
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
