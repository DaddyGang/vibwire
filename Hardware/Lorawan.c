#include "lorawan.h"
#include "app.h"
#include "usart.h"
#include <stdarg.h>
#include <string.h>

extern uint8_t TimeOut_Sign;

Node_Info LoRa_Node;
Node_Info *LoRa_Node_str = &LoRa_Node;

Send_Info LoRa_Send;
Send_Info *LoRa_Send_str = &LoRa_Send;

extern uint8_t Send_Sate_Sign;

extern USART_RECEIVETYPE Usart2_RX;
extern USART_RECEIVETYPE Usart1_RX;
extern USART_RECEIVETYPE LPUsart1_RX;

#define RXLEN	256
uint8_t AT_Data_buf[RXLEN];

/* OTAA mode default setting */
//uint8_t devEui[] = "00 4A 77 00 66 FF FE CA";

uint8_t appEui[] = "11 22 33 44 55 66 77 88";

uint8_t appKey[] = "00 11 22 33 44 55 66 77 88 99 AA BB CC DD EE FF";

/* ABP mode default setting */
uint8_t appSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";

uint8_t nwkSKey[] = "2B 7E 15 16 28 AE D2 A6 AB F7 15 88 09 CF 4F 3C";

uint8_t devAddr[] = "FF FF FE A6";


void LoraNode_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct; 
	//UART_HandleTypeDef UartHandle;	
	
	/* enable clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();
		
	/* gpio config */
	GPIO_InitStruct.Pin 	= LORANODE_WAKE_PIN|LORANODE_MODE_PIN|LORANODE_NRST_PIN;
	GPIO_InitStruct.Mode	= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_VERY_HIGH;	
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin 	= LORANODE_STAT_PIN|LORANODE_BUSY_PIN;
	GPIO_InitStruct.Mode	= GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void LoraNode_Init_Mode(LoraNode_Mode_T mode)
{
	LoraNode_Reset();
	
	LoraNode_Mode_Set(mode);	
	
	LoraNode_Wake_Sleep(MODE_WAKEUP);
	
}


//---------------------------------------------
//	@funtion:	set the lorawan module mode, command mode or transparent mode
//	@paramters:	MODE_CMD, or MODE_TRANSPARENT
//	@return:	void
//----------------------------------------------
void LoraNode_Mode_Set(LoraNode_Mode_T mode)
{
	if (mode == MODE_CMD )
		LORANODE_MODE_HIGH();
	if (mode == MODE_TRANSPARENT)
		LORANODE_MODE_LOW();
}

//----------------------------------------------------------------------
//	@funtion:	set the lorawan module mode, wakeup or sleep mode
//	@paramters:	MODE_WAKEUP, or MODE_SLEEP
//	@return:	void
//-------------------------------------------------------------------
void LoraNode_Wake_Sleep(LoraNode_SleepMode_T mode)
{
	if (mode == MODE_WAKEUP)
	{
		/* wake signal, high for module wakeup, low for module sleep */	
		if (LoraNode_IO_State(LORANODE_WAKE_PIN) != GPIO_PIN_SET)
		{
			/* wake module first, and wait 10 ms */
			LORANODE_WAKE_HIGH();
			
			LoraNode_Delay(20);				
		}
	}
	
	if (mode == MODE_SLEEP)
	{
		if (LoraNode_IO_State(LORANODE_WAKE_PIN) != GPIO_PIN_RESET)
		{
			LORANODE_WAKE_LOW();			
		}
	}
}




static void LoraNode_Reset(void)
{
	LORANODE_NRST_LOW();	
	
	LoraNode_Delay(15);	//15ms
	
	LORANODE_NRST_HIGH();	
}

//-------------------------------------------------------AT --------------------------------


//
//	@function:	get the lorawan module version
//	@return:	the point of version information
//
char *LoraNode_GetVer(void)
{
	char ASK_VER[] = "AT+VER?\r\n";
	char *temp = "+VER:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(ASK_VER);
	
	LoraNode_Read(AT_Data_buf);	/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		return ptr;	
	}

	return ptr;
}

