#ifndef __VIBWIRE_H__
#define __VIBWIRE_H__

#include "stm32l0xx_hal.h"

#define TRIGGER_PORT		GPIOA
#define TRIGGER_PIN 		GPIO_PIN_8

#define VIBWIRE_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define VIBWIRE_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOA_CLK_DISABLE()

#define TRIGGER_ON  HAL_GPIO_WritePin(TRIGGER_PORT, TRIGGER_PIN, GPIO_PIN_SET)
#define TRIGGER_OFF  HAL_GPIO_WritePin(TRIGGER_PORT, TRIGGER_PIN, GPIO_PIN_RESET)
#define TRIGGER_TOGGLE  HAL_GPIO_TogglePin(TRIGGER_PORT, TRIGGER_PIN)

#define DATAZ_M 0x05
#define DATAZ_L 0x06
#define DATAY_M 0x07
#define DATAY_L 0x08



void VIB_Init(void);
void VIB_Test(void);

#endif