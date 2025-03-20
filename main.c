#include "main.h"
/*
创建3个任务：

Task1（优先级2）：周期读取DHT11温湿度+SGP30空气质量（I2C）（√）

Task2（优先级1）：处理BH1750光照强度数据（硬件中断触发）(√)

Task3（优先级3）：通过LoRa模块（如SX1278）发送加密数据包(√)


低功耗优化

采用Tickless模式：当无任务运行时进入STOP模式（功耗<5μA）(√)

动态频率调节：数据采集时切到72MHz，空闲时降频至8MHz(√)

电源管理：对非必要外设（如调试串口）进行硬件断电

安全通信

实现AES-128加密算法（纯C编写）（√）

自定义协议帧结构：

c
复制

可视化调试

通过0.96寸OLED显示实时数据(√)
*/

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		1060  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

void oled_aes_send_data_task(void);
void sunlight_task(void);
void tem_moi_task(void);
void check_space_task(void);
void stop_task( void );
void restare_task(void);

uint8_t openTextInput_t_m[5];

//任务优先级
#define OLED_TASK_PRIO		4//需要大于除了自己以外的所有任务，不然会影响整体系统稳定。
#define SUN_LIGHT_TASK_PRIO 2
#define TEM_MOI_TASK_PRIO  2
#define CHECK_TASK_PRIO  3
#define STOP_TASK_PRIO  0//
#define RESTARE_TASK_PRIO  1//
//任务堆栈大小	
#define OLED_TASK_SIZE 		 350  
#define SUN_LIGHT_TASK_SIZE 		 128  
#define TEM_MOI_TASK_SIZE 		 128  
#define CHECK_TASK_SIZE 		 128  
#define STOP_TASK_SIZE     128
#define RESTARE_TASK_SIZE     128
//任务句柄
TaskHandle_t OLED_Task_Handler;
TaskHandle_t SUN_LIGHT_Task_Handler;
TaskHandle_t TEM_MOI_Task_Handler;
TaskHandle_t CHECK_TASK_Handler;
TaskHandle_t STOP_TASK_Handler;
TaskHandle_t RESTARE_TASK_Handler;



uint16_t arrange_sun_main=0;
//任务函数



/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
int main()
{
	SysTick_Init(72);
  OLED_Init();
  adc_init();
  TIM2_Init();//时钟
  blue_tooch_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
//  vTaskSuspend(STOP_TASK_Handler);

	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
      
				
     //创建显示屏任务
    xTaskCreate((TaskFunction_t)oled_aes_send_data_task,
                (const char*)"oled_aes_send_data_task",
                  (uint16_t)OLED_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)OLED_TASK_PRIO,
                (TaskHandle_t*)&OLED_Task_Handler);
////    //创光照传感器任务
    xTaskCreate((TaskFunction_t)sunlight_task,
                (const char*)"sunlight_task",
                  (uint16_t)SUN_LIGHT_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)SUN_LIGHT_TASK_PRIO,
                (TaskHandle_t*)&SUN_LIGHT_Task_Handler);
////     //创温湿度传感器任务
    xTaskCreate((TaskFunction_t)tem_moi_task,
                (const char*)"tem_moi_task",
                  (uint16_t)TEM_MOI_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)TEM_MOI_TASK_PRIO,
                (TaskHandle_t*)&TEM_MOI_Task_Handler);
//     检测任务是否继续执行的任务
    xTaskCreate((TaskFunction_t)check_space_task,
                (const char*)"check_space_task",
                  (uint16_t)CHECK_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)CHECK_TASK_PRIO,
                (TaskHandle_t*)&CHECK_TASK_Handler);
////     停止任务 
    xTaskCreate((TaskFunction_t)stop_task,
                (const char*)"stop_task",
                  (uint16_t)STOP_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)STOP_TASK_PRIO,
                (TaskHandle_t*)&STOP_TASK_Handler);
//恢复任务
     xTaskCreate((TaskFunction_t)restare_task,
                (const char*)"restare_task",
                  (uint16_t)RESTARE_TASK_SIZE,
                (void*)NULL,
                (UBaseType_t)RESTARE_TASK_PRIO,
                (TaskHandle_t*)&RESTARE_TASK_Handler);

         
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
} 


