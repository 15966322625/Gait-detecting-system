#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <vector>
#include <iostream>
#include "tensorflow/lite/examples/LowerMachine/config.h"
#include "tensorflow/lite/examples/LowerMachine/global.h"
#include "tensorflow/lite/examples/LowerMachine/ADC_OL.h"


//	����� ���캯��
ADC_OL::ADC_OL()
{
	//	ADCbuf = (unsigned char *)malloc(sizeof(unsigned char)*ADC_CH_NUM * 2);	//����ռ䣬�����洢ADת��֮����ֽ�������
}
//����� ��������
ADC_OL::~ADC_OL()
{
	//	free(ADCbuf);
	//	free(Data);
}
//	ADCģ�鹤����ʼ��
bool ADC_OL::init()
{
	wiringPiSetup();		//��ݮ��IO����ģʽ����
	setSpiSpeed(ADC_SPICLK);	// ����AD7606��SPI����ͨѶ
	setGPIO();					//���IO���Ź�������
	setOS(0);					//����������ģʽ�趨,default:OS2 OS1 OS0 = 000
	setRange(0);				//�趨���뷶Χģʽ,default:range = -5 ~ +5    1: ��10
	reset();					//ADC��λ����
	setIOtoCVT();
	return true;
}
// �趨SPI����ͨѶ�ٶ�
void ADC_OL::setSpiSpeed(unsigned int speed = 5000000)
{
	int fd;
	if ((fd = wiringPiSPISetupMode(SPI_CHANNEL_0, speed, SPI_MODE_2)) < 0)
	{
		fprintf(stderr, "Can't open the SPI bus: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
//	�������GPIO���� 
void ADC_OL::setGPIO()
{
	pinMode(RST, OUTPUT);
	pinMode(RANGE, OUTPUT);
	pinMode(CVAB, OUTPUT);
	pinMode(CS, OUTPUT);
	pinMode(BUSY, INPUT);
	pinMode(OS_0, OUTPUT);
	pinMode(OS_1, OUTPUT);
	pinMode(OS_2, OUTPUT);

	pullUpDnControl(BUSY, PUD_UP);
	digitalWrite(RANGE, LOW);
	digitalWrite(RST, LOW);
	digitalWrite(CS, HIGH);
	digitalWrite(CVAB, HIGH);
	digitalWrite(OS_0, LOW);
	digitalWrite(OS_1, LOW);
	digitalWrite(OS_2, LOW);
}
//����������ģʽ�趨
void ADC_OL::setOS(uint8_t _ucMode = 0)//default:OS2 OS1 OS0 = 000
{
	switch (_ucMode) {			//			����������
	case 0:		// NO OS					1
		digitalWrite(OS_0, LOW);
		digitalWrite(OS_1, LOW);
		digitalWrite(OS_2, LOW);
		break;
	case 1: 	// 						2
		digitalWrite(OS_0, HIGH);
		digitalWrite(OS_1, LOW);
		digitalWrite(OS_2, LOW);
		break;
	case 2: 	//						4
		digitalWrite(OS_0, LOW);
		digitalWrite(OS_1, HIGH);
		digitalWrite(OS_2, LOW);
		break;
	case 3: 	// 						8
		digitalWrite(OS_0, HIGH);
		digitalWrite(OS_1, HIGH);
		digitalWrite(OS_2, LOW);
		break;
	case 4: 	//						16
		digitalWrite(OS_0, LOW);
		digitalWrite(OS_1, LOW);
		digitalWrite(OS_2, HIGH);
		break;
	case 5: 	//						32
		digitalWrite(OS_0, HIGH);
		digitalWrite(OS_1, LOW);
		digitalWrite(OS_2, HIGH);
		break;
	case 6: 	//						64
		digitalWrite(OS_0, LOW);
		digitalWrite(OS_1, HIGH);
		digitalWrite(OS_2, HIGH);
		break;
	default: 	//						��Ч		
		break;
	}
}

//�趨���뷶Χģʽ	0:��5V 1:��10V
void ADC_OL::setRange(int _ucRange = 0)
{
	switch (_ucRange) {
	case 0:
		digitalWrite(RANGE, LOW);		//��5V
		MAXRange = 5;
		break;
	case 1:
		digitalWrite(RANGE, HIGH);		//��10V
		MAXRange = 10;
		break;
	default:
		break;
	}
}

//ADC���и�λ����
void ADC_OL::reset()
{
	digitalWrite(CS, HIGH);
	digitalWrite(CVAB, HIGH);         /* AD7606 is high level reset��at least 50ns */
	digitalWrite(RST, LOW);
	//delayMicroseconds(1);        
	digitalWrite(RST, HIGH);
	//delayMicroseconds(1);        
	digitalWrite(RST, LOW);
}

//����IO���ã�֪ͨADģ�����ADת��
void ADC_OL::setIOtoCVT()
{
	/* Conv in rising edge at least 25ns  */
	digitalWrite(CVAB, LOW);
	delayMicroseconds(1);
	digitalWrite(CVAB, HIGH);
}


//	���û�ȡһ֡�µ����ݣ�������һ��ADC����ת��
bool ADC_OL::getData(float *pData)
{
	int ret;
	if (digitalRead(BUSY) == 0) {
		digitalWrite(CS, LOW);
		/*	�����߶�ȡ����		*/
		ret = wiringPiSPIDataRW(SPI_CHANNEL_0, ADCbuf, 16);
		if (ret == -1) {
			printf("ONLine: Sample failure! < %s >\n", strerror(errno));
			return false;
		}
		digitalWrite(CS, HIGH);
		setIOtoCVT();
		while (digitalRead(BUSY) == 1);		//�ȴ�����ADת�����
		for (int _CH = 0; _CH < 8; _CH++)
			pData[_CH] = compute(ADCbuf[_CH * 2], ADCbuf[_CH * 2 + 1]);
		ofset(pData);
		return true;
	}
	else
		return false;
}

//����AD�����������㣬���AD�����ĵ�ѹֵ
float ADC_OL::compute(unsigned char HH, unsigned char LL)
{
	unsigned int tmp;
	float disp;
	tmp = (HH << 8) + LL;
	disp = 1.0 * tmp *(MAXRange * 2) / 65535;
	if (disp >= MAXRange)
	{
		disp = MAXRange * 2 - disp;
		return disp * (-1);
	}
	else
		return disp;
}

bool ADC_OL::ofset(float *pData)
{
	//static float ADC_OFSET[8] = { 0.016, 0.017, 0.018, 0.016, 0.022, 0.028, 0.013, 0.019 };
	// static float ADC_OFSET[8] = {0.02,0.026,0.023,0.011,0,0.024,0.020,0.020,};
	//static float ADC_OFSET[8] = {0.015,0.016,0.018,0.016,0.022,0.026,0.023,0.018};// old
	static float ADC_OFSET[8] = {0.02,0.026,0.021,0.011,0.007,0.023,0.019,0.019};
	for (int n = 0; n < 8; n++)
		pData[n] -= ADC_OFSET[n];
	return true;
}
