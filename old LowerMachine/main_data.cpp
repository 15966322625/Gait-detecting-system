#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "main_data.h"
#include "data_ADC_main.h"
#include "data_IMU_main.h"
#include "data_FOOT_main.h"
#include "config.h"
#include "global.h"
#include "data_WIN.h"
#include "Sem.h"
#include "shm.h"
#include "filt_ADC.h"
//#include "data_ADC_OFL_main.h"


//��ʱ����pid
static pid_t pid_dsp = 0;
static pid_t pid_DATA = 0;
static pid_t pid_main = 0;

//����״̬־
static int proc_status = STATUS_ON;

//ʱ�������
static unsigned int ms_T_count = 0;
static int W_count = 0;

//�ļ�������
static FILE* fp_ADC;
static FILE* fp_IMU;
static FILE* fp_FT;
static FILE* fp_ADC_OFF;
static FILE* fp_IMU_OFF;


//����������
static data_WIN *DATA_WIN = new data_WIN;


static void APP_exit()
{
	delete[] DATA_WIN;
	fclose(fp_IMU);
	fclose(fp_ADC);
	fclose(fp_FT);
	printf("=============== DATA EXIT! ==============\n");
	exit(0);
}


//注册三个信号
static void SigH(int Sig)
{
	switch (Sig)
	{
	case SIGINT://信号一 ctrl+c产生的信号 主要用于使整个程序退出
		break;
	case SIGALRM://信号二 在数据采集进程中为了捕获时钟中断 从而控制采集进程的信号
		ms_T_count += 1;
		break;
	case SIGUSR1://信号三 自定义信号 主要为了通知进程某个进程执行
		break;
	case SIGUSR2:
		proc_status = STATUS_OFF;//信号四 退出信号
		break;
	default:
		break;
	}
}


static void T_WIN_SLD()
{
	Sem_P(semid_timer);
	p_shm_TIMER->Timer1.tv_sec = p_shm_TIMER->Timer2.tv_sec;
	p_shm_TIMER->Timer1.tv_usec = p_shm_TIMER->Timer2.tv_usec;
	p_shm_TIMER->Timer2.tv_sec = p_shm_TIMER->Timer3.tv_sec;
	p_shm_TIMER->Timer2.tv_usec = p_shm_TIMER->Timer3.tv_usec;
	p_shm_TIMER->Timer3.tv_sec = p_shm_TIMER->Timer4.tv_sec;
	p_shm_TIMER->Timer3.tv_usec = p_shm_TIMER->Timer4.tv_usec;
	p_shm_TIMER->Timer4.tv_sec = p_shm_TIMER->Timer5.tv_sec;
	p_shm_TIMER->Timer4.tv_usec = p_shm_TIMER->Timer5.tv_usec;
	p_shm_TIMER->Timer5.tv_sec = p_shm_TIMER->Timer6.tv_sec;
	p_shm_TIMER->Timer5.tv_usec = p_shm_TIMER->Timer6.tv_usec;
	gettimeofday(&(p_shm_TIMER->Timer6), NULL);
	Sem_V(semid_timer);
}

//初始化四个信号
static void init_sig()
{
	signal(SIGINT, SigH);
	signal(SIGUSR1, SigH);
	signal(SIGALRM, SigH);
	signal(SIGUSR2, SigH);
}

//初始化周期时钟
//初始化一个定时器，这个定时器每隔1ms执行一次判断，将信号发送给数据采集进程，来控制采集速率;
//然后程序陷入死循环 开始数据采集;
static bool init_timer()
{
	struct itimerval timer_val;
	timer_val.it_value.tv_sec = 0;
	timer_val.it_value.tv_usec = 1000;				//1ms֮��ִ��
	timer_val.it_interval.tv_sec = 0;
	timer_val.it_interval.tv_usec = 1000;			//���1ms
	setitimer(ITIMER_REAL, &timer_val, NULL);
	W_count = 0;
	return true;
}

//等待ADC采集进程是否就绪
//如果ADC进程准备好了 会将共享内存中标志设置为true 
static void wait_ADC()
{
	int flag = false;
	while (!flag) {
		if (!Sem_P(semid_adcs))
		{
			printf("3\n");
			exit(1);
		}
		flag = p_shm_ADC->nflag;
		p_shm_ADC->nflag = false;                                
		Sem_V(semid_adcs);
	}
	printf(".......ADC.......OK !\n");
}

//等待IMU数据采集进程是否就绪
//就绪后将其在共享内存中的标志设置为true
static void wait_IMU()
{
	int flag = false;
	while (!flag) {
		if (!Sem_P(semid_imus))
		{
			printf("4\n");
			exit(1);
		}
		flag = p_shm_IMU->nflag;
		p_shm_IMU->nflag = false;
		Sem_V(semid_imus);
	}
	printf(".......IMU.......OK !\n");
}

static void wait_FT()
{
	int flag = false;
	while (!flag)
	{
		if (!Sem_P(semid_foot))
		{
			printf("4\n");
			exit(1);
		}
		flag = p_shm_FOOT->nflag;
		p_shm_FOOT->nflag = false;
		Sem_V(semid_foot);
	}
	printf(".......FOOT......OK !\n");
}

