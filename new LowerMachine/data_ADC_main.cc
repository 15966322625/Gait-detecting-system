#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>


#include "tensorflow/lite/examples/LowerMachine/data_ADC_main.h"
#include "tensorflow/lite/examples/LowerMachine/Sem.h"
#include "tensorflow/lite/examples/LowerMachine/global.h"
#include "tensorflow/lite/examples/LowerMachine/shm.h"
#include "tensorflow/lite/examples/LowerMachine/ADC_OL.h"
#include "tensorflow/lite/examples/LowerMachine/filt_ADC.h"
#include "tensorflow/lite/examples/LowerMachine/config.h"
#include "tensorflow/lite/examples/LowerMachine/filt_ADC.h"

//**********************************����������***********************//

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

//*************************************  ������  ************************//

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

static void init_sig()
{
	signal(SIGINT, SigH);
	signal(10, SigH);
	signal(12, SigH);
}

//��adc���̺�д��glb
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




void data_ADC_main()
{
	init_sig();
	init_glbs();
	//ADC��ʼ����
#ifdef ADC_MODE_OL
	ADC->init();
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
#ifdef ADC_MODE_OL
		ADC->getData(SRC_Data);
#endif
		ADC_filt(FLT_Data, SRC_Data);
		//
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