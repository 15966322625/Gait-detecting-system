/*************************************************
*		活动段检测类的设计
*************************************************/
#pragma once
#include <stdio.h>
#include <sys/time.h>
#include <sys/signal.h>
#include "config.h"
#include "main_data.h"
#include "main_dsp.h"
#include "global.h"
#include "Sem.h"



class dsp_ACTClass
{
public:
	dsp_ACTClass();
	~dsp_ACTClass();
public:
	bool Init(pid_t PID);		//初始化
	bool CpyD2A(SHM_DATA_t *pSrc);		//数据报数据备份到DATW
	bool CpyA2A(SHM_ACT_DATA *pDest);	//�� ACTDATA�е����ݿ�����Ŀ������
	bool Check();	//活动段端点检测
	bool EnAble();		//激活检测
	bool Reset();	//活动段检测复位
	bool Clear(SHM_ACT_DATA *pDest);	//清空活动段数据缓存
	bool Clear();
	SHM_ACT_DATA ACT_DATA;	//活动段数据
//private:
public:
	SHM_DATA_t DATW;
	SHM_ACT_DATA ACT_DATA_tmp;

	//int ACT_status_t; //�ж��Ƿ��ڻ���ڡ�  0�����ǻ�Σ� 1�������ڻ�Ρ�
	int T_status;  //0������ʼ��ʱ�� 1����֪ͨ����ʱ�ˡ�
	int ACT_STATUS = 0;   //1 ȷ����ν���������֪ͨrcg���̾���״̬��0��ʼ��
	pid_t pid_RCG;
	int W_cnt = 0;	//数据报计数
	int T_now = 0;	//当前最新数据包时间戳
	int W_cnt_S = 0;	//活动段起点号
	int W_cnt_E = 0;	//活动段终点号
	float SLPW_buf[SLPW_LEN] = { 0 };	//滑动窗数据
	float SampEN_num = 0;	//样本熵计算结果
	int Status = 0;		//当前活动段端点检测状态Status
	bool EN_flag = false;  //��⵽������׼����ʼʶ��
	bool MoveForward();
private:
	bool Push(SHM_ACT_DATA *pDest);		//将最新的数据包增加到活动段数据缓存
	bool Append(SHM_ACT_DATA *pDest, SHM_ACT_DATA *pSrc);
	bool Status_0();		//活动段起点检测
	bool Status_1();		//活动段终点检测
	bool Mk_slpw();		    //构建滑动窗
	bool SampEnCal();       //计算数据样本熵
	float SampEn_Br(float *Data, int m, float r, int N);
private:

	int Ecount = 0;	//检测到可能活动段端点的话就开始自加一
};




