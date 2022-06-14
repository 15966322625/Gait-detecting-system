#include "ActiveSection.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QMessageBox>
#include <QtMath>

//检测活动段函数实现
ActiveSection::ActiveSection(QString sampENpath)
{
    this->sampEN_path = sampENpath;
    flag = false;
    jump_flag = false;
    ACT_STATUS = false;
    status = false;
    SampEn_num = 0;
    imu_SampEn_num = 0.0;
    this->begin = 0;
    this->end = 0;
    this->ACT_BEGIN = 0;
    this->ACT_END =0;
    this->DATA = new ACT_DATA;
    memset(this->DATA, 0, sizeof(ACT_DATA));
}
//初始化所有样本熵文件
void ActiveSection::open_file_all_samp()
{
    sampEN_path_2 = "./Filterdata/ALL_sampEN.csv";
    file_samp_2.setFileName(sampEN_path_2);
    file_samp_2.open(QIODevice::WriteOnly);
    out_2.setDevice(&file_samp_2);
    out_2.setCodec("UTF-8");
}
//初始化样本熵文件
void ActiveSection::open_file_samp()
{
    file_samp.setFileName(sampEN_path);
    file_samp.open(QIODevice::WriteOnly);
    out.setDevice(&file_samp);
    out.setCodec("UTF-8");
}
//打开imu样本数据
void ActiveSection::open_file_imu_samp()
{
    imu_sampEN_path = "./Filterdata/imu_sampEN.csv";
    imu_file_samp.setFileName(imu_sampEN_path);
    imu_file_samp.open(QIODevice::WriteOnly);
    imu_out.setDevice(&imu_file_samp);
    imu_out.setCodec("UTF-8");
}
void ActiveSection::clear()
{
    memset(this->DATA, 0, sizeof(ACT_DATA));
}
void ActiveSection::clear_slpw()
{
    memset(this->SLPW_buf,0,sizeof (this->SLPW_buf));
}
//析构函数 关闭文件
ActiveSection::~ActiveSection()
{
    delete this->DATA;
    file_samp.close();
    file_samp_2.close();
    imu_file_samp.close();
}
//追加ADC数据
void ActiveSection::ADC_append(float* data_adc)
{
    this->DATA->point = data_adc[0];
    for (int i = 0; i < 8; i++)
    {
        DATA->ADC_DATA_CH[i].Data[DATA->adc_length] = data_adc[i + 1];
    }
    DATA->adc_length++;
    DATA->point = data_adc[0];//记录第几行数据
}
//追加IMU数据
void ActiveSection::IMU_append(float IMU_data[8][12])
{
    for (int _CH = 0; _CH < IMU_CH; _CH++)
    {
        DATA->IMU_DATA_CH[_CH].ACC_x[DATA->IMU_length] = IMU_data[_CH][0];
        DATA->IMU_DATA_CH[_CH].ACC_y[DATA->IMU_length] = IMU_data[_CH][1];
        DATA->IMU_DATA_CH[_CH].ACC_z[DATA->IMU_length] = IMU_data[_CH][2];
        DATA->IMU_DATA_CH[_CH].GYRO_x[DATA->IMU_length] = IMU_data[_CH][3];
        DATA->IMU_DATA_CH[_CH].GYRO_y[DATA->IMU_length] = IMU_data[_CH][4];
        DATA->IMU_DATA_CH[_CH].GYRO_z[DATA->IMU_length] = IMU_data[_CH][5];
        DATA->IMU_DATA_CH[_CH].Q_0[DATA->IMU_length] = IMU_data[_CH][6];
        DATA->IMU_DATA_CH[_CH].Q_1[DATA->IMU_length] = IMU_data[_CH][7];
        DATA->IMU_DATA_CH[_CH].Q_2[DATA->IMU_length] = IMU_data[_CH][8];
        DATA->IMU_DATA_CH[_CH].Q_3[DATA->IMU_length] = IMU_data[_CH][9];
        DATA->IMU_DATA_CH[_CH].Pitch[DATA->IMU_length] = IMU_data[_CH][10];
        DATA->IMU_DATA_CH[_CH].Yaw[DATA->IMU_length] = IMU_data[_CH][11];
    }
    DATA->IMU_length++;
}

