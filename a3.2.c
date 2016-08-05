#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
int main(){
	int fd[2];
	int read_pipe;
	int write_pipe;
	pid_t pid;
	pipe(fd);
	read_pipe=fd[0];
	write_pipe=fd[1];
	pid=fork();
	if(pid == -1){
		printf("fork() error!\n");
		return 1;
	}else if(pid == 0){
		char message[100];
		close(read_pipe);
		printf("child:I'm ready to write message to pipe.\n");
		strcpy(message,"this is a message from child process!");
		lockf(1,F_LOCK,0);
		sleep(1);
		write(write_pipe,message,strlen(message)+1);
		printf("child:I has wrote the message!\n");
		lockf(1,F_ULOCK,0);
		close(write_pipe);
		return 0;
	}else{
		char rd[100];
		close(write_pipe);
		printf("parent:I'm ready to read message to pipe.\n");
		read(read_pipe,rd,100);
		printf("the message from child is:%s\n",rd);
		close(write_pipe);
		wait(0);
		return 0;
	}
}