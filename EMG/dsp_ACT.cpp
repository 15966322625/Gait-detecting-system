#include <math.h>
#include "config.h"
#include "dsp_ACT.h"
#include "dsp_RCGClass.h"

///活动段检测类的设计=================================================

//构造函数 如果需要将活动段识别结果离线 则打开一个离线文件 将识别结果写入这个离线文件中
dsp_ACTClass::dsp_ACTClass()
{
	T_status = 0;
}

dsp_ACTClass::~dsp_ACTClass()//主要讲打开的离线文件关闭
{

}
//讲活动段检测类中的一些变量进行初始化
//主要是初始化计数 初始化活动窗数据缓存 讲数据标志设置为假
bool dsp_ACTClass::Init(pid_t PID)
{
	W_cnt = 0;
	W_cnt_S = 0;
	W_cnt_E = 0;
	pid_RCG = PID;
	Clear(&ACT_DATA);	//��ջ�����ݴ�����
	Clear(&ACT_DATA_tmp);
	EN_flag = false;
	Status = 0;
	return false;
}

//��ջ�����ݻ���
bool dsp_ACTClass::Clear(SHM_ACT_DATA *pDest)
{
	pDest->DATLen = 0;
	pDest->ADCLen = 0;
	pDest->IMULen = 0;
	pDest->Tnow = 0;
	return false;
}
bool dsp_ACTClass::Clear()
{
	Clear(&ACT_DATA);
	Clear(&ACT_DATA_tmp);
	return false;
}


