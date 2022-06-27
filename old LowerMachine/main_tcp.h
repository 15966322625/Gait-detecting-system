#pragma once

//typedef enum {
//	Status_1,
//	Status_2,
//	Status_3,
//	Status_4,
//	Status_t,	//����״̬ 
//	Status_NULL
//}ST_t;

typedef struct {
	int Status;
	double DurTime;
}SHM_TCP_t;		//TCP �����ڴ�


struct Result
{
	char label;//直接对五种步态进行打标签 ,但是在最新版的设计中 并不是这样的 而是先对每个动作打标签  再对每个动作的详细步态进行打标签 分两步完成;
	float time_process;
	long int second;
	long int usecond;
};

//TCP主进程
void main_tcp();