/*
**	@function:	get deveui 
**	@return:	the point of the deveui
*/
void LoraNode_GetDevEUI(Node_Info *LoRa_temp)
{
	char GetDevEUI[] = "AT+DEVEUI?\r\n";
	char *temp = "+DEVEUI:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetDevEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */	
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<8;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->DevEUI[i] = temp1*16 + temp2;
		}
	}
	
}


/*
**	@function:	get appeui 
**	@return:	the point of the appeui
*/
void LoraNode_GetAppEUI(Node_Info *LoRa_temp)
{
	char GetAPPEUI[] = "AT+APPEUI?\r\n";
	char *temp = "+APPEUI:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAPPEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<8;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppEUI[i] = temp1*16 + temp2;
		}
	}
	
}

/*
**	@function:	get appkey 
**	@return:	the point of the response of appkey
*/
void LoraNode_GetAppKey(Node_Info *LoRa_temp)
{
	char GetAPPKEY[] = "AT+APPKEY?\r\n";	
	char *temp = "+APPKEY:";
	char *ptr = NULL;	

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAPPKEY);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 8+(3*i))+1);
			temp2 = *((ptr + 8+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppKEY[i] = temp1*16 + temp2;
		}
	}
	
}

/*
**	@function:	get devaddr 
**	@return:	the point of the Devaddr
*/
void LoraNode_GetDevAddr(Node_Info *LoRa_temp)
{
	char GetDEVAddr[] = "AT+DEVADDR?\r\n";
	char *temp = "+DEVADDR:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetDEVAddr);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<4;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->DevADDR[i] = temp1*16 + temp2;
		}
	}
	
}


/*
**	@function:	get appskey 
**	@return:	the point of the response of appskey
*/
void LoraNode_GetAppSKey(Node_Info *LoRa_temp)
{
	char GetAppSKey[] = "AT+APPSKEY?\r\n";
	char *temp = "+APPSKEY:";
	char *ptr = NULL;

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetAppSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->AppSKEY[i] = temp1*16 + temp2;
		}
	}
}


/*
**	@function:	get nwkskey 
**	@return:	the point of the response of nwkskey
*/
void LoraNode_GetNwkSKey(Node_Info *LoRa_temp)
{
	char GetNwkSKey[] = "AT+NWKSKEY?\r\n";	
	char *temp = "+NWKSKEY:";
	char *ptr = NULL;	

	uint8_t temp1=0,temp2=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetNwkSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		for(uint8_t i=0;i<16;i++)
		{
			temp1 = *((ptr + 9+(3*i))+1);
			temp2 = *((ptr + 9+(3*i))+2);
			if(temp1 > 0x40)
			{
				temp1 = temp1 - 55;
			}else{
					temp1 = temp1 - 48;
				}
			if(temp2 > 0x40)
			{
				temp2 = temp2 - 55;
			}else{
					temp2 = temp2 - 48;
				}
			LoRa_temp->NwkSKEY[i] = temp1*16 + temp2;
		}
	}

}


/*
**	@function:	get uplink value
**	@return:	the point of the uplink buffer
*/
void LoraNode_GetNetMode(Node_Info *LoRa_temp)
{
	char GetNetMode[] = "AT+OTAA?\r\n";
	char *temp = "+OTAA:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetNetMode);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		if(AT_Data_buf[18] == 0x30)
		{
			LoRa_temp->NET_Mode = 0;
		}
		if(AT_Data_buf[18] == 0x31)
		{
			LoRa_temp->NET_Mode = 1;
		}
	}
	
}



