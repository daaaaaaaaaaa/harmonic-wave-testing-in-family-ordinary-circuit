#include "sys.h"
#include "fft.h"

complex x[N],W[N];//�ֱ�Ϊ�������У��任��


void RaderReverse()
{
	u16 i,j,k;
	//��һ�������һ����λ�ò��䣬�ʲ�����
	for(i=1,j=N/2; i<N-1; ++i) 
	{
		//ԭʼ����С�ڱ任����Ž�������ֹ�ظ�
		if(i<j)
		{
			complex temp = x[j];
			x[j] = x[i];
			x[i] = temp;
		}
		k = N/2; // ���ڱȽ����λ
		while(k <= j)
		{ // λ�ж�Ϊ1
			j = j-k;// ��λ��Ϊ0
			k = k/2;// ���ڱȽ���һ��λ
		}
		j = j+k;// �ж�Ϊ0��λ��Ϊ1
	}
}
void fft()
{
	int i=0,j=0,k=0,l=0;
	complex up,down,product;
	RaderReverse();

	//w�ǵ���ϵ��
	for(int i=0;i<N;++i) 
	{
		W[i].real = cos((2*M_PI*i)/N);
		W[i].img = -sin((2*M_PI*i)/N);	
	}

	for(i=0; i<log(N)/log(2); ++i)  /*log(n)/log(2) ���������� stage */  
	{
		l = 1<<i;
		for(j=0;j<N;j+= 2*l) /*һ��������� group,ÿ��group�ĵ������ӳ�����ͬ*/ 
		{
			for(k=0;k<l;++k) /*һ���������� ÿ��group�ڵĵ�������ĵ������ӳ����ɹ��ɱ仯*/  
			{
				//product = x[j+k+l]*W[n*k/2/l]; //һ�γ˷�
				mul(x[j+k+l], W[N*k/2/l], &product);
//				up   = x[j+k] + product; //һ�μӷ����õ��α任�ϰ벿�����
//				down = x[j+k] - product;//һ�μ������õ��α任�°벿�����
				add(x[j+k], product, &up);
				sub(x[j+k], product, &down);
				x[j+k]   = up;//��ռ���µĿռ䣬�������������������λ��
				x[j+k+l] = down;
			}
		}
	}
}

//Ѱ�һ������ߴ�г����Ӧ�ĵ�
double Amplitude[6], Phase[6];//������ֵ, ��λ
volatile u16 j, m, k;//��֪����Ƶ��,����Ƶ�ʣ������ܵ���
double I2,I1;
void Calculate_Wave()
{
	
	volatile double Max_Amplitude = 0.0;
	volatile double t =0.0;
	
//	m = j*256/4339+1;//�������Ƶ�ʶ�Ӧ�ĵ�
//	m = 0;
	//����ֱ������
	Amplitude[0] = (sqrt(x[0].real*x[0].real+x[0].img*x[0].img))/256;//������Ӧ�ĵ�
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
	 m = k*j;//����Ƶ��j��Ӧ�ĵ㣬��һ����Ϊ����Ƶ��
	 Amplitude[k] = (sqrt(x[m].real*x[m].real+x[m].img*x[m].img))/128;//�ߴ�г����Ӧ�ĵ�
	 Phase[k] = 180*atan2(x[m].img, x[m].real)/M_PI;
	}
    } 
	I2 = (Amplitude[0] + Amplitude[1]*0.7071)/R;
	I1 = I2 * 100;
}



void add(complex a,complex b,complex *c)  //�����ӷ�����
{
	c->real=a.real+b.real;
	c->img=a.img+b.img;
}


void mul(complex a,complex b,complex *c)  //�����˷�����
{
	c->real=a.real*b.real - a.img*b.img;
	c->img=a.real*b.img + a.img*b.real;
}


void sub(complex a,complex b,complex *c)  //������������
{
	c->real=a.real-b.real;
	c->img=a.img-b.img;
}

