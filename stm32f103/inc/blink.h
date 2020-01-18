#ifndef __blink_h
#define __blink_h

#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#define SYS_LED_RCC	RCC_APB2Periph_GPIOA
#define SYS_LED_GPIO	GPIOA
#define SYS_LED_PIN	GPIO_Pin_2

void vBlinkTask(void *vpars);
void blink_toggle();
void blink_init();

#endif
