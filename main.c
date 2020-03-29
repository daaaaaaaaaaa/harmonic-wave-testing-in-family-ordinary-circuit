#include "sys.h"
#include "adc.h"
#include "fft.h"
#include "stdio.h"

extern complex x[N];//分别为输入序列，变换核
extern double Amplitude[6], Phase[6];
extern uint16_t ADC_ConvertedValue[256];
extern u16 j, m;
extern double I2,I1;




int main(void)
{
	u16 number;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
   

    Adc_Init();//初始化
    ADC_DMACmd(ADC1,ENABLE);
	DMA1_Config();
    DMA1_Enable();
	TIM2_Init(37,7199);		//72000000/7200=10000Hz，每3900us采集一次

	while(1)
	{
	while(1)
	{
		 if(DMA_GetITStatus(DMA1_IT_TC1)!=RESET)//等待通道1传输完成
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

       // windowing();//对采集到的数据进行加窗处理，哈明窗


        fft();//对采集到的数据进行FFT
        Calculate_Wave();
}
    	
	

}

