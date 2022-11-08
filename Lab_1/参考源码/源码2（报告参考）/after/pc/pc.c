#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include"../semaphore.h"

#define NUMBER 52 /*打出数字总数*/
#define CHILD 4 /*消费者进程数*/
#define BUFSIZE 5 /*缓冲区大小*/
int fno;//文件描述符
sem_t* empty,*full,*mutex;
int  buf_out = 0; /*从缓冲区读取位置*/
int  buf_in = 0; /*写入缓冲区位置*/
int  data;
void producer() {
	int i;
	for(i = 0; i < NUMBER; i++) {
	    sem_wait(empty);
		sem_wait(mutex);
			lseek(fno,buf_in * sizeof(int),SEEK_SET);
			write(fno,(char*)&i,sizeof(int));
			buf_in = (buf_in + 1) % BUFSIZE;
		sem_post(mutex);
		sem_post(full);
	}
}

void consumer() {
	int k;
	for(k = 0; k < NUMBER/CHILD; k++) {
		sem_wait(full);
		sem_wait(mutex);
			//获得读取位置
			lseek(fno,10 * sizeof(int),SEEK_SET);
			read(fno,(char*)&buf_out,sizeof(int));
			//读取数据
			lseek(fno,buf_out * sizeof(int),SEEK_SET);
			read(fno,(char*)&data,sizeof(int));
			//写入读取位置
			buf_out = (buf_out + 1) % BUFSIZE;
			lseek(fno,10 * sizeof(int),SEEK_SET);
			write(fno,(char*)&buf_out,sizeof(int));

		sem_post(mutex);
		sem_post(empty);

			printf("%d: %d\n",gettid(),data);
			//mysleep(0.1);
	}
	return;
}
int main()
{
	int tid1,tid2,tid3,tid4,tid5;
	int  i,j,k;
	thread_create(&tid1,producer);
	printf("create thread %d\n",tid1);
	thread_create(&tid2,consumer);
	printf("create thread %d\n",tid2);
	thread_create(&tid3,consumer);
	printf("create thread %d\n",tid3);
	thread_create(&tid4,consumer);
	printf("create thread %d\n",tid4);
	thread_create(&tid5,consumer);
	printf("create thread %d\n",tid5);

	empty = sem_open("empty",5);
	full = sem_open("full",0);
	mutex = sem_open("mutex",1);
	
	fno = open("buffer.dat",O_CREAT|O_RDWR|O_TRUNC,0666);
	/* 将待读取位置存入buffer后,以便 子进程 之间通信 */
	lseek(fno,10*sizeof(int),SEEK_SET);
	write(fno,(char *)&buf_out,sizeof(int));
	
	thread_join(tid1);
	thread_join(tid2);
	thread_join(tid3);
	thread_join(tid4);
	thread_join(tid5);
	
	sem_unlink("open");
	sem_unlink("full");
	sem_unlink("mutex");
	return 0;
}
