#include "app.h"
#include "hdc1000.h"
#include "usart.h"
#include "opt3001.h"
#include "MPL3115.h"
#include "mma8451.h"
#include "lorawan.h"
#include <string.h>
#include "Production_Test.h"
#include "LowPower.h"
#include "vibwire.h"


#define SENDTIME 60000

uint8_t TimeOut_Sign = 0;  //��ʱ�����־
uint8_t Send_Sate_Sign = 0;
uint8_t KFB_B_EN = 0;      //������ B ����ʹ�ܱ�־

DEVICE_SATE Device_Sate = START_MODE;
DEVICE_SATE *Device_Sate_str = &Device_Sate;

GPS_DATA GPS_Data;
GPS_DATA *GPS_Data_str = &GPS_Data;

SENSOR Sensor_Data;
SENSOR *Sensor_Data_str = &Sensor_Data;

extern USART_RECEIVETYPE Usart2_RX;
extern USART_RECEIVETYPE Usart1_RX;
extern USART_RECEIVETYPE LPUsart1_RX;

extern uint8_t Gps_START[12];
extern uint8_t Gps_STOP[12];

extern Node_Info LoRa_Node;
extern Node_Info *LoRa_Node_str;

extern Send_Info LoRa_Send;
extern Send_Info *LoRa_Send_str;


uint8_t SS_Data[32]={0};

uint8_t GPS_sate = 0;

uint32_t EV_Send_Time = 60000;
uint32_t GPS_Send_Time = 60000;

uint8_t Test_Data[64]={0};
uint8_t Test_len = 0;
uint8_t Test_ON = 0;


