#include "main.h"

void fun1()
{
    int i = 10;
    while (i--)
    {
        printf("thread_1 is running----");
        printf("Time clock = %.2lf\n", (double)clock() / CLOCKS_PER_SEC);
        //printf("running %d thread\n",current->id);
        mysleep(2);
    }
}

void fun2()
{
    int i = 10;
    while (i--)
    {
        printf("thread_2 is running----");
        printf("Time clock = %.2lf\n", (double)clock() / CLOCKS_PER_SEC);
        //printf("running %d thread\n",current->id);
        mysleep(1);
    }
}

void fun3()
{
    int i = 2;
    while (i--)
    {
        printf("thread_3 is running----");
        printf("Time clock = %.2lf\n", (double)clock() / CLOCKS_PER_SEC);
        mysleep(5);
    }
}

void fun4() {
  int i = 15;
  while(i--) {
    printf("thread_4 is running----");
    printf("Time clock = %.2lf\n", (double)clock() / CLOCKS_PER_SEC);
  }
}

int main()
{

    int tid1, tid2, tid3, tid4;
    thread_create(&tid1, fun1);
    printf("Thread_ %d has been created\n", tid1);
    thread_create(&tid2, fun2);
    printf("Thread_ %d has been created\n", tid2);
    thread_create(&tid3, fun3);
    printf("Thread_ %d has been created\n", tid3);
    thread_create(&tid4, fun4);
    printf("Thread_ %d has been created\n", tid4);
    printf("<====================================>\n");

    int num = 2;
    while (num--)
    {
        printf("Main thread is running\n");
        mysleep(3);
    }
    //detach(tid1);
    //detach(tid2);
    //detach(tid3);
    //detach(tid4);
    thread_join(tid1);
    thread_join(tid2);
    thread_join(tid3);
    thread_join(tid4);
    printf("Main thread is running\n");
    return 0;
}

