#include"main.h"

static struct thread_struct init_task = {0, NULL, THREAD_STATUS_RUNNING, 0, 0, 15, 15, {0}};
struct thread_struct *current = &init_task;
struct thread_struct *task[THR_TASKS] = {&init_task,};

//�߳�����������Ϊ��ͳһ�����̹߳��̺������������������һ��start��
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

//�̴߳�������
int thread_create(int *tid, void (*start_routine)())
{
    int id = -1;                                                                                    //��ʼ���߳�id
    struct thread_struct *tsk = (struct thread_struct *)malloc(sizeof(struct thread_struct));     //Ϊ�̷߳���һ���ṹ��
    while (++id < THR_TASKS && task[id]);                                                           //���̶߳�����Ѱ��λ��
    if (id == THR_TASKS)                                                                            //���û��λ�����򴴽�ʧ��
        return -1;
    task[id] = tsk;                                                                                  //���߳̽ṹ�����task�̶߳�����
    if (tid) *tid = id;                                                                              //���̶߳��е���������Ϊid�Ŵ���tid������֮�󴫳�

    //��ʼ���߳̽ṹ��
    tsk->id = id;                                                                                    //�����߳�id
    tsk->thread_func = start_routine;                                                                //�̹߳��̺�������Ӧ֮����д��fun1,fun2...
    int *stack = tsk->stack;                                                                         //�߳�����ջ
    tsk->esp = (int)(stack + STACK_SIZE - 11);                                                       //��ȡespջ��ָ��
    tsk->status = THREAD_STATUS_RUNNING;                                                             //�߳�״̬����ΪRUNNING
    tsk->wakeuptime = 0;                                                                             //����Wakeuptime
    tsk->counter= 15;                                                                                       //ʱ��Ƭ�ĵ�λ�������롢΢����ߺ��룬������������˴���ʼ��Ϊ15�������
    tsk->priority = 15;                                                                              //�����߳����ȼ�

    make_context(stack,tsk,start_routine);
    //for(int i=1;i<12;i++){
	//printf("thread_%d's origin stack[%d] is %d\n",id,12-i,stack[STACK_SIZE-(12-i)]);
    //}
    //printf("<=======================>\n");
    return 0;

}

//�߳�����ʽ����
int thread_join(int tid)
{
    while(task[tid]->status != THREAD_STATUS_EXIT)
    {
        schedule();
    }
    free(task[tid]);
    task[tid] = NULL;
}

//�̷߳���ʽ����
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

