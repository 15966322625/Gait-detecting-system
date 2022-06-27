#include <signal.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "tensorflow/lite/examples/LowerMachine/dsp_RCGClass.h"
#include "tensorflow/lite/examples/LowerMachine/dsp_RCG_main.h"
#include "tensorflow/lite/examples/LowerMachine/shm.h"
#include "tensorflow/lite/examples/LowerMachine/Sem.h"
#include "tensorflow/lite/examples/LowerMachine/global.h"
#include "tensorflow/lite/examples/LowerMachine/main_data.h"
#include "tensorflow/lite/examples/LowerMachine/dsp_ACT.h"
#include "tensorflow/lite/examples/LowerMachine/main_tcp.h"
#include "tensorflow/lite/examples/LowerMachine/BP_DATA.h"
#include "tensorflow/lite/examples/LowerMachine/global.h"



static FILE *fp_De;

//��ʱ����pid
static pid_t pid_main_tcp = 0;
static pid_t pid_RCG = 0;

//״̬־
static int en_RCG_flag;
static int ACT_STATUS = 0;
static int T_status;

//��������
static dsp_RCGClass *RCG = new dsp_RCGClass;

//״̬־
static int proc_status = STATUS_ON;
static int TCP_status = STATUS_OFF;

//����
static float ST_tmp[6] = { 0 };    //0  վ��   1 �¶�   2 ����  3 ƽ��  BPʶ������
static int T_Flag = 0;  //��־λ T_Flag = 0ʱ���վ�����¶ף�1ʱ�����Ծ����У� 2�����ؼ�ƽ��  3������ƽ���������
static int Label = 0;  // 0 站立 1 预备起跳 2 起跳 3 空中  4 落地下蹲 5 起身
static int Count = 0;  //

//��ʱ����
static timeval T_start;
static timeval T_end;
static timeval T_last_end;
static double DUR;

static void APP_exit()
{
	BP_delete();
	
	#ifdef ANS
	BP_off.close();
	#endif
	delete RCG;

	printf("=============== RCG  EXIT! ==============\n");
	exit(0);

}


static void SigH(int sig)
{
	switch (sig) 
	{
	case SIGINT:			//�ⲿ������Ҫ������˳���
		break;
	case 10:				
		Sem_P(semid_glbs);
		//RCG��־
		en_RCG_flag = p_shm_GLB->proc_status.en_rcg;
		p_shm_GLB->proc_status.en_rcg = STATUS_OFF;
		//ACT������־
		ACT_STATUS = p_shm_GLB->proc_status.proc_ACT_status;
		//��ʱ��־��
		T_status = p_shm_GLB->proc_status.T_status;
		Sem_V(semid_glbs);
		break;
	case 12:
		proc_status = STATUS_OFF;
		break;
	default:
		break;
	}
}


static void init_sig()
{
	signal(SIGINT, SigH);
	signal(10, SigH);
	signal(12, SigH);
}

//��RCG���̺�д�룬����ȡtcp���̺�
static void init_glbs()
{
	pid_RCG = getpid();
	Sem_P(semid_glbs);
	p_shm_GLB->PID.PID_RCG = pid_RCG;
	Sem_V(semid_glbs);
	do
	{
		usleep(300);
		Sem_P(semid_glbs);
		pid_main_tcp = p_shm_GLB->PID.PID_MAIN_TCP;
		Sem_V(semid_glbs);
	} while (pid_main_tcp == 0);
	
}

static void wait_TCP()
{
	do 
	{
		usleep(300);
		Sem_P(semid_glbs);
		TCP_status = p_shm_GLB->proc_status.proc_TCP_status;
		Sem_V(semid_glbs);
	} while (TCP_status == STATUS_OFF);
	printf("TCP.........OK!!!\n");
}


static void OF_L()
{
	fp_De = fopen("A_BP_DE.csv", "w");
}

