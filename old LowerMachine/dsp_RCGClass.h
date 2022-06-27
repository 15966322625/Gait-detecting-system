#pragma once

/*****************************************************
*		������ݴ�����ؼ������
*****************************************************/
#pragma once
#include <stdio.h>
#include "main_dsp.h"
#include "BPNet.h"
#include "config.h"
#include <fstream>

#ifdef ANS
extern std::ofstream BP_off;
#endif


struct TZ_vec_t 
{
	int VECLen;
	float Data[50];
};

class dsp_RCGClass
{
public:
	dsp_RCGClass();
	~dsp_RCGClass();
public:
	bool Init();	//��ʼ��
	bool BackUp(SHM_ACT_DATA *pSrc);	//���ݿ���
	bool Recg();	//������ݴ���
public:
	float RCG_ret[6];
	int Max_Index;
	SHM_ACT_DATA ACTD;	//���������Ļ������
private:
	bool TZ_calc();	//���������ļ���
	float calc_WAV(float *IN, int len);
	float calc_WL(float *IN, int len);
	float calc_AV(float *IN, int len);
	float calc_IAR(float *IN, int len);
	float calc_RMS(float *IN, int len);
	float calc_SPEN(float *IN, int len);
	float calc_Euler(float *IN, int x, int y);
	float calc_ACC(float*x, float*y, float*z, int i);

private:
	// dsp_FFTClass FFT;
	TZ_vec_t VEC;
	BPNet *BP;
};


