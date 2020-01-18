#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "stm32f10x_gpio.h"
#include "stdlib.h"
#include "cdcio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "strtok.h"
#include "adc.h"
#include "chat.h"
#include "flash.h"
#include "gpio.h"
#include "logger.h"
#include "alarm.h"
#include "blink.h"


int main(void) {

	flash_load();

	if (CheckUSB()) {
	//if (1) {
		portBASE_TYPE err;
		// set SYSCLK to 72 MHz
		SetFreqHigh();

		// USB configuration
		Set_USBClock();
		USB_Interrupts_Config();
		USB_Init();
		usb_dp_set();

		// Enable clock and power for RTC
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		// Enable access to the BKP domain
		PWR_BackupAccessCmd(ENABLE);
		// Configure the TAMPER pin as an ALARM output
	        BKP_RTCOutputConfig(BKP_RTCOutputSource_Alarm);
		RTC_WaitForLastTask();

		// run the alarm task for proper alarm operation after the USB communication finished
		err = xTaskCreate(vAlarmTask, "alarm", 64, NULL, tskIDLE_PRIORITY + 1, NULL );
		// run the chat task for communication with X86
		err = xTaskCreate(vChatTask, "chat", 256, NULL, tskIDLE_PRIORITY + 1, NULL );
		// run the blink task
		err = xTaskCreate(vBlinkTask, "blink", 64, NULL, tskIDLE_PRIORITY + 1, NULL );
		// initiate tesks
		vTaskStartScheduler();
	
		while (1);
	}
	else {
		uint32_t tm;
		// reading of the measurement schedule
		ReadProgramSettings();
		// Enable clock and power for RTC
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		// Enable access to the BKP domain
		PWR_BackupAccessCmd(ENABLE);
		// Configure the TAMPER pin as an ALARM output
	        BKP_RTCOutputConfig(BKP_RTCOutputSource_Alarm);
		RTC_WaitForLastTask();
		// BLINK for the testing purposes
		//blink_init();
		//GPIO_SetBits(SYS_LED_GPIO, SYS_LED_PIN);
		// chek, if current time is in the range of the measurement programm frame
		tm = GetTime();
		if ((tm > loggerSettings.start - 10) & (tm < loggerSettings.finish + 10)) {
			// Making a Temperature measurement
			//MakeMeasurementLong();
			MakeMeasurement();
		}
		// Set the next wake up time
		SetWakeUp(1);		
		// power off
		PowerOFF(); 
	}
}

void vApplicationStackOverflowHook(xTaskHandle xTask,
			           signed portCHAR *pcTaskName )
{
	while(1)
		;
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
/* User can add his own implementation to report the file name and line number,
 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
		;
}
#endif
