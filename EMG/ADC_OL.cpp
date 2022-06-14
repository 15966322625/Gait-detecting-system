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
#include "config.h"
#include "global.h"
#include "ADC_OL.h"


//构造函数和析构函数都是默认的
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
//	ADC模块的初始化操作
bool ADC_OL::init()
{
	wiringPiSetup();		//树莓派IO操作模式配置;
	setSpiSpeed(ADC_SPICLK);	// 配置AD7606的SPI总线通讯;
	setGPIO();					//相关IO引脚功能配置
	setOS(0);					//过采样工作模式设定,default:OS2 OS1 OS0 = 000
	setRange(0);				//设定输入范围模式,default:range = -5 ~ +5    1: ��10
	reset();					//ADC复位操作
	setIOtoCVT();
	return true;
}
// 设置SPI通信的通信速率 通信速率设置为5M 使用库函数进行设置
void ADC_OL::setSpiSpeed(unsigned int speed = 5000000)
{
	int fd;
	if ((fd = wiringPiSPISetupMode(SPI_CHANNEL_0, speed, SPI_MODE_2)) < 0)
	{
		fprintf(stderr, "Can't open the SPI bus: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
//设置GPIO引脚 使用的是库函数 主要是将这些引脚配置为输入输出模式 便于控制AD7606 有些引脚设置为输入 有些设置为输出 有的设置初始状态为高 有的为低
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
//设置过采样工作模式 由于我们的采样率较低 因此不需要过采样
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

//设置输入范围模式  设置为+-5v
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

//ADC复位操作 AD7606是高电平复位 高电平持续50ns 给这个引脚一个上升沿信号
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

//设置IO设置 通知AD模块进行AD转换 主要是给CVAB一个上升沿信号
void ADC_OL::setIOtoCVT()
{
	/* Conv in rising edge at least 25ns  */
	digitalWrite(CVAB, LOW);
	delayMicroseconds(1);
	digitalWrite(CVAB, HIGH);
}


//获取数据
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
	static float ADC_OFSET[8] = { 0.01, 0.01, 0, 0.02, 0.02, 0.02, 0.01, 0.02 };
	for (int n = 0; n < 8; n++)
		pData[n] -= ADC_OFSET[n];
	return true;
}
