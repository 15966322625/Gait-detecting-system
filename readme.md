# Gait detecting system
基于Cpp11实现的步态检测人机交互系统设计
## 整体框架
- 整个项目的实现分为上位机Gra3_0和下位机EMG两部分。
- 上位机主要实现界面设计方面的工作，实现和实验人员的交互作用，效果展示等作用。
- 上位机主要会展示连接状态、识别时间以及各个步态的划分结果，还包括数据标注、模型移植等方面的按钮。
- 界面主要是在Qt环境下基于C++多线程高并发等功能实现的人机交互系统。

## 主要实现技术
- 函数的作用是什么
- 利用Epoll与线程池实现Reactor高并发模型
- 利用状态机与正则实现HTTP请求报文解析，可同时处理GET与POST请求
- 用vector容器实现了一个可自动扩容的缓冲区
- 基于epoll_wait实现定时功能，关闭超时的非活动连接，并用小根堆作为容器管理定时器
- 利用单例模式实现了一个简单的线程池，减少了线程创建与销毁的开销
- 利用单例模式实现连接MySQL的数据库连接池，减少数据库连接建立与关闭的开销，实现了用户注册登录功能
- 利用单例模式与阻塞队列实现异步日志系统，记录服务器运行状态
## 环境要求
- Linux
- C++11

##整体程序框架
### 上位机(upptercomputer)整体代码目录树
```
.
├── bin
│   └── uppercomputer 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── ActiveSection      活动段检测部分
│   ├── config       配置文件
│   ├── Dataset     数据集制作部分
│   ├── filterdata       数据滤波部分
│   ├── FootDetect        根据足底开关信号检测足部事件处理代码
│   ├── main.cpp     主函数
│   ├── IMUPitchDetect      IMU姿态传感器数据特殊点检测模块
│   ├── JumpFootDetect  跳跃动作滞空检测阶段
│   ├── mainwindow   主界面
│   ├── MyDataStruct  制作自己的数据集阶段
│   ├── mypushbutton  Qt里的按钮和槽
│   ├── MyThread  自定义线程，完成数据标注，制作数据集工作。
│   ├── Mytool  工具函数
│   └── tcpclient       自定义tcp连接类：上位机充当和下位机建立连接的上位机
├── log              日志目录
├── Makefile
├── README.md
└── resources        静态资源
```

### 下位机(lowercomputer)整体代码目录树
```
.
├── bin
│   └── lowercomputer 可执行文件
├── build
│   └── Makefile
├── code             源代码
│   ├── CMakeLists cmake文件 
│   ├── BP_DATA、BPNet、class_Dense 手写的BP网络模型结构  
│   ├── ADC_OL sEMG信号采集芯片AD7606初始化
│   ├── CAN_class      can通信类
│   ├── FootDetect        足底压力检测模块
│   ├── config.h     配置文件
│   ├── data_ADC_main sEMG信号采集进程
│   ├── data_Foot_main  足底开关信号采集进程
│   ├── IMU_data   IMU传感器初始化类   
│   ├── data_IMU_main  IMU信号采集进程  
│   ├── main_data 数据同步类  
│   ├── data_WIN  数据窗类
│   ├── dsp_ACT  判断活动段类
│   ├── dsp_RCGClass   调用行为模型类   
│   ├── filt_ADC  数据滤波  
│   ├── global 全局变量 定义结构体  
│   ├── main_dsp  处理数据 判断活动段
│   ├── dsp_RCG_main  处理数据，进行行为识别
│   ├── main_tcp 建立TCP\IP连接通信 下位机为服务端 上位机为客户端
│   └── main       程序入口
├── log              日志目录
├── Makefile
├── README.md
└── resources        静态资源
```
## 项目启动
需要先配置好数据库
```
//创建数据库
create database webdb;
//创建user表
USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
)ENGINE=InnoDB;
//添加数据
INSERT INTO user(username, passwd) VALUES('your name', 'your password');

//webdb是数据库名，user是表名，需要在main函数中传入
```
然后编译运行
```
make
./bin/simpleserver
```
浏览器访问
```
127.0.0.1:9006
#9006是在main函数中传入的服务器监听端口
```
## TODO
- config配置

## 致谢
Linux高性能服务器编程，游双著

[markparticle/WebServer](https://github.com/markparticle/WebServer)
