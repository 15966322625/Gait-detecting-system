#pragma once
//实现两种数据的滤波算法
void ADC_filt(float *dest, float *src);
void IMU_filter(float dest[8][12], float src[8][12]);