//-----------------�û�Ӧ�ã��豸�����л�--------------------------
void User_App(void)
{
	static uint8_t start = 0 ,lowpoer = 0;
	int8_t send_log = 0;
	uint16_t i = 0;
	uint16_t shu = 0;
	
	switch(Device_Sate)
	{
		case START_MODE:
			{
				if(start != 1)
				{
					DEBUG_Printf("****************** ---LORAWAN_NODE ��������� V1.10--- ******************\r\n");
					DEBUG_Printf("��ͨ�� ������ K1 ��������ģʽѡ��: \r\n");
					DEBUG_Printf(" 1������ģʽ\r\n");
					DEBUG_Printf(" 2��͸��ģʽ\r\n");
					DEBUG_Printf(" 3����ʱ���ʹ���������\r\n");
					DEBUG_Printf(" 4���͹��Ķ�ʱ����ģʽ\r\n");
					DEBUG_Printf(" ����ʾ: ��2��3ģʽ�¿�ͨ�� K2 �������򿪺͹ر�ģ��ĵ�����Ϣ��� \r\n");
					DEBUG_Printf(" ����ʾ: ��Ҫʹ��Һ����GPS���ܣ���������GPS�����롮YES��(����״̬����Ч)�� \r\n");
					DEBUG_Printf("         �ر������롮OFF����ϵͳ�ᱣ��GPSʹ��״̬������ÿ���ϵ��ѡ�� \r\n");
					DEBUG_Printf("****************** ----------------------------------- ******************\r\n");
					DEBUG_Printf(" ����2017.12.24\r\n");
                                        start = 1;
					KFB_B_EN = *((__IO uint8_t*)(0x08080000));
					Test_ON = *((__IO uint8_t*)(0x08080002));
					if(Test_ON == 1)
					{
	
						DEBUG_Printf(" -->���������ڴ��ڲ���ģʽ \r\n");
					}
					
					EV_Send_Time = *((__IO uint32_t*)(0x08080004));
					if(EV_Send_Time == 0)
					{
						EV_Send_Time = 60000;
						EEPROM_WriteOneByte(0x00000004,0x60);
						EEPROM_WriteOneByte(0x00000005,0xea);
						EEPROM_WriteOneByte(0x00000006,0x00);
						EEPROM_WriteOneByte(0x00000007,0x00);
					}

					if(GPS_Send_Time == 0)
					{
						GPS_Send_Time = 60000;
						EEPROM_WriteOneByte(0x00000008,0x60);
						EEPROM_WriteOneByte(0x00000009,0xea);
						EEPROM_WriteOneByte(0x0000000a,0x00);
						EEPROM_WriteOneByte(0x0000000b,0x00);
					}
					
					Test_len = *((__IO uint8_t*)(0x0808000c));

					for(uint8_t j = 0;j<Test_len;j++)
					{
						Test_Data[j] = *((__IO uint8_t*)(0x0808000d + j));
					}
				}

				CMD_Check();

				if(KFB_B_EN == 1)
				{
					GPS_ON;
					Usart1SendData(Gps_START,12);
					DEBUG_Printf("-->���GPSͨ����");
					while(Usart1_RX.receive_flag == 0)
					{
						if(Time_Out_Break(5000)==1)
						{
							break;
						}
						HAL_Delay(1);
						i++;
						if(i >= 500)
						{
							i = 0;
							DEBUG_Printf("��");
						}
					}
					TimeOut_Sign = 0;
					if(Usart1_RX.receive_flag == 1)
					{
						KFB_B_EN = 2;
						DEBUG_Printf("\r\n-->GPSͨ������,�����ѿ���\r\n");
						EEPROM_WriteOneByte(0x00000000,1);
					}else if(Usart1_RX.receive_flag == 0)
						{
							KFB_B_EN = 0;
							DEBUG_Printf("\r\n-->nGPSͨ��ʧ�ܣ���ȷ��GPS�������Ӻ�����\r\n");
						}
					GPS_OFF;
					Usart1SendData(Gps_STOP,12);
					HAL_Delay(100);
					Usart1_RX.receive_flag = 0;
				}
			}
		break;
		
		case TEST_MODE:
			{
				if(start != 99)
				{
					DEBUG_Printf("���ѽ����Լ����ģʽ��\r\n");
					start = 99;
				}

				Test_task();
                                DEBUG_Printf("\r\n-->���������...\r\n");
                                VIB_Test();
                                DEBUG_Printf("\r\n-->������Խ���\r\n");

				DEBUG_Printf("���Լ��������ɡ�\r\n");

				LED1_ON;
				LED2_ON;
				LED3_ON;
				LED4_ON;

				HAL_Delay(1000);

				LED1_OFF;
				LED2_OFF;
				LED3_OFF;
				LED4_OFF;
				
				Device_Sate = START_MODE;
			}
		break;

		case CMD_MODE:
			{
				if(start != 2)
				{
					LED1_ON;
					LED2_OFF;
					LED3_OFF;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);
					
					DEBUG_Printf("���ѽ�������ģʽ��\r\n");
					start = 2;
					LoraNode_Mode_Set(MODE_CMD);
				}

				if(LPUsart1_RX.receive_flag == 1)
			  	{
			  		LPUsart1_RX.receive_flag = 0;
			  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			 	}

				if(Usart2_RX.receive_flag == 1)
			  	{
			  		Usart2_RX.receive_flag = 0;
			  		LPUsart1SendData(Usart2_RX.RX_Buf,Usart2_RX.rx_len);
			 	}
			}
		break;

		case TRANSPARENT_MODE:
			{
				if(start != 3)
				{
					LED1_OFF;
					LED2_ON;
					LED3_OFF;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);

					DEBUG_Printf("���ѽ���͸��ģʽ��\r\n");
					start = 3;
					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_SetDebug(0);
					LoraNode_GetNetMode(LoRa_Node_str);
					LoraNode_GetConfirm(LoRa_Node_str);
					HAL_Delay(10);
					LoraNode_Mode_Set(MODE_TRANSPARENT);
					if(LoRa_Node.NET_Mode ==1)
					{
						DEBUG_Printf("--> ģ������ģʽ : OTAA \r\n");
						DEBUG_Printf("--> ģ�� Join �С�");
						if(LoRa_Node.Join == 1)
						{
							DEBUG_Printf("\r\n--> ����������\r\n");
						}
					}else
						{DEBUG_Printf("--> ģ������ģʽ : ABP");}
				}

				while(LoRa_Node.Join == 0)
				{
					DEBUG_Printf("��");
					LoraNode_STA_Check(LoRa_Node_str);
					LED2_ON;
					HAL_Delay(300);
					LED2_OFF;
					HAL_Delay(300);
					LED2_ON;

					if(Device_Sate != TRANSPARENT_MODE)
					{break;}
				}

				if(Usart2_RX.receive_flag == 1)
			  	{
			  		Usart2_RX.receive_flag = 0;
				
					if(LoRa_Node.Join == 1)
					{
						LED4_ON;
				  		send_log = LoraNode_Write_Receive_Bytes(Usart2_RX.RX_Buf,Usart2_RX.rx_len,LoRa_Node.Confirm);

						if(send_log == 1)
						{
							DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
						}
						
						if(send_log == -1)
						{
							DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
						}
	
						if(send_log == -2)
						{
							DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
						}
	
						if(send_log == -3)
						{
							DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
						}
	
						if(send_log == -4)
						{
							DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
						}
						
						LED4_OFF;
						if(send_log != 1)
						{
							LoraNode_Mode_Set(MODE_CMD);
							HAL_Delay(100);
							LoraNode_GetState(LoRa_Send_str);
							HAL_Delay(100);
							LoraNode_Mode_Set(MODE_TRANSPARENT);

							DEBUG_Printf("--> ����ʧ����Ϣ:\r\n");

							switch(LoRa_Send.Up_Result)
							{
								case 0:
									{
										DEBUG_Printf("        ������״̬: �����ݲ���\r\n");
									}
								break;

								case 1:
									{
										DEBUG_Printf("        ������״̬: ���ݷ�����\r\n");
									}
								break;

								case 2:
									{
										DEBUG_Printf("        ������״̬: ���ݷ���ʧ��\r\n");
									}
								break;

								case 3:
									{
										DEBUG_Printf("        ������״̬: ���ݷ��ͳɹ�\r\n");
									}
								break;

								case 4:
									{
										DEBUG_Printf("        ������״̬: join �ɹ�\r\n");
									}
								break;

								case 5:
									{
										DEBUG_Printf("        ������״̬: join ʧ��\r\n");
									}
								break;
							}
							
							DEBUG_Printf("        �������ŵ�:%d \r\n",LoRa_Send.Up_CH);
							DEBUG_Printf("        ����������:%d \r\n",LoRa_Send.Up_RATE);
							DEBUG_Printf("        �����й���:%d \r\n",LoRa_Send.Up_DB);
							DEBUG_Printf("        �����м���ֵ:%d \r\n",LoRa_Send.Up_Link);
							DEBUG_Printf("        �����л�����ֵ:%d \r\n",LoRa_Send.Up_Cache);
							DEBUG_Printf("        �������ط�����:%d \r\n",LoRa_Send.Resend);
						}

						if(LPUsart1_RX.receive_flag == 1)
					  	{
					  		LPUsart1_RX.receive_flag = 0;
							DEBUG_Printf("--> ģ����յ���������(�ַ�):");
							DEBUG_Printf("    <");
					  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
							DEBUG_Printf("> \r\n");

							DEBUG_Printf("--> ģ����յ���������(16����):");
							DEBUG_Printf("    <");
							for(uint8_t q=0;q<LPUsart1_RX.rx_len;q++)
							{
								if(LPUsart1_RX.RX_Buf[q]<10)
								{DEBUG_Printf("0");}
								DEBUG_Printf("%x",LPUsart1_RX.RX_Buf[q]);
							}
							DEBUG_Printf("> \r\n");
					 	}

					}
					else{DEBUG_Printf("--> ����δ����\r\n");}
			 	}
			}
		break;

		case TRANSPARENT_DEBUG_MODE:
			{
				if(start != 4)
				{
					LED1_OFF;
					LED2_ON;
					LED3_OFF;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);

					DEBUG_Printf("\r\n���ѽ���͸��ģʽ�������Ѵ򿪡�\r\n");
					start = 4;
					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_SetDebug(1);
					LoraNode_GetNetMode(LoRa_Node_str);
					HAL_Delay(10);
					LoraNode_Mode_Set(MODE_TRANSPARENT);
				}

				while(LoRa_Node.Join == 0)   // �״ν�������ȷ��
				{
					LoraNode_STA_Check(LoRa_Node_str);
					LED2_ON;
					HAL_Delay(300);
					LED2_OFF;
					HAL_Delay(300);
					LED2_ON;

					if(Device_Sate != TRANSPARENT_DEBUG_MODE)
					{break;}

					if(LPUsart1_RX.receive_flag == 1)
				  	{
				  		LPUsart1_RX.receive_flag = 0;
				  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
				 	}
				}

				if(LPUsart1_RX.receive_flag == 1)
			  	{
			  		LPUsart1_RX.receive_flag = 0;
			  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			 	}

				if(Usart2_RX.receive_flag == 1)
			  	{
			  		Usart2_RX.receive_flag = 0;
			
					if(LoRa_Node.Join == 1)
					{
						LED4_ON;
				  		send_log = LoraNode_Write_Receive_Bytes(Usart2_RX.RX_Buf,Usart2_RX.rx_len,LoRa_Node.Confirm);
						if(send_log == 1)
						{
							DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
						}
						
						if(send_log == -1)
						{
							DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
						}
	
						if(send_log == -2)
						{
							DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
						}
	
						if(send_log == -3)
						{
							DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
						}
	
						if(send_log == -4)
						{
							DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
						}
						LED4_OFF;
					}
					else{DEBUG_Printf("--> ����δ����\r\n");}
			 	}
			}
		break;

		case TIMESEND_MODE:
			{
				if(start != 5)
				{
					LED1_OFF;
					LED2_OFF;
					LED3_ON;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);
					
					DEBUG_Printf("\r\n���ѽ��붨ʱ����ģʽ��\r\n");

					if(KFB_B_EN == 0)
					{
						DEBUG_Printf("--> �趨�ķ��ͼ��Ϊ: %d s\r\n",EV_Send_Time/1000);
					}else if(KFB_B_EN == 2)
						{
							DEBUG_Printf("--> �趨�ķ��ͼ��Ϊ: %d s\r\n",GPS_Send_Time/1000);
						}

					start = 5;

					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_SetDebug(0);
					LoraNode_GetNetMode(LoRa_Node_str);
					HAL_Delay(10);
					LoraNode_Mode_Set(MODE_TRANSPARENT);
				}

				while(LoRa_Node.Join == 0)   // �״ν�������ȷ��
				{
					LoraNode_STA_Check(LoRa_Node_str);
					LED3_ON;
					HAL_Delay(300);
					LED3_OFF;
					HAL_Delay(300);
					LED3_ON;

					if(Device_Sate != TIMESEND_MODE)
					{break;}
				}

				if(KFB_B_EN == 0)
				{
					Env_Send_Task(EV_Send_Time);
				}else if(KFB_B_EN == 2)
					{
						System_Task(80000,GPS_Send_Time);
					}

				
				if(LPUsart1_RX.receive_flag == 1)
			  	{
			  		LPUsart1_RX.receive_flag = 0;
					DEBUG_Printf("--> ģ����յ���������(�ַ�):");
					DEBUG_Printf("    <");
			  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
					DEBUG_Printf("> \r\n");

					DEBUG_Printf("--> ģ����յ���������(16����):");
					DEBUG_Printf("    <");
					for(uint8_t q=0;q<LPUsart1_RX.rx_len;q++)
					{
						if(LPUsart1_RX.RX_Buf[q]<10)
						{DEBUG_Printf("0");}
						DEBUG_Printf("%x",LPUsart1_RX.RX_Buf[q]);
					}
					DEBUG_Printf("> \r\n");

					if(LPUsart1_RX.rx_len == 1)
					{
						if(LPUsart1_RX.RX_Buf[0] == 0x01)
						{
							LED1_ON;
							Sensor_Data.Led = 1;
							SS_Data[0] = 0x01;
							LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
						}else if(LPUsart1_RX.RX_Buf[0] == 0x00)
							{
								LED1_OFF;
								Sensor_Data.Led = 0;
								SS_Data[0] = 0x00;
								LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
							}
					}
			 	}
			}
		break;

		case TIMESEND_DEBUG_MODE:
			{
				if(start != 6)
				{
					LED1_OFF;
					LED2_OFF;
					LED3_ON;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);
					
					DEBUG_Printf("���ѽ��붨ʱ����ģʽ �����Ѵ򿪡�\r\n");
					if(KFB_B_EN == 0)
					{
						DEBUG_Printf("--> �趨�ķ��ͼ��Ϊ: %d s\r\n",EV_Send_Time/1000);
					}else if(KFB_B_EN == 2)
						{
							DEBUG_Printf("--> �趨�ķ��ͼ��Ϊ: %d s\r\n",GPS_Send_Time/1000);
						}
					start = 6;
					
					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_SetDebug(1);
					LoraNode_GetNetMode(LoRa_Node_str);
					HAL_Delay(10);
					LoraNode_Mode_Set(MODE_TRANSPARENT);
				}

				while(LoRa_Node.Join == 0)   // �״ν�������ȷ��
				{
					LoraNode_STA_Check(LoRa_Node_str);
					LED3_ON;
					HAL_Delay(300);
					LED3_OFF;
					HAL_Delay(300);
					LED3_ON;

					if(Device_Sate != TIMESEND_DEBUG_MODE)
					{break;}

					if(LPUsart1_RX.receive_flag == 1)
				  	{
				  		LPUsart1_RX.receive_flag = 0;
				  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
				 	}
				}

				if(LPUsart1_RX.receive_flag == 1)
			  	{
			  		LPUsart1_RX.receive_flag = 0;
			  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
			 	}

				if(KFB_B_EN == 0)
				{
					Env_Send_Task(EV_Send_Time);
				}else if(KFB_B_EN == 2)
					{
						System_Task(80000,GPS_Send_Time);
					}
			}
		break;

		case LOWPOWER_MODE:
			{
				if(start != 7)
				{
					LED1_OFF;
					LED2_OFF;
					LED3_OFF;
					LED4_OFF;

					LoraNode_Wake_Sleep(MODE_WAKEUP);
					
					DEBUG_Printf("��3s �����͹��Ķ�ʱ����ģʽ��\r\n");
					start = 7;
					lowpoer = 0;

					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_SetDebug(0);
					LoraNode_GetNetMode(LoRa_Node_str);
					HAL_Delay(10);
					LoraNode_Mode_Set(MODE_TRANSPARENT);
					
				}

				while(LoRa_Node.Join == 0)
				{
					LoraNode_STA_Check(LoRa_Node_str);
					LED4_ON;
					HAL_Delay(300);
					LED4_OFF;
					HAL_Delay(300);

					shu ++;
					if(shu >= 1000)
					{
						shu = 0;
						DEBUG_Printf("--> Join ��ʱ��ϵͳ����������\r\n");
						LoRaNode_Go_To_Sleep();
					}

					if(Device_Sate != LOWPOWER_MODE)
					{break;}
				}

				if(LoRa_Node.Join == 1)
				{
					if(lowpoer == 0)
					{
						while(Device_Sate == LOWPOWER_MODE)
						{
							if(Time_Out_Break(3000) == 1)
							{break;}
						}
						TimeOut_Sign = 0;
							
					}
					if(Device_Sate == LOWPOWER_MODE)
					{
						lowpoer = 1;
						LowPower_Send_Task(EV_Send_Time/1000);   // �������60 s
					}
				}
			}
		break;

		case LOWPOWER_DEBUG_MODE:
			{
				
			}
		break;
		
	}
}

