#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/timerfd.h>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>

#include "tensorflow/lite/examples/LowerMachine/main_data.h"
#include "tensorflow/lite/examples/LowerMachine/data_ADC_main.h"
#include "tensorflow/lite/examples/LowerMachine/data_IMU_main.h"
#include "tensorflow/lite/examples/LowerMachine/data_FOOT_main.h"
#include "tensorflow/lite/examples/LowerMachine/config.h"
#include "tensorflow/lite/examples/LowerMachine/global.h"
#include "tensorflow/lite/examples/LowerMachine/data_WIN.h"
#include "tensorflow/lite/examples/LowerMachine/Sem.h"
#include "tensorflow/lite/examples/LowerMachine/shm.h"
#include "tensorflow/lite/examples/LowerMachine/filt_ADC.h"
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
static FILE* fp_IMU_orignal;
static FILE* fp_FT;
static FILE* fp_ADC_OFF;
static FILE* fp_IMU_OFF;

static std::ifstream emgin;
static std::ifstream imuin;




//����������
static data_WIN *DATA_WIN = new data_WIN;


static void APP_exit()
{
	delete[] DATA_WIN;
	fclose(fp_IMU);
	fclose(fp_ADC);
	fclose(fp_FT);
	fclose(fp_IMU_orignal);
	printf("=============== DATA EXIT! ==============\n");
	exit(0);
}

static void SigH(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		break;
	case SIGALRM:
		ms_T_count += 1;
		break;
	case SIGUSR1:
		break;
	case SIGUSR2:
		proc_status = STATUS_OFF;
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

//��װ�ź�
static void init_sig()
{
	signal(SIGINT, SigH);
	signal(SIGUSR1, SigH);
	signal(SIGALRM, SigH);
	signal(SIGUSR2, SigH);
}

//	��ʼ����ʱ��
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

// static int init_timeFd(){
// 	int timefd = timerfd_creat(CLOCK_MONOTONIC,0);
// 	struct itimerspec timer_val;
// 	timer_val.it_value.tv_sec = 0;
// 	timer_val.it_value.tv_usec = 1000;				//1ms֮��ִ��
// 	timer_val.it_interval.tv_sec = 0;
// 	timer_val.it_interval.tv_usec = 1000;			//���1ms
// 	int status = timerfd_settime(timefd,0,&timer)

// }


static void wait_ADC()
{
	#ifdef ADC_MODE_OL
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
	#endif
	printf(".......ADC.......OK !\n");
}


static void wait_IMU()
{
	#ifdef IMU_MODE_OL
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
	#endif
	printf(".......IMU.......OK !\n");
}

static void wait_FT()
{
	#ifdef FOOT_MODE_OL
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
	#endif
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
	fp_IMU_orignal = fopen("file_IMU_Orignal.csv", "w");
	if (NULL == fp_IMU || NULL==fp_IMU_orignal)
	{
		printf("ERROR: Open file %s !\n ", IMU_FILE_NAME);
	}

	for (int i=0;i<IMU_CH;i++)
	{
	fprintf(fp_IMU, ",ACC_x,ACC_y,ACC_z,GYR_x,GYR_y,GYR_z,HX,HY,Hz,ROll,Pitch,Yaw");
	}
	fprintf(fp_IMU, "\n");

	for (int i=0;i<IMU_CH;i++)
	{
	fprintf(fp_IMU_orignal, ",ACC_x,ACC_y,ACC_z,GYR_x,GYR_y,GYR_z,HX,HY,Hz,ROll,Pitch,Yaw");
	}
	fprintf(fp_IMU_orignal, "\n");


#endif 

#ifdef FT_OF_DATA
	fp_FT = fopen(FT_FILE_NAME, "w");
	if (NULL == fp_FT)
	{
		printf("ERROR: Open file %s !\n ", FT_FILE_NAME);
	}
#endif 



#ifdef ADC_MODE_OL
#else
	//离线ADC文件
	emgin.open("./data/file_ADC.csv");
	if(!emgin.is_open())
	{
		std::cout<<"load emg error";
	}
	imuin.open("./data/file_IMU.csv");
	if(!imuin.is_open())
	{
		std::cout<<"load IMU error";
	}
	std::string buff;
	getline(emgin,buff);
#endif

#ifdef IMU_MODE_OL
#else
#endif
}

static bool ADC_getData(float pData[8])
{
	using namespace std;
	std::string buff;
	if(getline(emgin,buff)){
		istringstream str_in;
		str_in.str(buff);
		string temp;
		int ch=0;
		while(getline(str_in,temp,',')){
			if(ch == 0){
				ch++;
				continue;
			}
			float data = (float)atof(temp.c_str());
			pData[ch-1] = data;
			ch++;
			temp.clear();
		}
	}
	else 
		return false;
	static long Ts;
	bool result = true;
	return result;
}

static void OFL_ADC(unsigned int Tnow, float SRC_Data[])
{
	fprintf(fp_ADC, "%d", Tnow);
	for (int n = 0; n < 8; n++) {
		fprintf(fp_ADC, ",%.6f", SRC_Data[n]);
	}
	fprintf(fp_ADC, "\n");
//	fflush(fp_ADC);
}

static void SHM_WRITE_ADC(unsigned int Tnow)
{
	static unsigned long T_count_ms_last_adc = Tnow;
	static float SRC_Data[8];
	static float FLT_Data[8];
	static unsigned int Ts = 1;
	if (Tnow - T_count_ms_last_adc >= Ts)
	{
		
		//在线读取数据
		T_count_ms_last_adc = Tnow;

		#ifdef ADC_MODE_OL
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
		#else
		//读取离线文件
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
		
		//记录数据
		#ifdef ADC_OF_DATA
		OFL_ADC(Tnow, SRC_Data);
		#endif

		DATA_WIN->Append_ADC(FLT_Data);
	}
}


//
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
//	fflush(fp_IMU);
	return true;
}