static void Get_timer()
{
	switch (T_status)
	{
	case 0:
		break;
	case 1:
		//
		Sem_P(semid_glbs);
		p_shm_GLB->proc_status.T_status = STATUS_OFF;
		Sem_V(semid_glbs);
		//
		Sem_P(semid_timer);
		T_start.tv_sec = p_shm_TIMER->Timer1.tv_sec;
		T_start.tv_usec = p_shm_TIMER->Timer1.tv_usec;
		Sem_V(semid_timer);
		T_status = 0;
		break;
	case 2:
		//���ı�־λ��
		Sem_P(semid_glbs);
		p_shm_GLB->proc_status.T_status = STATUS_OFF;
		Sem_V(semid_glbs);

		//��ȡ200ms�ĵ�һ�����ݰ���
		Sem_P(semid_timer);
		T_start.tv_sec = p_shm_TIMER->Timer5.tv_sec;
		T_start.tv_usec = p_shm_TIMER->Timer5.tv_usec;
		Sem_V(semid_timer);

		//========
		T_start.tv_sec = T_last_end.tv_sec;
		T_start.tv_usec = T_last_end.tv_usec;

		T_status = 0;
		break;
	default:
		break;
	}
}

//检测步行动作的五个步态相位
static int decisionfoot(int U, float outputs[5], int& Tflag){
	//检测步态的五个相位，从0开始,站立时为0
	static int count2 = 0;
	int Label = 0;
	//步态初始状态 0或4，
	//步态 0-》1-》2-》3-》4->0
	switch(Tflag){
		//检测站立与下蹲
		case 0: {
			//检测摆动准备期，从0或4转成1
			if(outputs[1]>outputs[0]){
				count2++;
				Label = 0;
				Tflag = 0;
				if(count2 >=2){
					//进入摆动准备期
					Label = 1;
					count2 = 0;
					Tflag = 1;
				}
			}else{
				//还没有进入摆动准备期
				Label = 0;
				count2 = 0;
				Tflag = 0;
			}
			break;
		}
		case 1:{
			//摆动准备期到摆动期 从1转成2
			if(outputs[1]>outputs[2]){
				Label = 1;
				count2 = 0;
				Tflag = 1;
			}else{
				count2++;
				Label = 1;
				Tflag = 1;    
				if (count2 >= 2)
				{
					Tflag = 2;
					Label = 2; 
					count2 = 0;
				}
			}
			break;

		}
		case 2:{
			//摆动准备期与支撑前期 从2-》3
			if(outputs[2]>outputs[3]){
				Label = 2;
				count2 = 0;
				Tflag = 2;
			}else{
				count2++;
				Label = 2;
				Tflag = 2;    
				if (count2 >= 2)
				{
					Tflag = 3;
					Label = 3; 
					count2 = 0;
				}
			}
			break;
		}
		case 3:{
			//支撑前期3到支撑中期4  从3到4
			if(outputs[3]>outputs[4]){
				Label = 3;
				count2 = 0;
				Tflag = 3;
			}else{
				count2++;
				Label = 3;
				Tflag = 3;    
				if (count2 >= 2)
				{
					Tflag = 4;
					Label = 4; 
					count2 = 0;
				}
			}
			break;
		}
		case 4:{
			//支撑中期4到支撑末期0   
			if(outputs[4]>outputs[0]){
				Label = 4;
				count2 = 0;
				Tflag = 4;
			}else{
				count2++;
				Label = 4;
				Tflag = 4;    
				if (count2 >= 2)
				{
					Tflag = 0;
					Label = 0; 
					count2 = 0;
				}
			}
			break;
		}
		default:break;
	}
	return Label;

}

//检测跳跃动作的五个步态相位
static int decisionjump(int U, float outputs[5], int& Tflag){
	static int count = 0;
	int Label = 0;
	//跳跃 1-》2-》3-》4-》0
	switch(Tflag){
		//检测站立与下蹲
		case 0: {
			//检测下蹲1与起跳2 
			if(outputs[1]>outputs[2]){
				Label = 1;
				count = 0;
				Tflag = 0;
			}else{
				count++;
				Label = 1;
				Tflag = 0;    
				if (count >= 2)
				{
					Tflag = 1;
					Label = 2; 
					count = 0;
				}
			}
			break;
		}
		case 1:{
			//检测起跳2与空中3
			if(outputs[2]>outputs[3]){
				Label = 2;
				count = 0;
				Tflag = 1;
			}else{
				count++;
				Label = 2;
				Tflag = 1;    
				if (count >= 2)
				{
					Tflag = 2;
					Label = 3; 
					count = 0;
				}
			}
			break;

		}
		case 2:{
			//检测空中3与落地4 
			if(outputs[3]>outputs[4]){
				Label = 3;
				count = 0;
				Tflag = 2;
			}else{
				count++;
				Label = 3;
				Tflag = 2;    
				if (count >= 2)
				{
					Tflag = 3;
					Label = 4; 
					count = 0;
				}
			}
			break;
		}
		case 3:{
			//检测落地4与起立0
			if(outputs[4]>outputs[0]){
				Label = 4;
				count = 0;
				Tflag = 3;
			}else{
				count++;
				Label = 4;
				Tflag = 3;    
				if (count >= 2)
				{
					Tflag = 4;
					Label = 0; 
					count = 0;
				}
			}
			break;
		}
		case 4: {
			//检测起立0 与下蹲1 
			if(outputs[0]>outputs[1]){
				Label = 0;
				count = 0;
				Tflag = 4;
			}else{
				count++;
				Label = 0;
				Tflag = 4;    
				if (count >= 2)
				{
					Tflag = 0;
					Label = 1; 
					count = 0;
				}
			}
			break;
		}
		default:break;
	}
	return Label;
}