static void init_OFL()
{
#ifdef ADC_OF_DATA
	fp_ADC = fopen(ADC_FILE_NAME, "w");
	if (NULL == fp_ADC) 
	{
		printf("ERROR: Open file %s !\n ", ADC_FILE_NAME);
	}
#endif

#ifdef IMU_OF_DATA
	fp_IMU = fopen(IMU_FILE_NAME, "w");
	if (NULL == fp_IMU)
	{
		printf("ERROR: Open file %s !\n ", IMU_FILE_NAME);
	}
	for (int i=0;i<IMU_CH;i++)
	{
	fprintf(fp_IMU, ",ACC_x,ACC_y,ACC_z,GYR_x,GYR_y,GYR_z,Q_0,Q_1,Q_2,Q_3,Pitch,Yaw");
	}
	fprintf(fp_IMU, "\n");
#endif 

#ifdef FT_OF_DATA
	fp_FT = fopen(FT_FILE_NAME, "w");
	if (NULL == fp_FT)
	{
		printf("ERROR: Open file %s !\n ", FT_FILE_NAME);
	}
#endif 
	//���߶�ȡ����
#ifdef ADC_MODE_OL
#else
	fp_ADC_OFF = fopen("file_ADC2.csv", "r");
#endif

#ifdef IMU_MODE_OL
#else
	fp_IMU_OFF = fopen("file_IMU2.csv", "r");
	char ch[200];
	fgets(ch, 1024, fp_IMU_OFF);
#endif
}

static bool ADC_getData(float pData[8])
{
	static long Ts;
	bool result = true;
	int ret = fscanf(fp_ADC_OFF, "%ld,%f,%f,%f,%f,%f,%f,%f,%f\n",
		&Ts,
		&pData[0],
		&pData[1],
		&pData[2],
		&pData[3],
		&pData[4],
		&pData[5],
		&pData[6],
		&pData[7]
	);
	if (ret != 9)
	{
		printf("END: MAIN_DATA OFL_getData Failure!		%d\n", ret);
		result = false;
	}
	return result;
}


static void OFL_ADC(unsigned int Tnow, float SRC_Data[])
{
	fprintf(fp_ADC, "%d", Tnow);
	for (int n = 0; n < 8; n++) {
		fprintf(fp_ADC, ",%.6f", SRC_Data[n]);
	}
	fprintf(fp_ADC, "\n");
}

//���һ������ADC���ݣ�д�봰��
static void SHM_WRITE_ADC(unsigned int Tnow)
{
	static unsigned long T_count_ms_last_adc = Tnow;
	static float SRC_Data[8];
	static float FLT_Data[8];
#ifdef ADC_MODE_OL
	static unsigned int Ts = 1;
#else
	static unsigned int Ts = 1;
#endif
	if (Tnow - T_count_ms_last_adc >= Ts)
	{

		//�ɼ����ݡ�
		T_count_ms_last_adc = Tnow;
		if (!Sem_P(semid_adcs))
		{
			printf("4\n");
			exit(1);
		}
		for (int _CH = 0; _CH < 8; _CH++) 
		{
			SRC_Data[_CH] = p_shm_ADC->Data_src_CH[_CH];
			FLT_Data[_CH] = p_shm_ADC->Data_filt_CH[_CH];
		}
		p_shm_ADC->nflag = false;
		Sem_V(semid_adcs);
///���߶�ȡADC����
#ifdef ADC_MODE_OL
#else
		if (!ADC_getData(SRC_Data))
		{
			kill(pid_main, SIGINT);
		}
		ADC_filt(FLT_Data, SRC_Data);
		//FLT_Data = SRC_Data;
		for (int _CH = 0; _CH < 8; _CH++)
		{
			FLT_Data[_CH] = SRC_Data[_CH];
		}
#endif
		
#ifdef ADC_OF_DATA
		OFL_ADC(Tnow, SRC_Data);
#endif
		//	�����ݴ��봰����
		DATA_WIN->Append_ADC(FLT_Data);
	}
}


// ��������������
static bool OFL_IMU(int time, float pSrc[8][12])
{
	fprintf(fp_IMU, "%d", time);
	for (int _CH = 0; _CH < IMU_CH; _CH++)
	{
		//fprintf(fp_IMU, "\t");
		for (int n = 0; n < 12; n++)
		{
			fprintf(fp_IMU, ",%.6f", pSrc[_CH][n]);
		}
	}
	fprintf(fp_IMU, "\n");
	return true;
}

