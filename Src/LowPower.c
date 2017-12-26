#include "LowPower.h"
#include "usart.h"
#include "main.h"
#include "MPL3115.h"



extern RTC_HandleTypeDef hrtc;

extern uint8_t Send_Sate_Sign;


void LowPower_Init(void)
{

	OPT3001IntoSleep();
    MPL3115IntoSleep();
    MMA8451IntoSleep(); 

	GPIOIntoSleep();
        
	/* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
}

void Set_RTC_WakeUp(uint16_t D_time)
{
	uint32_t time;
	
	time = D_time*(32768/16);
	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, D_time-1, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);

	HAL_NVIC_SetPriority(RTC_IRQn, 1, 0);//设置rtc中断优先级
	HAL_NVIC_EnableIRQ(RTC_IRQn);   
}
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	if(Send_Sate_Sign == 1)
	{
		Send_Sate_Sign = 11;
	}
}

void GPIOIntoSleep(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();    
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
    
//    /*Configure UART1 PA9  UATR2 PA2 pin Output Level */
//    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_9, GPIO_PIN_SET);
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
    
//     /*Configure LPUART PB10 pin Output Level */
//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_0|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);
    
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC,  GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10, GPIO_PIN_RESET);     

    /*Configure GPIO pins : PA0 PA1 PA4 PA5 PA6 PA7 PA8 PA11 PA12 PA15*/
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
//    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
//    /*Configure GPIO pins : PA13 PA14*/   烧写失败
//    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /*Configure GPIO pins : UART2 PA3 UART1 PA10*/
   // GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_10;
    //GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
   // GPIO_InitStruct.Pull = GPIO_NOPULL;
  //  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
    /*Configure GPIO pins : PB3 PB8 PB9 PB10 PB12 PB13 PB14 PB15 */
//    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
        GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
//    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
//    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PC0 PC1 PC2 PC3 PC4 PC5 PC10 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);    
        
    /*Configure GPIO pins : PD2 */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PH0 PH1 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
    
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
}


void LoRaNode_Go_To_Sleep(void)
{
	DEBUG_Printf("--> 系统进入常规休眠\r\n");
	LoraNode_Wake_Sleep(MODE_SLEEP);
	LowPower_Init();
	SystemRecovery();
	DEBUG_Printf("--> 系统已唤醒\r\n");
}

void LoRaNode_Send_To_Sleep(void)
{
	DEBUG_Printf("--> 系统进入发送休眠\r\n");
	PrepareLPUARTToStopMode();
	Busy_Gpio_exit();
	LowPower_Init();
	SystemRecovery();
	DEBUG_Printf("--> 系统已唤醒\r\n");
}

void SystemRecovery(void)
{
	RecoveryClock();
	
	//HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
	WRITE_REG(LPUART1->ICR, USART_ICR_WUCF);
	CLEAR_BIT(LPUART1->CR3, USART_CR3_WUFIE);
	CLEAR_BIT(LPUART1->CR1, USART_CR1_UESM);
  
	Clear_UART2_IT();
	Clear_UART1_IT();
	Clear_LPUART1_IT();
	HDC1000_Init();
	OPT3001_Init();
	MPL3115_Init(MODE_BAROMETER);
	MMA8451_Init();	
	HAL_Delay(70);
}


void RecoveryClock(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
  	RCC_ClkInitTypeDef RCC_ClkInitStruct;
  	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/* 使能PWR外设时钟 */
    __PWR_CLK_ENABLE();

    /**Configure the main internal regulator output voltage 
    */
	  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	    /**Initializes the CPU, AHB and APB busses clocks 
	    */
	  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	  RCC_OscInitStruct.HSICalibrationValue = 16;
	  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	  {
	    Error_Handler();
	  }

	    /**Initializes the CPU, AHB and APB busses clocks 
	    */
	  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
	                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
	                              |RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_I2C1
	                              |RCC_PERIPHCLK_RTC;
	  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_LSE;
	  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	  {
	    Error_Handler();
	  }

}


//---------------------------串口 LPUART 唤醒回调函数 -----------------------------
void LPUART_CharReception_Callback(void) 
{
	if(Send_Sate_Sign == 1)
	{
		Send_Sate_Sign = 12;
	}
}

