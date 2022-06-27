
//*******************************************************
//		BP神经网络类的设计---主要用于特征识别
//********************************************************

#pragma once

class BPNet
{
public:
	BPNet();
	~BPNet();
public:
	void Init(int BP_layer[]);	 //初始化BP神经网络
	bool BPForward(float *pVec); //BP神经网络前向传播算法
public:
	float BP_ret[6];			//BP网络输出结果
	int max_index;
private:
    //两个激活函数
	void activation_tanh(float* output, float* intput, int nodes);       //输出层 输入层 和节点个数
	void activation_softmax(float* output, float* intput, int nodes);
private:
	bool BP_Init_flag = false;  //判断网络是不是已经初始化了

	float *BN_1_out;
	float *dense_1_out, *dense_2_out,*dense_3_out;


};

