#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "main_data.h"
#include "main_dsp.h"
#include "Sem.h"
#include "shm.h"
#include "global.h"
#include "config.h"
#include "main_tcp.h"
#include "data_ADC_main.h"
#include "data_IMU_main.h"
#include "data_FOOT_main.h"
#include "main_tcp.h"
#include "main_can.h"
#include "dsp_RCG_main.h"

//��־λ
static int proc_status = STATUS_ON;

//PID
static pid_t pid_data;
static pid_t pid_dsp;
static pid_t pid_tcp;
static pid_t pid_main;
static pid_t pid_adc;
static pid_t pid_imu;
static pid_t pid_rcg;
static pid_t pid_foot;
//tcp_flag
int tcp_flag =0;
static void APP_exit()
{
	Sem_P(semid_glbs);
	pid_adc = p_shm_GLB->PID.PID_ADC;
	pid_data = p_shm_GLB->PID.PID_DATA;
	pid_dsp = p_shm_GLB->PID.PID_DSP;
	pid_imu = p_shm_GLB->PID.PID_IMU;
	pid_tcp = p_shm_GLB->PID.PID_MAIN_TCP;
	pid_rcg = p_shm_GLB->PID.PID_RCG;
	pid_foot = p_shm_GLB->PID.PID_FOOT;
	Sem_V(semid_glbs);

	kill(pid_data, 12);
	kill(pid_dsp, 12);
	kill(pid_tcp, 12);
	kill(pid_adc, 12);
	kill(pid_imu, 12);
	kill(pid_rcg, 12);
	kill(pid_foot, 12);


	waitpid(pid_data, NULL, 0);
	waitpid(pid_dsp, NULL, 0);
	waitpid(pid_tcp, NULL, 0);
	waitpid(pid_adc, NULL, 0);
	waitpid(pid_imu, NULL, 0);
	waitpid(pid_rcg, NULL, 0);
	waitpid(pid_foot, NULL, 0);


	Shm_remove(semid_ACT, shmid_ACT, p_shm_ACT);
	Shm_remove(semid_adcs, shmid_adcs, p_shm_ADC);
	Shm_remove(semid_DATA, shmid_DATA, p_shm_DATA);
	Shm_remove(semid_imus, shmid_imus, p_shm_IMU);
	Shm_remove(semid_foot, shmid_foot, p_shm_FOOT);
	Shm_remove(semid_TCP, shmid_TCP, p_shm_TCP);
	Shm_remove(semid_glbs, shmid_glbs, p_shm_GLB);
	printf("=============== MAIN EXIT! ==============\n");
	exit(0);
}

//	�źŴ���
static void SigH(int sig)
{
	switch (sig) 
	{
	case SIGINT:		//�ⲿ������Ҫ������˳���
		proc_status = STATUS_OFF;
		break;
	default:
		break;
	}
}


static bool init_sig()
{
	signal(SIGINT, SigH);
	return true;
}

static void init_sem()
{
	semid_glbs = Sem_creat_GLB();
	Sem_set(semid_glbs); //�ź�����Ϊ1
	semid_ACT = Sem_creat_ACT();
	Sem_set(semid_ACT);
	semid_TCP = Sem_creat_TCP();
	Sem_set(semid_TCP);
	semid_adcs = Sem_creat_ADC();
	Sem_set(semid_adcs);
	semid_imus = Sem_creat_IMU();
	Sem_set(semid_imus);
	semid_foot = Sem_creat_FOOT();
	Sem_set(semid_foot);
	semid_DATA = Sem_creat_DATA();
	Sem_set(semid_DATA);
	semid_timer = Sem_creat_TIMER();
	Sem_set(semid_timer);
	//printf("semid_GLB: %d\nsemid_ACT: %d\nsemid_TCP: %d\nsemid_ADC: %d\nsemid_IMU: %d\nsemid_DATA: %d\n", semid_glbs, semid_ACT, semid_TCP, semid_adcs, semid_imus, semid_DATA);
}


//	��ʼ�������ڴ�
static void init_shm()
{
	shmid_glbs = Shm_creat_glb(sizeof(GLB));
	void *shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_glbs);
	p_shm_GLB = (GLB *)shm_tmp;


	//*********************ADC*******************
	//
	//****************************************
	shmid_adcs = Shm_creat_adc(sizeof(SHM_ADC_t));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_adcs);
	p_shm_ADC = (SHM_ADC_t *)shm_tmp;

	//*********************IMU*******************
	//
	//****************************************
	shmid_imus = Shm_creat_imu(sizeof(SHM_IMU_t));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_imus);
	p_shm_IMU = (SHM_IMU_t *)shm_tmp;

	//*********************FOOT*******************
	//
	//******************************************
	shmid_foot = Shm_creat_foot(sizeof(SHM_FT_DATA));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_foot);
	p_shm_FOOT = (SHM_FT_DATA *)shm_tmp;

	//*********************DATA*******************
	//
	//****************************************
	shmid_DATA = Shm_creat_data(sizeof(SHM_DATA_t));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_DATA);
	p_shm_DATA = (SHM_DATA_t *)shm_tmp;