bool ActiveSection::data_full()
{
    if (DATA->adc_length >= 40 && DATA->IMU_length>=4)//构成一个数据包（40个adc，4个imu）
    {
        DATA->IMU_length = 0;
        DATA->adc_length = 0;
        return true;
    }
    return false;
}
void ActiveSection::MK_slpw()
{
    //前24个为上一个的后24个数据，后40个为最新的数据
    for (int i = 0; i < 24; i++)
    {
        SLPW_buf[i] = SLPW_buf[i + 40];
    }
    for (int i = 24; i < SLPW_LEN; i++)
    {
        SLPW_buf[i] = abs(DATA->ADC_DATA_CH[0].Data[i - 24]);
    }
}
//imu
void ActiveSection::imu_MK_slpw()
{
    for (int i = 0; i < 2; i++)
    {
        imu_SLPW_buf[i] = imu_SLPW_buf[i + 4];
    }
    for (int i = 2; i < 6; i++)
    {
        imu_SLPW_buf[i] = sqrt(DATA->IMU_DATA_CH[0].ACC_x[i-2]*DATA->IMU_DATA_CH[0].ACC_x[i-2]+DATA->IMU_DATA_CH[0].ACC_y[i-2]*DATA->IMU_DATA_CH[0].ACC_y[i-2]+\
                DATA->IMU_DATA_CH[0].ACC_z[i-2]*DATA->IMU_DATA_CH[0].ACC_z[i-2]);
    }
}
float ActiveSection::SampEn_Br(float* Data, int m, float r, int N)
{
    unsigned int count;
    float tmp = 0;
    for (int i = 0; i <= N - m; i++)
    {
        count = 0;
        for (int j = 0; j <= N - m; j++) {
            if (j != i)
            {
                for (int k = 0; k <= m - 1; k++)
                {
                    if (abs(Data[i + k] - Data[j + k]) > r) break;
                    if (k == (m - 1)) count += 1;
                }
            }
        }
        tmp += (float)count / (float)(N - m);
    }
    tmp = tmp / (N - m + 1);
    return tmp;
}
void ActiveSection::SampEnCal()
{
    float Br, Br_1;
    Br = SampEn_Br(SLPW_buf, SAMPEN_m, SAMPEN_r, SLPW_LEN);
    Br_1 = SampEn_Br(SLPW_buf, SAMPEN_m + 1, SAMPEN_r, SLPW_LEN);
    SampEn_num = log(Br / Br_1);
    if (SampEn_num > 2.5) SampEn_num = 2.5;
}
//imu
void ActiveSection::imu_SampEnCal()
{
    float Br, Br_1;
    Br = SampEn_Br(imu_SLPW_buf, 1, 0.05, 6);
    Br_1 = SampEn_Br(imu_SLPW_buf, 2, 0.05, 6);
    imu_SampEn_num = log(Br / Br_1);
    if (imu_SampEn_num > 2.5) imu_SampEn_num = 2.5;
}
//检测活动段跳跃起点
bool ActiveSection::check_action()
{
    static int count = 0;
    if (this->SampEn_num >= ACT_LIM_S)
    {
        count++;
        if (count >= 5)
        {
            if (check_action_IMU())
            {
                //flag = true;//进入活动段
                this->jump_flag = true;
                this->begin = this->DATA->point - 199;
                count = 0;
                return true;
            }
            else
                count--;

        }
    }
    else
    {
        count = 0;
        return false;
    }
    return false;
}
//检测活动段起点(检测所有的活动段)
bool ActiveSection::check_action_2()
{
    static int count = 0;
    if (this->SampEn_num >= ACT_LIM_S)
    {
        count++;
        if (count >= 5)
        {
            flag = true;//进入活动段
            this->ACT_BEGIN = this->DATA->point - 199;
            count = 0;
            return true;
        }
    }
    else
    {
        count = 0;
        return false;
    }
    return false;
}

//检测IMU数据的活动段
bool ActiveSection::check_action_IMU()
{
    int count_num = 0;
    for (int i = 0; i < IMU_CH; i++)
    {
        if(!(this->DATA->IMU_DATA_CH[0].ACC_x[i]-ACC_LOW>0 && this->DATA->IMU_DATA_CH[0].ACC_x[i]-ACC_HIGH<0))
        {
            count_num++;
            if(count_num>=3)//有3个不在区间内
            {
                return true;
            }
        }
    }
    return false;
}

