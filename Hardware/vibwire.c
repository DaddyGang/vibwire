#include "vibwire.h"
#include "math.h"
#include "usart.h"
#include "app.h"
void VIB_Init(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* enable vibwire gpio port */
	VIBWIRE_GPIO_CLK_ENABLE();
	
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(TRIGGER_PORT, TRIGGER_PIN, GPIO_PIN_RESET);
	
	/* config output pin */
	GPIO_InitStruct.Pin = TRIGGER_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(TRIGGER_PORT, &GPIO_InitStruct);
}

void VIB_Test(void){
	int i;
	for(i=0;i<5000;i++){
            TRIGGER_TOGGLE;
            DEBUG_Printf("TOGGLE\r\n");
            HAL_Delay(100);
        }
}