//---------------------------------------------------------------------------

void LowPower_Send_Task(uint16_t Time)
{
	int8_t S_log =0;
	static uint32_t send_num = 0,send_ok = 0;
	
	if(Test_ON == 0)
	{
		SENSOR_Testing(Sensor_Data_str);
		ENV_Data_TO();
	}

	DEBUG_Printf("--> ��ʼ��������\r\n");
	send_num ++;

	if(Test_ON == 0)
	{
			S_log = LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
	}else if(Test_ON == 1)
		{ 
			S_log = LoraNode_Write_Receive_Bytes(Test_Data,Test_len,LoRa_Node.Confirm);   //���Ͳ�������
		}
	
	if(S_log == 1)
	{
		DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
		send_ok ++;

		LED4_ON;
		HAL_Delay(500);
		LED4_OFF;
	}else
		{
			if(S_log == 2)
			{	
				send_ok ++;
				if(LPUsart1_RX.receive_flag == 1)
			  	{
			  		LPUsart1_RX.receive_flag = 0;
					DEBUG_Printf("--> ģ����յ���������(�ַ�):");
					DEBUG_Printf("    <");
			  		Usart2SendData(LPUsart1_RX.RX_Buf,LPUsart1_RX.rx_len);
					DEBUG_Printf("> \r\n");

					DEBUG_Printf("--> ģ����յ���������(16����):");
					DEBUG_Printf("    <");
					for(uint8_t q=0;q<LPUsart1_RX.rx_len;q++)
					{
						if(LPUsart1_RX.RX_Buf[q]<10)
						{DEBUG_Printf("0");}
						DEBUG_Printf("%x",LPUsart1_RX.RX_Buf[q]);
					}
					DEBUG_Printf("> \r\n");

					if(LPUsart1_RX.rx_len == 1)
					{
						if(LPUsart1_RX.RX_Buf[0] == 0x01)
						{
							LED1_ON;
							Sensor_Data.Led = 1;
							SS_Data[0] = 0x01;
							LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
						}else if(LPUsart1_RX.RX_Buf[0] == 0x00)
							{
								LED1_OFF;
								Sensor_Data.Led = 0;
								SS_Data[0] = 0x00;
								LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
							}
					}
			 	}
			}else
				{
					if(S_log == -1)
					{
						DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
					}

					if(S_log == -2)
					{
						DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
					}

					if(S_log == -3)
					{
						DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
					}

					if(S_log == -4)
					{
						DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
					}

					LED2_ON;
					HAL_Delay(500);
					LED2_OFF;

					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_GetState(LoRa_Send_str);
					HAL_Delay(100);
					LoraNode_Mode_Set(MODE_TRANSPARENT);

					DEBUG_Printf("--> ����ʧ����Ϣ:\r\n");

					switch(LoRa_Send.Up_Result)
					{
						case 0:
							{
								DEBUG_Printf("        ������״̬: �����ݲ���\r\n");
							}
						break;

						case 1:
							{
								DEBUG_Printf("        ������״̬: ���ݷ�����\r\n");
							}
						break;

						case 2:
							{
								DEBUG_Printf("        ������״̬: ���ݷ���ʧ��\r\n");
							}
						break;

						case 3:
							{
								DEBUG_Printf("        ������״̬: ���ݷ��ͳɹ�\r\n");
							}
						break;

						case 4:
							{
								DEBUG_Printf("        ������״̬: join �ɹ�\r\n");
							}
						break;

						case 5:
							{
								DEBUG_Printf("        ������״̬: join ʧ��\r\n");
							}
						break;
					}
					
					DEBUG_Printf("        �������ŵ�:%d \r\n",LoRa_Send.Up_CH);
					DEBUG_Printf("        ����������:%d \r\n",LoRa_Send.Up_RATE);
					DEBUG_Printf("        �����й���:%d \r\n",LoRa_Send.Up_DB);
					DEBUG_Printf("        �����м���ֵ:%d \r\n",LoRa_Send.Up_Link);
					DEBUG_Printf("        �����л�����ֵ:%d \r\n",LoRa_Send.Up_Cache);
					DEBUG_Printf("        �������ط�����:%d \r\n",LoRa_Send.Resend);
						
				}
		}

	DEBUG_Printf("--> ����ͳ��: ���� %d ,�ɹ� %d \r\n",send_num,send_ok);

	if(Device_Sate == LOWPOWER_MODE)
	{
		Set_RTC_WakeUp(Time);
		LoRaNode_Go_To_Sleep();
	}
}

