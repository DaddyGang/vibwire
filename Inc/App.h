#ifndef __APP_H
#define __APP_H

#include "stm32l0xx_hal.h"
#include "max7q.h"
#include "mma8451.h"

typedef enum {
	START_MODE,
	TEST_MODE,
	CMD_MODE,
	TRANSPARENT_MODE,
	TRANSPARENT_DEBUG_MODE,
	TIMESEND_MODE,
	TIMESEND_DEBUG_MODE,
	LOWPOWER_MODE,
	LOWPOWER_DEBUG_MODE,
} DEVICE_SATE;

typedef struct {
	int32_t longitude;		//*100000
	int32_t latitude;		//*100000
}GPS_T;

typedef struct {
	uint8_t Led;		//0 , 1
	uint16_t Pressure;	//Hpa/10
	int16_t Altitude;	//m*10
	int16_t Tempter;	//¡æ*100
	int16_t Humidi;		//%RH
	int16_t Lux;		//lux*100
	ACCELER_T acc;		//g*100, x,y,z
	uint8_t battery;	//0 for extern, 1~254 level
	GPS_T	gps;		//longitude, latitude
	int16_t reserve1;
	int16_t reserve2;
	int16_t reserve3;
} SENSOR;


void User_App(void);
void System_Task(uint32_t A_time,uint32_t B_time);
void CMD_Check(void);
void Env_Send_Task(uint32_t D_time);
void SENSOR_Testing(SENSOR *Env_temp);
void HDC1000_Update(SENSOR *Env_temp);
void OPT3001_Update(SENSOR *Env_temp);
void MPL3115_Update(SENSOR *Env_temp);
void MMA8451_Upate();
uint8_t GPS_UpData(GPS_DATA *data_temp);
uint8_t Time_Out_Break(uint16_t MAX_time);
void GPS_Data_TO(void);
void ENV_Data_TO(void);
void Int2Str(uint8_t* str, int32_t intnum);
uint8_t *StringCat(uint8_t *str, const uint8_t *string);
void EEPROM_WriteOneByte(uint32_t add, uint8_t data);
void LowPower_Send_Task(uint16_t Time);



#define LED1_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET)
#define LED1_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET)

#define LED2_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
#define LED2_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)

#define LED3_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET)
#define LED3_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET)

#define LED4_ON  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET)
#define LED4_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET)


#endif 

