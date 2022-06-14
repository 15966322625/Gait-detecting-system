#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>


#include "data_ADC_main.h"
#include "Sem.h"
#include "global.h"
#include "shm.h"
#include "ADC_OL.h"
#include "filt_ADC.h"
#include "config.h"
#include "filt_ADC.h"

//**********************************ADC数据采集进程***********************//

#ifdef ADC_MODE_OL
ADC_OL *ADC = new ADC_OL;
#endif


//��ʱ����pid
static pid_t pid_ADC;
static pid_t pid_main;

//ADC����
static float SRC_Data[8] = { 0 };
static float FLT_Data[8] = { 0 };

//����״̬־
static int proc_status = STATUS_ON;

//*************************************  数据采集进程  ************************//
//初始化几个信号
static void SigH(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		break;
	case 10:
		break;
	case 12:
		proc_status = STATUS_OFF;
		break;
	default:
		break;
	}
}
//初始化三个信号
static void init_sig()
{
	signal(SIGINT, SigH);
	signal(10, SigH);
	signal(12, SigH);
}

//初始化全局
static void init_glbs()
{
	pid_ADC = getpid();
	do {
		usleep(300);
		Sem_P(semid_glbs);
		p_shm_GLB->PID.PID_ADC = pid_ADC;
		pid_main = p_shm_GLB->PID.PID_MAIN;
		Sem_V(semid_glbs);
	} while (pid_main == 0);
}

static void APP_exit()
{
#ifdef ADC_MODE_OL
	delete ADC;
#endif
	printf("=============== ADC  EXIT! ==============\n");
	fflush(stdout);
	exit(0);
}

//进行数据的循环采集 每一次采集都要判断进程是否需要退出 如果需要退出 指向APP_exit函数
void data_ADC_main()
{
	init_sig();
	init_glbs();
	//ADC��ʼ����
#ifdef ADC_MODE_OL
	ADC->init();//初始化数据采集，主要是为了读取离线文件的数据，打开一个离线文件;
	printf("ONLINE***********\n");
#else
	//ADC_init();
	printf("OFFLINE***********\n");
#endif
	Sem_P(semid_adcs);
	p_shm_ADC->nflag = false;
	Sem_V(semid_adcs);
	while (1)
	{
		usleep(200);
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
		//����ѭ���ɼ�
		//ÿ�βɼ�һ֡������
#ifdef ADC_MODE_OL
		ADC->getData(SRC_Data);
#endif
		ADC_filt(FLT_Data, SRC_Data);
		//�����ڴ�
		Sem_P(semid_adcs);
		for (int _CH = 0; _CH < 8; _CH++) 
		{
			p_shm_ADC->Data_src_CH[_CH] = SRC_Data[_CH];
			p_shm_ADC->Data_filt_CH[_CH] = FLT_Data[_CH];
		}
		p_shm_ADC->nflag = true;
		Sem_V(semid_adcs);
	}
}