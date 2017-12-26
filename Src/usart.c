/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>


USART_RECEIVETYPE Usart2_RX; 
USART_RECEIVETYPE Usart1_RX;
USART_RECEIVETYPE LPUsart1_RX;

/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* LPUART1 init function */

void MX_LPUART1_UART_Init(void)
{
	LPUART1_UART_Init();
}
/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspInit 0 */

  /* USER CODE END LPUART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_LPUART1_CLK_ENABLE();
  
    /**LPUART1 GPIO Configuration    
    PB10     ------> LPUART1_TX
    PB11     ------> LPUART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_LPUART1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN LPUART1_MspInit 1 */

  /* USER CODE END LPUART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart1_rx.Instance = DMA1_Channel3;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_3;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart2_rx.Instance = DMA1_Channel5;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==LPUART1)
  {
  /* USER CODE BEGIN LPUART1_MspDeInit 0 */

  /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LPUART1_CLK_DISABLE();
  
    /**LPUART1 GPIO Configuration    
    PB10     ------> LPUART1_TX
    PB11     ------> LPUART1_RX 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN LPUART1_MspDeInit 1 */

  /* USER CODE END LPUART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART2_IRQn);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

//--------------------UART2---------------------------------

void Usart2SendData(uint8_t *pdata, uint16_t Length)  
{  
     uint32_t i = 0;
		
	for (i = 0; i < Length; i++)
	{
		UART2_SendByte(pdata[i]);
	}
}  

void UART2_SendByte(uint8_t data)
{
  while((USART2->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
  USART2->TDR = data;
}

void USART2_SendString(char *str)
{
  while((*str)!=0)
  {
    while((USART2->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
    USART2->TDR = *str++;       
  }
}

void Clear_UART2_IT(void)
{
	__HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_IDLE);               //清除空闲中断标志
	HAL_Delay(2);
	__HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_TC);                 //清除发送标志
  	HAL_UART_Receive_DMA(&huart2, Usart2_RX.RX_Buf, RECEIVELEN); //开启DMA接收 
  	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);                 //使能空闲中断
 	__HAL_UART_CLEAR_FLAG(&huart2,UART_FLAG_IDLE);               //清除空闲中断标志	
}

//串口接收空闲中断  
void Usart2Receive_IDLE(void)  
{  
    uint32_t temp;  
  
    if((__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);  
        HAL_UART_DMAStop(&huart2);  
        temp = huart2.hdmarx->Instance->CNDTR;  
        Usart2_RX.rx_len =  RECEIVELEN - temp;   
        Usart2_RX.receive_flag=1;  
        HAL_UART_Receive_DMA(&huart2,Usart2_RX.RX_Buf,RECEIVELEN);  
    }  
}


//--------------------UART1---------------------------------

void Usart1Receive_IDLE(void)  
{  
    uint32_t temp;  
  
    if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);  
        HAL_UART_DMAStop(&huart1);  
        temp = huart1.hdmarx->Instance->CNDTR;  
        Usart1_RX.rx_len =  RECEIVELEN - temp;   
        Usart1_RX.receive_flag=1;  
        HAL_UART_Receive_DMA(&huart1,Usart1_RX.RX_Buf,RECEIVELEN);  
    }  
}

void Usart1SendData(uint8_t *pdata, uint16_t Length)  
{  
     uint32_t i = 0;
		
	for (i = 0; i < Length; i++)
	{
		UART1_SendByte(pdata[i]);
	}
}  

void UART1_SendByte(uint8_t data)
{
  while((USART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
  USART1->TDR = data;
}

void USART1_SendString(char *str)
{
  while((*str)!=0)
  {
    while((USART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
    USART1->TDR = *str++;       
  }
}


void Clear_UART1_IT(void)
{
	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_IDLE);               //清除空闲中断标志
	HAL_Delay(2);
	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_TC);                 //清除发送标志
  	HAL_UART_Receive_DMA(&huart1, Usart1_RX.RX_Buf, RECEIVELEN); //开启DMA接收 
  	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);                 //使能空闲中断
 	__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_IDLE);               //清除空闲中断标志	
}


//--------------------LPUART1---------------------------------


void LPUsart1SendData(uint8_t *pdata, uint16_t Length)  
{  
    uint32_t i = 0;
		
	for (i = 0; i < Length; i++)
	{
		LPUART1_SendByte(pdata[i]);
	}
}

void LPUART1_SendByte(uint8_t data)
{
  while((LPUART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
  LPUART1->TDR = data;
}
void LPUART1_SendString(char *str)
{
  while((*str)!=0)
  {
    while((LPUART1->ISR&UART_FLAG_TXE)!=UART_FLAG_TXE);//等待发送寄存器为空
    LPUART1->TDR = *str++;       
  }
}


void LPUsart1Receive_IDLE(uint8_t *temp,uint16_t lenth)  
{    
    if((__HAL_UART_GET_FLAG(&hlpuart1,UART_FLAG_IDLE) != RESET))  
    {   
        __HAL_UART_CLEAR_IDLEFLAG(&hlpuart1);  
		for(uint16_t i =0;i<lenth;i++)
		{
			LPUsart1_RX.RX_Buf[i] = temp[i];
		}
        LPUsart1_RX.rx_len =  lenth;   
        LPUsart1_RX.receive_flag=1;  
    }  
}

void Clear_LPUART1_IT(void)
{	
	__HAL_UART_CLEAR_FLAG(&hlpuart1,UART_FLAG_TC);                 //清除发送标志
	__HAL_UART_CLEAR_FLAG(&hlpuart1,UART_FLAG_RXNE);               //清除接收中断标志	
	__HAL_UART_CLEAR_FLAG(&hlpuart1,UART_FLAG_IDLE);               //清除空闲中断标志		
}

//------------------------------------------------------------------------------------------------

void PrepareLPUARTToStopMode(void)
{

  /* Clear OVERRUN flag */
  WRITE_REG(LPUART1->ICR, USART_ICR_ORECF);

  /* Configure LPUART1 transfer interrupts : */
  /* Clear WUF flag and enable the UART Wake Up from stop mode Interrupt */
  WRITE_REG(LPUART1->ICR, USART_ICR_WUCF);
  SET_BIT(LPUART1->CR3, USART_CR3_WUFIE);

  /* Enable Wake Up From Stop */
  SET_BIT(LPUART1->CR1, USART_CR1_UESM);
}



