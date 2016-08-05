#include <stdio.h>
#include <stdlib.h>
#define ALLOC 1 				//表示已分配
#define FREE 0 					//表示未分配
#define Status int
#define ERROR -1 				//函数错误返回值
#define OK 0					//函数正确返回值
#define TRUE 1
#define FALSE 0
#define Addr int 				//地址返回值
#define min_size 9 				//最小的分区长度单位
typedef struct free_memory{
	struct free_memory *pre;	//指向前一节点(分区)的指针
	int size;					//节点(分区)的长度
	Addr addr_start;			//节点(分区)的首地址
	int state;					//节点(分区)的状态
	struct free_memory *next;	//指向后一节点(分区)的指针
}FM;
typedef struct fm_linklist{		//管理分区链表的数据结构
	FM *head;					//分区链表的(虚)头指针
	FM *tail;					//分区链表的尾结点(最后一个分区)
	int len;					//分区链表的长度，即分区的个数
}FM_Link;
FM_Link fmlink;			//全局变量，link为空闲分区链
Addr start_add;
Status InitFMLink(FM_Link *link,int mem_size);
//初始化空闲分区链，并建立第一块大小为全部内存的空闲分区，加入空闲分区链
Status DestroyLink(FM_Link *link);
//销毁空闲分区链
int FMLinkLength(FM_Link link);
//返回分区的个数
int FMLinkLength_free(FM_Link link);
//返回分区链表中空闲分区的个数
Status MakeNode(FM **node,int size,Addr addr_start);
//分配FM节点并设置分区的长度与起始地址等信息
void PrintLink(FM_Link link);
//打印分区链表link
Status InsertFM(FM_Link *link,FM *p1,FM *p2);
//p1是分区链link中的节点，将p2节点插入p1节点之后
Status DelectFM(FM_Link *link,FM *p);
//将p节点从分区链link中删除
FM *LocateFM(FM_Link link,int request_size);
//在link中,从第posi个位置开始，搜索大小超过request_size的空闲分区
Addr Alloc_FF(int request_size);
//首次适应分配算法
Status Free_FF(Addr addr_start);
//首次适应回收算法
Addr Alloc_BF(int request_size);
//最佳适应分配算法
Status Free_BF(Addr addr_start);
//最佳适应回收算法
Addr Alloc_NF(int request_size);
//循环首次适应分配算法
Status Free_NF(Addr addr_start);
//循环首次适应回收算法
Addr Alloc_WF(int request_size);
//最坏适应分配算法
Status Free_WF(Addr addr_start);
//最坏适应回收算法
void PrintMenu();
//打印主菜单
void PrintMenu_sub();
//打印子菜单
void RequestTest(Addr (*myalloc)(int),Status (*myfree)(Addr),int size);
void RandomTest(Addr (*myalloc)(int),Status (*myfree)(Addr),int size);
//指定内存动态分配算法，进行随机测试
void MemoryDynamic(Addr (*myalloc)(int),Status (*myfree)(Addr),int size);
//动态分配算法，若size为0，则创建的内存大小为默认的1M(640 KB)
Status InitFMLink(FM_Link *link,int mem_size){
	FM *node;
	if(mem_size<=0)return ERROR;
	link->head=(FM *)malloc(sizeof(FM));
	if(!link->head)return ERROR;
	link->head->pre=link->head;
	link->head->size=link->head->addr_start=link->head->state=-1;
	node=link->head->next=(FM *)malloc(sizeof(FM));
	if(!node)return ERROR;
	node->size=mem_size;
	node->pre=link->head;
	node->next=NULL;
	node->addr_start=0;
	node->state=FREE;
	link->len=1;
	link->tail=node;
	return OK;
}
Status DestroyLink(FM_Link *link){
	FM *node;
	FM *temp;
	if(!link)return ERROR;
	node=link->head;
	while(node){
		temp=node;
		node=node->next;
		free(temp);						//释放分区链中的各分区节点
	}
	link->head=link->tail=NULL;			//释放虚头指针并将头尾指针指向空
	link->len=-1;						//设置分区链的长度为-1
}
int FMLinkLength(FM_Link link){
	return link.len;
}
Status FMLinkLength_free(FM_Link link){
	int i;
	FM *node;
	node=link.head;
	for(i=0,node=node->next;node;node=node->next)
		if(node->state == FREE)i++;
	return i;
}
FM *LocateFM(FM_Link link,int request_size){
	FM *node;
	node=link.head;
	for(node=node->next;node;node=node->next){
		if(node->state==FREE && node->size >= request_size)return node;
	}
	return NULL;
}
Status InsertFM(FM_Link *link,FM *p1,FM *p2){
	FM *node;
	if(!link || !p1 || !p2)return ERROR;
	p2->next=p1->next;
	p1->next=p2;
	p2->pre=p1;
	if(p1 == link->tail){
		link->tail=p2;
	}else{
		p2->next->pre=p2;
	}
	link->len++;
	return OK;
}
Status DelectFM(FM_Link *link,FM *p){
	FM *pre;
	if(!p)return ERROR;
	pre=p->pre;
	pre->next=p->next;
	if(p == link->tail){
		link->tail=pre;
	}else {
		p->next->pre=pre;
	}
	link->len--;
	return OK;
}
Status MakeNode(FM **node,int size,Addr addr_start){
	(*node)=(FM*)malloc(sizeof(FM));
	if(!(*node))return ERROR;
	(*node)->pre=(*node)->next=NULL;
	(*node)->size=size;
	(*node)->addr_start=addr_start;
	(*node)->state=FREE;
	return OK;
}
void PrintLink(FM_Link link){
	FM *node;
	int i;
	printf("%3s%5s%5s%6s\n","NUM","ADDR","SIZE","STATE");
	printf("-------------------\n");
	for(node=(link.head)->next,i=0;node;node=node->next,i++){
		printf("%3d%5d%5d",i,node->addr_start,node->size);
		switch(node->state){
			case FREE:printf("%6s\n","FREE");break;
			case ALLOC:printf("%6s\n","ALLOC");break;
		}
	}
	printf("\n");
}
Addr Alloc_FF(int request_size){
	FM *node;
	FM *temp;
	int reminder;
	node=LocateFM(fmlink,request_size);
	if(node == NULL)return ERROR;
	reminder=node->size-request_size;
	if(reminder>min_size){
		if(MakeNode(&temp,reminder,node->addr_start+request_size) == ERROR)return ERROR;
		node->size-=reminder;
		if(InsertFM(&fmlink,node,temp) == ERROR)return ERROR;
	}
	node->state=ALLOC;
	return node->addr_start;
}
Status Free_FF(Addr addr_start){
	FM *node;
	for(node=(fmlink.head)->next;node;node=node->next)if(node->addr_start==addr_start)break;
	if(!node)return ERROR;
	if(node->pre->state == FREE){
		FM *temp=node->pre;
		temp->size+=node->size;
		if(DelectFM(&fmlink,node) == ERROR)return ERROR;
		free(node);
		node=temp;
	}
	if(node->next && node->next->state == FREE){
		FM *temp=node->next;
		node->size+=temp->size;
		if(DelectFM(&fmlink,temp) == ERROR)return ERROR;
		free(temp);
	}
	node->state=FREE;
	return OK;
}
Addr Alloc_BF(int request_size){
	FM *node;
	FM *temp;
	int reminder;
	node=LocateFM(fmlink,request_size);
	if(node == NULL)return ERROR;
	reminder=node->size-request_size;
	node->state=ALLOC;
	if(reminder>min_size){
		FM *temp2;
		if(MakeNode(&temp,reminder,node->addr_start+request_size) == ERROR)return ERROR;
		node->size-=reminder;
		if(temp2=LocateFM(fmlink,reminder))temp2=temp2->pre;
		else temp2=fmlink.tail;
		if(InsertFM(&fmlink,temp2,temp) == ERROR)return ERROR;
	}
	return node->addr_start;
}
Status Free_BF(Addr addr_start){
	FM *node;
	FM *pre;
	FM *next;
	FM *temp;
	for(node=(fmlink.head)->next;node;node=node->next)
		if(node->addr_start==addr_start)break;
	if(!node)return ERROR;

	if(node->addr_start){
		for(pre=(fmlink.head)->next;pre;pre=pre->next)
			if(node->addr_start==pre->addr_start+pre->size)break;
	}else pre=fmlink.head;

	for(next=(fmlink.head)->next;next;next=next->next)
		if(next->addr_start==node->addr_start+node->size)break;

	if(pre->state == FREE){
		pre->size+=node->size;
		if(DelectFM(&fmlink,node) == ERROR)return ERROR;
		free(node);
		node=pre;
	}
	if(next && next->state == FREE){
		node->size+=next->size;
		if(DelectFM(&fmlink,next) == ERROR)return ERROR;
		free(next);
	}
	if(DelectFM(&fmlink,node) == ERROR)return ERROR;
	if(temp=LocateFM(fmlink,node->size))temp=temp->pre;
	else temp=fmlink.tail;
	if(InsertFM(&fmlink,temp,node) == ERROR)return ERROR;
	node->state=FREE;
	return OK;
}
Addr Alloc_NF(int request_size){
	FM *node;
	FM *temp;
	int reminder;
	for(node=(fmlink.head)->next;node;node=node->next){
		if(node->state==FREE && node->addr_start>start_add &&node->size >= request_size)break;
	}
	if(!node)node=LocateFM(fmlink,request_size);
	if(!node)return ERROR;
	start_add=node->addr_start;
	reminder=node->size-request_size;
	if(reminder>min_size){
		if(MakeNode(&temp,reminder,node->addr_start+request_size) == ERROR)return ERROR;
		node->size-=reminder;
		if(InsertFM(&fmlink,node,temp) == ERROR)return ERROR;
		start_add=temp->addr_start;
	}
	node->state=ALLOC;
	return node->addr_start;
}
Status Free_NF(Addr addr_start){
	return Free_FF(addr_start);
}
Addr Alloc_WF(int request_size){
	FM *node;
	FM *temp;
	int reminder;
	node=LocateFM(fmlink,request_size);
	if(node == NULL)return ERROR;
	reminder=node->size-request_size;
	node->state=ALLOC;
	if(reminder>min_size){
		FM *temp2;
		if(MakeNode(&temp,reminder,node->addr_start+request_size) == ERROR)return ERROR;
		node->size-=reminder;
		temp2=fmlink.head;
		for(temp2=temp2->next;temp2;temp2=temp2->next){
			if(temp2->state==FREE && temp2->size <= reminder)break;
		}
		if(temp2)temp2=temp2->pre;
		else temp2=fmlink.tail;
		if(InsertFM(&fmlink,temp2,temp) == ERROR)return ERROR;
	}
	return node->addr_start;
}
Status Free_WF(Addr addr_start){
	FM *node;
	FM *pre;
	FM *next;
	FM *temp;
	for(node=(fmlink.head)->next;node;node=node->next)
		if(node->addr_start==addr_start)break;
	if(!node)return ERROR;							//确定要释放的分区位置

	if(node->addr_start){
		for(pre=(fmlink.head)->next;pre;pre=pre->next)
			if(node->addr_start==pre->addr_start+pre->size)break;
	}else pre=fmlink.head;							//确定要释放分区的前一个临接分区
													//若释放分区是第一个分区，则返回虚头指针
	for(next=(fmlink.head)->next;next;next=next->next)
		if(next->addr_start==node->addr_start+node->size)break;
													//确定要释放分区的后一个临接分区
	if(pre->state == FREE){
		pre->size+=node->size;
		if(DelectFM(&fmlink,node) == ERROR)return ERROR;
		free(node);
		node=pre;
	}
	if(next && next->state == FREE){
		node->size+=next->size;
		if(DelectFM(&fmlink,next) == ERROR)return ERROR;
		free(next);
	}
	if(DelectFM(&fmlink,node) == ERROR)return ERROR;
	temp=fmlink.head;
	for(temp=temp->next;temp;temp=temp->next){
		if(temp->state==FREE && temp->size <= node->size)break;
	}
	if(temp)temp=temp->pre;
	else temp=fmlink.tail;
	if(InsertFM(&fmlink,temp,node) == ERROR)return ERROR;
	node->state=FREE;
	return OK;
}
void PrintMenu(){
	printf("\n1.首次适应算法(FF)\n");
	printf("2.循环首次适应算法(NF)\n");
	printf("3.最佳适应算法(BF)\n");
	printf("4.最坏适应算法(WF)\n");
	printf("0.退出\n");
	printf("请选择算法:");
}
void PrintMenu_sub(){
	printf("\n1.申请内存\n");
	printf("2.释放内存\n");
	printf("3.测试默认数据\n");
	printf("4.测试随机数据\n");
	printf("5.查看分区链\n");
	printf("0.重新选择算法\n");
	printf("请选择操作:");
}
void RequestTest(Addr (*myalloc)(int),Status (*myfree)(Addr),int allsize){
	int addr[8];
	int size;
	DestroyLink(&fmlink);
	InitFMLink(&fmlink,640);
	printf("作业 1 申请 130 KB\n");
	addr[1]=myalloc(130);
	PrintLink(fmlink);
	getchar();
	printf("作业 2 申请  60 KB\n");
	addr[2]=myalloc(60);
	PrintLink(fmlink);
	getchar();
	printf("作业 3 申请 100 KB\n");
	addr[3]=myalloc(100);
	PrintLink(fmlink);
	getchar();
	printf("作业 2 释放  60 KB\n");
	if(myfree(addr[2]) == ERROR)return;
	PrintLink(fmlink);
	getchar();
	printf("作业 4 申请 200 KB\n");
	addr[4]=myalloc(200);
	PrintLink(fmlink);
	getchar();
	printf("作业 3 释放 100 KB\n");
	if(myfree(addr[3]) == ERROR)return;
	PrintLink(fmlink);
	getchar();
	printf("作业 1 释放 130 KB\n");
	if(myfree(addr[1]) == ERROR)return;
	PrintLink(fmlink);
	getchar();
	printf("作业 5 申请 140 KB\n");
	addr[5]=myalloc(140);
	PrintLink(fmlink);
	getchar();
	printf("作业 6 申请  60 KB\n");
	addr[6]=myalloc(60);
	PrintLink(fmlink);
	getchar();
	printf("作业 7 申请  50 KB\n");
	addr[7]=myalloc(50);
	PrintLink(fmlink);
	getchar();
	printf("作业 6 释放  60 KB\n");
	if(myfree(addr[6]) == ERROR)return;
	PrintLink(fmlink);
	DestroyLink(&fmlink);
	InitFMLink(&fmlink,allsize);
}
void RandomTest(Addr (*myalloc)(int),Status (*myfree)(Addr),int allsize){
	int ran;
	int addr;
	int size;
	int block;
	int i;
	srand((int)time(NULL));
	block=allsize/10;
	DestroyLink(&fmlink);
	InitFMLink(&fmlink,allsize);
	for(i=0;i<6;i++){
		size=rand()%block+1;
		addr=myalloc(size);
		if(addr == ERROR)printf("申请失败！\n");
		else{
			printf("内存首地址：%d\n",addr);
			printf("分配长度：%d\n",size);
			PrintLink(fmlink);
		}
	}
	while(fmlink.len > 1){
		if(rand()%100 < 45){
			size=rand()%block+1;
			addr=myalloc(size);
			if(addr == ERROR)printf("申请失败！\n");
			else{
				printf("内存首地址：%d\n",addr);
				printf("分配长度：%d\n",size);
				PrintLink(fmlink);
			}
		}else{
			int num;
			FM *node;
			num=fmlink.len-FMLinkLength_free(fmlink);
			ran=rand()%num;
			num=0;
			for(i=-1,node=(fmlink.head)->next;node;node=node->next){
				if(node->state == ALLOC)i++;
				if(i == ran)break;
				num++;
			}
			addr=node->addr_start;
			size=node->size;
			if(myfree(addr) == ERROR)printf("回收失败\n");
			else{
				printf("回收分区序号:%d\n",num);
				printf("内存首地址：%d\n",addr);
				printf("回收长度：%d\n",size);
				PrintLink(fmlink);
			}
		}
		getchar();
	}
}
void MemoryDynamic(Addr (*myalloc)(int),Status (*myfree)(Addr),int allsize){
	int choice;
	int addr;
	int size;
	if(allsize <= 0)allsize=640;
	InitFMLink(&fmlink,allsize);
	if(myalloc == Alloc_NF)start_add=0;
	while(1){
		PrintMenu_sub();
		scanf("%d",&choice);
		switch(choice){
			case 0:return;
			case 1:printf("请输入申请的内存大小：");
				   scanf("%d",&size);
				   addr=myalloc(size);
				   if(addr == ERROR)printf("申请失败！\n");
				   break;
			case 2:printf("请输入所释放的内存首地址：");
				   scanf("%d",&addr);
				   if(myfree(addr) == ERROR)printf("释放失败！\n");
				   break;
			case 3:RequestTest(myalloc,myfree,allsize);break;
			case 4:RandomTest(myalloc,myfree,allsize);break;
			case 5:PrintLink(fmlink);break;
			default:printf("请输入正确的选项！");
		}
	}
	DestroyLink(&fmlink);
}
int main(){
	int choice;
	while(1){
		PrintMenu();
		scanf("%d",&choice);
		switch(choice){
			case 0:return 0;
			case 1:MemoryDynamic(Alloc_FF,Free_FF,0);break;
			case 2:MemoryDynamic(Alloc_NF,Free_FF,0);break;
			case 3:MemoryDynamic(Alloc_BF,Free_BF,0);break;
			case 4:MemoryDynamic(Alloc_WF,Free_WF,0);break;
			default:printf("请输入正确的选项！");
		}
	}
	return 0;
}