//���һ������IMU���ݣ� д�봰��
static void SHM_WRITE_IMU(unsigned int Tnow)
{
	static unsigned long T_count_ms_last = Tnow;
	static float Data_tmp_src[8][12];		//IMU ���ݻ�����������8ͨ����ÿͨ��12������
	static float Data_tmp_dest[8][12];
#ifdef ADC_MODE_OL
	static int unsigned Ts = 10;
#else
	static int unsigned Ts = 10;
#endif
	if (Tnow - T_count_ms_last >= Ts)
	{
		T_count_ms_last = Tnow;
		//	�Ӷ�Ӧ�Ĺ����ڴ������
		if (!Sem_P(semid_imus))
		{
			printf("5\n");
			exit(1);
		}
		for (int _CH = 0; _CH <IMU_CH; _CH++)
		{
			for (int n = 0; n < 12; n++)
			{
				Data_tmp_src[_CH][n] = p_shm_IMU->Data_src[_CH][n];
			}
		}
		Sem_V(semid_imus);
		IMU_filter(Data_tmp_dest,Data_tmp_src);

//IMU���߶�ȡ
#ifdef IMU_MODE_OL
#else
		static int a;
		fscanf(fp_IMU_OFF, "%d", &a);
		for (int _CH = 0; _CH < 3; _CH++)
		{
			for (int i = 0; i < 12; i++)
			{
				fscanf(fp_IMU_OFF, ",%f", &Data_tmp_src[_CH][i]);
			}
		}
		fscanf(fp_IMU_OFF, "\n");
		IMU_filter(Data_tmp_dest,Data_tmp_src);
#endif

#ifdef IMU_OF_DATA
		OFL_IMU(Tnow, Data_tmp_src);	//�������߻���
		
#endif

		//	�����ݴ��빲���ڴ�
		DATA_WIN->Append_IMU(Data_tmp_dest);
	}

}


//���ѹ����������
static void OFL_FT(unsigned int Tnow, int FT_Data[])
{
	fprintf(fp_FT, "%d", Tnow);
	for (int n = 0; n < 4; n++)
	{
		fprintf(fp_FT, ",%d", FT_Data[n]);
	}
	fprintf(fp_FT, "\n");
}

static void SHM_WRITE_FT(unsigned int Tnow)
{
	static unsigned long T_count_ms_last_adc = Tnow;
	static int FT_Data[4];
	if (Tnow - T_count_ms_last_adc >= 1)
	{
		T_count_ms_last_adc = Tnow;
		Sem_P(semid_foot);
		for (int i = 0; i < 4; i++)
		{
			FT_Data[i] = p_shm_FOOT->FT_DATA[i];
		}
		p_shm_ADC->nflag = false;
		Sem_V(semid_foot);
		//��������
#ifdef FT_OF_DATA
		OFL_FT(Tnow, FT_Data);
#endif
		//	�����ݴ��봰����
		//DATA_WIN->Append_FT(FT_Data);
	}
}


//��ȫ�ֱ����е�pid��á�
static void init_glbs()
{
	//��DATA���̺�д��GLB������ȡdsp���̺�
	pid_DATA = getpid();
	if (!Sem_P(semid_glbs))
	{
		printf("46\n");
		exit(1);
	}
	p_shm_GLB->PID.PID_DATA = pid_DATA;
	Sem_V(semid_glbs);

	//�ӳ�300us���ڶ�ȡ��
	do 
	{
		usleep(300);
		if (!Sem_P(semid_glbs))
		{
			printf("6\n");
			exit(1);
		}
		pid_dsp = p_shm_GLB->PID.PID_DSP;
		pid_main = p_shm_GLB->PID.PID_MAIN;
		Sem_V(semid_glbs);
		
	} while (pid_dsp == 0 || pid_main == 0);
}

static void init_WIN_SLD()
{
	Sem_P(semid_timer);
	memset(p_shm_TIMER,0,sizeof(SHM_TIMER_t));
	Sem_V(semid_timer);
}



void main_data()
{
	init_sig();//初始化信号
	init_glbs();//初始化全局
	wait_ADC();//等待ADC
	wait_IMU();
	wait_FT();
	init_OFL();//初始化数据离线保存  	//���ɼ������������ߡ�
	int Tnow;
	init_WIN_SLD();
	
	init_timer();//初始化时钟周期
	while (true)
	{	//	�������ݻ������ķ���ѭ��
		if (proc_status == STATUS_OFF)
		{
			APP_exit();//进程退出
		}
		Tnow = ms_T_count;
		//��ʱ��������ÿ��������������һ�Ρ��ڸ�������֮ǰ���м�ʱ��
		if (W_count == 0)
		{
			W_count++;
			T_WIN_SLD();
		}
		SHM_WRITE_ADC(Tnow);		//��һ�������ж�ȡ  35  ����ͨ�����ݣ������������ʱ��ͬ�����ݽṹ�С�
		SHM_WRITE_IMU(Tnow);
		SHM_WRITE_FT(Tnow);
		//�ж��ǲ��Ǵ�����40��.
		if (DATA_WIN->Enable())
		{
			//ÿ40ms��ʱ���֮���ٴμ�ʱ��
			W_count = 0;
			Sem_P(semid_DATA);
			DATA_WIN->DataCpy(p_shm_DATA, &(DATA_WIN->DATW));	//�������ݿ����������ڴ�
			p_shm_DATA->Tnow = Tnow;
			Sem_V(semid_DATA);
			DATA_WIN->Clear();
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.en_dsp = true;
			Sem_V(semid_glbs);
			kill(pid_dsp, 10);
		}
		usleep(10);
	}
}
