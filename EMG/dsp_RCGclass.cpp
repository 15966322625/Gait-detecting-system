#include <math.h>
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "config.h"
#include "dsp_RCGClass.h"
#include "BP_DATA.h"

#include <fstream>
#include <iostream>


using namespace std;
#ifdef ANS
std::ofstream BP_off;
#endif


dsp_RCGClass::dsp_RCGClass()
{
	BP = new(BPNet);
}

dsp_RCGClass::~dsp_RCGClass()
{
	delete BP;
}
//��ʼ�� 初始化
bool dsp_RCGClass::Init()
{
	BP->Init(layer);	//��ʼ��BP������ 初始化BP神经网络
	#ifdef ANS
	BP_off.open("./A_BP.csv");
    if(!BP_off.is_open())
    {
        cout<<"ERROR: open BP_off file\n"<<endl;  
    }
	#endif

	return true;
}
//���ݿ���数据拷贝
bool dsp_RCGClass::BackUp(SHM_ACT_DATA *pSrc)
{
	ACTD.Tnow = pSrc->Tnow;
	ACTD.WS_SP = pSrc->WS_SP;
	ACTD.ADCLen = pSrc->ADCLen;
	ACTD.IMULen = pSrc->IMULen;
	ACTD.DATLen = pSrc->DATLen;
	for (int _CH = 0; _CH < 8; _CH++) 
	{
		//ADC
		for (int n = 0; n < ACTD.ADCLen; n++) 
		{
			ACTD.ADC_Data_CH[_CH].Data[n] = pSrc->ADC_Data_CH[_CH].Data[n];
		}
		//IMU
		for (int n = 0; n < ACTD.IMULen; n++) 
		{
			ACTD.IMU_Data_CH[_CH].ACC_x[n] = pSrc->IMU_Data_CH[_CH].ACC_x[n];
			ACTD.IMU_Data_CH[_CH].ACC_y[n] = pSrc->IMU_Data_CH[_CH].ACC_y[n];
			ACTD.IMU_Data_CH[_CH].ACC_z[n] = pSrc->IMU_Data_CH[_CH].ACC_z[n];
			ACTD.IMU_Data_CH[_CH].GYRO_x[n] = pSrc->IMU_Data_CH[_CH].GYRO_x[n];
			ACTD.IMU_Data_CH[_CH].GYRO_y[n] = pSrc->IMU_Data_CH[_CH].GYRO_y[n];
			ACTD.IMU_Data_CH[_CH].GYRO_z[n] = pSrc->IMU_Data_CH[_CH].GYRO_z[n];
			ACTD.IMU_Data_CH[_CH].Q_0[n] = pSrc->IMU_Data_CH[_CH].Q_0[n];
			ACTD.IMU_Data_CH[_CH].Q_1[n] = pSrc->IMU_Data_CH[_CH].Q_1[n];
			ACTD.IMU_Data_CH[_CH].Q_2[n] = pSrc->IMU_Data_CH[_CH].Q_2[n];
			ACTD.IMU_Data_CH[_CH].Q_3[n] = pSrc->IMU_Data_CH[_CH].Q_3[n];
			ACTD.IMU_Data_CH[_CH].Pitch[n] = pSrc->IMU_Data_CH[_CH].Pitch[n];
			ACTD.IMU_Data_CH[_CH].Yaw[n] = pSrc->IMU_Data_CH[_CH].Yaw[n];

		}
	}


	return false;
}
//	�����ȡ�������� 计算获取特征向量
bool dsp_RCGClass::TZ_calc()
{
	VEC.Data[0] = calc_WAV(ACTD.ADC_Data_CH[sEMG_CH_1].Data, ACTD.ADCLen);
	VEC.Data[1] = calc_WAV(ACTD.ADC_Data_CH[sEMG_CH_2].Data, ACTD.ADCLen);
	VEC.Data[2] = calc_WAV(ACTD.ADC_Data_CH[sEMG_CH_3].Data, ACTD.ADCLen);
	VEC.Data[3] = calc_WAV(ACTD.ADC_Data_CH[sEMG_CH_4].Data, ACTD.ADCLen);
	VEC.Data[4] = calc_WAV(ACTD.ADC_Data_CH[sEMG_CH_5].Data, ACTD.ADCLen);

	VEC.Data[5] = calc_WL(ACTD.ADC_Data_CH[sEMG_CH_1].Data, ACTD.ADCLen); //���γ���
	VEC.Data[6] = calc_WL(ACTD.ADC_Data_CH[sEMG_CH_2].Data, ACTD.ADCLen);
	VEC.Data[7] = calc_WL(ACTD.ADC_Data_CH[sEMG_CH_3].Data, ACTD.ADCLen);
	VEC.Data[8] = calc_WL(ACTD.ADC_Data_CH[sEMG_CH_4].Data, ACTD.ADCLen);
	VEC.Data[9] = calc_WL(ACTD.ADC_Data_CH[sEMG_CH_5].Data, ACTD.ADCLen);

	VEC.Data[10] = calc_AV(ACTD.ADC_Data_CH[sEMG_CH_1].Data, ACTD.ADCLen); //��ֵ��1
	VEC.Data[11] = calc_AV(ACTD.ADC_Data_CH[sEMG_CH_2].Data, ACTD.ADCLen);
	VEC.Data[12] = calc_AV(ACTD.ADC_Data_CH[sEMG_CH_3].Data, ACTD.ADCLen);
	VEC.Data[13] = calc_AV(ACTD.ADC_Data_CH[sEMG_CH_4].Data, ACTD.ADCLen);
	VEC.Data[14] = calc_AV(ACTD.ADC_Data_CH[sEMG_CH_5].Data, ACTD.ADCLen);

	VEC.Data[15] = calc_IAR(ACTD.ADC_Data_CH[sEMG_CH_1].Data, ACTD.ADCLen); //����
	VEC.Data[16] = calc_IAR(ACTD.ADC_Data_CH[sEMG_CH_2].Data, ACTD.ADCLen);
	VEC.Data[17] = calc_IAR(ACTD.ADC_Data_CH[sEMG_CH_3].Data, ACTD.ADCLen);
	VEC.Data[18] = calc_IAR(ACTD.ADC_Data_CH[sEMG_CH_4].Data, ACTD.ADCLen);
	VEC.Data[19] = calc_IAR(ACTD.ADC_Data_CH[sEMG_CH_5].Data, ACTD.ADCLen);

	VEC.Data[20] = calc_RMS(ACTD.ADC_Data_CH[sEMG_CH_1].Data, ACTD.ADCLen); //������
	VEC.Data[21] = calc_RMS(ACTD.ADC_Data_CH[sEMG_CH_2].Data, ACTD.ADCLen);
	VEC.Data[22] = calc_RMS(ACTD.ADC_Data_CH[sEMG_CH_3].Data, ACTD.ADCLen);
	VEC.Data[23] = calc_RMS(ACTD.ADC_Data_CH[sEMG_CH_4].Data, ACTD.ADCLen);
	VEC.Data[24] = calc_RMS(ACTD.ADC_Data_CH[sEMG_CH_5].Data, ACTD.ADCLen);

	VEC.Data[25] = calc_ACC(ACTD.IMU_Data_CH[0].ACC_x, ACTD.IMU_Data_CH[0].ACC_y, ACTD.IMU_Data_CH[0].ACC_z, 0);//ACC
	VEC.Data[26] = calc_ACC(ACTD.IMU_Data_CH[0].ACC_x, ACTD.IMU_Data_CH[0].ACC_y, ACTD.IMU_Data_CH[0].ACC_z, ACTD.IMULen / 2);
	VEC.Data[27] = calc_ACC(ACTD.IMU_Data_CH[0].ACC_x, ACTD.IMU_Data_CH[0].ACC_y, ACTD.IMU_Data_CH[0].ACC_z, ACTD.IMULen - 1);
	VEC.Data[28] = calc_ACC(ACTD.IMU_Data_CH[1].ACC_x, ACTD.IMU_Data_CH[1].ACC_y, ACTD.IMU_Data_CH[1].ACC_z, 0);
	VEC.Data[29] = calc_ACC(ACTD.IMU_Data_CH[1].ACC_x, ACTD.IMU_Data_CH[1].ACC_y, ACTD.IMU_Data_CH[1].ACC_z, ACTD.IMULen / 2);
	VEC.Data[30] = calc_ACC(ACTD.IMU_Data_CH[1].ACC_x, ACTD.IMU_Data_CH[1].ACC_y, ACTD.IMU_Data_CH[1].ACC_z, ACTD.IMULen - 1);
	VEC.Data[31] = calc_ACC(ACTD.IMU_Data_CH[2].ACC_x, ACTD.IMU_Data_CH[2].ACC_y, ACTD.IMU_Data_CH[2].ACC_z, 0);
	VEC.Data[32] = calc_ACC(ACTD.IMU_Data_CH[2].ACC_x, ACTD.IMU_Data_CH[2].ACC_y, ACTD.IMU_Data_CH[2].ACC_z, ACTD.IMULen / 2);
	VEC.Data[33] = calc_ACC(ACTD.IMU_Data_CH[2].ACC_x, ACTD.IMU_Data_CH[2].ACC_y, ACTD.IMU_Data_CH[2].ACC_z, ACTD.IMULen - 1);

	VEC.Data[34] = calc_Euler(ACTD.IMU_Data_CH[0].Pitch, ACTD.IMULen / 2, 0);//ŷ����
	VEC.Data[35] = calc_Euler(ACTD.IMU_Data_CH[0].Pitch, ACTD.IMULen - 1, ACTD.IMULen / 2);
	VEC.Data[36] = calc_Euler(ACTD.IMU_Data_CH[1].Pitch, ACTD.IMULen / 2, 0);
	VEC.Data[37] = calc_Euler(ACTD.IMU_Data_CH[1].Pitch, ACTD.IMULen - 1, ACTD.IMULen / 2);
	VEC.Data[38] = calc_Euler(ACTD.IMU_Data_CH[2].Pitch, ACTD.IMULen / 2, 0);
	VEC.Data[39] = calc_Euler(ACTD.IMU_Data_CH[2].Pitch, ACTD.IMULen - 1, ACTD.IMULen / 2);



	VEC.VECLen = 40;

	
	return true;
}
bool dsp_RCGClass::Recg()
{
	TZ_calc();

	

	////2.����������������ģʽʶ��
	BP->BPForward(VEC.Data);
	//3.��������ʶ��������н���ж�
	for (int i = 0; i < 6; i++)
	{
		RCG_ret[i] = BP->BP_ret[i];
	}
	Max_Index = BP->max_index;
	return true;
}

