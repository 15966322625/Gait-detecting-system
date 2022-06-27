#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "BPNet.h"
#include "BP_DATA.h"
#include "dsp_RCGClass.h"
#include "config.h"

//BP神经网络

BPNet::BPNet()
{
}

//析构函数用于删除申请的内存
BPNet::~BPNet()
{
	if (BP_Init_flag) {
		delete[] BN_1_out;

		delete[] dense_1_out;
		delete[] dense_2_out;
	}
}


//初始化网络 申请保存输入层 隐含层 输出层的节点值的空间 并将网络初始化标志设置为真
void BPNet::Init(int bp_layer[4])   //初始化神经网络 隐藏层0 1 2 3
{
	BN_1_out = new float[bp_layer[0]];
	dense_1_out = new float[bp_layer[1]];
	dense_2_out = new float[bp_layer[2]];
	dense_3_out = new float[bp_layer[3]];

	BP_Init_flag = true;       //初始化

}

//BP神经网络前向传播算法
bool BPNet::BPForward(float * pVec)
{

	if (!BP_Init_flag) return false;

	memset(BN_1_out, 0, sizeof(float)*40);
	memset(dense_1_out, 0, sizeof(float)* 30);
	memset(dense_2_out, 0, sizeof(float)* 15);
	memset(dense_3_out, 0, sizeof(float)* 6);

//memset()函数原型是extern void *memset(void *buffer, int c, int count)  ；这个函数在socket中多用于清空数组 
//buffer：为指针或是数组,
//c：是赋给buffer的值,
//count：是buffer的长度.   

	BN_1->BatchNormalization(BN_1_out,pVec);

	Dense_1->BP_forword(dense_1_out,BN_1_out);//dense_1_out=w*x+b
	activation_tanh(dense_1_out,dense_1_out,Dense_1->output_nodes);//dense_1_out=tanh(dense_1_out)

	Dense_2->BP_forword(dense_2_out,dense_1_out);//dense_1_out=w*x+b
	activation_tanh(dense_2_out,dense_2_out,Dense_2->output_nodes);//dense_1_out=tanh(dense_1_out)


	Dense_output->BP_forword(dense_3_out,dense_2_out);
	activation_softmax(BP_ret,dense_3_out,Dense_output->output_nodes);

	int _nOutput = Dense_output->output_nodes;
	//	7. max index
	int Max_tmp = 0;
	for (int n = 1; n < _nOutput; n++) 
	{
		if (BP_ret[n] > BP_ret[Max_tmp])
		{
			Max_tmp = n;
		}
	}
	 max_index = Max_tmp;

	#ifdef ANS
	BP_off<<"BP:,"<<Max_tmp<<',';
	
	for(int i = 0;i<_nOutput;i++)
	{
		BP_off<<BP_ret[i]<<',';
	}
	BP_off<<std::endl;
	#endif

	return true;
}

void BPNet::activation_tanh(float* output, float* intput, int nodes)
{
	for(int i=0;i<nodes;++i)
	{
		output[i] = 2/(1+exp(-2*intput[i]))-1;
	}
}
void BPNet::activation_softmax(float* output, float* intput, int nodes)
{
	float sum = 0;
	for(int i=0;i<nodes;i++)
	{
		sum += exp(intput[i]);
	}
	for(int i=0;i<nodes;i++)
	{
		output[i] = exp(intput[i])/sum;
	}
}