void oled_aes_send_data_task(void)//显示光照强度，温湿度。
{
  while(1)
  {
unsigned char openTextOutput_t_m[5] = { 0 };
unsigned char encryptedText_t_m[5] = { 0 };
unsigned char key[32] = "this_my_key_(yi)";

        // AES 128 测试
    for (uint32_t i = 0; i < sizeof(encryptedText_t_m); encryptedText_t_m[i++] = 0);
//    uint32_t start = millis;
    Encrypt(openTextInput_t_m, key, encryptedText_t_m);
   // timMeasures[measures::AES128_ENCRYPT] = millis - start;
    for (uint32_t i = 0; i < sizeof(openTextOutput_t_m); openTextOutput_t_m[i++] = 0);
//    start = millis;
    Decrypt(encryptedText_t_m, key, openTextOutput_t_m);
    
    
    
   
      OLED_ShowString(1,1,"SUN_LIGHT:");
      OLED_ShowNum(1,12,arrange_sun_main,4);
      //温度 加密前
      OLED_ShowString(2,1,"T:");
     	OLED_ShowNum(2, 3, openTextInput_t_m[2],2);
      OLED_ShowString(2,5,".");
      OLED_ShowNum(2,6,openTextInput_t_m[3],2);
//      
      
//    温度 加密后  
//      OLED_ShowString(2,1,"T:");
//     	OLED_ShowNum(2, 3, openTextOutput_t_m[2],2);
//      OLED_ShowString(2,5,".");
//      OLED_ShowNum(2,6, openTextOutput_t_m[3],2);
      
//湿度 加密前
      OLED_ShowString(3,1,"M:");
			OLED_ShowNum(3, 3, openTextInput_t_m[0],2);
      OLED_ShowString(3,5,".");
      OLED_ShowNum(3,6, openTextInput_t_m[1],2);
      
      
      if(arrange_sun_main>=3000)
      { Serial_SendString("dark\r\n");
        
        Serial_SendString("sun_light\r\n");
        
        Serial_SendNumber(arrange_sun_main, 4);
        
        Serial_SendString("\r\n");
        
       
       Serial_SendString("tem\r\n");
       Serial_SendNumber(openTextInput_t_m[2],2);
       Serial_SendString(".");
       Serial_SendNumber(openTextInput_t_m[3],2);
       Serial_SendString("\r\n");
        
        
       Serial_SendString("moi\r\n");
       Serial_SendNumber(openTextInput_t_m[0],2);
        Serial_SendString(".");
       Serial_SendNumber(openTextInput_t_m[1],2);
       Serial_SendString("\r\n");
        
        vTaskDelay(pdMS_TO_TICKS(1000));       
      }
      vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void sunlight_task(void)//三分钟获取一次光照
{
  while(1)
  {
   if(arrange_sun_main==ERROR)
   {
       arrange_sun_main=0xFFFF;
   }
   else 
   {   
    arrange_sun_main=get_value(ADC_Channel_0);
    vTaskDelay(pdMS_TO_TICKS(1000));
   }
   vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void tem_moi_task(void)//一分钟获取一次温湿度。
{
  while(1)
  {
  DHT_Get_Temp_Humi_Data(openTextInput_t_m);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void check_space_task(void)
{
  uint8_t flag_o,flag_s,flag_t_m,test_s=0,test_r=0,test_s_flag=0,test_t_m_flag=0;
  OLED_Init();
  while(1)
  {
  
   if (eTaskGetState(OLED_Task_Handler)==eBlocked||eTaskGetState(OLED_Task_Handler)==eSuspended)
   {
     flag_o=1;
   }
   else flag_o=0;
   OLED_ShowNum(2,9,flag_o,1);
   
    if (eTaskGetState(SUN_LIGHT_Task_Handler)==eBlocked||eTaskGetState(SUN_LIGHT_Task_Handler)==eSuspended)
   {
     //被强制休息
     flag_s=1;
     //唤醒
     vTaskResume(SUN_LIGHT_Task_Handler);
   }
   else 
   {
     flag_s=0;
     test_s_flag++;
   if (test_s_flag==5)//表示运行五次了
     {
       vTaskSuspend(SUN_LIGHT_Task_Handler);//挂起休息
       test_s_flag=0;
     }
   }
   
     OLED_ShowNum(2,11,flag_s,1);
   
   if(eTaskGetState(TEM_MOI_Task_Handler)==eBlocked||eTaskGetState(TEM_MOI_Task_Handler)==eSuspended)
   {
     flag_t_m=1;
     //唤醒
     vTaskResume(TEM_MOI_Task_Handler);
   }
   else 
   {
     flag_t_m=0;
     test_t_m_flag++;
     if(test_t_m_flag==5)
         {
       vTaskSuspend(TEM_MOI_Task_Handler);//挂起休息
       test_t_m_flag=0;
     }
   }
   OLED_ShowNum(2,13,flag_t_m,1);
   if(flag_o==1&&flag_s==1&&flag_t_m==1)//没有任务需要执行
   {
     vTaskSuspend(RESTARE_TASK_Handler);
     vTaskResume(STOP_TASK_Handler);
     test_r++;
     OLED_ShowNum(4,10,test_r,2);
   }
   else 
   {  
     test_s++;
     vTaskResume(RESTARE_TASK_Handler);//还有任务需要执行
       OLED_ShowNum(4,14,test_s,2);
     vTaskSuspend(STOP_TASK_Handler);
   }
   vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void stop_task(void)
{
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
   OLED_Init();
  while(1)
  {
    OLED_ShowString(4,1,"stoping");
     //停止模式。   
    //换时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    
    RCC_LSEConfig(RCC_LSE_ON);  // 开启 LSE 低速时钟
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    //进停止
    PWR_EnterSTOPMode(PWR_Regulator_ON,PWR_STOPEntry_WFI);
        vTaskDelay(pdMS_TO_TICKS(500));
      OLED_ShowString(4,1,"         ");
  }
}
void restare_task(void)
{
  OLED_Init();
  while(1)
  {
    OLED_ShowString(4,1,"restare");
     vTaskDelay(pdMS_TO_TICKS(1000));
 
    OLED_ShowString(4,1,"        ");
    RCC_HSEConfig(RCC_HSE_ON);
    RCC_PLLCmd(DISABLE);  // 先关闭 PLL 以便重新配置
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  // 8MHz × 9 = 72MHz
    RCC_PLLCmd(ENABLE);  // 使能 PLL
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  // 等待 PLL 稳定
    // 3. 切换系统时钟到 PLL
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);  // 等待切换完成
    // 4. 配置 AHB、APB 总线分频
    RCC_HCLKConfig(RCC_SYSCLK_Div1);  // AHB = 72MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);   // APB1 = 36MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);   // APB2 = 72MHz
    // 5. 使能 FLASH 预取指 & 2 级等待状态（适用于 72MHz）
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    // 6. 关闭 HSI（可选，节省功耗）
    RCC_HSICmd(DISABLE);
    vTaskDelay(pdMS_TO_TICKS(500));
  } 
}
