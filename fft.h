#ifndef __fft_H
#define __fft_H
#include "math.h"

#define N 256
#define R 0.33333;

#define M_PI 3.14159265358979323846 //PI ˫����

typedef struct
{
  double real;//ʵ��
  double img;//�鲿
}complex;







void RaderReverse(void);
void fft(void);
void Calculate_Wave(void);
void add(complex a,complex b,complex *c);//�����ӷ�
void mul(complex a,complex b,complex *c);//�����˷�
void sub(complex a,complex b,complex *c);//��������	

#endif
