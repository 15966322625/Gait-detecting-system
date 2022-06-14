#include "FootDetect.h"
#include <QTextStream>
#include <QMessageBox>
#include <QString>
#include <QDebug>
#include <QFile>
#include <algorithm>
#include <iostream>

FootDetect::FootDetect()
{

}
//寻找空中足底关键点
int FootDetect::FindAirFootPoint(int ch, std::vector<std::vector<int>> &chn){
    std::vector<std::vector<int>> temp_chn;
    chn.clear();
    QFile foot_ch;
    QString path = QString("./FilterData/filter_FOOTM2ch%1.csv").arg(ch);
    foot_ch.setFileName(path);
    if (!foot_ch.open(QIODevice::ReadOnly))
    {
        qDebug()<<"打开文件chn出错";
        return -1;
    }
    QTextStream in(&foot_ch);
    int last_sw = 0;
    int last_tnow = 0;
    std::vector<int> temp_point;
    temp_point.clear();
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(1).toInt();//足底开关当前值
        if(sw == 1){
            if(sw != last_sw){
//                此时为离开地面的点

                if(temp_point.empty()){
                    temp_point.push_back(Tnow);
                }else{
                    int N = temp_point.size();
                    int lastTnow = temp_point[N-1];
                    if(Tnow-lastTnow >= 3000){
                        //这一次离开地面与上一次接触地面间隔时间超过3000ms 视为另一个活动阶段
                        temp_chn.push_back(temp_point);
                        temp_point.clear();
                    }
                    temp_point.push_back(Tnow);
                }
            }
        }else{
            if(sw != last_sw){
                //此时为接触地面的点 直接追加即可
                //qDebug()<<"点"<<last_tnow<<Qt::endl;
                temp_point.push_back(last_tnow);
            }
        }
        last_sw = sw;
        last_tnow = Tnow;
    }
    if(!temp_point.empty()){
        temp_chn.push_back(temp_point);
    }
    for(int i=0;i<temp_chn.size();++i){
        if(temp_chn[i].size()>=12){
            chn.push_back(temp_chn[i]);
        }
    }
//    for(int i=0;i<chn.size();++i){
//        std::cout<<"chn:"<<ch<<"********************\n"<<std::endl;
//        for(int point:chn[i]){
//            std::cout<<point<<" ";
//        }
//        std::cout<<"\n"<<std::endl;
//    }
    foot_ch.close();
    return 0;
}

//寻找IMU峰值
int FootDetect::FindImuPeak(int begin, int end, std::vector<int> nums){
    QFile IMU;
    QString path = QString("./FilterData/filter_IMU.csv");
    IMU.setFileName(path);
    if (!IMU.open(QIODevice::ReadOnly))
    {
        return -2;
    }
    QTextStream in(&IMU);
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int now = linelist.at(0).toInt();
        if(now>begin){
            break;
        }
    }
    if(in.atEnd())
        return -1;
    float IMU_data[8][12];
    int Tnow=0;
    float last = -1800;
    int res = -1;
    while(!in.atEnd()&&Tnow<end){
        QString line = in.readLine();
        auto linelist = line.split(',');
        for (int i = 0; i < linelist.length() - 1; i++)
        {
            IMU_data[i / 12][i % 12] = linelist.at(i + 1).toFloat();
        }
        Tnow = linelist.at(0).toInt();
        float current = 0;
        for(int i=0;i<nums.size();++i){
            current += IMU_data[nums[i]/12][nums[i]%12];
        }
        if(current>last){
            last = current;
            res = Tnow;
        }
    }
    IMU.close();
    return res;
}

//寻找IMU谷值
int FootDetect::FindImuValley(int begin, int end, std::vector<int> nums){
    QFile IMU;
    QString path = QString("./FilterData/filter_IMU.csv");
    IMU.setFileName(path);
    if (!IMU.open(QIODevice::ReadOnly))
    {
        return -2;
    }
    QTextStream in(&IMU);
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int now = linelist.at(0).toInt();
        if(now>begin){
            break;
        }
    }
    if(in.atEnd())
        return -3;
    float IMU_data[8][12];
    int Tnow=0;
    float last = 1800;
    int res = -1;
    while(!in.atEnd()&&Tnow<end){
        QString line = in.readLine();
        auto linelist = line.split(',');
        for (int i = 0; i < linelist.length() - 1; i++)
        {
            IMU_data[i / 12][i % 12] = linelist.at(i + 1).toFloat();
        }
        Tnow = linelist.at(0).toInt();
        float current = 0;
        for(int i=0;i<nums.size();++i){
            current += IMU_data[nums[i]/12][nums[i]%12];
        }
        if(current<last){
            last = current;
            res = Tnow;
        }
    }
    IMU.close();
    return res;
}

