#ifndef FOOTDETECT_H
#define FOOTDETECT_H

/*
 * 根据足底压力找到步态相位关键点
 * 根据运动持续时间和运动间隔时间
*/
#include <vector>
class FootDetect
{
public:
    FootDetect();
public:
    //找到通道ch中的刚离开地面和刚接触地面的点 并分为各个运动大段
    int FindAirFootPoint(int ch,std::vector<std::vector<int>>& chn);
    //找到begin和end之前的imu通道nums和的峰值点
    int FindImuPeak(int begin,int end,std::vector<int> nums);
    //找到begin和end之前的imu通道nums和的峰谷点
    int FindImuValley(int begin,int end,std::vector<int> nums);
    //找到begin和end之前的foot 通道ch中左脚掌离开地面的点
    int FindLTO(int begin,int end,int ch);
    //找到begin和end之前的foot 通道ch中右脚跟接触地面的点
    int FindLHS(int begin,int end,int ch);
    //找到步态周期中右脚的RHS RTS RHO RTO
    int FindHeelToe();
    //找到水平行走第action运动段的步态5相位点
    int FindFiveGaitphasePoint(std::vector<int> action,std::vector<int>);
    //找到上楼梯第action运动段的步态5相位点
    int FindUpStairsGaitphasePoint(std::vector<int> action,std::vector<int>);
    //找到下楼梯第action运动段的步态5相位点
    int FindDownStairsGaitphasePoint(std::vector<int> action,std::vector<int>);
    //找到上坡第action运动段的步态5相位点
    int FindUpHillGaitphasePoint(std::vector<int> action,std::vector<int>);
    //找到下坡第action运动段的步态5相位点
    int FindDownHillGaitphasePoint(std::vector<int> action,std::vector<int>);
public:
    std::vector<std::vector<int>> ch1;
    std::vector<std::vector<int>> ch2;
    std::vector<std::vector<std::vector<int>>> RightHeelToe;
    std::vector<std::vector<int>> GaitPhase;
    std::vector<std::vector<int>> GaitPhaseUpStairs;
    std::vector<std::vector<int>> GaitPhaseDownStairs;
    std::vector<std::vector<int>> GaitPhaseDownHill;
    std::vector<std::vector<int>> GaitPhaseUpHill;

};

#endif // FOOTDETECT_H