void Env_Send_Task(uint32_t D_time)
{
	static uint32_t time=0;
	static uint8_t sta = 0;
	int8_t S_L=0;
	static uint32_t send_num = 0,send_ok = 0;

	time = HAL_GetTick();
	if(time%D_time == 0)
	{
		if(sta == 0)
		{
			sta = 1;

			if(Test_ON == 0)
			{
				SENSOR_Testing(Sensor_Data_str);
				ENV_Data_TO();
			}
				
			if(LoRa_Node.Join == 1)
			{
				LED4_ON;
				send_num++;
				if(Test_ON == 0)
				{
		  			S_L = LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);   //���ʹ���������
				}else if(Test_ON == 1)
					{ 
						S_L = LoraNode_Write_Receive_Bytes(Test_Data,Test_len,LoRa_Node.Confirm);   //���Ͳ�������
					}
				if(S_L == 1)
				{
					send_ok++;
					DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
				}
				
				if(S_L == -1)
				{
					DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
				}

				if(S_L == -2)
				{
					DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
				}

				if(S_L == -3)
				{
					DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
				}

				if(S_L == -4)
				{
					DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
				}

				DEBUG_Printf("--> ����ͳ��: ���� %d ,�ɹ� %d \r\n",send_num,send_ok);
				LED4_OFF;

				if(S_L != 1)
				{
					LoraNode_Mode_Set(MODE_CMD);
					HAL_Delay(100);
					LoraNode_GetState(LoRa_Send_str);
					HAL_Delay(100);
					LoraNode_Mode_Set(MODE_TRANSPARENT);

					DEBUG_Printf("--> ����ʧ����Ϣ:\r\n");

					switch(LoRa_Send.Up_Result)
					{
						case 0:
							{
								DEBUG_Printf("        ������״̬: �����ݲ���\r\n");
							}
						break;

						case 1:
							{
								DEBUG_Printf("        ������״̬: ���ݷ�����\r\n");
							}
						break;

						case 2:
							{
								DEBUG_Printf("        ������״̬: ���ݷ���ʧ��\r\n");
							}
						break;

						case 3:
							{
								DEBUG_Printf("        ������״̬: ���ݷ��ͳɹ�\r\n");
							}
						break;

						case 4:
							{
								DEBUG_Printf("        ������״̬: join �ɹ�\r\n");
							}
						break;

						case 5:
							{
								DEBUG_Printf("        ������״̬: join ʧ��\r\n");
							}
						break;
					}
					
					DEBUG_Printf("        �������ŵ�:%d \r\n",LoRa_Send.Up_CH);
					DEBUG_Printf("        ����������:%d \r\n",LoRa_Send.Up_RATE);
					DEBUG_Printf("        �����й���:%d \r\n",LoRa_Send.Up_DB);
					DEBUG_Printf("        �����м���ֵ:%d \r\n",LoRa_Send.Up_Link);
					DEBUG_Printf("        �����л�����ֵ:%d \r\n",LoRa_Send.Up_Cache);
					DEBUG_Printf("        �������ط�����:%d \r\n",LoRa_Send.Resend);
				}
			}
		}
	}
	else
		{	
			sta = 0;
		}
}

