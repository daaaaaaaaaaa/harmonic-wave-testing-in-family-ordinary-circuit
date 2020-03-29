#include "sys.h"
#include "fft.h"

complex x[N],W[N];//分别为输入序列，变换核


void RaderReverse()
{
	u16 i,j,k;
	//第一个和最后一个数位置不变，故不处理
	for(i=1,j=N/2; i<N-1; ++i) 
	{
		//原始坐标小于变换坐标才交换，防止重复
		if(i<j)
		{
			complex temp = x[j];
			x[j] = x[i];
			x[i] = temp;
		}
		k = N/2; // 用于比较最高位
		while(k <= j)
		{ // 位判断为1
			j = j-k;// 该位变为0
			k = k/2;// 用于比较下一高位
		}
		j = j+k;// 判断为0的位变为1
	}
}
void fft()
{
	int i=0,j=0,k=0,l=0;
	complex up,down,product;
	RaderReverse();

	//w是蝶形系数
	for(int i=0;i<N;++i) 
	{
		W[i].real = cos((2*M_PI*i)/N);
		W[i].img = -sin((2*M_PI*i)/N);	
	}

	for(i=0; i<log(N)/log(2); ++i)  /*log(n)/log(2) 级蝶形运算 stage */  
	{
		l = 1<<i;
		for(j=0;j<N;j+= 2*l) /*一组蝶形运算 group,每组group的蝶形因子乘数不同*/ 
		{
			for(k=0;k<l;++k) /*一个蝶形运算 每个group内的蝶形运算的蝶形因子乘数成规律变化*/  
			{
				//product = x[j+k+l]*W[n*k/2/l]; //一次乘法
				mul(x[j+k+l], W[N*k/2/l], &product);
//				up   = x[j+k] + product; //一次加法，得蝶形变换上半部分输出
//				down = x[j+k] - product;//一次减法，得蝶形变换下半部分输出
				add(x[j+k], product, &up);
				sub(x[j+k], product, &down);
				x[j+k]   = up;//不占用新的空间，将结果输出储存在输入的位置
				x[j+k+l] = down;
			}
		}
	}
}

//寻找基波、高次谐波对应的点
double Amplitude[6], Phase[6];//基波幅值, 相位
volatile u16 j, m, k;//已知基波频率,采样频率，采样总点数
double I2,I1;
void Calculate_Wave()
{
	
	volatile double Max_Amplitude = 0.0;
	volatile double t =0.0;
	
//	m = j*256/4339+1;//计算基波频率对应的点
//	m = 0;
	//计算直流分量
	Amplitude[0] = (sqrt(x[0].real*x[0].real+x[0].img*x[0].img))/256;//基波对应的点
	Phase[0] = 180*atan2(x[0].img, x[0].real)/M_PI;
	
	for(k=1; k<52; k++)
	{
		t = sqrt(x[k].real*x[k].real +x[k].img*x[k].img)/128;
		if(t > Max_Amplitude)
		{
			Max_Amplitude = t;
			j = k;
		}
	}
	
	if(j != 0)
	{
	for(k=1; k<6; k++)
	{
	 m = k*j;//计算频率j对应的点，第一个点为基波频率
	 Amplitude[k] = (sqrt(x[m].real*x[m].real+x[m].img*x[m].img))/128;//高次谐波对应的点
	 Phase[k] = 180*atan2(x[m].img, x[m].real)/M_PI;
	}
    } 
	I2 = (Amplitude[0] + Amplitude[1]*0.7071)/R;
	I1 = I2 * 100;
}



void add(complex a,complex b,complex *c)  //复数加法计算
{
	c->real=a.real+b.real;
	c->img=a.img+b.img;
}


void mul(complex a,complex b,complex *c)  //复数乘法计算
{
	c->real=a.real*b.real - a.img*b.img;
	c->img=a.real*b.img + a.img*b.real;
}


void sub(complex a,complex b,complex *c)  //复数减法计算
{
	c->real=a.real-b.real;
	c->img=a.img-b.img;
}

