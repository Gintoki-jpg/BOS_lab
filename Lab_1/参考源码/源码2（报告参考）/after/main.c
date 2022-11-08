#include<stdio.h>
#include<stdlib.h>
#include"thread.h"

void fun1() {
	int i = 4;
	int m,n;
	while(i--) {
		printf("hello,I'm fun1\n");
		for(m = 0; m < 10000; m++) 
		  for(n = 0; n < 10000; n++);
	}
}

void fun2() {
	int i = 5;
	int m,n;
	while(i--) {
		printf("hello,I'm fun2\n");
		for(m = 0; m < 10000; m++) 
		  for(n = 0; n < 10000; n++);
	}
}

void fun3() {
	int i = 5;
	while(i--) {
		printf("hello,I'm fun3\n");
		mysleep(1);
	}
}

void fun4() {
	int i = 6;
	int m,n;
	while(i--) {
		printf("hello,I'm fun4\n");
		for(m = 0; m < 10000; m++) 
		  for(n = 0; n < 10000; n++);
	}
}

int main()
{
	int tid1,tid2,tid3,tid4;
	thread_create(&tid1,fun1);
	printf("create thread %d\n",tid1);
	thread_create(&tid2,fun2);
	printf("create thread %d\n",tid2);
	thread_create(&tid3,fun3);
	printf("create thread %d\n",tid3);
	thread_create(&tid4,fun4);
	printf("create thread %d\n",tid4);
	
	thread_join(tid1);
	thread_join(tid2);
	thread_join(tid3);
	thread_join(tid4);
	
	return 0;
}