//--------------ϵͳ���񣬴������ߣ�����ʱ���񣬵�-------------------------------
void System_Task(uint32_t A_time,uint32_t B_time)    //A_time ��һ�λ�ȡgps��ʱ��     B_time ��ȡgps��Ķ�ʱ����ʱ��
{
	static uint32_t time=0;
	static uint8_t sta = 0,GPS_READY = 0;
	uint8_t temp = 0;
	int8_t SL = 0;
	static uint32_t send_num = 0,send_ok = 0;

	if(GPS_READY == 1)
	{
		A_time = B_time;
	}

	time = HAL_GetTick();
	if(time%A_time == 0)
	{
		if(sta == 0 && GPS_sate == 0)
		{
			sta = 1;
			LED1_ON;
			SENSOR_Testing(Sensor_Data_str);
			GPS_sate = 1;
			Usart1SendData(Gps_START,12);
		}
	}
	else
		{	
			sta = 0;
		}
	
	if(GPS_sate == 1)
	{
		temp = GPS_UpData(GPS_Data_str);
		
		if( temp == 1 || temp == 3)
		{
			GPS_sate = 0;
			GPS_READY= 0;
			LED1_OFF;

			ENV_Data_TO();

			DEBUG_Printf("γ��:  %c , %d \r\n", GPS_Data.NS,Sensor_Data.gps.latitude);
			DEBUG_Printf("����:  %c , %d \r\n", GPS_Data.EW,Sensor_Data.gps.longitude);
			DEBUG_Printf("����:  %d \r\n", (Sensor_Data.Altitude/10));
			
			LED2_ON;
			LoraNode_Mode_Set(MODE_TRANSPARENT);	
			HAL_Delay(10);
			send_num++;
			SL = LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);
			if(SL == 1)
			{
				send_ok++;
				DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
			}
			
			if(SL == -1)
			{
				DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
			}

			if(SL == -2)
			{
				DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
			}

			if(SL == -3)
			{
				DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
			}

			if(SL == -4)
			{
				DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
			}

			DEBUG_Printf("--> ����ͳ��: ���� %d ,�ɹ� %d \r\n",send_num,send_ok);
			LED2_OFF;

			if(SL != 1)
			{
				LoraNode_Mode_Set(MODE_CMD);
				HAL_Delay(100);
				LoraNode_GetState(LoRa_Send_str);
				HAL_Delay(100);
				LoraNode_Mode_Set(MODE_TRANSPARENT);

				DEBUG_Printf("--> ����ʧ����Ϣ:\r\n");

				switch(LoRa_Send.Up_Result)
				{
					case 0:
						{
							DEBUG_Printf("        ������״̬: �����ݲ���\r\n");
						}
					break;

					case 1:
						{
							DEBUG_Printf("        ������״̬: ���ݷ�����\r\n");
						}
					break;

					case 2:
						{
							DEBUG_Printf("        ������״̬: ���ݷ���ʧ��\r\n");
						}
					break;

					case 3:
						{
							DEBUG_Printf("        ������״̬: ���ݷ��ͳɹ�\r\n");
						}
					break;

					case 4:
						{
							DEBUG_Printf("        ������״̬: join �ɹ�\r\n");
						}
					break;

					case 5:
						{
							DEBUG_Printf("        ������״̬: join ʧ��\r\n");
						}
					break;
				}
				
				DEBUG_Printf("        �������ŵ�:%d \r\n",LoRa_Send.Up_CH);
				DEBUG_Printf("        ����������:%d \r\n",LoRa_Send.Up_RATE);
				DEBUG_Printf("        �����й���:%d \r\n",LoRa_Send.Up_DB);
				DEBUG_Printf("        �����м���ֵ:%d \r\n",LoRa_Send.Up_Link);
				DEBUG_Printf("        �����л�����ֵ:%d \r\n",LoRa_Send.Up_Cache);
				DEBUG_Printf("        �������ط�����:%d \r\n",LoRa_Send.Resend);
			}
		}
		if(temp == 2)     //�������ݸ���
		{
			GPS_sate = 0;
			GPS_READY = 1;
			LED1_OFF;

			GPS_Data_TO();
			ENV_Data_TO();

			
			DEBUG_Printf("γ��:  %c , %d \r\n", GPS_Data.NS,Sensor_Data.gps.latitude);
			DEBUG_Printf("����:  %c , %d \r\n", GPS_Data.EW,Sensor_Data.gps.longitude);
			DEBUG_Printf("����:  %d \r\n", (Sensor_Data.Altitude/10));
			
			LED4_ON;
			LoraNode_Mode_Set(MODE_TRANSPARENT);	
			HAL_Delay(10);
			send_num++;
			SL = LoraNode_Write_Receive_Bytes(SS_Data,32,LoRa_Node.Confirm);
			
			if(SL == 1)
			{
				send_ok++;
				DEBUG_Printf("--> ���ݷ��ͳɹ�\r\n");
			}
			
			if(SL == -1)
			{
				DEBUG_Printf("--> ģ����æ��BUSY=0 \r\n");
			}

			if(SL == -2)
			{
				DEBUG_Printf("--> ģ��û�н��յ��������� \r\n");
			}

			if(SL == -3)
			{
				DEBUG_Printf("--> ģ�鷢�ͳ�ʱ\r\n");
			}

			if(SL == -4)
			{
				DEBUG_Printf("--> ģ��û���յ��ظ�\r\n");
			}

			DEBUG_Printf("--> ����ͳ��: ���� %d ,�ɹ� %d \r\n",send_num,send_ok);
			LED4_OFF;

			if(SL != 1)
			{
				LoraNode_Mode_Set(MODE_CMD);
				HAL_Delay(100);
				LoraNode_GetState(LoRa_Send_str);
				HAL_Delay(100);
				LoraNode_Mode_Set(MODE_TRANSPARENT);

				DEBUG_Printf("--> ����ʧ����Ϣ:\r\n");

				switch(LoRa_Send.Up_Result)
				{
					case 0:
						{
							DEBUG_Printf("        ������״̬: �����ݲ���\r\n");
						}
					break;

					case 1:
						{
							DEBUG_Printf("        ������״̬: ���ݷ�����\r\n");
						}
					break;

					case 2:
						{
							DEBUG_Printf("        ������״̬: ���ݷ���ʧ��\r\n");
						}
					break;

					case 3:
						{
							DEBUG_Printf("        ������״̬: ���ݷ��ͳɹ�\r\n");
						}
					break;

					case 4:
						{
							DEBUG_Printf("        ������״̬: join �ɹ�\r\n");
						}
					break;

					case 5:
						{
							DEBUG_Printf("        ������״̬: join ʧ��\r\n");
						}
					break;
				}
				
				DEBUG_Printf("        �������ŵ�:%d \r\n",LoRa_Send.Up_CH);
				DEBUG_Printf("        ����������:%d \r\n",LoRa_Send.Up_RATE);
				DEBUG_Printf("        �����й���:%d \r\n",LoRa_Send.Up_DB);
				DEBUG_Printf("        �����м���ֵ:%d \r\n",LoRa_Send.Up_Link);
				DEBUG_Printf("        �����л�����ֵ:%d \r\n",LoRa_Send.Up_Cache);
				DEBUG_Printf("        �������ط�����:%d \r\n",LoRa_Send.Resend);
			}
		}
		
	}
}

