#ifndef __ADC_H
#define __ADC_H
#include "sys.h"
#include "fft.h"


void windowing(void);
void Adc_Init(void);
void TIM2_Init(u16 arr,u16 psc);
void ADC_Start(void);
void DMA1_Enable(void);
void DMA1_Config(void);


#endif
