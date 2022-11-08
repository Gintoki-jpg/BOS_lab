#include<stdio.h>
#include<unistd.h>

int task[3] = {0,0,0};
int cur = 0;
void switch_to(int n);

void fun1() {
	while(1) {
		printf("hello,I'm fun1\n");
		sleep(1);
		switch_to(2);
	}
}
void fun2() {
	while(1) {
		printf("hello,I'm fun2\n");
		sleep(1);
		switch_to(1);
	}
}

void start(int n) {
	if(n == 1) fun1();
	else if(n == 2) fun2();
}

int main() {
	int stack1[1024] = {0};
	int stack2[1024] = {0};

	task[1] = (int)(stack1 + 1013);
	task[2] = (int)(stack2 + 1013);
	stack1[1013] = 7;//eflags
	stack1[1014] = 6;
	stack1[1015] = 5;
	stack1[1016] = 4;
	stack1[1017] = 3;
	stack1[1018] = 2;
	stack1[1019] = 1;
	stack1[1020] = 0;//old ebp
	stack1[1021] = (int)start;
	stack1[1022] = 100;//ret to unknown
	stack1[1023] = 1;
	
	stack2[1013] = 7;//eflags
	stack2[1014] = 6;
	stack2[1015] = 5;
	stack2[1016] = 4;
	stack2[1017] = 3;
	stack2[1018] = 2;
	stack2[1019] = 1;
	stack2[1020] = 0;//old ebp
	stack2[1021] = (int)start;
	stack2[1022] = 100;//ret to unknown
	stack2[1023] = 2;

	switch_to(1);
	return 0;
}
