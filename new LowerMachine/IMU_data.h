#pragma once
#include <pthread.h> 
#include <fstream>
#include <iostream>
using namespace std;
extern FILE* IMU_OF_fd;

int init_IMU(const char path[],int,int);
void IMU_getdata(int ch_num,int imu_fd,int base,float data[8][12]);
void* thread_imu(void *);


struct IMU_CFG{
    int imu_fd;
    int ch;
    float data[8][12];//处理的数据
    float data_original[8][12];//原始数据
    float last_data[8][12];//上一次的数据
    int flag;
    int base;
    int count;
    bool firstflag;
    bool changeflag;
    ofstream imu_file;
    ofstream imu_original_file;
    pthread_mutex_t mptr;
    pthread_mutex_t Bptr;
    pthread_mutex_t Eptr; 
};






