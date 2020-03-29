#include "adc.h"

volatile uint16_t ADC_ConvertedValue[256];			//ADC采样的数据
u16 DMA1_MEM_LEN;
extern complex x[N];
#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1的地址

//TIM2配置，arr为重加载值，psc为预分频系数
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB2Periph_GPIOA, ENABLE); 		//时钟使能


    //定时器TIM2初始化
    TIM_TimeBaseStructure.TIM_Period = arr; 		//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =psc; 			//设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; 		//设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//根据指定的参数初始化TIMx的时间基数单位

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//比较输出使能
    TIM_OCInitStructure.TIM_Pulse = 10;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
    TIM_OC2Init(TIM2, & TIM_OCInitStructure);		//初始化外设TIM2_CH2

    TIM_Cmd(TIM2, ENABLE); 			//使能TIM2
}

//DMA1配置
void DMA1_Config()
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	  			//使能ADC1通道时钟

	DMA1_MEM_LEN = 256;
    //DMA1初始化
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;				//ADC1地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; 		//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//方向(从外设到内存)
    DMA_InitStructure.DMA_BufferSize = 256; 						//传输的数据量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//内存地址固定
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //外设数据单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //内存数据单位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 		//DMA模式：循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High ; 		//优先级：高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//禁止内存到内存的传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //配置DMA1
}

void DMA1_Enable()
{
	 DMA_Cmd(DMA1_Channel1,DISABLE);
	 DMA_SetCurrDataCounter(DMA1_Channel1,DMA1_MEM_LEN);    
    DMA_Cmd(DMA1_Channel1,ENABLE);
}




void Adc_Init()
{
 
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE);	  //使能GPIOA时钟

    //PA6 作为模拟通道输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
    //ADC1初始化
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  			//关闭扫描方式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//关闭连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//使用外部触发模式
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 			//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1; 			//要转换的通道数目
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//配置ADC时钟，为PCLK2的6分频，即12MHz，（239.5+12.5）/12Mhz=21us

    ADC_Cmd(ADC1,ENABLE);

    ADC_ResetCalibration(ADC1);				//复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1));				//等待校准寄存器复位完成

    ADC_StartCalibration(ADC1);				//ADC校准
    while(ADC_GetCalibrationStatus(ADC1));				//等待校准完成

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);		//配置ADC1通道6为239.5个采样周期 ,采样周期为21us
	 ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//设置外部昂一触发模式使能
}

//u16 Get_Adc(u8 ch)   
//{
//  	//设置指定ADC的规则组通道，一个序列，采样时间     
//    ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//设置外部昂一触发模式使能
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

//	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
//}


//u16 bubble(u8 ch,u8 times)
//{
//	u32 t,i,temp;
//	u16 adc_value[7];
//	for(t=0;t<times;t++)
//	{
//		adc_value[t] = Get_Adc(ch);
////		temp_val+=Get_Adc(ch);
////		delay_ms(5);
//	}
//	
//	//采用冒泡算法升序
//	for(t=0;t<6;t++)//
//    {
//        for(i=0;i<6-t;i++)//
//        {
//            if(adc_value[i]>adc_value[i+1])//后一位数比前一位数小的话，就交换两个数的位置（升序）
//            {
//               temp=adc_value[i+1];
//               adc_value[i+1]=adc_value[i];
//               adc_value[i]=temp;
//            }
//        }
//    }
//	
//	return adc_value[3];
//} 	 



u16 Collect_Number;

void windowing(void)
{
    double window[N];
    for(Collect_Number=0; Collect_Number<N; Collect_Number++)
    {
        window[Collect_Number] = 0.54-0.46*cos(2*M_PI*Collect_Number/(N-1));
        x[Collect_Number].real = x[Collect_Number].real * window[Collect_Number];
    }
}
