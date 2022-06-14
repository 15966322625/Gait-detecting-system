#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <wiringSerial.h>
#include "data_IMU_main.h"
#include "Sem.h"
#include "global.h"
#include "shm.h"
#include "IMU_data.h"
#include "config.h"
//============================= IMU数据采集进程的设计 ====================================

static pid_t pid_imu;

static int proc_status = STATUS_ON;

//imu����imu[8][12]
float IMU_DATA[ROW][COLUMN] = { {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0} };

static void APP_exit()
{
	serialClose(IMU_OL_fd);
	printf("=============== IMU  EXIT! ==============\n ");
	exit(0);
}



//初始化两个信号 SiGINT 和SIGUSR1 主要是为了主进程的退出和本进程的退出;
void SigH(int Sig)
{
	switch (Sig)
	{
	case SIGINT:
		proc_status = STATUS_OFF;
		break;
	case SIGUSR1:
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

static void init_glbs()
{
	pid_imu = getpid();
	Sem_P(semid_glbs);
	p_shm_GLB->PID.PID_IMU = pid_imu;
	Sem_V(semid_glbs);
}


void data_IMU_main()
{
	init_sig();
	init_glbs();
	init_IMU();

	//��ѭ���ɼ�����
	while (1)
	{
		usleep(6000);
		//�ж�
		if (proc_status == STATUS_OFF)
		{
			APP_exit();
		}
		IMU_getdata(IMU_DATA);
		if (!Sem_P(semid_imus))
		{
			printf("16\n");
			exit(1);
		}
		for (int _CH = 0; _CH <IMU_CH; _CH++) 
		{
			for (int n = 0; n < COLUMN; n++)
			{
				p_shm_IMU->Data_src[_CH][n] = IMU_DATA[_CH][n];
			}
		}
		p_shm_IMU->nflag = true;
		Sem_V(semid_imus);
	}
}
