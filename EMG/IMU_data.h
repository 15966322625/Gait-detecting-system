#pragma once

extern int IMU_OL_fd;
extern FILE* IMU_OF_fd;

void init_IMU();
void IMU_getdata(float data[8][12]);