#include <stdio.h>
#include <time.h>
#define Status int
#define OK 0
#define ERROR -1
#define TRUE 1
#define FALSE 0
#define RUN 0
#define READY 1
#define CREAT 10
typedef struct _process{
	int pid;
	int priority;
	int cputime;
	int needtime;
	int state;
	int createtime;
}PCB;
void InitPro(PCB *pcb);
//初始化进程PCB，pid置0
Status CreatPro(PCB *pcb,int createtime);
//创建一个进程，随机赋予其优先权，需要运行时间，并设置其状态为ready
void PrintPro(PCB *pcb);
//打印PCB列表
Status Iteration(PCB *pcb);
//若进程PCB列表无就绪或阻塞进程，则停止循环，否则继续循环
Status RemovePro(PCB *pcb,int pid);
//撤销进程ID为pid的进程PCB
int SelectPro(PCB *pcb);
//选择一个就绪进程中优先级最高的进程，并返回其下标
int RunPro(PCB *pcb);
double Clean(PCB *pcb,int index,int iterator);

void InitPro(PCB *pcb){
	int i;
	srand((int)time(NULL));
	for(i=0;i<100;i++)pcb[i].pid=-1;
}
Status CreatPro(PCB *pcb,int createtime){
	int i;
	for(i=0;i<100;i++){
		if(pcb[i].pid == -1){
			pcb[i].pid=i;
			pcb[i].priority=rand()%50;
			pcb[i].cputime=0;
			pcb[i].needtime=rand()%10+1;
			pcb[i].createtime=createtime;
			pcb[i].state=READY;
			break;
		}
	}
	if(i==100)return ERROR;
	else return i;
}
Status RemovePro(PCB *pcb,int pid){
	if(pid < 0 ||pid > 100)return ERROR;
	pcb[pid].pid=-1;
	return OK;
}
void PrintPro(PCB *pcb){
	int i;
	for(i=0;i<51;i++)printf("=");
	printf("\n");
	printf("%3s%10s%9s%10s%7s%11s\n","ID","PRIORITY","CPUTIME","NEEDTIME","STATE","CREATETIME");
	for(i=0;i<100;i++){
		if(pcb[i].pid != -1){
			printf("%3d%10d%9d%10d",pcb[i].pid,pcb[i].priority,pcb[i].cputime,pcb[i].needtime);
			switch(pcb[i].state){
				case READY:printf("%7s","READY");break;
				case RUN :printf("%7s","RUN");break;
			}
			printf("%11d\n",pcb[i].createtime);
		}	
	}
	for(i=0;i<51;i++)printf("=");
	printf("\n");
}
Status Iteration(PCB *pcb){
	int i;
	for(i=0;i<100;i++){
		if(pcb[i].pid != -1)return TRUE;
	}
	return FALSE;
}
int SelectPro(PCB *pcb){
	int max,index;
	int temp;
	int i;
	index=-1;
	max=-1;
	for(i=0;i<100;i++){
		if(pcb[i].pid != -1 && pcb[i].priority > max){
			max=pcb[i].priority;
			index=i;
		}
	}
	return index;
}
int RunPro(PCB *pcb){
	int index;
	int temp;
	int i;
	if(!Iteration(pcb))return ERROR;
	index=SelectPro(pcb);
	pcb[index].priority-=3;
	if(pcb[index].priority < 0)pcb[index].priority=0;
	pcb[index].needtime--;
	pcb[index].cputime++;
	pcb[index].state=RUN;
	for(i=0;i<100;i++){
		if(i != index)pcb[i].priority++;
	}
	PrintPro(pcb);
	pcb[index].state=READY;
	getchar();
	return index;
}
double Clean(PCB *pcb,int index,int iterator){
	double average=0;
	if(!pcb[index].needtime){
		RemovePro(pcb,index);
		average=(iterator-pcb[index].createtime)*1.0/pcb[index].cputime;
		printf("process %d is removed:\n",index);
		printf("create time:%2d\n",pcb[index].createtime);
		printf("remove time:%2d\n",iterator);
		printf("cycling time:%2d\n",iterator-pcb[index].createtime);
		printf("cycling time with weight:%.2f\n",average);
		//getchar();
	}
	return average;
}
int main(){
	PCB pcb[100];
	int i,temp;
	int index,iterator,pronum;
	double average,cycling;
	InitPro(pcb);
	for(i=0;i<5;i++)CreatPro(pcb,0);		 //随机创建5个进程
	pronum=5;
	average=0;
	cycling=0;
	PrintPro(pcb);
	iterator=0;
	while(Iteration(pcb)){
		iterator++;
		index=RunPro(pcb);
		temp=Clean(pcb,index,iterator);
		average+=temp;
		cycling+=temp*pcb[index].cputime;
		if(rand()%100+1 <= CREAT){
			printf("Create a new process:\n");
			if((index=CreatPro(pcb,iterator)) == ERROR)return -1;
			else{
				pronum++;
				printf("pid:%d\npriority:%d\nneedtime=%d\n",pcb[index].pid,pcb[index].priority,pcb[index].needtime);
			}
			getchar();
		}
	}
	printf("\nrunning time:%3d\n",iterator);
	printf("process num:%d\n",pronum);
	printf("average of cycling time:%.2f\n",cycling/pronum);
	printf("average of cycling time with weight:%.2f\n",average/pronum);
	return 0;
}
