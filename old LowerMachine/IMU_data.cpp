#include <wiringSerial.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "IMU_data.h"
#include "config.h"


int IMU_OL_fd;
//FILE* IMU_OF_fd;

void init_IMU()
{
#ifdef IMU_MODE_OL
	for(int i=0;i<1;i++)
	{
		if ((IMU_OL_fd = serialOpen("/dev/ttyUSB0", 230400)) < 0)
		{
			printf("open USB0 serial error!\n");
			exit(1);
		}
	}

	int bite_rec = 0;
	unsigned char CMD[8] = {0x50,0x03,0x00,0x34,0x00,0x0C,0x09,0x80};
	unsigned char buff[100] = {0};
	serialFlush(IMU_OL_fd);
	write(IMU_OL_fd, CMD, 8);
	bite_rec = read(IMU_OL_fd, buff, 29);
	if(bite_rec>0)
	{
		printf("open IMU_USB0 success!\n");
		serialFlush(IMU_OL_fd);
	}
	else
	{
		serialClose(IMU_OL_fd);
		IMU_OL_fd = -1;
		if ((IMU_OL_fd = serialOpen("/dev/ttyUSB1", 460800)) < 0)
		{
			printf("open IMU_USB1 serial error!\n"); 
			exit(1);
		}
		serialFlush(IMU_OL_fd);
		write(IMU_OL_fd, CMD, 8);
		bite_rec = read(IMU_OL_fd, buff, 29);
		if(bite_rec > 0)
		{
			printf("open IMU_USB1 success!\n");
		 	serialFlush(IMU_OL_fd);
		}
	}
	
#endif



}

void IMU_getdata(float data[8][12])
{
#ifdef IMU_MODE_OL
	static unsigned char CMD[8][8] = {{0x50,0x03,0x00,0x34,0x00,0x0C,0x09,0x80},\
	 {0x51,0x03,0x00,0x34,0x00,0x0C,0x08,0x51},\
	 {0x52,0x03,0x00,0x34,0x00,0x0C,0x08,0x62},\
	 {0x53,0x03,0x00,0x34,0x00,0x0C,0x09,0xB3},\
	 {0x54,0x03,0x00,0x34,0x00,0x0C,0x08,0x04},\
	 {0x55,0x03,0x00,0x34,0x00,0x0C,0x09,0xD5},\
	 {0x56,0x03,0x00,0x34,0x00,0x0C,0x09,0xE6},\
	 {0x57,0x03,0x00,0x34,0x00,0x0C,0x08,0x37} };
	static unsigned char CMD2[8][8] = {{0x50,0x03,0x00,0x51,0x00,0x04,0x18,0x59},\
	{0x51,0x03,0x00,0x51,0x00,0x04,0x19,0x88},\
	{0x52,0x03,0x00,0x51,0x00,0x04,0x19,0xBB},\
	{0x53,0x03,0x00,0x51,0x00,0x04,0x18,0x6A},\
	{0x54,0x03,0x00,0x51,0x00,0x04,0x19,0xDD},\
	{0x55,0x03,0x00,0x51,0x00,0x04,0x18,0x0C},\
	{0x56,0x03,0x00,0x51,0x00,0x04,0x18,0x3F},\
	{0x57,0x03,0x00,0x51,0x00,0x04,0x19,0xEE}};
	static unsigned char tmp_buf[100];
	for (int _CH = 0; _CH < IMU_CH; _CH++)
	{
		// CMD[0] = 0x50 + _CH;
		// CMD2[0] = 0x50 + _CH;
		serialFlush(IMU_OL_fd);
		write(IMU_OL_fd, CMD[_CH], 8);
		read(IMU_OL_fd, tmp_buf, 29);
                //printf("read %d imu ok\n",_CH);
		data[_CH][0] = (float)((short)((tmp_buf[3] << 8) | tmp_buf[4])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)
		data[_CH][1] = (float)((short)((tmp_buf[5] << 8) | tmp_buf[6])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)
		data[_CH][2] = (float)((short)((tmp_buf[7] << 8) | tmp_buf[8])) / 32768.0 * 16.0;  //xx g  (g=9.8m/s2)																		   
		//���ٶ�
		data[_CH][3] = (float)((short)((tmp_buf[9] << 8) | tmp_buf[10])) / 32768 * 2000;
		data[_CH][4] = (float)((short)((tmp_buf[11] << 8) | tmp_buf[12])) / 32768 * 2000;
		data[_CH][5] = (float)((short)((tmp_buf[13] << 8) | tmp_buf[14])) / 32768 * 2000;
		//�ų�
		data[_CH][6] = (float)((short)((tmp_buf[15] << 8) | tmp_buf[16]));
		data[_CH][7] = (float)((short)((tmp_buf[17] << 8) | tmp_buf[18]));
		data[_CH][8] = (float)((short)((tmp_buf[19] << 8) | tmp_buf[20]));
		//�Ƕ�
		data[_CH][9] = (float)((short)((tmp_buf[21] << 8) | tmp_buf[22])) / 32768 * 180;
		data[_CH][10] = (float)((short)((tmp_buf[23] << 8) | tmp_buf[24])) / 32768 * 180;
		data[_CH][11] = (float)((short)((tmp_buf[25] << 8) | tmp_buf[26])) / 32768 * 180;

		write(IMU_OL_fd, CMD2[_CH], 8);
		read(IMU_OL_fd, tmp_buf, 13);
		data[_CH][6] = (float)((short)((tmp_buf[3] << 8) | tmp_buf[4])) / 32768.0;
		data[_CH][7] = (float)((short)((tmp_buf[5] << 8) | tmp_buf[6])) / 32768.0;
		data[_CH][8] = (float)((short)((tmp_buf[7] << 8) | tmp_buf[8])) / 32768.0;
		data[_CH][9] = (float)((short)((tmp_buf[9] << 8) | tmp_buf[10])) / 32768.0;
	}
#else
	//static int a;
	//fscanf(IMU_OF_fd, "%d", &a);
	//for (int _CH = 0; _CH < 3; _CH++)
	//{
	//	for (int i = 0; i < 12; i++)
	//	{
	//		fscanf(IMU_OF_fd, " %f", &data[_CH][i]);
	//	}	
	//}
	//fscanf(IMU_OF_fd, "\n");
#endif
}
