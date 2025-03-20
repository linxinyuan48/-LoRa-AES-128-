#include "stm32f10x.h"                  // Device header
#include "blue.h"
#include "OLED.h"
//在这里完成的事情是，对串口的配置和对蓝牙接口的配置。电脑发送AT指令来验证蓝牙串口模块的连接是否成功。
uint8_t b_t_data;
static uint8_t flag=0, flag_p=0,flag_y=0,flag_r=0;
uint8_t finish=0;
uint16_t pitch_value,roll_value,yaw_value;
void blue_tooch_init(void)
{

	//配置时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//GPIO 蓝牙
	GPIO_InitTypeDef GPIO_B_C_STRUCTOR1;
	GPIO_B_C_STRUCTOR1.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_B_C_STRUCTOR1.GPIO_Pin=GPIO_Pin_10;
	GPIO_B_C_STRUCTOR1.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_B_C_STRUCTOR1);
  
	GPIO_B_C_STRUCTOR1.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_B_C_STRUCTOR1.GPIO_Pin=GPIO_Pin_9;
	GPIO_B_C_STRUCTOR1.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_B_C_STRUCTOR1);

  USART_InitTypeDef USART_B_STRUCTOR;
  USART_B_STRUCTOR.USART_BaudRate=9600;
  USART_B_STRUCTOR.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
  USART_B_STRUCTOR.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
  USART_B_STRUCTOR.USART_Parity=USART_Parity_No;
  USART_B_STRUCTOR.USART_StopBits=USART_StopBits_1;
  USART_B_STRUCTOR.USART_WordLength=USART_WordLength_8b;
  USART_Init(USART1,&USART_B_STRUCTOR);
  //输入要配置中断的，
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
  //开始配置NVIC
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitTypeDef NVIC_B_C_STRUCTOR;
  NVIC_B_C_STRUCTOR.NVIC_IRQChannel=USART1_IRQn;
  NVIC_B_C_STRUCTOR.NVIC_IRQChannelCmd=ENABLE;
  NVIC_B_C_STRUCTOR.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_B_C_STRUCTOR.NVIC_IRQChannelSubPriority=1;
  NVIC_Init(&NVIC_B_C_STRUCTOR);
  //USART启动
  USART_Cmd(USART1,ENABLE);
  
}
void send_data_u_t(uint16_t data)
{
  USART_SendData(USART1,data);
  while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//数据传输完毕后此条件满足。
  
}


void USART1_IRQHandler(void)
{	
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
    ;
	}
//需要手动清除标识位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}
  
uint8_t get_flag()
{
  if(flag==1)
  {
    flag=0;
    return 1;
  }
  return 0;
}
uint8_t get_finish()
{
  if(finish==1)
  {
    finish=0;
    return 1;
  }
  return 0;
}
//uint16_t get_b_t_data()
//{
//  return b_t_data;
//}
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		send_data_u_t(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}
int fputc(int ch, FILE *f)
{
	send_data_u_t(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}


void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);		//将字节数据写入数据寄存器，写入后USART自动生成时序波形
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	//等待发送完成
	/*下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位*/
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}