bool ActiveSection::check_still() //可以由imu和熵同时判断，达到一个条件就离开活动段
{
    static int count = 0;
    static int acc_count=0;
    static bool adc_flag = false;
    static bool imu_flag = false;
    //条件一 熵 连续5个数据包的熵 小于阈值
    if (this->SampEn_num <= ACT_LIM_E)
    {
        count++;
        if (count >= 5)
        {
            count = 0;
            adc_flag = true;
        }
    }
    else
    {
        count = 0;
        adc_flag = false;
    }
    //条件2 imu——acc 连续5个数据包的acc 大于0.985且小于1.02
    if(check_still_IMU())
    {
        acc_count++;
        if(acc_count>= 5)//连续5个数据包的acc都大于0.98 小于1.02（表示站立状态）
        {
            acc_count = 0;
            imu_flag = true;
        }
    }
    else
    {
        acc_count = 0;
        imu_flag = false;
    }
    if(imu_flag)//只以IMU为标准
    {
        jump_flag = false;
        count = 0;
        acc_count = 0;
        imu_flag = false;
        adc_flag = false;
        this->end = this->DATA->point - 199;
        return true;
    }
    return false;
}
//检测活动段终点
bool ActiveSection::check_still_2()
{
    static int count = 0;
    if (this->SampEn_num <= ACT_LIM_E)
    {
        count++;
        if (count >= 5)
        {
            flag = false;
            count = 0;
            this->ACT_END = this->DATA->point - 199;
            return true;
        }
    }
    else
    {
        count = 0;
    }
    return false;
}
bool ActiveSection::check_still_IMU()
{
    for (int i = 0; i < 4; i++)
    {
        if (!(this->DATA->IMU_DATA_CH[0].ACC_x[i] -ACC_LOW >= 0 && ACC_HIGH-this->DATA->IMU_DATA_CH[0].ACC_x[i] >= 0))
            return false;
    }
    return true;
}
//检测活动段(检测所有的活动段)
int ActiveSection::check_2()
{
    this->MK_slpw();
    this->SampEnCal();
    out_2 << (this->DATA->point)-39 << "," << this->SampEn_num << Qt::endl;
    if(flag)
    {
        //进入活动段，检测结束点
        //结束时返回节点，没结束返回1
        if(this->check_still_2())
        {
            this->ACT_STATUS = false;
            return this->ACT_END;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(this->check_action_2())
        {
            this->ACT_STATUS = true;
            return this->ACT_BEGIN;
        }
        else
        {
            return 0;
        }
    }
}
//检测活动段中跳跃起点
int ActiveSection::check()
{
    this->MK_slpw();
    this->SampEnCal();
    out << (this->DATA->point)-39 << "," << this->SampEn_num << Qt::endl;
    if (jump_flag)
    {   //进入活动段，检测活动段中跳跃结束点
        //结束时返回节点，没结束返回1
        if (this->check_still())
        {
            this->status = false; //离开活动段，返回结束点
            return this->end;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        //检测 活动段中跳跃起点
        //没进入活动段 返回0
        if (this->check_action())
        {
            this->status = true;    //进入活动段，返回起点
            return this->begin;
        }
        else
        {
            return 0;
        }
    }
}

void ActiveSection::imu_check()
{
    this->imu_MK_slpw();
    this->imu_SampEnCal();
    imu_out << (this->DATA->point)-39 << "," << this->imu_SampEn_num << Qt::endl;
}

//初始化所有的文件
static bool init_file(QFile* IMU, QFile* ADC,QString IMU_path,QString ADC_path)
{
    IMU->setFileName(IMU_path);
    if (!IMU->open(QIODevice::ReadOnly))
    {
        return false;
    }
    ADC->setFileName(ADC_path);
    if (!ADC->open(QIODevice::ReadOnly))
    {
        return false;
    }
    return true;
}

//关闭文件
static void file_close(QFile* IMU, QFile* ADC)
{
    IMU->close();
    ADC->close();
    delete IMU;
    delete ADC;
    IMU = nullptr;
    ADC = nullptr;
}

//sEMG转到float
static void str_to_ADC(QString str,float *data)
{
    auto temp_list = str.split(',');
    data[0] = temp_list.at(0).toInt();
    for (int i = 1; i < temp_list.length(); ++i)
    {
        data[i] = temp_list.at(i).toFloat();
    }
}

//文件记录IMU转到float
static void str_to_IMU(QString str, float data[8][12])
{
    auto linelist = str.split(',');
    for (int i = 0; i < linelist.length() - 1; ++i)
    {
        data[i / 12][i % 12] = linelist.at(i + 1).toFloat();
    }
}
//文件指针 跳跃指定间隔
static bool jump_gap(int time,QTextStream &adc,QTextStream &imu)
{
    for(int i=0;i<time*1000;i++)
    {
        if (adc.atEnd())
        {
            return true;
        }
        adc.readLine();
    }
    for(int i=0;i<time*100;i++)
    {
        if (imu.atEnd())
        {
            return true;
        }
        imu.readLine();
    }
    return false;
}

//检测跳跃活动段
int ActiveSectionDetection(ACT_point &ACT_ans,QString path_adc, QString path_imu, QString path_samp,int time_gap,float high,float low)
{
    ActiveSection ACT(path_samp);
    ACT.ACC_HIGH = high;
    ACT.ACC_LOW = low;
    ACT.open_file_samp();
    QFile* IMU = new QFile;
    QFile* ADC = new QFile;
    QString IMU_path = path_imu;
    QString ADC_path = path_adc;
    float temp_ADC_data[9] = { 0 };
    float temp_IMU_data[8][12];
    memset(temp_IMU_data,0,sizeof(temp_IMU_data));
    memset(&ACT_ans, 0, sizeof(ACT_point));
    if (!init_file(IMU, ADC, IMU_path, ADC_path))
    {
        return -1;
    }
    QTextStream adc_in(ADC);
    QTextStream imu_in(IMU);
    for (int i = 0; i < 1000; i++)
    {
        if (adc_in.atEnd())
        {
            exit(0);
        }
        adc_in.readLine();
    }
    for (int i = 0; i < 100; i++)
    {
        if (imu_in.atEnd())
        {
            exit(0);
        }
        imu_in.readLine();
    }//头一秒的数据舍弃（等待IMU稳定）
    while (!adc_in.atEnd())
    {
        static int adc_count = 0;
        QString temp_str = adc_in.readLine();
        str_to_ADC(temp_str, temp_ADC_data);
        adc_count++;
        //每10个sEMG数据，读取一次IMU数据
        if (adc_count >= 10)
        {
            if(imu_in.atEnd())
            {
                file_close(IMU, ADC);
                return 0;
            }
            QString imu_str = imu_in.readLine();
            str_to_IMU(imu_str, temp_IMU_data);
            ACT.IMU_append(temp_IMU_data);
            adc_count = 0;
        }
        ACT.ADC_append(temp_ADC_data);
        if (ACT.data_full())
        {
            int point = ACT.check();
            if (point != 0 && point != 1)
            {
                if (ACT.status)
                {
                    ACT_ans.begin[ACT_ans.begin_count] = point;
                    ACT_ans.begin_count++;
                }
                else
                {
                    ACT_ans.end[ACT_ans.end_count] = point;
                    ACT_ans.end_count++;//到达活动熵结束点
                    //跳过两次跳跃间隔时间（中间有其他动作）
                    if(time_gap!=0)
                    {
                        if(!jump_gap(time_gap,adc_in,imu_in))
                        {
                               //重新计算样本熵，清楚样本熵窗口
                            ACT.clear_slpw();
                        }
                        else
                            return 0;
                    }
                }
            }
        }
    }
    file_close(IMU, ADC);
    return 0;
}

int AllActiveSectionDetection(ACT_point &ACT_ans,QString path_adc, QString path_imu, QString path_samp,float high,float low)
{
    ActiveSection ACT(path_samp);
    ACT.ACC_HIGH = high;
    ACT.ACC_LOW = low;
    ACT.open_file_all_samp();
    ACT.open_file_imu_samp();
    QFile* IMU = new QFile;
    QFile* ADC = new QFile;
    QString IMU_path = path_imu;
    QString ADC_path = path_adc;
    float temp_ADC_data[9] = { 0 };
    float temp_IMU_data[8][12];
    memset(temp_IMU_data,0,sizeof(temp_IMU_data));
    memset(&ACT_ans, 0, sizeof(ACT_point));
    if (!init_file(IMU, ADC, IMU_path, ADC_path))
    {
        return -1;
    }
    QTextStream adc_in(ADC);
    QTextStream imu_in(IMU);
    for (int i = 0; i < 1000; i++)
    {
        if (adc_in.atEnd())
        {
            exit(0);
        }
        adc_in.readLine();
    }
    for (int i = 0; i < 100; i++)
    {
        if (imu_in.atEnd())
        {
            exit(0);
        }
        imu_in.readLine();
    }//头一秒的数据舍弃（等待IMU稳定）
    while (!adc_in.atEnd())
    {
        static int adc_count = 0;
        QString temp_str = adc_in.readLine();
        str_to_ADC(temp_str, temp_ADC_data);
        adc_count++;
        if (adc_count >= 10)
        {
            if(imu_in.atEnd())
            {
                file_close(IMU, ADC);
                return 0;
            }
            QString imu_str = imu_in.readLine();
            str_to_IMU(imu_str, temp_IMU_data);
            ACT.IMU_append(temp_IMU_data);
            adc_count = 0;
        }
        ACT.ADC_append(temp_ADC_data);
        if (ACT.data_full())
        {
            //imu
            ACT.imu_check();
            int point = ACT.check_2();
            if (point != 0 && point != 1)
            {
                if (ACT.ACT_STATUS)
                {
                    ACT_ans.ACT_begin[ACT_ans.ACT_begin_count] = point;
                    ACT_ans.ACT_begin_count++;
                }
                else
                {
                    ACT_ans.ACT_end[ACT_ans.ACT_end_count] = point;
                    ACT_ans.ACT_end_count++;//到达活动熵结束点
                }
            }
        }
    }
    file_close(IMU, ADC);
    return 0;
}


