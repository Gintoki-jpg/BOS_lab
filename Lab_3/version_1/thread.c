#include"main.h"

static struct thread_struct init_task = {0, NULL, THREAD_STATUS_RUNNING, 0, 0, 15, 15, {0}};
struct thread_struct *current = &init_task;
struct thread_struct *task[THR_TASKS] = {&init_task,};

//线程启动函数（为了统一启动线程过程函数，所以设计了这样一个start）
void start(struct thread_struct *tsk)
{
    tsk->thread_func();
    tsk->status = THREAD_STATUS_EXIT;
    printf("Thread_%d excited\n",tsk->id);
    schedule();
}

int make_context(int *stack,struct thread_struct *tsk,void (*start_routine)()){
    stack[STACK_SIZE - 11] = 0;
    stack[STACK_SIZE - 10] = 0;
    stack[STACK_SIZE - 9] = 0;
    stack[STACK_SIZE - 8] = 0;
    stack[STACK_SIZE - 7] = 0;
    stack[STACK_SIZE - 6] = 0;
    stack[STACK_SIZE - 5] = 0;
    stack[STACK_SIZE - 4] = 0;
    stack[STACK_SIZE - 3] = (int)start;
    stack[STACK_SIZE - 2] = 100;
    stack[STACK_SIZE - 1] = (int)tsk;
}

//线程创建函数
int thread_create(int *tid, void (*start_routine)())
{
    int id = -1;                                                                                    //初始化线程id
    struct thread_struct *tsk = (struct thread_struct *)malloc(sizeof(struct thread_struct));     //为线程分配一个结构体
    while (++id < THR_TASKS && task[id]);                                                           //在线程队列中寻找位置
    if (id == THR_TASKS)                                                                            //如果没有位置了则创建失败
        return -1;
    task[id] = tsk;                                                                                  //将线程结构体放在task线程队列中
    if (tid) *tid = id;                                                                              //将线程队列的索引号作为id号传给tid，便于之后传出

    //初始化线程结构体
    tsk->id = id;                                                                                    //设置线程id
    tsk->thread_func = start_routine;                                                                //线程过程函数，对应之后会编写的fun1,fun2...
    int *stack = tsk->stack;                                                                         //线程运行栈
    tsk->esp = (int)(stack + STACK_SIZE - 11);                                                       //获取esp栈顶指针
    tsk->status = THREAD_STATUS_RUNNING;                                                             //线程状态设置为RUNNING
    tsk->wakeuptime = 0;                                                                             //设置Wakeuptime
    tsk->counter= 15;                                                                                       //时间片的单位不是纳秒、微秒或者毫秒，而是嘀嗒数，此处初始化为15个嘀嗒数
    tsk->priority = 15;                                                                              //设置线程优先级

    make_context(stack,tsk,start_routine);
    //for(int i=1;i<12;i++){
	//printf("thread_%d's origin stack[%d] is %d\n",id,12-i,stack[STACK_SIZE-(12-i)]);
    //}
    //printf("<=======================>\n");
    return 0;

}

//线程阻塞式切入
int thread_join(int tid)
{
    while(task[tid]->status != THREAD_STATUS_EXIT)
    {
        schedule();
    }
    free(task[tid]);
    task[tid] = NULL;
}

//线程分离式切入
void detach(int tid){
  if(task[tid]!=NULL && task[tid]->status==THREAD_STATUS_STOP&& task[tid]->status!=THREAD_STATUS_EXIT){
    task[tid]->status=THREAD_STATUS_RUNNING;
    schedule();
  }

}

//make thread wait
void wait_thread(struct thread_struct *tsk){
    if(tsk!=NULL && tsk->status!=THREAD_STATUS_EXIT){
    tsk->status=THREAD_STATUS_BLOCK;
    schedule();
    }
}


//make thread exit
void exit_thread(struct thread_struct *tsk) {
    int i=tsk->id;
    if (tsk!=NULL){
        tsk->status=THREAD_STATUS_EXIT;
        free(tsk->stack);
        free(tsk);
    }
    tsk=NULL;
    printf("pthread %d exited",i);
    schedule();
}

