#include "blink.h"
#include "logger.h"

void blink_init() {
	GPIO_InitTypeDef sGPIOinit;

	RCC_APB2PeriphClockCmd(SYS_LED_RCC, ENABLE);
	sGPIOinit.GPIO_Mode 	= GPIO_Mode_Out_PP;
	sGPIOinit.GPIO_Speed	= GPIO_Speed_2MHz;
	sGPIOinit.GPIO_Pin	= SYS_LED_PIN;
	GPIO_Init(SYS_LED_GPIO, &sGPIOinit);
	GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
}

void blink_toggle() {
	if (GPIO_ReadOutputDataBit(SYS_LED_GPIO, SYS_LED_PIN))
		GPIO_ResetBits(SYS_LED_GPIO, SYS_LED_PIN);
	else
		GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
}

void vBlinkTask(void *vpars) {
	blink_init();
	// set the BLINK mode to make it slow
	SaveBlinkMode(0);
	while (1) {
		blink_toggle();
		if (GetBlinkMode())
			vTaskDelay(50);
		else
			vTaskDelay(250);
	}
}