void LoraNode_GetConfirm(Node_Info *LoRa_temp)
{
	char GetConfirm[] = "AT+CONFIRM?\r\n";
	char *temp = "+CONFIRM:";
	char *ptr = NULL;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetConfirm);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		if(AT_Data_buf[24] == 0x30)
		{
			LoRa_temp->Confirm= 0;
		}
		if(AT_Data_buf[24] == 0x31)
		{
			LoRa_temp->Confirm= 1;
		}
	}
	
}



void LoraNode_GetState(Send_Info *LoRa_temp)
{
	char GetConfirm[] = "AT+STATUS?\r\n";
	char *temp = "+STATUS:";
	char *ptr = NULL;
	uint8_t dec=0,dec1=0,dec2=0,dec3=0,dec4=0,dec5=0,dec6=0,dec7=0;
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GetConfirm);
	HAL_Delay(100);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		dec = StrToHex(AT_Data_buf[11]);
		dec1 = StrToHex(AT_Data_buf[12]);

		LoRa_temp->Up_Result = dec*16 + dec1;

		dec = StrToHex(AT_Data_buf[14]);
		dec1 = StrToHex(AT_Data_buf[15]);

		LoRa_temp->Up_CH = dec*16 + dec1;

		dec = StrToHex(AT_Data_buf[17]);
		dec1 = StrToHex(AT_Data_buf[18]);

		LoRa_temp->Up_RATE = dec*16 + dec1;

		dec = StrToHex(AT_Data_buf[20]);
		dec1 = StrToHex(AT_Data_buf[21]);

		LoRa_temp->Up_DB = dec*16 + dec1;

		dec = StrToHex(AT_Data_buf[68]);
		dec1 = StrToHex(AT_Data_buf[69]);
		dec2 = StrToHex(AT_Data_buf[71]);
		dec3 = StrToHex(AT_Data_buf[72]);

		LoRa_temp->Up_Cache = dec*16*256 + dec1*256 +dec2*16 + dec3 ;

		dec = StrToHex(AT_Data_buf[74]);
		dec1 = StrToHex(AT_Data_buf[75]);
		dec2 = StrToHex(AT_Data_buf[77]);
		dec3 = StrToHex(AT_Data_buf[78]);

		LoRa_temp->Resend = dec*16*256 + dec1*256 +dec2*16 + dec3 ;

		dec = StrToHex(AT_Data_buf[119]);
		dec1 = StrToHex(AT_Data_buf[120]);
		dec2 = StrToHex(AT_Data_buf[122]);
		dec3 = StrToHex(AT_Data_buf[123]);
		dec4 = StrToHex(AT_Data_buf[125]);
		dec5 = StrToHex(AT_Data_buf[126]);
		dec6 = StrToHex(AT_Data_buf[128]);
		dec7 = StrToHex(AT_Data_buf[129]);

		LoRa_temp->Up_Link = dec*4096*65536 + dec1*256*65536 +dec2*16*65536 + dec3*256*256 + dec4*16*256 + dec5*256 +dec6*16 + dec7;

		
	}
	
}


