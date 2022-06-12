#pragma once


#include "main_data.h"


class data_WIN
{
public:
	bool Init();	//初始化
	bool Clear();				//清空数据缓存区
	bool Enable();				//数据满了
	bool DataCpy(SHM_DATA_t *pDest, SHM_DATA_t *pSrc);	//将数据复制到共享内存
	SHM_DATA_t DATW;
	bool Append_ADC(float Data[8]);	//最新一组数据
	bool Append_IMU(float Data[8][12]);	//最新一组数据
};