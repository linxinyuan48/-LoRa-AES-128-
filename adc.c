#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"
void adc_init(void)
{
  uint16_t time=0,time_out=10000;
	//启动时钟，gpio的，adc的
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	//还要设置ADC的时钟才行，adccle
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//选择8分频，因为规定是14mhz，我们选的apb2是72mhz，采用8分频为9mhz
	//配置adc
	ADC_InitTypeDef ADC_STRUCTOR;

	//要的是单连通，不连续
	ADC_STRUCTOR.ADC_ContinuousConvMode=DISABLE;//这是连续转换
	ADC_STRUCTOR.ADC_DataAlign=ADC_DataAlign_Right;//这是数据对齐
	ADC_STRUCTOR.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None; //是否外部触发，我们是软件触发
  ADC_STRUCTOR.ADC_Mode=ADC_Mode_Independent;
	ADC_STRUCTOR.ADC_NbrOfChannel=1;
	ADC_STRUCTOR.ADC_ScanConvMode=DISABLE;
	ADC_Init(ADC1,&ADC_STRUCTOR);
	//启动ADC
	ADC_Cmd(ADC1,ENABLE);
	//配置GPIO口
	GPIO_InitTypeDef GPIO_ADC_STRUCTOR;
	GPIO_ADC_STRUCTOR.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_ADC_STRUCTOR.GPIO_Pin=GPIO_Pin_0;
	GPIO_ADC_STRUCTOR.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_ADC_STRUCTOR);

	//复位校准
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)==SET)
  {
    if(time<time_out)
    {
      time++;
    }
    else break;
    time=0;
  }
  //启动校准
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)==SET)
  {
    if(time<time_out)
    {
      time++;
    }
    else break;
  }
}
uint16_t get_value(uint8_t ADC_Channel)
{
		//使用规则组，所以要单独配置一下
	ADC_RegularChannelConfig(ADC1, ADC_Channel,1,ADC_SampleTime_55Cycles5);
	//软件触发adc转换时
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
  TickType_t startTick = xTaskGetTickCount();
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET)
  {
    if ((xTaskGetTickCount() - startTick) > pdMS_TO_TICKS(10))  // 超时 10ms
        return ERROR;
  }
	return ADC_GetConversionValue(ADC1);
}

