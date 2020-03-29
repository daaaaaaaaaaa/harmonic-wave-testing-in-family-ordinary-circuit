#ifndef __fft_H
#define __fft_H
#include "math.h"

#define N 256
#define R 0.33333;

#define M_PI 3.14159265358979323846 //PI 双精度

typedef struct
{
  double real;//实部
  double img;//虚部
}complex;







void RaderReverse(void);
void fft(void);
void Calculate_Wave(void);
void add(complex a,complex b,complex *c);//复数加法
void mul(complex a,complex b,complex *c);//复数乘法
void sub(complex a,complex b,complex *c);//复数减法	

#endif