//--------------------------------------���������������

void CMD_Check(void)
{
	if(Usart2_RX.receive_flag == 1)
	{
		if(Usart2_RX.rx_len ==3)
		{
			if(Usart2_RX.RX_Buf[0] == 0x59 && Usart2_RX.RX_Buf[1] == 0x45)   //  YES
			{
				if(Usart2_RX.RX_Buf[2] == 0x53)
				{
					KFB_B_EN = 1;
				}
			}

			if(Usart2_RX.RX_Buf[0] == 0x4f && Usart2_RX.RX_Buf[1] == 0x46)  //  OFF
			{
				if(Usart2_RX.RX_Buf[2] == 0x46)
				{
					KFB_B_EN = 0;
					EEPROM_WriteOneByte(0x00000000,0);
					DEBUG_Printf("-->GPS�����ѹر�\r\n");
				}
			}
		}

		if(Usart2_RX.RX_Buf[0] == 0xFF && Usart2_RX.RX_Buf[Usart2_RX.rx_len - 1] == 0xFF)
		{
			if(Usart2_RX.RX_Buf[1] == 0xAA && Usart2_RX.RX_Buf[Usart2_RX.rx_len - 2] == 0xAA)
			{
				if(Usart2_RX.rx_len == 8)
				{
					EV_Send_Time = Usart2_RX.RX_Buf[2]*65536*256 + Usart2_RX.RX_Buf[3]*65536 + Usart2_RX.RX_Buf[4]*256 + Usart2_RX.RX_Buf[5] ;
					EEPROM_WriteOneByte(0x00000004,Usart2_RX.RX_Buf[5]);
					EEPROM_WriteOneByte(0x00000005,Usart2_RX.RX_Buf[4]);
					EEPROM_WriteOneByte(0x00000006,Usart2_RX.RX_Buf[3]);
					EEPROM_WriteOneByte(0x00000007,Usart2_RX.RX_Buf[2]);
					
					DEBUG_Printf(" -->���óɹ� ����ʱ��Ϊ :%d  ms\r\n",EV_Send_Time);
				}
			}

			if(Usart2_RX.RX_Buf[1] == 0xDD && Usart2_RX.RX_Buf[Usart2_RX.rx_len - 2] == 0xDD)
			{
				if(Usart2_RX.rx_len == 8)
				{
					EV_Send_Time = Usart2_RX.RX_Buf[2]*65536*256 + Usart2_RX.RX_Buf[3]*65536 + Usart2_RX.RX_Buf[4]*256 + Usart2_RX.RX_Buf[5] ;
					EEPROM_WriteOneByte(0x00000008,Usart2_RX.RX_Buf[5]);
					EEPROM_WriteOneByte(0x00000009,Usart2_RX.RX_Buf[4]);
					EEPROM_WriteOneByte(0x0000000a,Usart2_RX.RX_Buf[3]);
					EEPROM_WriteOneByte(0x0000000b,Usart2_RX.RX_Buf[2]);
					
					DEBUG_Printf(" -->���óɹ� ����ʱ��Ϊ :%d  ms\r\n",EV_Send_Time);
				}
			}

			if(Usart2_RX.RX_Buf[1] == 0xBB && Usart2_RX.RX_Buf[Usart2_RX.rx_len - 2] == 0xBB)
			{
				Test_len = Usart2_RX.rx_len - 4;
				EEPROM_WriteOneByte(0x0000000c,Test_len);
				
				for(uint8_t i = 0;i<Test_len;i++)
				{
					Test_Data[i] = Usart2_RX.RX_Buf[2+i];
					EEPROM_WriteOneByte(0x0000000d+i,Usart2_RX.RX_Buf[2+i]);
				}
				DEBUG_Printf(" -->���óɹ� ��������Ϊ :  ");
				for(uint8_t q=0;q<Test_len;q++)
				{
					if(Test_Data[q]<10)
					{DEBUG_Printf("0");}
					DEBUG_Printf("%x",Test_Data[q]);
					DEBUG_Printf(" ");
				}
				DEBUG_Printf("\r\n");
			}

			if(Usart2_RX.RX_Buf[1] == 0xCC && Usart2_RX.RX_Buf[Usart2_RX.rx_len - 2] == 0xCC)
			{
				if(Usart2_RX.RX_Buf[2] == 0x00)
				{
	
					Test_ON = 0;
					EEPROM_WriteOneByte(0x00000002,Test_ON);
					DEBUG_Printf(" -->����ģʽ�ѹر� \r\n");
				}
				if(Usart2_RX.RX_Buf[2] == 0x01)
				{
	
					Test_ON = 1;
					EEPROM_WriteOneByte(0x00000002,Test_ON);
					DEBUG_Printf(" -->�����嶨ʱ���������Զ��� \r\n");
				}
			}
		}
		
		Usart2_RX.receive_flag = 0;
	}
}