//*********************ACT*******************
//
//******************************************
	shmid_ACT = Shm_creat_ACT(sizeof(SHM_ACT_DATA));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_ACT);
	p_shm_ACT = (SHM_ACT_DATA *)shm_tmp;

//*********************TCP*******************
//
//******************************************
	shmid_TCP = Shm_creat_TCP(sizeof(SHM_TCP_t));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_TCP);
	p_shm_TCP = (SHM_TCP_t *)shm_tmp;


//*********************TIMER*******************
//
//******************************************
	shmid_timer = Shm_creat_TIMER(sizeof(SHM_TIMER_t));
	shm_tmp = NULL;
	//ӳ�䱾��ָ��
	shm_tmp = Shm_mat(shmid_timer);
	p_shm_TIMER = (SHM_TIMER_t *)shm_tmp;

}

//д��MAIN��PID������������Ϊ0��
static void init_glbs()
{
	pid_main = getpid();
	Sem_P(semid_glbs);
	p_shm_GLB->PID.PID_ADC = 0;
	p_shm_GLB->PID.PID_DATA = 0;
	p_shm_GLB->PID.PID_DSP = 0;
	p_shm_GLB->PID.PID_IMU = 0;
	p_shm_GLB->PID.PID_MAIN_TCP = 0;
	p_shm_GLB->PID.PID_RCG = 0;
	p_shm_GLB->PID.PID_FOOT = 0;
	p_shm_GLB->PID.PID_MAIN = pid_main;
	p_shm_GLB->proc_status.en_dsp = 0;
	p_shm_GLB->proc_status.en_rcg = 0;
	p_shm_GLB->proc_status.en_tcp = 0;
	p_shm_GLB->proc_status.proc_TCP_status = STATUS_OFF;
	p_shm_GLB->proc_status.proc_status_proc = STATUS_ON;
	p_shm_GLB->proc_status.proc_ACT_status = STATUS_OFF;
	p_shm_GLB->proc_status.T_status = STATUS_OFF;
	p_shm_GLB->proc_status.ACT_OVER = 0;
	Sem_V(semid_glbs);
}

static void proc_monitor()
{
	while (1)
	{
		usleep(1000);
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
	}
}


static void  init_proc()
{
	pid_t pid;
	if ((pid = fork()) < 0)
	{
		perror("fork error!");
	}
	else if (pid == 0)
	{
// DATA ***********
		main_data();
	}
	else
	{
		if ((pid = fork()) < 0)
		{
			perror("fork error!");
		}
		else if (pid == 0)
		{

//DSP   *************
			main_dsp();
		}
		else
		{
			if ((pid = fork()) < 0)
			{
				perror("fork error!");
			}
			else if (pid == 0)
			{
//TCP   ***************
				if(tcp_flag == 1)
				{
					main_can();
				}
				else
				{
					main_tcp();
				}
				// #ifdef TCP_send
				// main_tcp();
				// #else
				// main_can();
				// #endif
			}
			else
			{
				if ((pid = fork()) < 0)
				{
					perror("fork error!");
				}
				else if (pid == 0)
				{
//ADC   *****************
					data_ADC_main();
				}
				else
				{
					if ((pid = fork()) < 0)
					{
						perror("fork error!");
					}
					else if (pid == 0)
					{
//IMU   ****************
						data_IMU_main();
					}
					else
					{
						if ((pid = fork()) < 0)
						{
							perror("fork error!");
						}
						else if (pid == 0)
						{
//RCG  ****************
							dsp_RCG_main();
						}
						else
						{
							if ((pid = fork()) < 0) 
							{
								perror("fork error!");
							}
							else if (pid == 0)
							{
//FOOT
								data_FOOT_main();
							}
							else 
							{
//MAIN	*****************
								proc_monitor();
							}	
						}
					}
				}
			}
		}
	}
}

using namespace std;
int main(int argc, char** argv)
{
	if(argc != 3)
    {
        cout<<"please input your Password and tcp or can"<<endl;
        exit(-1);
    }

 // /*  if(strcmp(argv[1],"518chunSheng520") != 0)
 //   {
 //       cout<<"your Password is wrong!!!"<<endl;
 //       exit(-1);
 //   }*/

	if(strcmp(argv[2],"tcp") == 0)
	{
		tcp_flag = 0;
	}
	else if(strcmp(argv[2],"can") == 0)
	{
		tcp_flag = 1;
	}
	else
	{
		tcp_flag = 1;
	}


	init_sig();		//	1.��ʼ���ź���Ӧ
	init_sem();		//	2.��ʼ���ź���
	init_shm();		//	3.��ʼ�������ڴ�
	init_glbs();
	init_proc();
}
