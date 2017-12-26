#ifndef __LOWPOWER_H
#define __LOWPOWER_H

#include "stm32l0xx_hal.h"
#include "lorawan.h"


void LowPower_Init(void);
void GPIOIntoSleep(void);
void LowPower_Handler(Node_Info *LoRa_temp);
void LoRaNode_Go_To_Sleep(void);
void LoRaNode_Send_To_Sleep(void);
void Set_RTC_WakeUp(uint16_t D_time);
void LPUART_CharReception_Callback(void);
void SystemRecovery(void);
void RecoveryClock(void);

#endif