//---------------------------------------------------------------------------------------------------------
void SENSOR_Testing(SENSOR *Env_temp)
{
	HDC1000_Update(Env_temp);
	OPT3001_Update(Env_temp);
	MPL3115_Update(Env_temp);
	MMA8451_Upate(Env_temp);
}


//----------------------------------------------------------------------------------

void HDC1000_Update(SENSOR *Env_temp)
{
	uint16_t temper, humi;

	temper = HDC1000_Read_Temper();
	humi = HDC1000_Read_Humidi();
	
	temper = (int16_t)(((float)temper/(float)BASE_NUM*165-40)*100);   //��100���ڱ���С������λ
	humi = (int16_t)(((float)humi/(float)BASE_NUM)*100);

	Env_temp->Tempter= temper;
	Env_temp->Humidi = humi;
	
	DEBUG_Printf("\r\n��ǰ�¶� :%.2f ��       ", (float)(temper/100.00));
	DEBUG_Printf("ʪ�� :%d% \r\n", humi);
}

void OPT3001_Update(SENSOR *Env_temp)
{
	float lux; 
	
	lux = OPT3001_Get_Lux();

	Env_temp->Lux = (int16_t)(100*lux);
	
	DEBUG_Printf("��ǰ���ն� : %.2f Lux \r\n", lux);
}

void MPL3115_Update(SENSOR *Env_temp)
{
	float pressure, temper;

	pressure = MPL3115_ReadPressure();
//	temper = MPL3115_ReadTemp();

	DEBUG_Printf("��ǰ��ѹ  %.2f Pa \r\n", pressure);
//	DEBUG_Printf("MPL3115 Temper: %.2f\r\n", temper);
	
	
	if(pressure != 0)
	{
		Env_temp->Pressure = (uint16_t)(pressure/100.0);
	}
}

void MMA8451_Upate(SENSOR *Env_temp)
{
	ACCELER_T tAccel;	
		
	tAccel = MMA8451_ReadAcceleration();

	Env_temp->acc.accel_x = tAccel.accel_x;
	Env_temp->acc.accel_y = tAccel.accel_y;
	Env_temp->acc.accel_z = tAccel.accel_z;
	
	DEBUG_Printf("X�� : %d ��", tAccel.accel_x);
	DEBUG_Printf("Y�� : %d ��", tAccel.accel_y);
	DEBUG_Printf("Z�� : %d  \r\n \r\n", tAccel.accel_z);
	
	//pData->acc.accel_x = tAccel.accel_x;
	//pData->acc.accel_y = tAccel.accel_y;
	//pData->acc.accel_z = tAccel.accel_z;
}


uint8_t GPS_UpData(GPS_DATA *data_temp)
{
	static uint8_t num = 0;
	if(Usart1_RX.receive_flag == 1)
	{
		Usart1_RX.receive_flag = 0;
		if(Usart1_RX.rx_len >10)
		{
			//Usart1_RX.rx_len = 0;
			GPS_Analysis(data_temp,Usart1_RX.RX_Buf);
			if(data_temp->Gps_Sta != 0)
			{
				if(data_temp->Latitude !=0)
				{
					Usart1SendData(Gps_STOP,12);
					HAL_Delay(20);
					num = 0;
					data_temp->Gps_Sta = 0;

					return 2;
				}else{
						Usart1SendData(Gps_STOP,12);
						//������Ҫ������������
						HAL_Delay(20);
						num = 0;
						data_temp->Gps_Sta = 0;

						return 3;
					}
				
			}
			else
				{
					num ++;
					if(num >= 60)
					{
						num = 0;
						Usart1SendData(Gps_STOP,12);
						DEBUG_Printf("GPS ���ź� \r\n");

						return 1;
					}
				}
		}
	}

	return 0;
}


