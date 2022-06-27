/************************************************************
*			ADC�����ײ����
***************************************************************/
#pragma once
#include <stdint.h>

#define RST 			0		//IO���Ŷ���
#define CVAB 			2
#define CS 				3
#define BUSY  			4
#define RANGE			25
#define MISO			13
#define SCLK			14
#define OS_0			22
#define OS_1			23
#define OS_2			24
#define ADC_SPICLK 		5000000		// SPI����ͨѶ����
#define SPI_MODE_0		0u   //CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
#define SPI_MODE_1		1u   //CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
#define SPI_MODE_2		2u   //CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
#define SPI_MODE_3		3u   //CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
#define SPI_CHANNEL_0	0u
#define SPI_CHANNEL_1	1u

//#define ADC_CH_NUM			8

class ADC_OL
{
public:
	ADC_OL();
	~ADC_OL();
public:
	bool init();	//��ʼ��������ָ����ǰ��port_num��ͨ��������AD����
	bool getData(float *pData);	//��ȡ����
//public:
//	float Data[8];
private:
	void setSpiSpeed(unsigned int speed);	//SPI���߳�ʼ������
	void setGPIO();				//���IO���Ź�������
	void setOS(uint8_t _ucMode);	//����������ģʽ�趨
	void setRange(int _ucRange);	//�趨���뷶Χģʽ
	void reset();				//ADC��λ����
	void setIOtoCVT();			//����IO���ã�֪ͨADģ�����ADת��
	float compute(unsigned char HH, unsigned char LL);	//����AD�����������㣬���AD�����ĵ�ѹֵ
	bool ofset(float *pData);	//Ϊԭʼ��������ƫ��
private:
	int MAXRange = 5;
	unsigned char ADCbuf[16];
};




