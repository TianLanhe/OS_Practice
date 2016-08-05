#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <error.h>
#define REQ 5 								//the type of message
#define SVKEY ((key_t)75)					//the key of the message box
typedef struct _msgnod{
	long mtype;
	int procid;
	char msg[50];
}Msgnode;
int main(){
	int mid;
	int sign=0;
	Msgnode message;
	mid=msgget(SVKEY,IPC_CREAT|0660);
	if(mid == -1){
		perror("client creat message");
		return 1;
	}
	printf("the pid of client:%d\n",getpid());
	while(1){
		sign++;
		message.mtype = REQ;
		message.procid = getpid();
		printf("Plase enter the content:");
		fgets(message.msg,50,stdin);				//get the message content from the stardarn input
		sign=msgsnd(mid,&message,sizeof(Msgnode)-sizeof(long),0); //send a message to the server
		if(sign == -1){								//if failed to send
			printf("client sends error!\n");
			return 1;
		}
		printf("client has sended the message!\n");
		sleep(3);									//sleep after sending a message
		sign=msgrcv(mid,&message,sizeof(Msgnode)-sizeof(long),getpid(),0);	//and received a respond from the server
		if(sign == -1){
			printf("client receive error!\n");
			return 1;
		}
		printf("receive reply from:%d\n",message.procid);	//receive succeed
	}
}
