#pragma once

extern int IMU_OL_fd;
extern FILE* IMU_OF_fd;

void init_IMU();
void IMU_getdata(float data[8][12]);//这里读取的数据有加速度 角速度磁场和角度