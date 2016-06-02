#include<string.h>
#include<stdio.h>
#include <stdlib.h>
#include<conio.h>
#include"includes.h"
#define TASK_STK_SIZE 512
OS_STK MenuTaskStk[TASK_STK_SIZE];
OS_STK GameTaskStk[TASK_STK_SIZE];
OS_STK CountTaskStk[TASK_STK_SIZE];

void MenuTask(void *data);
void GameTask(void *data);
void CountTask(void *data);

void BallMove(void);
void BalHit(void);
void BGprint(void);

char *s; //MyTask发送消息的指针
char *ss; //YouTask接受到的消息的指针
INT8U err;
INT8U y=0;
INT32U  Times=0;
OS_EVENT *Str_Box;    //定义事件控制块指针 定义消息邮箱的指针

INT16S key;
int scr[22][50], q=14, w=30, ban=20, score=1, sx=-1, zy=-1, speed=1, chang=30,i=0,j=0;

void main (void)
{
	OSInit();
	PC_DOSSaveReturn();
	PC_VectSet(uCOS,OSCtxSw);
	OSTaskCreate(MenuTask,NULL,&MenuTaskStk[TASK_STK_SIZE-1],0);
	OSStart();
} 

void MenuTask(void *pdata)
{
	#if OS_CRITICAL_METHOD ==3
		OS_CPU_SR cpu_sr;
	#endif
		pdata = pdata;
		OS_ENTER_CRITICAL();
		PC_VectSet(0x08,OSTickISR);
		PC_SetTickRate(OS_TICKS_PER_SEC);
		OS_EXIT_CRITICAL();
		OSStatInit();

		q=14, w=30, ban=20, score=1, sx=-1 , zy=-1 ,speed=1, chang=30;
		memset(scr,0, sizeof(int)*1100);//清空内存
		system("cls");
		
		printf("RULES: \n press A to move LEFT,\n press D to move RIGHT,\n press W to PAUSE \n ANY KEY to START!!!\n ");
		if(getch())
			OSTaskCreate(GameTask,NULL,&GameTaskStk[TASK_STK_SIZE-1],2);
			OSTaskSuspend(OS_PRIO_SELF);
		
		if(PC_GetKey(&key) == TRUE)//按esc退出
		{
			if(key == 0x1B)
			{
				PC_DOSReturn();
			}
		}
		

}

void GameTask(void *pdata)
{
	#if OS_CRITICAL_METHOD ==3
		OS_CPU_SR cpu_sr;
	#endif
		pdata = pdata;

	for(j=0;j<50;j++)
		scr[i][j]=4;//将最上面的一行赋值为4,用于绘制边界
	j=0;i=1;
	for(i=1;i<5;i++)
		for(j=2;j<48;j++)
			scr[i][j]=3;//3表示砖头位置
	scr[q][w]=2;//2表示球的位置

	while(1)
	{
		for(j=0;j<20;j++)//将板位置归0
			scr[21][ban+j]=0;
		if(kbhit())//控制板的移动
		switch(getch())
		{
			case'A':
				if(ban>1&&ban<=29) ban--;
				break;
			case'D':
				if(ban>0&&ban<=28) ban++;
				break;
			case'W':
				getch();
				break;//暂停键
			case'Q':
				PC_DOSReturn();
				break;
		}
		for(j=0;j<20;j++)//建立板位置为1
			scr[21][ban+j]=1;
			
		BalHit();
		BallMove();
		BGprint();
		
		sprintf(s,"%d",Times);  //把Times赋给s
		OSMboxPost(Str_Box,s);  //发送消息s,其中两个参数Str_Box是OS_EVENT *pevent表示消息邮箱指针(是事件控制块指针)，s是void *msg表示消息指针该函数表示把消息s发送到消息邮箱Str_Box中
		Times++;  //MyTask的运行次数加1
		OSTimeDlyHMSM(0,0,0,130);
	}	
}

