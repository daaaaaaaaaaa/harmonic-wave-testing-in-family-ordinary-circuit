#include "adc.h"

volatile uint16_t ADC_ConvertedValue[256];			//ADC����������
u16 DMA1_MEM_LEN;
extern complex x[N];
#define ADC1_DR_Address    ((u32)0x4001244C)		//ADC1�ĵ�ַ

//TIM2���ã�arrΪ�ؼ���ֵ��pscΪԤ��Ƶϵ��
void TIM2_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB2Periph_GPIOA, ENABLE); 		//ʱ��ʹ��


    //��ʱ��TIM2��ʼ��
    TIM_TimeBaseStructure.TIM_Period = arr; 		//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; 			//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; 		//����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		//TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);			//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = 10;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
    TIM_OC2Init(TIM2, & TIM_OCInitStructure);		//��ʼ������TIM2_CH2

    TIM_Cmd(TIM2, ENABLE); 			//ʹ��TIM2
}

//DMA1����
void DMA1_Config()
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	  			//ʹ��ADC1ͨ��ʱ��

	DMA1_MEM_LEN = 256;
    //DMA1��ʼ��
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;				//ADC1��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue; 		//�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 				//����(�����赽�ڴ�)
    DMA_InitStructure.DMA_BufferSize = 256; 						//�����������
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 		//�����ַ�̶�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 			//�ڴ��ַ�̶�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ; //�������ݵ�λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord ;    //�ڴ����ݵ�λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular  ; 		//DMAģʽ��ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_High ; 		//���ȼ�����
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;   		//��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //����DMA1
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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE);	  //ʹ��GPIOAʱ��

    //PA6 ��Ϊģ��ͨ����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ
    //ADC1��ʼ��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 			//����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  			//�ر�ɨ�跽ʽ
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//�ر�����ת��ģʽ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_CC2;   	//ʹ���ⲿ����ģʽ
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 			//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1; 			//Ҫת����ͨ����Ŀ
    ADC_Init(ADC1, &ADC_InitStructure);

    RCC_ADCCLKConfig(RCC_PCLK2_Div6);				//����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz����239.5+12.5��/12Mhz=21us

    ADC_Cmd(ADC1,ENABLE);

    ADC_ResetCalibration(ADC1);				//��λУ׼�Ĵ���
    while(ADC_GetResetCalibrationStatus(ADC1));				//�ȴ�У׼�Ĵ�����λ���

    ADC_StartCalibration(ADC1);				//ADCУ׼
    while(ADC_GetCalibrationStatus(ADC1));				//�ȴ�У׼���

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);		//����ADC1ͨ��6Ϊ239.5���������� ,��������Ϊ21us
	 ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//�����ⲿ��һ����ģʽʹ��
}

//u16 Get_Adc(u8 ch)   
//{
//  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��     
//    ADC_ExternalTrigConvCmd(ADC1, ENABLE);		//�����ⲿ��һ����ģʽʹ��
//	 
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

//	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
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
//	//����ð���㷨����
//	for(t=0;t<6;t++)//
//    {
//        for(i=0;i<6-t;i++)//
//        {
//            if(adc_value[i]>adc_value[i+1])//��һλ����ǰһλ��С�Ļ����ͽ�����������λ�ã�����
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
