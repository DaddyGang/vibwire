#include "key.h"

KEY App_Key;
KEY *App_Key_str = &App_Key;

extern DEVICE_SATE Device_Sate;
extern DEVICE_SATE *Device_Sate_str;

extern uint8_t Send_Sate_Sign;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t tt = 0;
 	if(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
 	{
 		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
	 	{
	 		App_Key.A_KEY1 = 1;
	 		while(HAL_GPIO_ReadPin(GPIOC,A_K1) == 0)
	 		{
	 			HAL_Delay(1);
	 			if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
 				{
 					tt ++;
					if(tt > 400)    //  组合按键超过3s
				 	{
						App_Key.A_KEY1_KEY2 = 1;
						App_Key.A_KEY1 = 0;
					}
	 			}
	 		}
	 	}
 	}

	if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
 	{
 		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 0)
	 	{
	 		App_Key.A_KEY2 = 1;
	 	}
 	}

	KEY_DO(App_Key_str,Device_Sate_str);

	
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == 1)
 	{
 		HAL_Delay(20);
		if(HAL_GPIO_ReadPin(GPIOC,A_K2) == 1)
	 	{
			if(Send_Sate_Sign == 1)
			{
				Send_Sate_Sign = 13;
			}
		}
	}

}


void KEY_DO(KEY *key_temp,DEVICE_SATE *Device_Sate_temp)
{
	if(key_temp->A_KEY1 == 1 )
	{
		switch(*Device_Sate_temp)
		{		
			case START_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
                        
                        case CMD_MODE:
                                *Device_Sate_temp = TRANSPARENT_MODE;
				break;

			case TRANSPARENT_MODE:
				*Device_Sate_temp = TIMESEND_MODE;
				break;

			case TIMESEND_MODE:
				*Device_Sate_temp = LOWPOWER_MODE;
				break;

			case LOWPOWER_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;

			case TRANSPARENT_DEBUG_MODE:
				*Device_Sate_temp = TIMESEND_MODE;
				break;

			case TIMESEND_DEBUG_MODE:
				*Device_Sate_temp = LOWPOWER_MODE;
				break;
			
			case LOWPOWER_DEBUG_MODE:
				*Device_Sate_temp = CMD_MODE;
				break;
		}

		key_temp->A_KEY1 = 0;	
	}

	if(key_temp->A_KEY2 == 1)
	{
		if(*Device_Sate_temp == TRANSPARENT_MODE)
		{
			*Device_Sate_temp = TRANSPARENT_DEBUG_MODE;
		}else if(*Device_Sate_temp == TRANSPARENT_DEBUG_MODE)
			{
				*Device_Sate_temp = TRANSPARENT_MODE;
			}

		
		if(*Device_Sate_temp == TIMESEND_MODE)
		{
			*Device_Sate_temp = TIMESEND_DEBUG_MODE;
		}else if(*Device_Sate_temp == TIMESEND_DEBUG_MODE)
			{
				*Device_Sate_temp = TIMESEND_MODE;
			}

		key_temp->A_KEY2 = 0;
	}

	if(key_temp->A_KEY1_KEY2 == 1)
	{
		if(*Device_Sate_temp == START_MODE)
		{
			*Device_Sate_temp = TEST_MODE;
		}

		key_temp->A_KEY1_KEY2 = 0;
	}
}

