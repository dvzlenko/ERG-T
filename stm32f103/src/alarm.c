#include "alarm.h"
#include "logger.h"

void vAlarmTask(void *vpars) {
	uint32_t tm, wkptm, delay;
	// test delay
	vTaskDelay(100);
	// enable access to RTC
	RTC_Init();
	// reset the operation correction number
	SaveNumberCorrection(0);
	// set the BLINK mode to make it slow
	SaveBlinkMode(0);
	// get program settings
	ReadProgramSettings();
	// delay definition
	if (loggerSettings.freq < 600)
		delay = 600;
	else
		delay = loggerSettings.freq;

	while (1) {
		tm = RTC_GetCounter();
		wkptm = SetWakeUp(0);
		// if WakeUpTime smaller than the current Time + delay, it moves the WakeUpTime forward!
		// otherwise it resets the default WakeUpTime according to the programm (see the SetWakeUp function)
		if ((wkptm < tm + delay) & (wkptm != 0))
			RTC_SetAlarm(wkptm + delay);
		else
			SetWakeUp(1);
		// power off
		PowerOFF();
		// wait a bit
		vTaskDelay(1000);
	}
}
