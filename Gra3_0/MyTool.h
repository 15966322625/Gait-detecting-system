#ifndef MYTOOL_H
#define MYTOOL_H

void ADC_filter(float* ADC_before, float* ADC_filter);       //EMG滤波
void IMU_filter(float imu_before[8][12], float IMU_filter[8][12]); //IMU 滤波
float calc_WAV(float* IN, int len);//绝对值积分平均
float calc_WL(float* IN, int len);//波形长度
float calc_AV(float* IN, int len);//均值
float calc_IAR(float* IN, int len);//方差
float calc_RMS(float* IN, int len);//均方根
float calc_Euler(float* IN, int x, int y);//欧拉角
float calc_ACC(float* x, float* y, float* z, int i);//加速度

#endif // AA_H
