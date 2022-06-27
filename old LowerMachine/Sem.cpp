#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "Sem.h"

//=================  创建信号量类的实现 ================================================

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

//ȫ�ֱ������ź���semid
int Sem_creat_GLB()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);//semget函数是信号量函数
	if (semid == -1)
	{
		perror("semget GLB error\n");
	}
	//printf("semid_glbs: %d\n", semid);
	return semid;
}

//ADC����һ�������ź���
int Sem_creat_ADC()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget ADC error\n");
	}
	//printf("semid_adcs: %d\n", semid);
	return semid;
}


//IMU����һ�������ź���
int Sem_creat_IMU()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget IMU error\n");
	}
	//printf("semid_imus: %d\n", semid);
	return semid;
}


int Sem_creat_FOOT()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget IMU error\n");
	}
	return semid;
}


int Sem_creat_DATA()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget DATA error\n");
	}
	//printf("semid_DATA: %d\n", semid);
	return semid;
}

int Sem_creat_TIMER()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget TIMER error\n");
	}
	//printf("semid_DATA: %d\n", semid);
	return semid;
}

int Sem_creat_ACT()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget ACT error\n");
	}
	//printf("semid_ACT: %d\n", semid);
	return semid;
}


int Sem_creat_TCP()
{
	int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	if (semid == -1)
	{
		perror("semget TCP error\n");
	}
	//printf("semid_TCP: %d\n", semid);
	return semid;
}


//�ź���P����
bool Sem_P(int semid)
{
	//bool result = true;
	int res;
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	do
	{
		res = semop(semid, &sem_b, 1);
		if ((res == -1) && (errno == 4))
		{
			res = semctl(semid, 0, GETVAL);
			usleep(5);
			printf("sys EINTR:res : %d\n", res);
		}
	} while((res == -1) && (errno == 4));
	//res = semctl(semid, 0, GETVAL);
	//if (res < 0)
	//{
	//	printf("semid:%d\tres = %d\n", semid, res);
	//}
	return true;
}


//�ź���V����
void Sem_V(int semid)
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if (semop(semid, &sem_b, 1) == -1)
	{
		printf("semid: %d, semop_V failed�� errno = %d\n", semid, errno);
		exit(1);
	}
}


//�ź�������Ϊ1
void Sem_set(int semid)
{
	semun sem_union;
	sem_union.val = 1;		
	if (semctl(semid, 0, SETVAL, sem_union) == -1)
	{
		perror("Semset error!\n");
		exit(1);
	}
}
void Sem_remove(int semid)
{
	if ((semctl(semid, IPC_RMID, 0)) == -1)
	{
		perror("semctl error:");
		exit(1);
	}
}