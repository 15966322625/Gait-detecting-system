#include "global.h"

//信号量
int semid_adcs;
int semid_imus;
int semid_foot;
int semid_DATA;
int semid_ACT;
int semid_TCP;
int semid_glbs;
int semid_timer;




//共享内存
int shmid_adcs;
int shmid_imus;
int shmid_foot;
int shmid_DATA;
int shmid_ACT;
int shmid_TCP;
int shmid_glbs;
int shmid_timer;




//指针
GLB *p_shm_GLB;
SHM_ADC_t *p_shm_ADC;
SHM_IMU_t *p_shm_IMU;
SHM_FT_DATA *p_shm_FOOT;
SHM_DATA_t *p_shm_DATA; //采集同步之后的数据满40个点
SHM_ACT_DATA *p_shm_ACT; //识别数据
SHM_TCP_t *p_shm_TCP;
SHM_TIMER_t *p_shm_TIMER;

