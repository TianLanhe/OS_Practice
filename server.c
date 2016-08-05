#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define SVKEY ((key_t)75)					//the key of the message box
#define REQ 5 								//the type of message
typedef struct _msgnod{
	long mtype;
	int procid;
	char msg[50];
}Msgnode;
int main(){
	int mid;
	Msgnode message;
	mid=msgget(SVKEY,IPC_CREAT|0660);		//create or get a message box
	if(mid == -1){							//failed to create or get the message box
		perror("server creat message");
		return 1;
	}
	printf("the pid of server:%d\n",getpid());
	while(1){
		msgrcv(mid,&message,sizeof(Msgnode)-sizeof(long),REQ,0);	//get a message from the message box connected before
		printf("serving for client:%d\n",message.procid);
		printf("   the contents of client sended:%s\n",message.msg);
		sleep(3);
		message.mtype = message.procid;
		message.procid = getpid();
		msgsnd(mid,&message,sizeof(Msgnode)-sizeof(long),0);
	}
}