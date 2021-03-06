#include "config.h"
#include "data_WIN.h"
#include <stdio.h>

//初始化
bool data_WIN::Init()
{
	Clear();
	return true;
}
//最新一组数据八个通道，每个通道最大为256.将最新的八个数据每个都加进去。
bool data_WIN::Append_ADC(float Data[8])
{
	for (int n = 0; n < 8; n++) 
	{
		DATW.ADC_DATA_CH[n].Data[DATW.ADC_LEN] = Data[n];
	}
	DATW.ADC_LEN += 1;
	DATW.DAT_LEN += 1;
	return true;
}
bool data_WIN::Append_IMU(float Data[8][12])
{
	for (int _CH = 0; _CH < 8; _CH++) 
	{
		DATW.IMU_DATA_CH[_CH].ACC_x[DATW.IMU_LEN] = Data[_CH][0];
		DATW.IMU_DATA_CH[_CH].ACC_y[DATW.IMU_LEN] = Data[_CH][1];
		DATW.IMU_DATA_CH[_CH].ACC_z[DATW.IMU_LEN] = Data[_CH][2];
		DATW.IMU_DATA_CH[_CH].GYRO_x[DATW.IMU_LEN] = Data[_CH][3];
		DATW.IMU_DATA_CH[_CH].GYRO_y[DATW.IMU_LEN] = Data[_CH][4];
		DATW.IMU_DATA_CH[_CH].GYRO_z[DATW.IMU_LEN] = Data[_CH][5];
		DATW.IMU_DATA_CH[_CH].Q_0[DATW.IMU_LEN] = Data[_CH][6];
		DATW.IMU_DATA_CH[_CH].Q_1[DATW.IMU_LEN] = Data[_CH][7];
		DATW.IMU_DATA_CH[_CH].Q_2[DATW.IMU_LEN] = Data[_CH][8];
		DATW.IMU_DATA_CH[_CH].Q_3[DATW.IMU_LEN] = Data[_CH][9];
		DATW.IMU_DATA_CH[_CH].Pitch[DATW.IMU_LEN] = Data[_CH][10];
		DATW.IMU_DATA_CH[_CH].Yaw[DATW.IMU_LEN] = Data[_CH][11];
	}
	DATW.IMU_LEN += 1;
	return true;
}
//清空数据缓存区
bool data_WIN::Clear()
{
	DATW.Tnow = 0;
	DATW.DAT_LEN = 0;
	DATW.ADC_LEN = 0;
	DATW.IMU_LEN = 0;
	return true;
}


bool data_WIN::Enable()
{
	if (DATW.DAT_LEN >= DATW_LEN) return true;
	else return false;
}


bool data_WIN::DataCpy(SHM_DATA_t *pDest, SHM_DATA_t *pSrc)
{
	pDest->Tnow = pSrc->Tnow;
	pDest->DAT_LEN = pSrc->DAT_LEN;
	pDest->ADC_LEN = pSrc->ADC_LEN;
	pDest->IMU_LEN = pSrc->IMU_LEN;
	for (int _CH = 0; _CH < 8; _CH++) {
		for (int n = 0; n < pDest->ADC_LEN; n++)
			pDest->ADC_DATA_CH[_CH].Data[n] = pSrc->ADC_DATA_CH[_CH].Data[n];
		for (int n = 0; n < pDest->IMU_LEN; n++) {
			pDest->IMU_DATA_CH[_CH].ACC_x[n] = pSrc->IMU_DATA_CH[_CH].ACC_x[n];
			pDest->IMU_DATA_CH[_CH].ACC_y[n] = pSrc->IMU_DATA_CH[_CH].ACC_y[n];
			pDest->IMU_DATA_CH[_CH].ACC_z[n] = pSrc->IMU_DATA_CH[_CH].ACC_z[n];
			pDest->IMU_DATA_CH[_CH].GYRO_x[n] = pSrc->IMU_DATA_CH[_CH].GYRO_x[n];
			pDest->IMU_DATA_CH[_CH].GYRO_y[n] = pSrc->IMU_DATA_CH[_CH].GYRO_y[n];
			pDest->IMU_DATA_CH[_CH].GYRO_z[n] = pSrc->IMU_DATA_CH[_CH].GYRO_z[n];
			pDest->IMU_DATA_CH[_CH].Q_0[n] = pSrc->IMU_DATA_CH[_CH].Q_0[n];
			pDest->IMU_DATA_CH[_CH].Q_1[n] = pSrc->IMU_DATA_CH[_CH].Q_1[n];
			pDest->IMU_DATA_CH[_CH].Q_2[n] = pSrc->IMU_DATA_CH[_CH].Q_2[n];
			pDest->IMU_DATA_CH[_CH].Q_3[n] = pSrc->IMU_DATA_CH[_CH].Q_3[n];
			pDest->IMU_DATA_CH[_CH].Pitch[n] = pSrc->IMU_DATA_CH[_CH].Pitch[n];
			pDest->IMU_DATA_CH[_CH].Yaw[n] = pSrc->IMU_DATA_CH[_CH].Yaw[n];
		}
	}

	return false;
}
