#include <sys/shm.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>
#include "shm.h"
//#include "global.h"

//===================== 共享内存类的实现 ==================================================

//全局共享
int Shm_creat_glb(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1) //shmget共享内存函数用于得到一个共享内存标识符活创建一个共享内存对象;
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}

int Shm_creat_adc(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}

int Shm_creat_imu(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}

int Shm_creat_foot(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_FOOT error!\n");
		exit(1);
	}
	return shmid;
}

int Shm_creat_data(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}


int Shm_creat_ACT(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}


int Shm_creat_TCP(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_glbs error!\n");
		exit(1);
	}
	return shmid;
}


int Shm_creat_TIMER(int size)
{
	int shmid;
	if ((shmid = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT)) == -1)
	{
		printf("shmget_timer error!\n");
		exit(1);
	}
	return shmid;
}


void Shm_remove(int semid, int shmid, void *shm_p)
{
	if ((semctl(semid, IPC_RMID, 0)) == -1)
	{
		perror("semctl error:");
		exit(1);
	}
	if ((shmdt(shm_p)) == -1)
	{
		printf("remove shmdt error!\n");
		exit(1);
	}

	if ((shmctl(shmid, IPC_RMID, 0)) == -1)
	{
		printf("remove shm error!\n");
		exit(1);
	}

}

//�����ڴ�ӳ��
void *Shm_mat(int shmid_t)
{
	void *addr_t;
	addr_t = shmat(shmid_t, NULL, 0);
	if (addr_t == (void *)-1)
	{
		perror("Shmat :");
	}
	return addr_t;
}