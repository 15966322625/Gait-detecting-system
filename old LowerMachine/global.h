#pragma once
#include <sys/time.h>
#include "data_ADC_main.h"
#include "data_IMU_main.h"
#include"data_FOOT_main.h"
#include "main_data.h"
#include "main_dsp.h"
#include "main_tcp.h"
#include "unistd.h"

#define STATUS_OFF 0
#define STATUS_ON 1


//��ʱ���ṹ��
struct SHM_TIMER_t
{
	struct timeval Timer1;
	struct timeval Timer2;
	struct timeval Timer3;
	struct timeval Timer4;
	struct timeval Timer5;
	struct timeval Timer6;
};


struct PID_t
{
	pid_t PID_MAIN_TCP; //������
	pid_t PID_IMU; 
	pid_t PID_DSP;
	pid_t PID_ADC;
	pid_t PID_RCG;
	pid_t PID_DATA;
	pid_t PID_FOOT;
	pid_t PID_MAIN;
};
//һ��5�����̣�����main_tcp, �ӣ�dsp�� imu�� adc , dsp��
struct proc_status_t
{

	int proc_status_proc;
	int en_dsp;
	int en_rcg;
	int en_tcp;
	int ACT_OVER;
	int proc_TCP_status;
	int proc_ACT_status;   //֪ͨrcg����
	int T_status;   //��ʱ״̬־��0Ϊ��ʼֵ��1��ʼ
};



struct GLB 
{
	struct PID_t PID;
	struct proc_status_t proc_status;
};


//�ź���
extern int semid_adcs;
extern int semid_imus;
extern int semid_foot;
extern int semid_DATA;
extern int semid_ACT;
extern int semid_TCP;
extern int semid_glbs;
extern int semid_timer;


//�����ڴ�
extern int shmid_adcs;
extern int shmid_imus;
extern int shmid_foot;
extern int shmid_DATA;
extern int shmid_ACT;
extern int shmid_TCP;
extern int shmid_glbs;
extern int shmid_timer;

//ָ��
extern GLB *p_shm_GLB;
extern SHM_ADC_t *p_shm_ADC;
extern SHM_IMU_t *p_shm_IMU;
extern SHM_FT_DATA  *p_shm_FOOT;
extern SHM_DATA_t *p_shm_DATA;
extern SHM_ACT_DATA *p_shm_ACT;
extern SHM_TCP_t *p_shm_TCP;
extern SHM_TIMER_t *p_shm_TIMER;