int FootDetect::FindLTO(int begin, int end, int ch){
    //定位首次离开地面 从0变到1
    QFile foot_ch;
    QString path = QString("./FilterData/filter_FOOTch%1.csv").arg(ch);
    foot_ch.setFileName(path);
    if (!foot_ch.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QTextStream in(&foot_ch);
    int last_Tnow = -1;
    int last_value = 0;
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int now = linelist.at(0).toInt();
        int sw = linelist.at(1).toInt();//足底开关当前值
        last_value = sw;
        last_Tnow = now;
        if(now>=begin){
            break;
        }
    }
    if(in.atEnd())
        return -1;
    int res = -1;
    if(last_value == 1){
        return -3;
    }
    while(!in.atEnd() && last_Tnow<end){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(1).toInt();//足底开关当前值
        if(sw == 0){
            last_Tnow = Tnow;
            last_value = sw;
        }else{
            if(sw != last_value){
                res = Tnow;
                break;
            }
        }
    }
    return res;
}

int FootDetect::FindLHS(int begin, int end, int ch){
    //定位首次接触地面 从1变到0
    QFile foot_ch;
    QString path = QString("./FilterData/filter_FOOTch%1.csv").arg(ch);
    foot_ch.setFileName(path);
    if (!foot_ch.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    QTextStream in(&foot_ch);
    int last_Tnow = -1;
    int last_value = 0;
    while(!in.atEnd()){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int now = linelist.at(0).toInt();
        int sw = linelist.at(1).toInt();//足底开关当前值
        last_value = sw;
        last_Tnow = now;
        if(now>=begin){
            break;
        }
    }
    if(in.atEnd())
        return -1;
    int res = -1;
    if(last_value == 0){
        return -3;
    }
    while(!in.atEnd() && last_Tnow<end){
        QString line = in.readLine();
        auto linelist = line.split(',');
        int Tnow = linelist.at(0).toInt();//当前时间戳
        int sw = linelist.at(1).toInt();//足底开关当前值
        if(sw == 1){
            last_Tnow = Tnow;
            last_value = sw;
        }else{
            if(sw != last_value){
                res = last_Tnow;
                break;
            }
        }
    }
    return res;
}
int FootDetect::FindHeelToe(){
    RightHeelToe.clear();
    std::vector<std::vector<int>> action;
    int nums_action = ch1.size();
    if(ch1.size() == ch2.size()){
        for(int i=0;i<nums_action;i=i+1){
            // 从脚后跟开始找 与这一次匹配的脚掌
            for(int j=0;j<ch2[i].size();j=j+2){
                //找匹配这一次脚后跟的脚掌序号
                int num = -1;
                for(int k=0;k<ch1[i].size();k=k+2){
                    if(ch1[i][k]>ch2[i][j]){
                        //首次脚后跟离地后的脚掌离地
                        num = k;
                        break;
                    }
                }
                if(num == -1)
                    continue;
                if(ch1[i][num] > ch2[i][j+1])//在脚后跟接触地面，脚掌才离地，所以这次脚掌缺失
                    continue;
                std::vector<int> temp;
                temp.push_back(ch2[i][j]);//后跟离地
                temp.push_back(ch1[i][num]);//脚掌离地
                temp.push_back(ch2[i][j+1]);//后跟接触地面
                temp.push_back(ch1[i][num+1]);//脚掌离地

                action.push_back(temp);
            }
            RightHeelToe.push_back(action);
            action.clear();
        }
    }else{
        qDebug()<<"脚掌与脚后跟的活动次数不一样";
        return -1;
    }
    //对RightHeelToe再做细微的调整 排个序
    for(int i=0;i<RightHeelToe.size();++i){
        for(int j=0;j<RightHeelToe[i].size();++j){
           std::sort(RightHeelToe[i][j].begin(),RightHeelToe[i][j].end());
        }
    }
    for(int i=0;i<RightHeelToe.size();++i){
        qDebug()<<"活动"<<i<<" ==:";
        for(int j=0;j<RightHeelToe[i].size();++j){
           qDebug()<<RightHeelToe[i][j][0]<<" "<<RightHeelToe[i][j][1]<<" "<<RightHeelToe[i][j][2]<<" "<<RightHeelToe[i][j][3]<<" ";
        }
    }
    return 0;

}
//****************行走**********************************************************
int FootDetect::FindFiveGaitphasePoint(std::vector<int> action,std::vector<int> nums){
    GaitPhase.clear();
    for(int ac = 0;ac<action.size();++ac){
        for(int i=0;i<RightHeelToe[action[ac]].size()-1;++i){
            int A = RightHeelToe[action[ac]][i][0];
            if(A <= 0) continue;
            int B = RightHeelToe[action[ac]][i][1];
            if(B <= 0) continue;
            int C = RightHeelToe[action[ac]][i][2];
            if(C <= 0) continue;
            int D = RightHeelToe[action[ac]][i][3];
            if(D <= 0) continue;
            int E = RightHeelToe[action[ac]][i+1][0];
            if(E <= 0) continue;
            int mid = FindImuPeak(D,E,nums);
            if(mid <= 0) continue;
            GaitPhase.push_back({A,B,C,D,mid,E});
        }
    }
    return 0;
}
//****************上楼梯**********************************************************
int FootDetect::FindUpStairsGaitphasePoint(std::vector<int> action, std::vector<int> nums){
    GaitPhaseUpStairs.clear();
    for(int ac = 0;ac<action.size();++ac){
        for(int i=0;i<RightHeelToe[action[ac]].size()-1;++i){
            int RHO = RightHeelToe[action[ac]][i][0];
            if(RHO <= 0) continue;
            int RTO = RightHeelToe[action[ac]][i][1];
            if(RTO <= 0) continue;
            int RHS = RightHeelToe[action[ac]][i][2];
            if(RHS <= 0) continue;
            int RTS = RightHeelToe[action[ac]][i][3];
            if(RTS <= 0) continue;
            int Next_RHO = RightHeelToe[action[ac]][i+1][0];
            if(Next_RHO <= 0) continue;
            int LTO = FindLTO(RHS,Next_RHO,3);
            if(LTO <= 0) continue;

            //int mid = FindImuValley(RHS,Next_RHO,nums);
            //int mid = FindImuPeak(RHS,Next_RHO,nums);
            int mid = FindImuPeak(LTO,Next_RHO,nums);


            if(mid <= 0) continue;
            GaitPhaseUpStairs.push_back({RHO,RTO,RHS,LTO,mid,Next_RHO});
        }
    }
    return 0;
}
//****************下楼梯**********************************************************
int FootDetect::FindDownStairsGaitphasePoint(std::vector<int> action, std::vector<int> nums){
    GaitPhaseDownStairs.clear();
    for(int ac = 0;ac<action.size();++ac){
        for(int i=0;i<RightHeelToe[action[ac]].size()-1;++i){
            int RHO = RightHeelToe[action[ac]][i][0];
            if(RHO <= 0) continue;
            int RTO = RightHeelToe[action[ac]][i][1];
             if(RTO <= 0) continue;
            int RTS = RightHeelToe[action[ac]][i][2];
            if(RTS <= 0) continue;
            int RHS = RightHeelToe[action[ac]][i][3];
            if(RHS <= 0) continue;
            int Next_RHO = RightHeelToe[action[ac]][i+1][0];
            if(Next_RHO <= 0) continue;
            int LTO = FindLTO(RHS,Next_RHO,3);
            if(LTO <= 0) continue;
            int mid = FindImuPeak(RHS,Next_RHO,nums);
            if(mid <= 0) continue;
            GaitPhaseDownStairs.push_back({RHO,RTO,RTS,RHS,mid,Next_RHO});
        }
    }
    return 0;
}
//****************上坡**********************************************************
int FootDetect::FindUpHillGaitphasePoint(std::vector<int> action,std::vector<int> nums){

    GaitPhaseUpHill.clear();
    qDebug()<<"活动共几段"<<RightHeelToe.size();
    for(int ac = 0;ac<action.size();++ac){
        for(int i=0;i<RightHeelToe[action[ac]].size()-1;++i){
            int RHO = RightHeelToe[action[ac]][i][0];
            if(RHO<=0) continue;
            int RTO = RightHeelToe[action[ac]][i][1];
            if(RTO <=0) continue;
            int RHS = RightHeelToe[action[ac]][i][2];
            if(RHS <= 0) continue;
            int RTS = RightHeelToe[action[ac]][i][3];
            if(RTS <= 0) continue;
            int Next_RHO = RightHeelToe[action[ac]][i+1][0];
            int LTO = FindLTO(RHS,Next_RHO,3); //如果找不到咋办？
            if(LTO == -1){
                continue;
            }
            //int mid = FindImuPeak(RHS,Next_RHO,nums);//左小腿
            int mid = FindImuPeak(LTO,Next_RHO,nums);//左小腿
            if(LTO == -1){
                continue;
            }
            GaitPhaseUpHill.push_back({RHO,RTO,RHS,LTO,mid,Next_RHO});
        }
    }
    return 0;
}
//****************下坡**********************************************************
int FootDetect::FindDownHillGaitphasePoint(std::vector<int> action,std::vector<int> nums){
    GaitPhaseDownHill.clear();
    for(int ac = 0;ac<action.size();++ac){
        for(int i=0;i<RightHeelToe[action[ac]].size()-1;++i){
            int RHO = RightHeelToe[action[ac]][i][0];
            if(RHO<=0) continue;
            int RTO = RightHeelToe[action[ac]][i][1];
            if(RTO <=0) continue;
            int RHS = RightHeelToe[action[ac]][i][2];
            if(RHS <= 0) continue;
            int RTS = RightHeelToe[action[ac]][i][3];
            if(RTS <= 0) continue;
            int Next_RHO = RightHeelToe[action[ac]][i+1][0];
            int LTO = FindLTO(RHS,Next_RHO,3);
            if(LTO == -1){
                continue;
            }
            //int mid = FindImuValley(RHS,Next_RHO,nums);//左小腿
            int mid = FindImuValley(RTS,Next_RHO,nums);//左小腿
            GaitPhaseDownHill.push_back({RHO,RTO,RHS,RTS,mid,Next_RHO});
        }
    }
    return 0;
}