////---------------------------------------------------------------------------------------------------------------------

uint8_t Time_Out_Break(uint16_t MAX_time)
{
	static uint32_t time_start = 0;
	static uint32_t time_new = 0;
	uint16_t temp=0;

	if(TimeOut_Sign == 0)
	{
		TimeOut_Sign = 1;
		time_start = HAL_GetTick();
	}
	if(TimeOut_Sign == 1)
	{
		time_new = HAL_GetTick();
		temp = time_new - time_start;
		if(temp > MAX_time)
		{
			TimeOut_Sign = 0;
			return 1;
		}
		else
			{return 0;}
	}
}


/*---------------------ʹ��ʾ��----------------------------------
while(1)
	{
		if(Time_Out_Break(5000) == 1)
		{
			break;
		}
	}
	TimeOut_Sign = 0;*/
	
void GPS_Data_TO(void)
{
	uint32_t temp1 = 0,temp2 = 0;
	float s1 = 0.0 , s2 = 0.0;

	temp1 = (GPS_Data.Latitude/10000)*10000;
	temp2 = (GPS_Data.Longitude/10000)*10000;

	s1 = (((float)(GPS_Data.Latitude%10000))/60.0)*100.0;
	s2 = (((float)(GPS_Data.Longitude%10000))/60.0)*100.0;
	
	Sensor_Data.Altitude = GPS_Data.Altitude;
	Sensor_Data.gps.latitude = temp1 + (uint32_t)s1;
	Sensor_Data.gps.longitude = temp2 + (uint32_t)s2;


	if(GPS_Data.NS == 0x53)
	{
		Sensor_Data.gps.latitude = Sensor_Data.gps.latitude + 0x80000000;
	}
	if(GPS_Data.EW == 0x57)
	{
		Sensor_Data.gps.longitude = Sensor_Data.gps.longitude + 0x80000000;
	}
}



void ENV_Data_TO(void)
{
	SS_Data[0] = Sensor_Data.Led;
	
	SS_Data[1] = Sensor_Data.Pressure%256;
	SS_Data[2] = Sensor_Data.Pressure/256;
	
	SS_Data[3] = Sensor_Data.Altitude%256;
	SS_Data[4] = Sensor_Data.Altitude/256;
	
	SS_Data[5] = Sensor_Data.Tempter%256;
	SS_Data[6] = Sensor_Data.Tempter/256;
	
	SS_Data[7] = Sensor_Data.Humidi%256;
	SS_Data[8] = Sensor_Data.Humidi/256;

	SS_Data[9] = Sensor_Data.Lux%256;
	SS_Data[10] = Sensor_Data.Lux/256;

	SS_Data[11] = Sensor_Data.acc.accel_x%256;
	SS_Data[12] = Sensor_Data.acc.accel_x/256;

	SS_Data[13] = Sensor_Data.acc.accel_y%256;
	SS_Data[14] = Sensor_Data.acc.accel_y/256;

	SS_Data[15] = Sensor_Data.acc.accel_z%256;
	SS_Data[16] = Sensor_Data.acc.accel_z/256;

	SS_Data[17] = Sensor_Data.battery;
	
	SS_Data[18] = Sensor_Data.gps.longitude%256;
	SS_Data[19] = ((Sensor_Data.gps.longitude/256)%256);
	SS_Data[20] = ((Sensor_Data.gps.longitude/65536)%256);
	SS_Data[21] = ((Sensor_Data.gps.longitude/65536)/256);

	SS_Data[22] = Sensor_Data.gps.latitude%256;
	SS_Data[23] = ((Sensor_Data.gps.latitude/256)%256);
	SS_Data[24] = ((Sensor_Data.gps.latitude/65536)%256);
	SS_Data[25] = ((Sensor_Data.gps.latitude/65536)/256);
	
	SS_Data[26] = Sensor_Data.reserve1;
	SS_Data[27] = Sensor_Data.reserve1;
	
	SS_Data[28] = Sensor_Data.reserve2;
	SS_Data[29] = Sensor_Data.reserve2;
	
	SS_Data[30] = Sensor_Data.reserve3;
	SS_Data[31] = Sensor_Data.reserve3;

}

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;
 
  if(intnum < 0)
  {
	intnum = intnum*(-1);
	str[j++] = '-';
  }
  
  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;	/* '0' */

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}


/*
**	string concat
*/
uint8_t *StringCat(uint8_t *str, const uint8_t *string)
{
	uint8_t *s = str;
	
	while(*s)
	{
		s++;
	}
	
	while(*string)
	{
		*s++ = *string++;
	}
	
	*s++ = '\r';
	*s++ = '\n';
	*s = '\0';
			
	return str;		
}

/*******************************************************************************
** ����	      ��EEPROMдһ�ֽ�����
** ����       add:�洢����ַ,��0��ʼ
              data:Ҫд��������ֽ�
** ����ֵ	    ��
** ע��       
** �޸���־
*******************************************************************************/
void EEPROM_WriteOneByte(uint32_t add, uint8_t data)
{
    HAL_FLASHEx_DATAEEPROM_Unlock();//����EEPROM
    add += 0x08080000;//����EEPROMʵ�ʵ�ַ
    if(add > 0x080807FF)//��ַ������Χ
    {
        HAL_FLASHEx_DATAEEPROM_Lock();//EEPROM���� 
        return;
    }
    if(HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAM_FASTBYTE, add, data) == HAL_OK)//��EEPROMдһ�ֽ�����
    {
        
    }
    HAL_FLASHEx_DATAEEPROM_Lock();//EEPROM���� 
}