/*****************************************************************/
//����ֵ����ƽ��
float dsp_RCGClass::calc_WAV(float *IN, int len)
{
	float tmp = 0;
	for (int n = 0; n < len; n++)
	{
		tmp += abs(IN[n]);
	}
	return (tmp / len);
}

//���γ���
float dsp_RCGClass::calc_WL(float *IN, int len)
{
	float tmp = 0;
	for (int n = 0; n < len - 1; n++) {
		tmp += abs(IN[n] - IN[n + 1]);
	}
	return tmp;
}

//��ֵ
float dsp_RCGClass::calc_AV(float *IN, int len)
{
	float tmp = 0;
	for (int n = 0; n < len; n++) {
		tmp += IN[n];
	}
	return (tmp / len);
}

//����
float dsp_RCGClass::calc_IAR(float *IN, int len)
{
	float tmp = 0;
	for (int n = 0; n < len; n++) {
		tmp += pow(IN[n], 2);
	}
	tmp /= (len - 1);
	return (tmp / (len - 1));
}

//������
float dsp_RCGClass::calc_RMS(float *IN, int len)
{
	float tmp = 0;
	for (int n = 0; n < len; n++) {
		tmp += pow(IN[n], 2);
	}
	return sqrt(tmp / (len - 1));
}

//������
float dsp_RCGClass::calc_SPEN(float *IN, int len)
{
	float tmp = 0;

	return tmp;
}

float  dsp_RCGClass::calc_Euler(float *IN, int x, int y) 
{
	float result;
	result = IN[x] - IN[y];
	return result;
}

float dsp_RCGClass::calc_ACC(float*x, float*y, float*z, int i)
{
	float result;
	result = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
	return result;
}
