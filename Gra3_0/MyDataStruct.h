#ifndef MYDATASTRUCT_H
#define MYDATASTRUCT_H

struct ADC_Data
{
    float Data[256];
};
struct IMU_Data
{
    float ACC_x[50];
    float ACC_y[50];
    float ACC_z[50];
    float GYRO_x[50];
    float GYRO_y[50];
    float GYRO_z[50];
    float Q_0[50];
    float Q_1[50];
    float Q_2[50];
    float Q_3[50];
    float Pitch[50];
    float Yaw[50];
};
struct FT_Data
{
    int FT_L_P[256];
    int FT_L_H[256];
    int FT_R_P[256];
    int FT_R_H[256];
};
struct ACT_DATA {
    int point;
    int adc_length;
    int IMU_length;
    ADC_Data ADC_DATA_CH[8];
    IMU_Data IMU_DATA_CH[8];
    FT_Data FT_DATA_CH;
};
struct jump
{
    int count;
    int act_begin_point[20];
    int act_end_point[20];
    int peak_begin_point[20];
    int peak_end_point[20];
    int valley_point[20];
    int rise_begin_point[20];
    int rise_end_point[20];

};
struct ACT_point
{
    int begin[20];
    int ACT_begin[200];
    int ACT_begin_count;
    int begin_count;
    int end[20];
    int ACT_end[200];
    int ACT_end_count;
    int end_count;
};
struct ALL_point
{
    int count;
    int action_begin[20];
    int action_end[20];
    int jump_begin_point[20];
    int jump_end_point[20];
    int peak_begin_point[20];
    int peak_end_point[20];
    int valley_point[20];
    int rise_begin_point[20];
    int rise_end_point[20];
};

#endif // MYDATASTRUCT_H