//关于U V的定义：
//U是对六个动作的划分变量
//V是对六个动作的五个步态相位的划分变量 

void dsp_RCG_main()
{
	init_sig();		//��ʼ���ź�
	init_glbs();	//��ʼ��ȫ��
	BP_DATA();
	RCG->Init();		//������ݴ�����ʼ��
	wait_TCP();
	OF_L();
	int tflagjump=0;
	int tflagfoot=0;
	float outputsoftmax[5];
	int lastU = -1;
	static int countU = 0;
	while (true)
	{
		//usleep(100);
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}

		Get_timer();
		//����Ƿ�������������0״̬��ʼ��
		if (ACT_STATUS == 1)
		{
			ACT_STATUS = 0;
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.proc_ACT_status = STATUS_OFF;
			Sem_V(semid_glbs);
			T_Flag = 0;
			printf("ACT OK!!!\n");
			fprintf(fp_De, "ACT OK!!!\n");

			tflagjump=0;
			tflagfoot=0;
			lastU = -1;

			#ifdef ANS
			BP_off<<"=====================\n\n"<<std::endl;
			#endif
		}
		//�������ݴ���
		if (en_RCG_flag)
		{
			en_RCG_flag = STATUS_OFF;
			//1.���ݱ���
			Sem_P(semid_ACT);
			RCG->BackUp(p_shm_ACT);
			Sem_V(semid_ACT);
			//2.������ݼ����Լ�����ģʽʶ��
			int t_now = RCG->ACTD.Tnow;
			RCG->Recg();






			int U = RCG->actionU;//初次划分动作标识 U 表示处于六个动作的哪一个动作
			int V = RCG->gaitV;//二次划分步态标识 V 表示处于哪一个动作中的哪一个步态
			int label = 0;
			for(int i=0;i<5;++i){
				outputsoftmax[i] = RCG->outputsANS[i];
			}

			if(lastU == -1){
				//进入新的活动段，步态相位从头开始判断
				tflagjump=0;
				tflagfoot=0;
				lastU = U;
			}

			if(U == 5){
				label = decisionjump(U, outputsoftmax, tflagjump);//对跳跃动作的五个步态进行打标签动作
			}else{
				label = decisionfoot(U, outputsoftmax, tflagfoot);//对步行动作的五个步态进行打标签动作
			}
			
			V = label;//二次划分
			// static int count_2 = 0;
			gettimeofday(&T_end, NULL);
			DUR = (double)(1000 * (T_end.tv_sec - T_start.tv_sec) + (T_end.tv_usec - T_start.tv_usec)/1000.0);
			fprintf(fp_De, "%d,%d,%d,%f,%f,%f,%f,%f,%ld,%ld\n",t_now,U,V,outputsoftmax[0],outputsoftmax[1],outputsoftmax[2],outputsoftmax[3],outputsoftmax[4],T_end.tv_sec,T_end.tv_usec);
			fflush(fp_De);

			T_last_end.tv_sec = T_end.tv_sec;
			T_last_end.tv_usec = T_end.tv_usec;

			//TCP
			Sem_P(semid_TCP);
			p_shm_TCP->Status = U*10+V;
			p_shm_TCP->DurTime = DUR;
			Sem_V(semid_TCP);
			//֪ͨTCP
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.en_tcp = STATUS_ON;
			Sem_V(semid_glbs);

			kill(pid_main_tcp, 10); 
		}
	}
}