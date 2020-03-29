#include "sys.h"
#include "adc.h"
#include "fft.h"
#include "stdio.h"

extern complex x[N];//�ֱ�Ϊ�������У��任��
extern double Amplitude[6], Phase[6];
extern uint16_t ADC_ConvertedValue[256];
extern u16 j, m;
extern double I2,I1;




int main(void)
{
	u16 number;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
   

    Adc_Init();//��ʼ��
    ADC_DMACmd(ADC1,ENABLE);
	DMA1_Config();
    DMA1_Enable();
	TIM2_Init(37,7199);		//72000000/7200=10000Hz��ÿ3900us�ɼ�һ��

	while(1)
	{
	while(1)
	{
		 if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)//�ȴ�ͨ��1�������
		 {
			  DMA_ClearITPendingBit(DMA1_IT_TC1);
			// TIM_Cmd(TIM2, DISABLE);
			 for(number = 0; number<256; number++)
        {
            x[number].real = (double)(ADC_ConvertedValue[number]*3.3/4095);
            x[number].img  = 0.0;
        }
		break;
		 }

	}

       // windowing();//�Բɼ��������ݽ��мӴ�����������


        fft();//�Բɼ��������ݽ���FFT
        Calculate_Wave();
}
    	
	

}

