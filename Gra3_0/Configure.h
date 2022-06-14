//这里主要是一些配置文件：包括一些宏定义和一些全局变量
#ifndef CONFIGURE_H
#define CONFIGURE_H
#define IMU_CH 7
#define IMU_0 0 //胸前
#define IMU_1 1 //大腿
#define IMU_2 2 //小腿
#define NUMBER_IMU 11+12 //使用大腿上的IMU11+12
#define IMU_ACT_1 1.05
#define IMU_ACT_4  0.95
#define IMU_ACT_2  1.05
#define IMU_ACT_3  0.96
#define sEMG1 0
#define sEMG2 1
#define sEMG3 2
#define sEMG4 3
#define sEMG5 4
#define SLPW_LEN 64
#define SAMPEN_m 2
#define SAMPEN_r 0.02
#define ACT_LIM_S 0.01
#define ACT_LIM_E 0.001
#define EPSILON_2 0.000001
//滑动窗参数
#define STEP 40
#define DATA_PACKAGE_MAX 200
#define WIDTH 4					//窗的个数
#define MEAN_WIDTH 2			//几个计算一次mean
#define MAX_COUNT 1		//连续几次突变
#define EPSILON 0.000001 //float 精度
#define SEEK_BIGER 1
#define SEEK_SMALLER 0
#define FOOT_CH 2
#define EXTEND_POINT 200

#define database 40
#endif // CONFIGURE_H
