#include "Dataset.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include "Configure.h"
Dataset::Dataset()
{

}
Dataset::Dataset(QString adc_path, QString imu_path,QString EMGfeaturepath,QString IMUfeaturepath,QString actionLabelpath,QString gaitpath){
    this->ACTD = new ACT_DATA;
    memset(this->ACTD, 0, sizeof(ACT_DATA));
    this->EMGfeapath = EMGfeaturepath;
    this->IMUfeapath = IMUfeaturepath;
    this->gaitfeapath = gaitpath;

    //对步态分离记录
    QString emgpre = EMGfeapath.split('.').at(1);
    QString IMUpre = IMUfeapath.split('.').at(1);
    QString gaitpre = gaitfeapath.split('.').at(1);
    for(int i=0;i<5;++i){
        QString Epath = QString(".%1gait%2.csv").arg(emgpre).arg(i);
        QString Ipath = QString(".%1gait%2.csv").arg(IMUpre).arg(i);

        QString Epathgaussian = QString(".%1gaitgaussian%2.csv").arg(emgpre).arg(i);
        QString Ipathgaussian = QString(".%1gaitgaussian%2.csv").arg(IMUpre).arg(i);

        QString Epathtimescal0_9 = QString(".%1gaittimescal09%2.csv").arg(emgpre).arg(i);
        QString Ipathtimescal0_9 = QString(".%1gaittimescal09%2.csv").arg(IMUpre).arg(i);

        QString Epathtimescal1_1 = QString(".%1gaittimescal11%2.csv").arg(emgpre).arg(i);
        QString Ipathtimescal1_1 = QString(".%1gaittimescal11%2.csv").arg(IMUpre).arg(i);

        QString Gpath = QString(".%1gait%2.csv").arg(gaitpre).arg(i);



        //原始数据
        EMGfea[i].setFileName(Epath);
        if (!EMGfea[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        EMGout[i].setDevice(&EMGfea[i]);

        IMUfea[i].setFileName(Ipath);
        if (!IMUfea[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        IMUout[i].setDevice(&IMUfea[i]);
        //数据增强
        //高斯噪声
        EMGfeagaussian[i].setFileName(Epathgaussian);
        if (!EMGfeagaussian[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        EMGoutgaussian[i].setDevice(&EMGfeagaussian[i]);

        IMUfeagaussian[i].setFileName(Ipathgaussian);
        if (!IMUfeagaussian[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        IMUoutgaussian[i].setDevice(&IMUfeagaussian[i]);
        //缩放*0.9
        EMGfea09[i].setFileName(Epathtimescal0_9);
        if (!EMGfea09[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        EMGouttimescal0_9[i].setDevice(&EMGfea09[i]);

        IMUfea09[i].setFileName(Ipathtimescal0_9);
        if (!IMUfea09[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        IMUouttimescal0_9[i].setDevice(&IMUfea09[i]);

        //缩放*1.1
        EMGfea11[i].setFileName(Epathtimescal1_1);
        if (!EMGfea11[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        EMGouttimescal1_1[i].setDevice(&EMGfea11[i]);

        IMUfea11[i].setFileName(Ipathtimescal1_1);
        if (!IMUfea11[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        IMUouttimescal1_1[i].setDevice(&IMUfea11[i]);




        gaitfea[i].setFileName(Gpath);
        if (!gaitfea[i].open(QIODevice::Append))
        {
            QMessageBox::warning(NULL, u8"文件问题", u8"数据库dataset保存文件出现问题");
        }
        gaitout[i].setDevice(&gaitfea[i]);
    }
    ADC = new QFile;
    ADC->setFileName(adc_path);
    if (!ADC->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, u8"file problem", u8"ADC file problem");
        exit(-1);
    }
    this->ADC_in.setDevice(ADC);

    IMU = new QFile;
    IMU->setFileName(imu_path);
    if (!IMU->open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL, u8"file problem", u8"IMU file problem");
        exit(-1);
    }
    this->IMU_in.setDevice(IMU);

    EMGfeature = new QFile;
    EMGfeature->setFileName(EMGfeaturepath);
    if (!EMGfeature->open(QIODevice::Append))
    {
        QMessageBox::warning(NULL, u8"文件问题", u8"数据库保存文件出现问题");
    }
    EMGfeature_out.setDevice(EMGfeature);

    IMUfeature = new QFile;
    IMUfeature->setFileName(IMUfeaturepath);
    if (!IMUfeature->open(QIODevice::Append))
    {
        QMessageBox::warning(NULL, u8"文件问题", u8"数据库保存文件出现问题");
    }
    IMUfeature_out.setDevice(IMUfeature);

    gait = new QFile;
    gait->setFileName(gaitpath);
    if (!gait->open(QIODevice::Append))
    {
        QMessageBox::warning(NULL, u8"文件问题", u8"数据库保存文件出现问题");
    }
    gait_out.setDevice(gait);

    action = new QFile;
    action->setFileName(actionLabelpath);
    if (!action->open(QIODevice::Append))
    {
        QMessageBox::warning(NULL, u8"文件问题", u8"数据库保存文件出现问题");
    }
    action_out.setDevice(action);

}
Dataset::~Dataset()
{
    ADC->close();
    IMU->close();
    EMGfeature->close();
    IMUfeature->close();
    gait->close();
    action->close();
    for(int i=0;i<5;++i){
        EMGfea[i].close();
        IMUfea[i].close();
        gaitfea[i].close();

        EMGfea09[i].close();
        IMUfea09[i].close();
        EMGfeagaussian[i].close();
        IMUfeagaussian[i].close();
        EMGfea11[i].close();
        IMUfea11[i].close();
    }
    delete ADC;
    ADC = nullptr;
    delete IMU;
    IMU = nullptr;
    delete EMGfeature;
    EMGfeature = nullptr;
    delete IMUfeature;
    IMUfeature = nullptr;
    delete gait;
    gait = nullptr;
    delete action;
    action = nullptr;
}
int Dataset::file_skip(int point1, int point2)
{
    int Tnow_emg = 0;
    while(Tnow_emg != point1 || Tnow_emg < point1)
    {
        if (ADC_in.atEnd())
        {
            QMessageBox::warning(NULL, u8"file problem", u8"ADC file don't have enough sample");
            return -1;
        }
        QString line = ADC_in.readLine();
        auto ll = line.split(',');
        Tnow_emg = ll.at(0).toInt();
    }
    for (int i = 0; i < point2; i++)//因为如果从adc41点开始，那么imu就是41/10=4，第5个（50）开始
    {
        if (IMU_in.atEnd())
        {
            QMessageBox::warning(NULL, u8"file problem", u8"IMU file don't have enough sample data");
            return -1;
        }
        IMU_in.readLine();
    }
    return 0;
}
void Dataset::init_file()
{
    ADC_in.seek(0);
    IMU_in.seek(0);
}
void Dataset::move_forward()
{
    for (int ch = 0; ch < 8; ++ch)
    {
        for (int i = 0; i < ACTD->adc_length - STEP; ++i)
        {
            ACTD->ADC_DATA_CH[ch].Data[i] = ACTD->ADC_DATA_CH[ch].Data[i + STEP];
        }
    }
    ACTD->adc_length -= STEP;
    int STEP2 = STEP / 10;
    for (int _ch = 0; _ch < 7; _ch++)
    {
        for (int t = 0; t < ACTD->IMU_length - STEP2; ++t)
        {
            ACTD->IMU_DATA_CH[_ch].ACC_x[t] = ACTD->IMU_DATA_CH[_ch].ACC_x[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].ACC_y[t] = ACTD->IMU_DATA_CH[_ch].ACC_y[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].ACC_z[t] = ACTD->IMU_DATA_CH[_ch].ACC_z[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].GYRO_x[t] = ACTD->IMU_DATA_CH[_ch].GYRO_x[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].GYRO_y[t] = ACTD->IMU_DATA_CH[_ch].GYRO_y[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].GYRO_z[t] = ACTD->IMU_DATA_CH[_ch].GYRO_z[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Pitch[t] = ACTD->IMU_DATA_CH[_ch].Pitch[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Q_0[t] = ACTD->IMU_DATA_CH[_ch].Q_0[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Q_1[t] = ACTD->IMU_DATA_CH[_ch].Q_1[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Q_2[t] = ACTD->IMU_DATA_CH[_ch].Q_2[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Q_3[t] = ACTD->IMU_DATA_CH[_ch].Q_3[t + STEP2];
            ACTD->IMU_DATA_CH[_ch].Yaw[t] = ACTD->IMU_DATA_CH[_ch].Yaw[t + STEP2];
        }
    }
    ACTD->IMU_length -= STEP2;
}
void Dataset::clear_ACTD()
{
    this->ACTD->adc_length = 0;
    this->ACTD->IMU_length = 0;
}
void Dataset::write_feature(int gait, int actionlabel){

    //高斯噪声
    static double mean = 0.0;
    static double stddv = 0.1;
    static std::default_random_engine generator;
    static std::normal_distribution<double> dist(mean, stddv);
    //分开记录
    gaitout[gait]<<gait<<Qt::endl;
    //记录gaitlabel
    gait_out<<gait<<Qt::endl;
    //记录action
    action_out<<actionlabel<<Qt::endl;
    //记录EMG
    for(int i = 0; i < ACTD->adc_length; ++i){
        for(int ch=0; ch<7;++ch){
            float ori = ACTD->ADC_DATA_CH[ch].Data[i];
            EMGfeature_out<<ACTD->ADC_DATA_CH[ch].Data[i]<<',';
            EMGout[gait]<<ACTD->ADC_DATA_CH[ch].Data[i]<<',';
            //数据增强
            EMGoutgaussian[gait]<<(ori+dist(generator))<<',';
            EMGouttimescal0_9[gait]<<0.9*ori<<',';
            EMGouttimescal1_1[gait]<<1.1*ori<<',';
        }
        float ori = ACTD->ADC_DATA_CH[7].Data[i];
        EMGfeature_out<<ACTD->ADC_DATA_CH[7].Data[i]<<Qt::endl;
        EMGout[gait]<<ACTD->ADC_DATA_CH[7].Data[i]<<Qt::endl;
        EMGoutgaussian[gait]<<(ori+dist(generator))<<Qt::endl;
        EMGouttimescal0_9[gait]<<0.9*ori<<Qt::endl;
        EMGouttimescal1_1[gait]<<1.1*ori<<Qt::endl;
    }
    //记录IMU
    for (int t = 0; t < ACTD->IMU_length; ++t){
        for(int _ch=0;_ch<6;_ch++){


            float ax = ACTD->IMU_DATA_CH[_ch].ACC_x[t];
            float ay = ACTD->IMU_DATA_CH[_ch].ACC_y[t];
            float az = ACTD->IMU_DATA_CH[_ch].ACC_z[t];
            float accx = ACTD->IMU_DATA_CH[_ch].GYRO_x[t];
            float accy = ACTD->IMU_DATA_CH[_ch].GYRO_y[t];
            float accz = ACTD->IMU_DATA_CH[_ch].GYRO_z[t];
            float anglex = ACTD->IMU_DATA_CH[_ch].Q_3[t];
            float angley = ACTD->IMU_DATA_CH[_ch].Pitch[t];
            float anglez = ACTD->IMU_DATA_CH[_ch].Yaw[t];
            float temp = ax*ax+ay*ay+az*az;

            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].ACC_x[t]<<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].ACC_y[t]<<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].ACC_z[t]<<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].GYRO_x[t] <<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].GYRO_y[t] <<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].GYRO_z[t] <<',';
            //IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Pitch[t] <<',';
            //IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Q_0[t] <<',';
            //IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Q_1[t] <<',';
            //IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Q_2[t] <<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Q_3[t] <<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Pitch[t] <<',';
            IMUfeature_out<<ACTD->IMU_DATA_CH[_ch].Yaw[t] <<',';
            IMUfeature_out<<temp<<',';


            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].ACC_x[t]<<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].ACC_y[t]<<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].ACC_z[t]<<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].GYRO_x[t] <<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].GYRO_y[t] <<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].GYRO_z[t] <<',';
            //IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Pitch[t] <<',';
            //IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Q_0[t] <<',';
            //IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Q_1[t] <<',';
            //IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Q_2[t] <<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Q_3[t] <<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Pitch[t] <<',';
            IMUout[gait]<<ACTD->IMU_DATA_CH[_ch].Yaw[t] <<',';
            IMUout[gait]<<temp<<',';


            //高斯
            IMUoutgaussian[gait]<<ax+dist(generator)<<',';
            IMUoutgaussian[gait]<<ay+dist(generator)<<',';
            IMUoutgaussian[gait]<<az+dist(generator)<<',';
            IMUoutgaussian[gait]<<accx+dist(generator)<<',';
            IMUoutgaussian[gait]<<accy+dist(generator)<<',';
            IMUoutgaussian[gait]<<accz+dist(generator)<<',';
            IMUoutgaussian[gait]<<anglex+dist(generator)<<',';
            IMUoutgaussian[gait]<<angley+dist(generator)<<',';
            IMUoutgaussian[gait]<<anglez+dist(generator)<<',';
            IMUoutgaussian[gait]<<temp+dist(generator)<<',';
            //0.9
            IMUouttimescal0_9[gait]<<ax*0.9<<',';
            IMUouttimescal0_9[gait]<<ay*0.9<<',';
            IMUouttimescal0_9[gait]<<az*0.9<<',';
            IMUouttimescal0_9[gait]<<accx*0.9<<',';
            IMUouttimescal0_9[gait]<<accy*0.9<<',';
            IMUouttimescal0_9[gait]<<accz*0.9<<',';
            IMUouttimescal0_9[gait]<<anglex*0.9<<',';
            IMUouttimescal0_9[gait]<<angley*0.9<<',';
            IMUouttimescal0_9[gait]<<anglez*0.9<<',';
            IMUouttimescal0_9[gait]<<temp*0.9<<',';
            //1.1
            IMUouttimescal1_1[gait]<<ax*1.1<<',';
            IMUouttimescal1_1[gait]<<ay*1.1<<',';
            IMUouttimescal1_1[gait]<<az*1.1<<',';
            IMUouttimescal1_1[gait]<<accx*1.1<<',';
            IMUouttimescal1_1[gait]<<accy*1.1<<',';
            IMUouttimescal1_1[gait]<<accz*1.1<<',';
            IMUouttimescal1_1[gait]<<anglex*1.1<<',';
            IMUouttimescal1_1[gait]<<angley*1.1<<',';
            IMUouttimescal1_1[gait]<<anglez*1.1<<',';
            IMUouttimescal1_1[gait]<<temp*1.1<<',';

        }

        float ax = ACTD->IMU_DATA_CH[6].ACC_x[t];
        float ay = ACTD->IMU_DATA_CH[6].ACC_y[t];
        float az = ACTD->IMU_DATA_CH[6].ACC_z[t];
        float accx = ACTD->IMU_DATA_CH[6].GYRO_x[t];
        float accy = ACTD->IMU_DATA_CH[6].GYRO_y[t];
        float accz = ACTD->IMU_DATA_CH[6].GYRO_z[t];
        float anglex = ACTD->IMU_DATA_CH[6].Q_3[t];
        float angley = ACTD->IMU_DATA_CH[6].Pitch[t];
        float anglez = ACTD->IMU_DATA_CH[6].Yaw[t];
        float temp = ax*ax+ay*ay+az*az;

        IMUfeature_out<<ACTD->IMU_DATA_CH[6].ACC_x[t]<<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].ACC_y[t]<<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].ACC_z[t]<<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].GYRO_x[t] <<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].GYRO_y[t] <<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].GYRO_z[t] <<',';
        //IMUfeature_out<<ACTD->IMU_DATA_CH[6].Pitch[t] <<',';
        //IMUfeature_out<<ACTD->IMU_DATA_CH[6].Q_0[t] <<',';
        //IMUfeature_out<<ACTD->IMU_DATA_CH[6].Q_1[t] <<',';
        //IMUfeature_out<<ACTD->IMU_DATA_CH[6].Q_2[t] <<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].Q_3[t] <<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].Pitch[t] <<',';
        IMUfeature_out<<ACTD->IMU_DATA_CH[6].Yaw[t] <<',';
        IMUfeature_out<<temp<<Qt::endl;

        IMUout[gait]<<ACTD->IMU_DATA_CH[6].ACC_x[t]<<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].ACC_y[t]<<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].ACC_z[t]<<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].GYRO_x[t] <<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].GYRO_y[t] <<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].GYRO_z[t] <<',';
        //IMUout[gait]<<ACTD->IMU_DATA_CH[6].Pitch[t] <<',';
        //IMUout[gait]<<ACTD->IMU_DATA_CH[6].Q_0[t] <<',';
        //IMUout[gait]<<ACTD->IMU_DATA_CH[6].Q_1[t] <<',';
        //IMUout[gait]<<ACTD->IMU_DATA_CH[6].Q_2[t] <<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].Q_3[t] <<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].Pitch[t] <<',';
        IMUout[gait]<<ACTD->IMU_DATA_CH[6].Yaw[t] <<',';
        IMUout[gait]<<temp<<Qt::endl;

        //高斯
        IMUoutgaussian[gait]<<ax+dist(generator)<<',';
        IMUoutgaussian[gait]<<ay+dist(generator)<<',';
        IMUoutgaussian[gait]<<az+dist(generator)<<',';
        IMUoutgaussian[gait]<<accx+dist(generator)<<',';
        IMUoutgaussian[gait]<<accy+dist(generator)<<',';
        IMUoutgaussian[gait]<<accz+dist(generator)<<',';
        IMUoutgaussian[gait]<<anglex+dist(generator)<<',';
        IMUoutgaussian[gait]<<angley+dist(generator)<<',';
        IMUoutgaussian[gait]<<anglez+dist(generator)<<',';
        IMUoutgaussian[gait]<<temp+dist(generator)<<Qt::endl;
        //0.9
        IMUouttimescal0_9[gait]<<ax*0.9<<',';
        IMUouttimescal0_9[gait]<<ay*0.9<<',';
        IMUouttimescal0_9[gait]<<az*0.9<<',';
        IMUouttimescal0_9[gait]<<accx*0.9<<',';
        IMUouttimescal0_9[gait]<<accy*0.9<<',';
        IMUouttimescal0_9[gait]<<accz*0.9<<',';
        IMUouttimescal0_9[gait]<<anglex*0.9<<',';
        IMUouttimescal0_9[gait]<<angley*0.9<<',';
        IMUouttimescal0_9[gait]<<anglez*0.9<<',';
        IMUouttimescal0_9[gait]<<temp*0.9<<Qt::endl;
        //1.1
        IMUouttimescal1_1[gait]<<ax*1.1<<',';
        IMUouttimescal1_1[gait]<<ay*1.1<<',';
        IMUouttimescal1_1[gait]<<az*1.1<<',';
        IMUouttimescal1_1[gait]<<accx*1.1<<',';
        IMUouttimescal1_1[gait]<<accy*1.1<<',';
        IMUouttimescal1_1[gait]<<accz*1.1<<',';
        IMUouttimescal1_1[gait]<<anglex*1.1<<',';
        IMUouttimescal1_1[gait]<<angley*1.1<<',';
        IMUouttimescal1_1[gait]<<anglez*1.1<<',';
        IMUouttimescal1_1[gait]<<temp*1.1<<Qt::endl;
    }
}
int Dataset::datasetcal(std::vector<std::vector<int>>& tag,int actionlabel){
    std::vector<std::vector<std::vector<int>>> tagproc;
    //将不连续的步态运动分割
    std::vector<std::vector<int>> temp;
    for(int i=0;i<tag.size();++i)
    {
        if(i>0 && tag[i][0] != tag[i-1][5]){
            tagproc.push_back(temp);
            temp.clear();
        }
        temp.push_back(tag[i]);
    }
    if(!temp.empty()){
        tagproc.push_back(temp);
        temp.clear();
    }
    //对分割好的标签制作数据集
    int currenttag[6];
    for(int i=0;i<tagproc.size();++i){
        this->init_file();
        this->clear_ACTD();
        int row = tagproc[i].size();
        int col = tagproc[i][0].size();

        //6个点 A B C D mid E
        int A = tagproc[i][0][0];
        int begin2 = A/10;
        int E = tagproc[i][row-1][5];
        int Tnow = A;
        this->file_skip(A,begin2);

        //进入第一个步态相位周期
        for(int j=0;j<6;++j)
        {
            currenttag[j] = tagproc[i][0][j];
        }
        int current_num = 0;
        while (Tnow<E && !ADC_in.atEnd()) {
            Tnow++;
            static int time = 0;
            QString adc_str = ADC_in.readLine();
            auto adc_list = adc_str.split(',');
            for (int ch = 1; ch < adc_list.length(); ++ch)
            {
                this->ACTD->ADC_DATA_CH[ch - 1].Data[this->ACTD->adc_length] = adc_list.at(ch).toFloat();
            }
            this->ACTD->adc_length++;
            time++;
            if (time >= 10){
                if (IMU_in.atEnd())
                {
                    this->clear_ACTD();
                    return -1;
                }
                float data[8][12];
                QString imu_str = IMU_in.readLine();
                auto imu_list = imu_str.split(',');
                for (int k = 0;k<imu_list.length()-1;k++)
                {
                    data[k / 12][k % 12] = imu_list.at(k + 1).toFloat();
                }
                for (int ch = 0; ch < 7; ++ch)
                {
                    this->ACTD->IMU_DATA_CH[ch].ACC_x[ACTD->IMU_length] = data[ch][0];
                    this->ACTD->IMU_DATA_CH[ch].ACC_y[ACTD->IMU_length] = data[ch][1];
                    this->ACTD->IMU_DATA_CH[ch].ACC_z[ACTD->IMU_length] = data[ch][2];

                    this->ACTD->IMU_DATA_CH[ch].GYRO_x[ACTD->IMU_length] = data[ch][3];
                    this->ACTD->IMU_DATA_CH[ch].GYRO_y[ACTD->IMU_length] = data[ch][4];
                    this->ACTD->IMU_DATA_CH[ch].GYRO_z[ACTD->IMU_length] = data[ch][5];

                    this->ACTD->IMU_DATA_CH[ch].Q_0[ACTD->IMU_length] = data[ch][6];
                    this->ACTD->IMU_DATA_CH[ch].Q_1[ACTD->IMU_length] = data[ch][7];
                    this->ACTD->IMU_DATA_CH[ch].Q_2[ACTD->IMU_length] = data[ch][8];
                    this->ACTD->IMU_DATA_CH[ch].Q_3[ACTD->IMU_length] = data[ch][9]; //x

                    this->ACTD->IMU_DATA_CH[ch].Pitch[ACTD->IMU_length] = data[ch][10]; //y
                    this->ACTD->IMU_DATA_CH[ch].Yaw[ACTD->IMU_length] = data[ch][11]; //z
                }
                ACTD->IMU_length++;
                time = 0;
            }
            if (ACTD->adc_length >= DATA_PACKAGE_MAX && ACTD->IMU_length >= DATA_PACKAGE_MAX /10)//构成一个数据包
            {
                //判断当前数组是否走完
                if(Tnow>tagproc[i][current_num][5] && current_num+1< row)
                {
                    //进入下一个步态相位周期
                    for(int k=0;k<6;++k)
                    {
                        currenttag[k] = tagproc[i][current_num+1][k];
                    }
                    current_num++;
                }
                int gaitlabel = 0;
                if(Tnow < currenttag[0]+database)
                    gaitlabel = 0;
                else if(Tnow < currenttag[1]+database)
                    gaitlabel = 1;
                else if(Tnow < currenttag[2]+database)
                    gaitlabel = 2;
                else if(Tnow < currenttag[3]+database)
                    gaitlabel = 3;
                else if(Tnow < currenttag[4]+database)
                    gaitlabel = 4;
                else if(Tnow < currenttag[5]+database)
                    gaitlabel = 0;
                else
                    gaitlabel = 0;
                this->write_feature(gaitlabel,actionlabel);
                this->move_forward();
            }
        }

    }

    return 0;
}
int Dataset::datasetUpDownHillcal(std::vector<std::vector<int>> &tag, int actionlabel){
    std::vector<std::vector<std::vector<int>>> tagproc;
    //将不连续的步态运动分割
    std::vector<std::vector<int>> temp;
    int mytempcount = 0;
    int countgait = 0;
    for(int i=0;i<tag.size();++i)
    {
        if((i>0 && tag[i][0] != tag[i-1][5]) || countgait>=5){
            tagproc.push_back(temp);
            ++mytempcount;
            temp.clear();
            countgait = 0;
        }
        temp.push_back(tag[i]);
        ++countgait;
    }
    if(!temp.empty()){
        tagproc.push_back(temp);
        mytempcount++;
        temp.clear();
    }
    qDebug()<<"制作数据集对步态每个5个做一个分割，分割了"<<mytempcount<<"个";
    //对分割好的标签制作数据集
    int currenttag[6];
    //一种思路，每隔几个步态周期，重新跳转文件

    for(int i=0;i<tagproc.size();++i){
        this->init_file();
        this->clear_ACTD();
        int row = tagproc[i].size();
        int col = tagproc[i][0].size();

        //6个点 A B C D mid E
        int A = tagproc[i][0][0];
        int begin2 = A/10;
        int E = tagproc[i][row-1][5];
        int Tnow = A;
        //*******************主要修改的是定位到开始点的位置**********************
        //this->file_skip(A,begin2);
        {
            int Tnow_emg = 0;
            while(Tnow_emg != A)
            {
                if(Tnow_emg > A)
                    break;
                if (ADC_in.atEnd())
                {
                    qDebug()<<"adc有问题"<<" A: "<<A;
                    qDebug()<<tagproc[i][0][0]<<" "<<tagproc[i][0][1]<<" "<<tagproc[i][0][2]<<" "<<tagproc[i][0][3]<<" "<<tagproc[i][0][4]<<" "<<tagproc[i][0][5]<<" ";
                    QMessageBox::warning(NULL, u8"file problem", u8"ADC file don't have enough sample");
                    return -1;
                }
                QString line = ADC_in.readLine();
                auto ll = line.split(',');
                Tnow_emg = ll.at(0).toInt();
            }
            int imu_tnow = 0;
            while(!IMU_in.atEnd() && (imu_tnow+10)<=A){
                auto  imustr = IMU_in.readLine().split(',');
                int Tnow_emg = imustr.at(0).toInt();
                imu_tnow = Tnow_emg;
            }
            if (IMU_in.atEnd())
            {
                QMessageBox::warning(NULL, u8"file problem", u8"IMU file don't have enough sample data");
                return -1;
            }
        }
        //*******************************************************************

        //进入第一个步态相位周期
        for(int j=0;j<6;++j)
        {
            currenttag[j] = tagproc[i][0][j];
        }
        int current_num = 0;
        while (Tnow<E && !ADC_in.atEnd()) {
            Tnow++;
            static int time = 0;
            QString adc_str = ADC_in.readLine();
            auto adc_list = adc_str.split(',');
            for (int ch = 1; ch < adc_list.length(); ++ch)
            {
                this->ACTD->ADC_DATA_CH[ch - 1].Data[this->ACTD->adc_length] = adc_list.at(ch).toFloat();
            }
            this->ACTD->adc_length++;
            time++;
            if (time >= 10){
                if (IMU_in.atEnd())
                {
                    this->clear_ACTD();
                    return -1;
                }
                float data[8][12];
                QString imu_str = IMU_in.readLine();
                auto imu_list = imu_str.split(',');
                for (int k = 0;k<imu_list.length()-1;k++)
                {
                    data[k / 12][k % 12] = imu_list.at(k + 1).toFloat();
                }
                for (int ch = 0; ch < 7; ++ch)
                {
                    this->ACTD->IMU_DATA_CH[ch].ACC_x[ACTD->IMU_length] = data[ch][0];
                    this->ACTD->IMU_DATA_CH[ch].ACC_y[ACTD->IMU_length] = data[ch][1];
                    this->ACTD->IMU_DATA_CH[ch].ACC_z[ACTD->IMU_length] = data[ch][2];

                    this->ACTD->IMU_DATA_CH[ch].GYRO_x[ACTD->IMU_length] = data[ch][3];
                    this->ACTD->IMU_DATA_CH[ch].GYRO_y[ACTD->IMU_length] = data[ch][4];
                    this->ACTD->IMU_DATA_CH[ch].GYRO_z[ACTD->IMU_length] = data[ch][5];

                    this->ACTD->IMU_DATA_CH[ch].Q_0[ACTD->IMU_length] = data[ch][6];
                    this->ACTD->IMU_DATA_CH[ch].Q_1[ACTD->IMU_length] = data[ch][7];
                    this->ACTD->IMU_DATA_CH[ch].Q_2[ACTD->IMU_length] = data[ch][8];
                    this->ACTD->IMU_DATA_CH[ch].Q_3[ACTD->IMU_length] = data[ch][9];

                    this->ACTD->IMU_DATA_CH[ch].Pitch[ACTD->IMU_length] = data[ch][10];
                    this->ACTD->IMU_DATA_CH[ch].Yaw[ACTD->IMU_length] = data[ch][11];
                }
                ACTD->IMU_length++;
                time = 0;
            }
            if (ACTD->adc_length >= DATA_PACKAGE_MAX && ACTD->IMU_length >= DATA_PACKAGE_MAX /10)//构成一个数据包
            {
                //判断当前数组是否走完
                if(Tnow>tagproc[i][current_num][5] && current_num+1< row)
                {
                    //进入下一个步态相位周期
                    for(int k=0;k<6;++k)
                    {
                        currenttag[k] = tagproc[i][current_num+1][k];
                    }
                    current_num++;
                }
                int base = database;
                int gaitlabel = 0;
                if(Tnow < currenttag[0]+base)
                    gaitlabel = 0;
                else if(Tnow < currenttag[1]+base)
                    gaitlabel = 1;
                else if(Tnow < currenttag[2]+base)
                    gaitlabel = 2;
                else if(Tnow < currenttag[3]+base)
                    gaitlabel = 3;
                else if(Tnow < currenttag[4]+base)
                    gaitlabel = 4;
                else if(Tnow < currenttag[5]+base)
                    gaitlabel = 0;
                else
                    gaitlabel = 0;
                this->write_feature(gaitlabel,actionlabel);
                this->move_forward();
            }
        }

    }

    return 0;

}