/*
**	@function:	set gpio state
**	@paramters:	pin, 0, 1, 2, 3, 255; state 0, 1
**	@return:	0 success, 1 bad paramter
*/
uint32_t LoraNode_SetGPIO(uint32_t pin, uint32_t state)
{	
	uint8_t GPIO[20] = "AT+GPIO=";
	uint8_t buf[5] = {0};
	char *temp = "OK";
	char *ptr = (char*)GPIO;
	
	Int2Str(buf, pin);
	strcat((char *)GPIO, (char *)buf);
	
	while(*++ptr);	//get the end
	*ptr++ = ',';

	memset(buf,0,5);
	Int2Str(buf, state);
	StringCat(GPIO, buf);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(GPIO);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if((ptr = StringStr((char *)AT_Data_buf, temp)) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set net mode
**	@paramters: 0, abp mode; 1, otaa mode
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetNetMode(LoraNode_NetMode_T mode)
{	
	uint8_t NetMode[20] = "AT+OTAA=";
	uint8_t buf[10] = {0};
	char *temp = "OK";
	
	Int2Str(buf, mode);
	StringCat(NetMode, buf);

	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(NetMode);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set debug value
**	@paramters: 0, release; 1, debug
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetDebug(uint32_t value)
{
	uint8_t SetDebug[20] = "AT+DEBUG=";
	uint8_t buf[10] = {0};
	char *temp = "OK";
	
	Int2Str(buf, value);
	StringCat(SetDebug, buf);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetDebug);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


/*
**	@function:	set appeui 
**	@paramters: appeui 8 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppEUI(uint8_t *dev)
{
	uint8_t SetAppEUI[50] = "AT+APPEUI=";
	char *temp = "OK";
	
	StringCat(SetAppEUI, dev);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppEUI);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;

}

/*
**	@function:	set appkey, only in otaa mode
**	@paramters: appkey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppKey(uint8_t *key)
{
	uint8_t SetAppKey[80] = "AT+APPKEY="; 
	char *temp = "OK";
	
	StringCat(SetAppKey, key);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set appskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetAppSKey(uint8_t *skey)
{
	uint8_t SetAppSKey[80] = "AT+APPSKEY=";
	char *temp = "OK";
	
	StringCat(SetAppSKey, skey);
		
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetAppSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}

/*
**	@function:	set nwkskey, only in abp mode
**	@paramters: appskey 16 bytes
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_SetNwkSKey(uint8_t *nwk_skey)
{
	uint8_t SetNwkSKey[80] = "AT+NWKSKEY=";
	char *temp = "OK";
	
	StringCat(SetNwkSKey, nwk_skey);
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SetNwkSKey);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{
		return 0;
	}
	
	return -1;
}


/*
**	@function:	save config
**	@return:	0 success, -1 bad paramter
*/
uint32_t LoraNode_Save(void)
{
	char SAVE[] = "AT+SAVE\n\r";
	char *temp = "OK";
	
	memset(AT_Data_buf,0,RXLEN);	       /* clear the buffer and wPoint */
	
	LoraNode_Send_AT(SAVE);
	
	LoraNode_Read(AT_Data_buf);				/* receive the response of this command */
	
	if(StringStr((char *)AT_Data_buf, temp) != NULL)
	{	
		return 0;
	}
	
	return -1;
}



void LoraNode_Send_AT(char *at_buf)
{
	LPUART1_SendString(at_buf);
}
		
void LoraNode_Read(uint8_t *str)
{
	uint32_t i = 0;

	LoraNode_Delay(100);

	if(LPUsart1_RX.receive_flag == 1)
	{
		for(i=0 ; i < LPUsart1_RX.rx_len; i++)
		{
			str[i] = LPUsart1_RX.RX_Buf[i];
		}
		
		LPUsart1_RX.receive_flag = 0;
		LPUsart1_RX.rx_len = 0;
	}
}


//-------------------------------------------------------------------------------------------

int8_t LoRaNode_LowPower_Send(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	int8_t ret = 0;
	
	//----唤醒模块 ----
	LoraNode_Wake_Sleep(MODE_WAKEUP);

	LoraNode_Delay(10);

	//----判断模块处于透传模式
	if(LORANODE_MODE_STATUS == 0)	
	{
		//----等待 BUSY 引脚处于空闲状态(高电平)，等待时可以加超时判断
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -1;             // 错误 -1  : 模块没有准备好
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//----通过串口1发送所要发送的数据
		LPUsart1SendData(buffer, len);

		//----等待 BUSY 引脚处于工作状态(低电平)，等待时可以加超时判断
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_SET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -2;             // 错误 -2  : 模块没有接收到串口发送的数据，或者模块没有工作
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//-----系统在此时进入休眠----------
		Send_Sate_Sign = 1;          //将标志位至成发送状态
		Set_RTC_WakeUp(40);
		LoRaNode_Send_To_Sleep();

		if(Send_Sate_Sign == 11)
		{
			Send_Sate_Sign = 0;
			ret = -3;             // 错误 -2  : 模块发送数据出现异常
			return ret;
		}

		if(Send_Sate_Sign == 12)
		{
			Send_Sate_Sign = 0;
			ret = 2;             // 模块有接收到来至服务器的数据
			return ret;
		}

		if(Send_Sate_Sign == 13)
		{
			Send_Sate_Sign = 0;

			if(CONFIRM == 0)
			{
				ret = 1;             // 发送成功
				return ret;
			}else
				{
					while(LoraNode_IO_State(LORANODE_STAT_PIN) == GPIO_PIN_RESET)
					{
						if(Time_Out_Break(1000) == 1)
						{
							ret = -4;             // 错误 -4  : 模块没有接收到串口发送的数据，或者模块没有工作
							return ret;
						}
					}
					TimeOut_Sign = 0;
					ret = 1;
					return ret;
				}
			
		}
		
	}
}

//--------------------------------功能函数---------------------------------------------------

uint32_t LoraNode_Write_Receive_Bytes(uint8_t *buffer, int32_t len,uint8_t CONFIRM)
{
	uint32_t ret = 0;
	
	//清除接收标志
	//Receive_Log = 0; 
	
	//----唤醒模块 ----
	LoraNode_Wake_Sleep(MODE_WAKEUP);

	LoraNode_Delay(10);

	//----判断模块处于透传模式
	if(LORANODE_MODE_STATUS == 0)	
	{
		//----等待 BUSY 引脚处于空闲状态(高电平)，等待时可以加超时判断
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -1;             // 错误 -1  : 模块没有准备好
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//----通过串口1发送所要发送的数据
		LPUsart1SendData(buffer, len);

		//----等待 BUSY 引脚处于忙状态(低电平)，等待时可以加超时判断
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_SET)
		{
			if(Time_Out_Break(2000) == 1)
			{
				ret = -2;             // 错误 -2  : 模块没有接收到串口发送的数据，或者模块没有工作
				return ret;
			}
		}
		TimeOut_Sign = 0;

		//接收标志 置1 开始等待接收窗口接收数据
		//Receive_Log = 1;
		//Receive_Lenth = 0;

		//----等待数据发送完成  BUSY 引脚回到空闲状态(高电平)，等待时可以加超时判断
		while(LoraNode_IO_State(LORANODE_BUSY_PIN) == GPIO_PIN_RESET)
		{
			if(Time_Out_Break(40000) == 1)
			{
				ret = -3;             // 错误 -2  : 模块没有接收到串口发送的数据，或者模块没有工作
				return ret;
			}
		}
		TimeOut_Sign = 0;

		if(CONFIRM == 1)
		{
			//判断下网络状态，确保发送是成功的
			while(LoraNode_IO_State(LORANODE_STAT_PIN) == GPIO_PIN_RESET)
			{
				if(Time_Out_Break(2000) == 1)
				{
					ret = -4;             // 错误 -2  : 模块没有接收到串口发送的数据，或者模块没有工作
					return ret;
				}
			}
			TimeOut_Sign = 0;
		}
	}

	ret = 1;
	return ret;
}

/* Over-the-Air Activation */
void LoraNode_OTAA_Config(void)
{
	//	调用模式切换函数 将模块切换至命令模式工作
	LoraNode_Mode_Set(MODE_CMD);
	
	//调用网络模式设置函数 将模块配置成 OTAA 方式入网
	LoraNode_SetNetMode(NET_OTAA);

	//调用函数配置模块的 APPEUI
	LoraNode_SetAppEUI(appEui);
	
	LoraNode_Delay(30);
	
	//调用函数配置模块的 APPKEY
	LoraNode_SetAppKey(appKey);
	
	LoraNode_Delay(30);
	
	//调用保存配置函数
	LoraNode_Save();
	
	LoraNode_Delay(500);
	
	//调用重启模块函数
	LoraNode_Reset();
	
	LoraNode_Delay(500);
	
//	Uart_Printf("\n 配置已完成，请重启查看模块信息是否生效 \n");
	
}

void LoraNode_ABP_Config(void)
{	
	//mode pin high
	LoraNode_Mode_Set(MODE_CMD);
	
	//at+otaa=0
	LoraNode_SetNetMode(NET_ABP);
	
	LoraNode_Delay(30);
	
	//appskey
	LoraNode_SetAppSKey(appSKey);	//set default value
	
	LoraNode_Delay(30);
	
	//nwk_skey
	LoraNode_SetNwkSKey(nwkSKey);
	
	LoraNode_Delay(30);
	
	//devaddr
	//LoraNode_SetDevAddr(devAddr);
	
	LoraNode_Delay(50);
	
	//save
	LoraNode_Save();
	
	/*delay for the whole response by USART2 */
	LoraNode_Delay(500);
	
	//reset
	LoraNode_Reset();

	LoraNode_Delay(500);

}



void GET_LoraNode_Info(Node_Info *lora_temp)
{
	LoraNode_Mode_Set(MODE_CMD);	
	
	LoraNode_Wake_Sleep(MODE_WAKEUP);
	
	LoraNode_Delay(100);
	
	/* read some info after lora module initial */	
	LoraNode_GetDevEUI(lora_temp);
	
	LoraNode_Delay(10);
	
	LoraNode_GetAppEUI(lora_temp);	
	
	LoraNode_Delay(10);
	
	LoraNode_GetAppKey(lora_temp);	

	LoraNode_Delay(10);
	
	LoraNode_GetDevAddr(lora_temp);	

	LoraNode_Delay(10);	
	
	LoraNode_GetAppSKey(lora_temp);

	LoraNode_Delay(10);	
	
	LoraNode_GetNwkSKey(lora_temp);

	LoraNode_Delay(10);	

}


void LoraNode_STA_Check(Node_Info *LoRa_temp)
{
	static uint8_t online_log = 0;
	if(LORANODE_STAT_STATUS == 0)
	{
		LoRa_temp->ONline = 0;
	}else
		{
			LoRa_temp->ONline = 1;
		}

	if(LORANODE_BUSY_STATUS == 0)
	{
		LoRa_temp->BUSY = 0;
	}else
		{
			LoRa_temp->BUSY = 1;
		}

	if(LoRa_temp->NET_Mode == 1)
	{
		if(LoRa_temp->ONline == 1)
		{
			if(LoRa_temp->BUSY == 1)
			{
				LoRa_temp->Join = 1;
			}
		}else{
				LoRa_temp->Join = 0;
			 }
	}else if(LoRa_temp->NET_Mode == 0)
		{
			if(LoRa_temp->BUSY == 1)
			{
				LoRa_temp->Join = 1;
			}else{
				LoRa_temp->Join = 0;
			 }
		}

	if(LoRa_temp->Join != online_log )
	{
		if(LoRa_temp->Join == 1)
		{
			DEBUG_Printf("\r\n--> 网络已连接\r\n");
		}else{
				//Uart_Printf("\r\n 网关已断开连接 \r\n");
			}
	}
			
		online_log = LoRa_temp->Join;
}


//--------------------------------------------------------------------------------------------

uint32_t LoraNode_IO_State(uint32_t pin)
{
	return HAL_GPIO_ReadPin(LORANODE_GPIO_PORT, pin);	
}

void LoraNode_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}


char *StringStr(char *str, char *dest)
{
#define STR_BUFF_LEN	0x100
	int i = STR_BUFF_LEN;
	char *cp = str;
	char *s1, *s2;
	
	if (*dest == 0)
	{
		return str;
	}
	
//	while(*str)
	while(i--)
	{		
		s1 = cp;
		s2 = dest;
		
		while((*s1 == *s2) && *s1 && *s2)
		{
			s1++;
			s2++;
		}
		if(!*s2)
			return cp;
		cp++;
	}
	
	return NULL;
}

uint8_t StrToHex(uint8_t temp)
{
	uint8_t ret=0;
	if(temp>=48 && temp<=57)
	{
		ret = temp - 48;
		return ret;
	}

	if(temp>=65 && temp<=70)
	{
		ret = temp - 55;
		return ret;
	}

	if(temp>=97 && temp<=102)
	{
		ret = temp - 87;
		return ret;
	}
}




///-----------------------------------------------
void Printf_LoRaNode_Info(Node_Info LoRa_temp)
{
	DEBUG_Printf("DEVEUI: %x %x %x %x %x %x %x %x \r\n",LoRa_temp.DevEUI[0],LoRa_temp.DevEUI[1],LoRa_temp.DevEUI[2],LoRa_temp.DevEUI[3],LoRa_temp.DevEUI[4],LoRa_temp.DevEUI[5],LoRa_temp.DevEUI[6],LoRa_temp.DevEUI[7]);
	DEBUG_Printf("APPEUI: %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppEUI[0],LoRa_temp.AppEUI[1],LoRa_temp.AppEUI[2],LoRa_temp.AppEUI[3],LoRa_temp.AppEUI[4],LoRa_temp.AppEUI[5],LoRa_temp.AppEUI[6],LoRa_temp.AppEUI[7]);
	DEBUG_Printf("APPKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppKEY[0],LoRa_temp.AppKEY[1],LoRa_temp.AppKEY[2],LoRa_temp.AppKEY[3],LoRa_temp.AppKEY[4],LoRa_temp.AppKEY[5],LoRa_temp.AppKEY[6],LoRa_temp.AppKEY[7],
																			 		   LoRa_temp.AppKEY[8],LoRa_temp.AppKEY[9],LoRa_temp.AppKEY[10],LoRa_temp.AppKEY[11],LoRa_temp.AppKEY[12],LoRa_temp.AppKEY[13],LoRa_temp.AppKEY[14],LoRa_temp.AppKEY[15]);
	DEBUG_Printf("APPSKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.AppSKEY[0],LoRa_temp.AppSKEY[1],LoRa_temp.AppSKEY[2],LoRa_temp.AppSKEY[3],LoRa_temp.AppSKEY[4],LoRa_temp.AppSKEY[5],LoRa_temp.AppSKEY[6],LoRa_temp.AppSKEY[7],
																			 		   LoRa_temp.AppSKEY[8],LoRa_temp.AppSKEY[9],LoRa_temp.AppSKEY[10],LoRa_temp.AppSKEY[11],LoRa_temp.AppSKEY[12],LoRa_temp.AppSKEY[13],LoRa_temp.AppSKEY[14],LoRa_temp.AppSKEY[15]);
	DEBUG_Printf("NWKSKEY: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x \r\n",LoRa_temp.NwkSKEY[0],LoRa_temp.NwkSKEY[1],LoRa_temp.NwkSKEY[2],LoRa_temp.NwkSKEY[3],LoRa_temp.NwkSKEY[4],LoRa_temp.NwkSKEY[5],LoRa_temp.NwkSKEY[6],LoRa_temp.NwkSKEY[7],
																			 		   LoRa_temp.NwkSKEY[8],LoRa_temp.NwkSKEY[9],LoRa_temp.NwkSKEY[10],LoRa_temp.NwkSKEY[11],LoRa_temp.NwkSKEY[12],LoRa_temp.NwkSKEY[13],LoRa_temp.NwkSKEY[14],LoRa_temp.NwkSKEY[15]);
	DEBUG_Printf("DEVADDR: %x %x %x %x \r\n",LoRa_temp.DevADDR[0],LoRa_temp.DevADDR[1],LoRa_temp.DevADDR[2],LoRa_temp.DevADDR[3]);
}
//-------------------------------------------------

