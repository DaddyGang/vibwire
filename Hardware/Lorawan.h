#ifndef __LORAWAN_H
#define __LORAWAN_H

#include "stm32l0xx_hal.h"

typedef enum {
	MODE_INT,
	MODE_CMD,		//mode pin high
	MODE_TRANSPARENT,
} LoraNode_Mode_T;

typedef enum {
	NET_ABP,
	NET_OTAA,
} LoraNode_NetMode_T;

typedef enum {
	MODE_WAKEUP,
	MODE_SLEEP,	
} LoraNode_SleepMode_T;

typedef struct {
	uint8_t DevEUI[8];	
	uint8_t AppEUI[8];	
	uint8_t DevADDR[4];	
	uint8_t AppKEY[16];	
	uint8_t AppSKEY[16];	
	uint8_t NwkSKEY[16];

	uint8_t Join;
	LoraNode_Mode_T MODE;
	uint8_t ONline;
	uint8_t BUSY;
	uint8_t NET_Mode;
	uint8_t Confirm;
} Node_Info;

typedef struct {
	uint8_t Up_Result;	
	uint8_t Up_CH;	
	uint8_t Up_RATE;	
	uint8_t Up_DB;	
	uint32_t Up_Link;	
	uint16_t Up_Cache;
	uint16_t Resend;
} Send_Info;




/* Lorawan Node pin configuration */
#define LORANODE_WAKE_PIN					GPIO_PIN_0
#define LORANODE_WAKE_GPIO_PORT         	GPIOB
												
#define LORANODE_MODE_PIN					GPIO_PIN_2
#define LORANODE_MODE_GPIO_PORT         	GPIOB
													
#define LORANODE_NRST_PIN					GPIO_PIN_5
#define LORANODE_NRST_GPIO_PORT         	GPIOB
													
/* INPUT */													
#define LORANODE_STAT_PIN					GPIO_PIN_1
#define LORANODE_STAT_GPIO_PORT         	GPIOB
														
#define LORANODE_BUSY_PIN					GPIO_PIN_4
#define LORANODE_BUSY_GPIO_PORT         	GPIOB

/* stop the wfi sleep */	

#define LORANODE_GPIO_PORT				  LORANODE_BUSY_GPIO_PORT //LORANODE_STAT_GPIO_PORT 	

#define LORANODE_WAKE_HIGH()      HAL_GPIO_WritePin(LORANODE_WAKE_GPIO_PORT, LORANODE_WAKE_PIN, GPIO_PIN_SET)
#define LORANODE_WAKE_LOW()     HAL_GPIO_WritePin(LORANODE_WAKE_GPIO_PORT, LORANODE_WAKE_PIN, GPIO_PIN_RESET)													
													
#define LORANODE_MODE_HIGH()      HAL_GPIO_WritePin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN, GPIO_PIN_SET)
#define LORANODE_MODE_LOW()     HAL_GPIO_WritePin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN, GPIO_PIN_RESET) 

#define LORANODE_NRST_HIGH()      HAL_GPIO_WritePin(LORANODE_NRST_GPIO_PORT, LORANODE_NRST_PIN, GPIO_PIN_SET)
#define LORANODE_NRST_LOW()     HAL_GPIO_WritePin(LORANODE_NRST_GPIO_PORT, LORANODE_NRST_PIN, GPIO_PIN_RESET)

#define LORANODE_STAT_STATUS	HAL_GPIO_ReadPin(LORANODE_STAT_GPIO_PORT, LORANODE_STAT_PIN)
#define LORANODE_BUSY_STATUS	HAL_GPIO_ReadPin(LORANODE_BUSY_GPIO_PORT, LORANODE_BUSY_PIN)
#define LORANODE_MODE_STATUS	HAL_GPIO_ReadPin(LORANODE_MODE_GPIO_PORT, LORANODE_MODE_PIN)

void LoraNode_GPIO_Init(void);
void LoraNode_Init_Mode(LoraNode_Mode_T mode);
void LoraNode_Mode_Set(LoraNode_Mode_T mode);
void LoraNode_Wake_Sleep(LoraNode_SleepMode_T mode);
static void LoraNode_Reset(void);
char *LoraNode_GetVer(void);
void LoraNode_GetDevEUI(Node_Info *LoRa_temp);
void LoraNode_GetAppEUI(Node_Info *LoRa_temp);
void LoraNode_GetAppKey(Node_Info *LoRa_temp);
void LoraNode_GetDevAddr(Node_Info *LoRa_temp);
void LoraNode_GetAppSKey(Node_Info *LoRa_temp);
void LoraNode_GetNwkSKey(Node_Info *LoRa_temp);
void LoraNode_GetNetMode(Node_Info *LoRa_temp);
void LoraNode_GetConfirm(Node_Info *LoRa_temp);
void LoraNode_GetState(Send_Info *LoRa_temp);

uint32_t LoraNode_SetGPIO(uint32_t pin, uint32_t state);
uint32_t LoraNode_SetNetMode(LoraNode_NetMode_T mode);
uint32_t LoraNode_SetDebug(uint32_t value);
uint32_t LoraNode_SetAppEUI(uint8_t *dev);
uint32_t LoraNode_SetAppKey(uint8_t *key);
uint32_t LoraNode_SetAppSKey(uint8_t *skey);
uint32_t LoraNode_SetNwkSKey(uint8_t *nwk_skey);

uint32_t LoraNode_Save(void);

void LoraNode_OTAA_Config(void);
void LoraNode_ABP_Config(void);

void GET_LoraNode_Info(Node_Info *lora_temp);
void LoraNode_STA_Check(Node_Info *LoRa_temp);

void LoraNode_Send_AT(char *at_buf);
void LoraNode_Read(uint8_t *str);

int8_t LoRaNode_LowPower_Send(uint8_t *buffer, int32_t len,uint8_t CONFIRM);
uint32_t LoraNode_Write_Receive_Bytes(uint8_t *buffer, int32_t len,uint8_t CONFIRM);

uint32_t LoraNode_IO_State(uint32_t pin);
void LoraNode_Delay(uint32_t Delay);
char *StringStr(char *str, char *dest);


void Printf_LoRaNode_Info(Node_Info LoRa_temp);
uint8_t StrToHex(uint8_t temp);

#endif 

