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
	int labelaction;//对动作打标签 六个动作
	int labelgait;///对每个动作的五个步态进行打标签
	float time_process;
	long int second;
	long int usecond;
};

void main_tcp();