void LPUART1_UART_Init(void)
{


  hlpuart1.Instance        = LPUART1;
  hlpuart1.Init.BaudRate   = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits   = UART_STOPBITS_1;
  hlpuart1.Init.Parity     = UART_PARITY_NONE;
  hlpuart1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  hlpuart1.Init.Mode       = UART_MODE_TX_RX;

  if(HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
      
  }
  __HAL_UART_DISABLE(&hlpuart1);
  LPUART1->RQR = 1<<3;//clears the RXNE flag, and overrun flag
  LPUART1->CR1 |= 1<<5;//enable the RXNE interrupt
  LPUART1->CR1 |= 1<<4;//enable the IDLE interrupt
  HAL_NVIC_SetPriority(LPUART1_IRQn, 0, 2);//设置USART中断优先级:抢占优先级3；子优先级0（M0+内核无子优先级）
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);//使能USART中断
  __HAL_UART_ENABLE(&hlpuart1);//使能USART	//LPUART1->CR1 |= 1<<0;

  MODIFY_REG(LPUART1->CR3, USART_CR3_WUS, (USART_CR3_WUS_0 | USART_CR3_WUS_1));
}

//-----------------------------------------------------------------------------------------


void DEBUG_Printf(char *fmt, ...)
{		
	char buf[128];
    va_list va_args;

    // Start the varargs processing.
    va_start(va_args, fmt);

    vsnprintf((char *)buf, sizeof(buf), fmt, va_args);

    // End the varargs processing.
    va_end(va_args);

    /*
     * 真正的打印输出函数，不同平台修改
     */
//    Serial_PutString(buf);
	 USART2_SendString(buf);
}



/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
