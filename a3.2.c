#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
int main(){
	int fd[2];				//存储管道的读写文件描述符
	int read_pipe;			//读管道
	int write_pipe;			//写管道
	pid_t pid;				//子进程pid
	pipe(fd);				//创建一个无名管道
	read_pipe=fd[0];
	write_pipe=fd[1];
	pid=fork();				//创建一个子进程
	if(pid == -1){
		printf("fork() error!\n");
		return 1;
	}else if(pid == 0){		//在子进程里
		char message[100];
		close(read_pipe);	//关闭读管道
		printf("child:I'm ready to write message to pipe.\n");
		strcpy(message,"this is a message from child process!");
		lockf(1,F_LOCK,0);	//开启文件锁，保证管道的读写互斥
		sleep(1);
		write(write_pipe,message,strlen(message)+1);	//往管道写数据
		printf("child:I has wrote the message!\n");
		lockf(1,F_ULOCK,0);	//解锁
		close(write_pipe);	//操作完毕，关闭管道，子进程结束
		return 0;
	}else{					//在父进程里
		char rd[100];
		close(write_pipe);	//关闭写管道
		printf("parent:I'm ready to read message to pipe.\n");
		read(read_pipe,rd,100);	//读取管道里的内容，如果管道内没有内容，则进程阻塞，直至有内容了会被唤醒读取内容。
		printf("the message from child is:%s\n",rd);
		close(read_pipe);	//关闭读管道
		wait(0);			//等待子进程结束，然后自己也结束
		return 0;
	}
}