static bool OFL_IMU_orignal(int time, float pSrc[8][12])
{
	fprintf(fp_IMU_orignal, "%d", time);
	for (int _CH = 0; _CH < IMU_CH; _CH++)
	{
		//fprintf(fp_IMU, "\t");
		for (int n = 0; n < 12; n++)
		{
			fprintf(fp_IMU_orignal, ",%.6f", pSrc[_CH][n]);
		}
	}
	fprintf(fp_IMU_orignal, "\n");
	return true;
}

//���һ������IMU���ݣ� д�봰��
static void SHM_WRITE_IMU(unsigned int Tnow)
{
	static unsigned long T_count_ms_last = Tnow;
	static float Data_tmp_src[8][12];		//IMU ���ݻ�����������8ͨ����ÿͨ��12������
	static float Data_tmp_src_ori[8][12];
	static float Data_tmp_dest[8][12];
	static int unsigned Ts = 10;
	if (Tnow - T_count_ms_last >= Ts)
	{
		T_count_ms_last = Tnow;
		//	�Ӷ�Ӧ�Ĺ����ڴ������
#ifdef IMU_MODE_OL
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
#else
		using namespace std;
		string buff;
		string temp;
		if(getline(imuin,buff)){
			istringstream str_in;
			str_in.str(buff);
			int j=-1;
			while(getline(str_in,temp,',')){
				if(j==-1){
					++j;
					temp.clear();
					continue;
				}
				float data = (float)atof(temp.c_str()); //stof(temp);
				int ch = j / 12;  //取模
				int col =j % 12; //取余
				Data_tmp_src[ch][col] = data;
				j++;
				temp.clear();
			}
		}
#endif
		IMU_filter(Data_tmp_dest,Data_tmp_src);
#ifdef IMU_OF_DATA
		OFL_IMU(Tnow, Data_tmp_src);	
#endif
		DATA_WIN->Append_IMU(Data_tmp_dest);
	}

}

//
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
	static int FT_Data[4]={1,1,1,1};
	if (Tnow - T_count_ms_last_adc >= 1)
	{
		T_count_ms_last_adc = Tnow;
		#ifdef FOOT_MODE_OL
		Sem_P(semid_foot);
		for (int i = 0; i < 4; i++)
		{
			FT_Data[i] = p_shm_FOOT->FT_DATA[i];
		}
		p_shm_ADC->nflag = false;
		Sem_V(semid_foot);
		#endif 
#ifdef FT_OF_DATA
		OFL_FT(Tnow, FT_Data);
#endif
	}
}


//
static void init_glbs()
{
	//
	pid_DATA = getpid();
	if (!Sem_P(semid_glbs))
	{
		printf("46\n");
		exit(1);
	}
	p_shm_GLB->PID.PID_DATA = pid_DATA;
	Sem_V(semid_glbs);

	//
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

static void wait_TCP()
{
	static int TCP_status = STATUS_OFF;
	do 
	{
		usleep(300);
		Sem_P(semid_glbs);
		TCP_status = p_shm_GLB->proc_status.proc_TCP_status;
		Sem_V(semid_glbs);
	} while (TCP_status == STATUS_OFF);
	printf("main data TCP.........OK!!!\n");
}

void main_data()
{
	init_sig();
	init_glbs();
	wait_TCP();
	wait_ADC();
	wait_IMU();
	wait_FT();
	
	init_OFL();  	//���ɼ������������ߡ�
	int Tnow;
	init_WIN_SLD();
	printf("初始化计时器\n");
	init_timer();
	while (true)
	{	//	�������ݻ������ķ���ѭ��
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
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
			//
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