//���������ݰ����ӵ�������ݻ��档  ��ԭ�е����ݵĻ����ϼ����������ݡ����̣��������ݿ�����DATW�У��ٽ�DATW�е����ݿ�������������С�
bool dsp_ACTClass::Push(SHM_ACT_DATA *pDest)
{
	pDest->Tnow = DATW.Tnow;
	//ADC
	for (int _CH = 0; _CH < 8; _CH++)
	{
		for (int t = 0; t < DATW.ADC_LEN; t++) 
		{
			pDest->ADC_Data_CH[_CH].Data[pDest->ADCLen + t] = DATW.ADC_DATA_CH[_CH].Data[t];
		}
	}
	pDest->ADCLen += DATW.ADC_LEN;
	pDest->DATLen = pDest->ADCLen;
	//IMU
	for (int _CH = 0; _CH < 8; _CH++) {
		for (int t = 0; t < DATW.IMU_LEN; t++) {
			pDest->IMU_Data_CH[_CH].ACC_x[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].ACC_x[t];
			pDest->IMU_Data_CH[_CH].ACC_y[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].ACC_y[t];
			pDest->IMU_Data_CH[_CH].ACC_z[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].ACC_z[t];
			pDest->IMU_Data_CH[_CH].GYRO_x[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].GYRO_x[t];
			pDest->IMU_Data_CH[_CH].GYRO_y[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].GYRO_y[t];
			pDest->IMU_Data_CH[_CH].GYRO_z[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].GYRO_z[t];
			pDest->IMU_Data_CH[_CH].Q_0[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Q_0[t];
			pDest->IMU_Data_CH[_CH].Q_1[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Q_1[t];
			pDest->IMU_Data_CH[_CH].Q_2[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Q_2[t];
			pDest->IMU_Data_CH[_CH].Q_3[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Q_3[t];
			pDest->IMU_Data_CH[_CH].Pitch[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Pitch[t];
			pDest->IMU_Data_CH[_CH].Yaw[pDest->IMULen + t] = DATW.IMU_DATA_CH[_CH].Yaw[t];
		}
	}
	pDest->IMULen += DATW.IMU_LEN;
	return false;
}
//	��tmp����ӵ�ԭʼ���ݻ�����  &ACT_DATA,   &ACT_DATA_tmp
bool dsp_ACTClass::Append(SHM_ACT_DATA * pDest, SHM_ACT_DATA * pSrc)
{
	for (int _CH = 0; _CH < 8; _CH++)
	{
		//ADC
		for (int n = 0; n < pSrc->ADCLen; n++)
		{
			pDest->ADC_Data_CH[_CH].Data[pDest->ADCLen + n] = pSrc->ADC_Data_CH[_CH].Data[n];
		}
		//IMU
		for (int n = 0; n < pSrc->IMULen; n++)
		{
			pDest->IMU_Data_CH[_CH].ACC_x[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].ACC_x[n];
			pDest->IMU_Data_CH[_CH].ACC_y[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].ACC_y[n];
			pDest->IMU_Data_CH[_CH].ACC_z[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].ACC_z[n];
			pDest->IMU_Data_CH[_CH].GYRO_x[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].GYRO_x[n];
			pDest->IMU_Data_CH[_CH].GYRO_y[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].GYRO_y[n];
			pDest->IMU_Data_CH[_CH].GYRO_z[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].GYRO_z[n];
			pDest->IMU_Data_CH[_CH].Q_0[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Q_0[n];
			pDest->IMU_Data_CH[_CH].Q_1[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Q_1[n];
			pDest->IMU_Data_CH[_CH].Q_2[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Q_2[n];
			pDest->IMU_Data_CH[_CH].Q_3[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Q_3[n];
			pDest->IMU_Data_CH[_CH].Pitch[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Pitch[n];
			pDest->IMU_Data_CH[_CH].Yaw[pDest->IMULen + n] = pSrc->IMU_Data_CH[_CH].Yaw[n];
		}
	}
	pDest->Tnow = pSrc->Tnow;
	pDest->DATLen += pSrc->DATLen;
	pDest->ADCLen += pSrc->ADCLen;
	pDest->IMULen += pSrc->IMULen;

	return false;
}

//活动段起点检测
bool dsp_ACTClass::Status_0()
{
	//ACT_STATUS = 0;
	static int Tcount = 0;
	if (SampEN_num >= ACT_LIM_S) 
	{	//���������
		Tcount++;
		//֪ͨ��ʱ��
		Push(&ACT_DATA);   //�����µ����ݵ�DATW�У��ڽ��俽����ACT_DATA����������.
		EN_flag = false;   //........
		if (Tcount >= ACT_LS) //������5��֮��ʶ��Ϊ������tcount��Ϊ0��
		{
			//֪ͨRCG�����Ѿ�ȷ������������ȡ��ʼʱ�䡣
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.T_status = STATUS_ON;
			Sem_V(semid_glbs);
			kill(pid_RCG, 10);
			Status = 1;     //��ʼʶ����ء�
			Tcount = 0;
			Clear(&ACT_DATA_tmp);
			//			W_cnt_S = W_cnt - ACT_LS+1;
			//			ACT_DATA.WS_SP = W_cnt_S;
			//			printf("W_cnt_S:%d\n", W_cnt_S);
		}
	}
	else {
		Tcount = 0;
		T_status = 0;
		Clear(&ACT_DATA);
		Clear(&ACT_DATA_tmp);
		EN_flag = false;
	}
	return true;
}

//活动段终点检测
bool dsp_ACTClass::Status_1()
{
	static int Tcount = 0;
	if (SampEN_num <= ACT_LIM_E) 	//�����ǻ���յ�
	{
		Tcount++;
		Push(&ACT_DATA_tmp);
		if (Tcount >= ACT_LE) 
		{
			Tcount = 0;
			Clear(&ACT_DATA_tmp);
			Status = 0;
			ACT_STATUS = 1;

		}
	}
	else {
		if (Tcount == 0) //40������ʱ
		{
			//֪ͨRCG���̶�ȡ��ʼʱ�䡣
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.T_status = 2;
			Sem_V(semid_glbs);
			kill(pid_RCG, 10);
			Push(&ACT_DATA);
		}
		else {
			Tcount = 0;
			Sem_P(semid_glbs);
			p_shm_GLB->proc_status.T_status = 2;
			Sem_V(semid_glbs);
			kill(pid_RCG, 10);
			Append(&ACT_DATA, &ACT_DATA_tmp);
			Push(&ACT_DATA);
			Clear(&ACT_DATA_tmp);
		}
	}
	return true;
}
//	�������������ݣ�0/1ͨ�����ں����ݣ�
bool dsp_ACTClass::Mk_slpw()
{
	for (int t = 0; t < SLPW_LEN; t++) 
	{
		if (t < SLPW_LEN - DATW.DAT_LEN)
			SLPW_buf[t] = SLPW_buf[t + DATW.DAT_LEN]; //ǰ24������Ϊ��һ���������ڵĺ�24λ��
		else
			//25~64Ϊ���µ����ݡ�0~40����һ֡���ݡ�
			SLPW_buf[t] = abs(DATW.ADC_DATA_CH[0].Data[t - SLPW_LEN + DATW.DAT_LEN] + DATW.ADC_DATA_CH[1].Data[t - SLPW_LEN + DATW.DAT_LEN]) / 2.0;
	}
	return false;
}
//	�����ؼ���Br
float dsp_ACTClass::SampEn_Br(float *Data, int m, float r, int N) {
	unsigned int count;
	float tmp = 0;
	for (int i = 0; i <= N - m; i++) 
	{
		count = 0;
		for (int j = 0; j <= N - m; j++) {
			if (j != i) 
			{
				for (int k = 0; k <= m - 1; k++) 
				{
					if (abs(Data[i + k] - Data[j + k]) > r) break;
					if (k == (m - 1)) count += 1;
				}
			}
		}
		tmp += (float)count / (float)(N - m);
	}
	tmp = tmp / (N - m + 1);
	return tmp;
}
//样本熵计算
bool dsp_ACTClass::SampEnCal()
{
	float Br, Br_1;
	Br = SampEn_Br(SLPW_buf, SAMPEN_m, SAMPEN_r, SLPW_LEN);
	Br_1 = SampEn_Br(SLPW_buf, SAMPEN_m + 1, SAMPEN_r, SLPW_LEN);
	SampEN_num = log(Br / Br_1);
	if (SampEN_num > 2.5) SampEN_num = 2.5;
	return true;
}


//	��ζ˵��⣬ÿ�ɹ���һ�������ݾ�Checkһ��
bool dsp_ACTClass::Check()
{
	Mk_slpw();		//	1.���»�����
	SampEnCal();	//	2.�����ؼ���
	switch (Status)  //��ʼ��Ϊ0�������㡣
	{					//	4.�˵��ж�
	case 0:
		Status_0();		//��������
		break;
	case 1:
		Status_1();		//�������ֹ��
		break;
	default:
		break;
	}

	return false;
}

//激活 �ж��ǲ��Ǹü�����?	��200����	...�������֮��һ��Ҫ������ݻ���
bool dsp_ACTClass::EnAble()
{
	if ((ACT_DATA.DATLen + ACT_DATA_tmp.DATLen >= ACT_LEN) && (Status == 1)) 
	{
		//Append(&ACT_DATA, &ACT_DATA_tmp);
		//T_status = 0;
		Clear(&ACT_DATA_tmp);
		ACT_DATA.WS_SP = W_cnt; //�����һ֡�����ݰ�λ�á�
		EN_flag = true;
		return true;
	}
	return false;
}
//活动段检测复位
bool dsp_ACTClass::Reset()
{
	//W_cnt_S = 0;
	//W_cnt_E = 0;
	//Status = 0;
	//Clear();
	return false;
}

//数据包备份到DATW
bool dsp_ACTClass::CpyD2A(SHM_DATA_t *pSrc)
{
	W_cnt += 1;
	DATW.Tnow = pSrc->Tnow;
	DATW.DAT_LEN = pSrc->DAT_LEN;
	DATW.ADC_LEN = pSrc->ADC_LEN;
	DATW.IMU_LEN = pSrc->IMU_LEN;
	for (int _CH = 0; _CH < 8; _CH++) 
	{
		//ADC
		for (int n = 0; n < DATW.ADC_LEN; n++) {
			DATW.ADC_DATA_CH[_CH].Data[n] = pSrc->ADC_DATA_CH[_CH].Data[n];
		}
		//IMU
		for (int n = 0; n < DATW.IMU_LEN; n++) 
		{
			DATW.IMU_DATA_CH[_CH].ACC_x[n] = pSrc->IMU_DATA_CH[_CH].ACC_x[n];
			DATW.IMU_DATA_CH[_CH].ACC_y[n] = pSrc->IMU_DATA_CH[_CH].ACC_y[n];
			DATW.IMU_DATA_CH[_CH].ACC_z[n] = pSrc->IMU_DATA_CH[_CH].ACC_z[n];
			DATW.IMU_DATA_CH[_CH].GYRO_x[n] = pSrc->IMU_DATA_CH[_CH].GYRO_x[n];
			DATW.IMU_DATA_CH[_CH].GYRO_y[n] = pSrc->IMU_DATA_CH[_CH].GYRO_y[n];
			DATW.IMU_DATA_CH[_CH].GYRO_z[n] = pSrc->IMU_DATA_CH[_CH].GYRO_z[n];
			DATW.IMU_DATA_CH[_CH].Q_0[n] = pSrc->IMU_DATA_CH[_CH].Q_0[n];
			DATW.IMU_DATA_CH[_CH].Q_1[n] = pSrc->IMU_DATA_CH[_CH].Q_1[n];
			DATW.IMU_DATA_CH[_CH].Q_2[n] = pSrc->IMU_DATA_CH[_CH].Q_2[n];
			DATW.IMU_DATA_CH[_CH].Q_3[n] = pSrc->IMU_DATA_CH[_CH].Q_3[n];
			DATW.IMU_DATA_CH[_CH].Pitch[n] = pSrc->IMU_DATA_CH[_CH].Pitch[n];
			DATW.IMU_DATA_CH[_CH].Yaw[n] = pSrc->IMU_DATA_CH[_CH].Yaw[n];
		}
	}
	return true;
}

//将数据段保存到活动段检测的窗口内
//设计两个缓存：一级缓存和二级缓存 一级缓存主要是讲数据保存下来 二级缓存主要是讲数据保存在活动段中 进行活动段的检测;
bool dsp_ACTClass::CpyA2A(SHM_ACT_DATA *pDest)
{
	pDest->Tnow = ACT_DATA.Tnow;
	pDest->WS_SP = ACT_DATA.WS_SP;
	pDest->DATLen = ACT_DATA.DATLen;
	pDest->ADCLen = ACT_DATA.ADCLen;
	pDest->IMULen = ACT_DATA.IMULen;

	for (int _CH = 0; _CH < 8; _CH++) 
	{
		//ADC
		for (int n = 0; n < pDest->ADCLen; n++) 
		{
			pDest->ADC_Data_CH[_CH].Data[n] = ACT_DATA.ADC_Data_CH[_CH].Data[n];
		}
		//IMU
		for (int n = 0; n < pDest->IMULen; n++)
		{
			pDest->IMU_Data_CH[_CH].ACC_x[n] = ACT_DATA.IMU_Data_CH[_CH].ACC_x[n];
			pDest->IMU_Data_CH[_CH].ACC_y[n] = ACT_DATA.IMU_Data_CH[_CH].ACC_y[n];
			pDest->IMU_Data_CH[_CH].ACC_z[n] = ACT_DATA.IMU_Data_CH[_CH].ACC_z[n];
			pDest->IMU_Data_CH[_CH].GYRO_x[n] = ACT_DATA.IMU_Data_CH[_CH].GYRO_x[n];
			pDest->IMU_Data_CH[_CH].GYRO_y[n] = ACT_DATA.IMU_Data_CH[_CH].GYRO_y[n];
			pDest->IMU_Data_CH[_CH].GYRO_z[n] = ACT_DATA.IMU_Data_CH[_CH].GYRO_z[n];
			pDest->IMU_Data_CH[_CH].Q_0[n] = ACT_DATA.IMU_Data_CH[_CH].Q_0[n];
			pDest->IMU_Data_CH[_CH].Q_1[n] = ACT_DATA.IMU_Data_CH[_CH].Q_1[n];
			pDest->IMU_Data_CH[_CH].Q_2[n] = ACT_DATA.IMU_Data_CH[_CH].Q_2[n];
			pDest->IMU_Data_CH[_CH].Q_3[n] = ACT_DATA.IMU_Data_CH[_CH].Q_3[n];
			pDest->IMU_Data_CH[_CH].Pitch[n] = ACT_DATA.IMU_Data_CH[_CH].Pitch[n];
			pDest->IMU_Data_CH[_CH].Yaw[n] = ACT_DATA.IMU_Data_CH[_CH].Yaw[n];
		}
	}
	return true;
}



bool dsp_ACTClass::MoveForward() {
	for (int _ch = 0; _ch < 8; _ch++)
	{
		for (int t = 0; t < ACT_DATA.DATLen - DATW_LEN; t++)//�ӵ�40�����ݿ�ʼ������ǰ�ƶ���
		{
			ACT_DATA.ADC_Data_CH[_ch].Data[t] = ACT_DATA.ADC_Data_CH[_ch].Data[t + DATW_LEN];
		}
	}
	ACT_DATA.DATLen = ACT_DATA.DATLen - DATW_LEN;
	ACT_DATA.ADCLen = ACT_DATA.ADCLen - DATW_LEN;
	for (int _ch = 0; _ch < 8; _ch++) {
		for (int t = 0; t < ACT_DATA.IMULen - 4; t++) {
			ACT_DATA.IMU_Data_CH[_ch].ACC_x[t] = ACT_DATA.IMU_Data_CH[_ch].ACC_x[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].ACC_y[t] = ACT_DATA.IMU_Data_CH[_ch].ACC_y[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].ACC_z[t] = ACT_DATA.IMU_Data_CH[_ch].ACC_z[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].GYRO_x[t] = ACT_DATA.IMU_Data_CH[_ch].GYRO_x[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].GYRO_y[t] = ACT_DATA.IMU_Data_CH[_ch].GYRO_y[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].GYRO_z[t] = ACT_DATA.IMU_Data_CH[_ch].GYRO_z[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Pitch[t] = ACT_DATA.IMU_Data_CH[_ch].Pitch[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Q_0[t] = ACT_DATA.IMU_Data_CH[_ch].Q_0[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Q_1[t] = ACT_DATA.IMU_Data_CH[_ch].Q_1[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Q_2[t] = ACT_DATA.IMU_Data_CH[_ch].Q_2[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Q_3[t] = ACT_DATA.IMU_Data_CH[_ch].Q_3[t + 4];
			ACT_DATA.IMU_Data_CH[_ch].Yaw[t] = ACT_DATA.IMU_Data_CH[_ch].Yaw[t + 4];
		}
	}
	ACT_DATA.IMULen = ACT_DATA.IMULen - 4;
	return false;
}