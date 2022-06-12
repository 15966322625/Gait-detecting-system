#include <signal.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "dsp_RCGClass.h"
#include "dsp_RCG_main.h"
#include "shm.h"
#include "Sem.h"
#include "global.h"
#include "main_data.h"
#include "dsp_ACT.h"
#include "main_tcp.h"
#include "BP_DATA.h"
#include "global.h"



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
static int Count = 0;  //��������������

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
		//���ı�־λ��
		Sem_P(semid_glbs);
		p_shm_GLB->proc_status.T_status = STATUS_OFF;
		Sem_V(semid_glbs);
		//��ȡ200ms�ĵ�һ�����ݰ���
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

void dsp_RCG_main()
{
	init_sig();		//��ʼ���ź�
	init_glbs();	//��ʼ��ȫ��
	BP_DATA();
	RCG->Init();		//������ݴ�����ʼ��
	wait_TCP();
	OF_L();
	while (true)
	{
		//usleep(100);
		//	�����˳�
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
		//��ȡ��ʱ��ʼʱ�䡣
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
			#ifdef ANS
			BP_off<<"=====================\n\n"<<std::endl;
			#endif
		}
		//�������ݴ���
		if (en_RCG_flag)
		{
			en_RCG_flag = STATUS_OFF;
			printf("EN_RCG........................................................\n");
			//1.���ݱ���
			Sem_P(semid_ACT);
			RCG->BackUp(p_shm_ACT);
			Sem_V(semid_ACT);
			//2.������ݼ����Լ�����ģʽʶ��
			int t_now = RCG->ACTD.Tnow;
			RCG->Recg();
			//��������
			for (int i = 0; i < 6; i++)
			{
				ST_tmp[i] = RCG->RCG_ret[i];
			}
			//找出最大值索引
			int Max_index = RCG->Max_Index;
			static int count_2 = 0;
			switch (T_Flag)
			{
				// 检测站立与起跳下蹲
			case 0:
				if (ST_tmp[0] > ST_tmp[1])
				{
					Label = 0;
					T_Flag = 0;
					Count = 0;
				}
				else if (ST_tmp[0] < ST_tmp[1])
				{
					Count++;
					Label = 0;
					T_Flag = 0;    //վ��
					if (Count >= 2)
					{
						T_Flag = 1;
						Label = 1; //�¶�
						Count++;
					}
				}
				break;
				// 检测起跳下蹲与起跳起身
			case 1:
				if (ST_tmp[1] < ST_tmp[2])
				{
					Count++;
					count_2=0;
					T_Flag = 1;
					Label = 1;
					if (Count >= 2)
					{
						T_Flag = 2;
						Label = 2;
						Count = 0;
					}
				}
				else if(Max_index == 0 || Max_index == 5)
				{
					count_2++;
					Count = 0;
					T_Flag = 1;
					Label = 1;
					if(count_2>=4)
					{
						Label = 0;
						T_Flag = 0;
						Count = 0;
						count_2=0;
					}

				}
				else
				{
					count_2=0;
					Count = 0;
					Label = 1;
					T_Flag = 1;
				}
				break;
				//检测起身与空中
			case 2:
				if (ST_tmp[2] > ST_tmp[3])
				{
					Label = 2;
					T_Flag = 2;
					Count = 0;
				}
				else
				{
					Label = 2;
					T_Flag = 2;
					Count++;
					if (Count >= 2)
					{

						Label = 3;
						T_Flag = 3;
						Count = 0;
					}
					
				}
				break;
				//检测空中与落地
			case 3:
				if (ST_tmp[3] > ST_tmp[4])
				{
					Label = 3;
					T_Flag = 3;
					Count = 0;
				}
				else
				{
					Label = 3;
					T_Flag = 3;
					Count++;
					if (Count >= 2)
					{

						Label = 4;
						T_Flag = 4;
						Count = 0;
					}
				}
				break;
				//检测落地与起身
			case 4:
				if (ST_tmp[4] > ST_tmp[5])
				{
					Label = 4;
					T_Flag = 4;
					Count = 0;
				}
				else
				{
					Label = 4;
					T_Flag = 4;
					Count++;
					if (Count >= 2)
					{

						Label = 5;
						T_Flag = 5;
						Count = 0;
					}
				}
				break;
				//判断起身与站立
			case 5:
				if (ST_tmp[5] > ST_tmp[0])
				{
					Label = 5;
					T_Flag = 5;
					Count=0;
				}
				else
				{
					Label = 5;
					T_Flag = 5;
					Count++;
					if(Count >= 2)
					{
						Label = 0;
						T_Flag = 0;
						Count = 0;
					}
				}
				break;

			default:
				break;
			}



			gettimeofday(&T_end, NULL);
			DUR = (double)(1000 * (T_end.tv_sec - T_start.tv_sec) + (T_end.tv_usec - T_start.tv_usec)/1000.0);

			printf("BP: %d\n", Label);
			fprintf(fp_De, "%d,%d,%ld,%ld\n",t_now,Label,T_end.tv_sec,T_end.tv_usec);
			fflush(fp_De);


			T_last_end.tv_sec = T_end.tv_sec;
			T_last_end.tv_usec = T_end.tv_usec;

			//TCP�����ڴ档
			Sem_P(semid_TCP);
			p_shm_TCP->Status = Label;
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