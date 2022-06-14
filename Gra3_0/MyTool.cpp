#include "MyTool.h"
#include <math.h>
#include <QString>
#include "Configure.h"
void ADC_filter(float* ADC_before, float* ADC_filter)
{

    //差分方程
    // 20~300HZ bandpass parameters
    static const float A[7] = { 1,	-1.88248750455499,	0.628779286563966,	0.0740047414569212,	0.473360564074673,	-0.213082455915409,	-0.0779475568208643};
    static const float B[7] = { 0.290731520701913,	0,	-0.872194562105740,	0,	0.872194562105740,	0,	-0.290731520701913};
    static float temp_x[8][7] = { {0},{0},{0},{0},{0},{0},{0},{0} };
    static float temp_y[8][7] = { {0},{0},{0},{0},{0},{0},{0},{0} };
    //50HZ 工频陷波参数
    static const float A_50[5] = { 1,-3.77489,5.52628,-3.70669,0.96419};
    static const float B_50[5] = { 0.98193,-3.74079,5.52661,-3.74079,0.98193};
    static float temp_x_50[8][5] = { {0},{0},{0},{0},{0},{0},{0},{0} };
    static float temp_y_50[8][5] = { {0},{0},{0},{0},{0},{0},{0},{0} };


    //20~300带通滤波
    for (int ch = 0; ch < 8; ch++)
    {
        for (int i = 0; i < 6; i++)
        {
            temp_x[ch][i] = temp_x[ch][i + 1];
        }
        temp_x[ch][6] = ADC_before[ch];
    }//跟新Xn

    for (int ch = 0; ch < 8; ch++)
    {
        float sum1 = 0.0;
        for(int i=0;i<7;++i){
            sum1 = sum1 + B[i]*temp_x[ch][6-i];
        }
        float sum2 = 0.0;
        for(int i=1;i<7;++i){
            sum2 =sum2 + A[i]*temp_y[ch][7-i];
        }
        ADC_filter[ch] = sum1-sum2;

    }//得到最新的Yn

    for (int ch = 0; ch < 8; ch++)
    {
        for (int i = 0; i < 6; i++)
        {
            temp_y[ch][i] = temp_y[ch][i + 1];
        }
        temp_y[ch][6] = ADC_filter[ch];
    }//跟新Yn



    //50hz工频陷波
    for (int ch = 0; ch < 8; ch++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp_x_50[ch][j] = temp_x_50[ch][j + 1];
        }
        temp_x_50[ch][4] = ADC_filter[ch];
    }//跟新Xn
    for (int ch = 0; ch < 8; ch++)
    {
        float sum1 = 0.0;
        for(int i=0;i<5;++i){
            sum1 =sum1+B_50[i]*temp_x_50[ch][4-i];
        }
        float sum2 = 0.0;
        for(int i=1;i<5;++i){
            sum2 =sum2 + A_50[i]*temp_y_50[ch][5-i];
        }
        ADC_filter[ch] = sum1-sum2;

    }//得到最新的Yn
    for (int ch = 0; ch < 8; ch++)
    {
        temp_y_50[ch][0] = temp_y_50[ch][1];
        temp_y_50[ch][1] = temp_y_50[ch][2];
        temp_y_50[ch][2] = temp_y_50[ch][3];
        temp_y_50[ch][3] = temp_y_50[ch][4];
        temp_y_50[ch][4] = ADC_filter[ch];
    }//跟新Yn

}
void IMU_filter(float imu_before[8][12], float IMU_filter[8][12])
{

    static const float A[6] = { 1,-0.79820,0.85663,-0.29829,0.09124,-0.00841 };
    static const float B[6] = { 0.02634,0.13171,0.26343,0.26343,0.13171,0.02634 };
    static float Xn[8][12][6];
    static float Yn[8][12][6];
    for (int ch = 0; ch < IMU_CH; ch++)
    {
        for (int i = 0; i < 12; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                Xn[ch][i][j] = Xn[ch][i][j + 1];
            }
            Xn[ch][i][5] = imu_before[ch][i];
        }
    }//跟新Xn
    for (int ch = 0; ch < IMU_CH; ch++)
    {
        for (int i = 0; i < 12; i++)
        {
            IMU_filter[ch][i] = B[0] * Xn[ch][i][5] + B[1] * Xn[ch][i][4] + B[2] * Xn[ch][i][3] + B[3] * Xn[ch][i][2] + B[4] * Xn[ch][i][1] + B[5] * Xn[ch][i][0]\
                - A[1] * Yn[ch][i][5] - A[2] * Yn[ch][i][4] - A[3] * Yn[ch][i][3] - A[4] * Yn[ch][i][2] - A[5] * Yn[ch][i][1];
        }
    }//差分计算出yn
    for (int ch = 0; ch < IMU_CH; ch++)
    {
        for (int i = 0; i < 12; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                Yn[ch][i][j] = Yn[ch][i][j + 1];
            }
            Yn[ch][i][5] = IMU_filter[ch][i];
        }
    }//更新Yn


}
//绝对值积分平均
float calc_WAV(float* IN, int len)
{
    float tmp = 0;
    for (int n = 0; n < len; n++)
    {
        tmp += abs(IN[n]);
    }
    return (tmp / len);
}
//波形长度
float calc_WL(float* IN, int len)
{
    float tmp = 0;
    for (int n = 0; n < len - 1; n++) {
        tmp += abs(IN[n] - IN[n + 1]);
    }
    return tmp;
}
//均值
float calc_AV(float* IN, int len)
{
    float tmp = 0;
    for (int n = 0; n < len; n++) {
        tmp += IN[n];
    }
    return (tmp / len);
}
//方差
float calc_IAR(float* IN, int len)
{
    float tmp = 0;
    for (int n = 0; n < len; n++) {
        tmp += pow(IN[n], 2);
    }
    tmp /= (len - 1);
    return (tmp / (len - 1));
}
//均方根
float calc_RMS(float* IN, int len)
{
    float tmp = 0;
    for (int n = 0; n < len; n++) {
        tmp += pow(IN[n], 2);
    }
    return sqrt(tmp / (len - 1));
}
//欧拉角
float calc_Euler(float* IN, int x, int y)
{
    float result;
    result = IN[x] - IN[y];
    return result;
}
//加速度
float calc_ACC(float* x, float* y, float* z, int i)
{
    float result;
    result = sqrt(x[i] * x[i] + y[i] * y[i] + z[i] * z[i]);
    return result;
}