void BalHit(void) 
{
	int b;
	b=0;
	if(zy==1)//以下四个if用来检测撞砖，并改变球的运动方向
		if(scr[q][w+1]==3||w+1==49)
		{
			scr[q][w+1]=0;
			zy=-zy;
			b=1;
		}
	if(zy==-1)//判断运动又有方向
		if(scr[q][w-1]==3||w-1==0)
		{
			scr[q][w-1]=0;
			zy=-zy;
			b=1;//用来确认是否敲打左右上下的方块
		}

	if(sx==1)
		if(scr[q-1][w]==3||q-1==0)
		{
			scr[q-1][w]=0;
			sx=-sx;
			b=1;
		}
	if(sx==-1)
		if(scr[q+1][w]==3||q+1==21)
		{
			if(scr[q+1][w]==3)
			{
				scr[q+1][w]=0;
				sx=-sx;
				b=1;
			}
			if(q+1==21)
			{
				if(scr[q+1][w]==1)
					sx=-sx;
				else
				{
					printf("<!--GAME OVER-->");
					OSTaskCreate(CountTask,NULL,&CountTaskStk[TASK_STK_SIZE-1],4);
					OSTaskSuspend(2);
				}
			}
		}
	if(b==0)//斜的砖块
	{
		if(zy==1&&sx==1)
			if(scr[q-1][w+1]==3)
			{
				scr[q-1][w+1]=0;
				zy=-zy;
				sx=-sx;
			}


		if(zy==-1&&sx==1)
			if(scr[q-1][w-1]==3)
			{
				scr[q-1][w-1]=0;
				zy=-zy;
				sx=-sx;
			}


		if(zy==1&&sx==-1)
			if(scr[q+1][w+1]==3)
			{
				scr[q+1][w+1]=0;
				zy=-zy;
				sx=-sx;
			}


		if(zy==-1&&sx==-1)
			if(scr[q+1][w-1]==3)
			{
				scr[q+1][w-1]=0;
				zy=-zy;
				sx=-sx;
			}//斜砖完毕
	}
}


void BallMove(void)
{
	scr[q][w]=0;//使原来的球位置为0,利用下面的if来移动球的位置 
	if(sx==1&&zy==1)
	{
		q--;
		w++;
	}
	if(sx==1&&zy==-1)
	{
		q--;
		w--;
	}
	if(sx==-1&&zy==1)
	{
		q++;
		w++;
	}
	if(sx==-1&&zy==-1)
	{
		q++;
		w--;
	}
	scr[q][w]=2;//在新的位置赋值2方便绘制上新的小球
}


void BGprint(void)
{
	int i, j;
	system("cls");
	for(i=0;i<22;i++)
	{
		for(j=0;j<50;j++)
		{
			if(scr[i][j]==0)printf(" ");
			if(scr[i][j]==1)printf("\3");
			if(scr[i][j]==2)printf("\4");
			if(scr[i][j]==3)printf("\5");
			if(scr[i][j]==4)printf("+");
			if(j==49)
				printf("+\n");
		}
	}
}

void  CountTask(void *pdata)
{
	#if OS_CRITICAL_METHOD == 3       
		OS_CPU_SR  cpu_sr;
	#endif
	pdata=pdata;
	
	memset(scr,0, sizeof(int)*1100);//清空内存
	system("cls");
	ss=OSMboxPend(Str_Box,10,&err);  //请求消息邮箱,参数表示：Str_Box是消息邮箱指针，10表示等待时间，err表示错误信息，返回值ss是邮箱中的消息指针
	PC_DispStr(10,++y,ss,DISP_BGND_BLACK+DISP_FGND_WHITE);  //显示消息的内容
	OSTimeDlyHMSM(0,0,1,0);    //等待1s               

	if(PC_GetKey(&key) == TRUE)//按esc退出
	{
		if(key == 0x1B)
		{
			PC_DOSReturn();
		}
	}
}

