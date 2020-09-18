#include "logger.h"


//char data[SECTOR_SIZE];
//uint8_t MY_FREQ;

void usb_dp_set()	{
      GPIO_InitTypeDef gpio_init = {
              .GPIO_Speed = GPIO_Speed_10MHz,
              .GPIO_Mode = GPIO_Mode_Out_PP,
              .GPIO_Pin = USB_DP_PU_PIN,
      };

      RCC_APB2PeriphClockCmd(USB_DP_PU_RCC, ENABLE);
      GPIO_Init(USB_DP_PU_GPIO, &gpio_init);
      GPIO_WriteBit(USB_DP_PU_GPIO, USB_DP_PU_PIN, Bit_SET);
}


void usb_dp_reset()	{
      GPIO_InitTypeDef gpio_init = {
              .GPIO_Speed = GPIO_Speed_10MHz,
              .GPIO_Mode = GPIO_Mode_Out_PP,
              .GPIO_Pin = USB_DP_PU_PIN,
      };

      RCC_APB2PeriphClockCmd(USB_DP_PU_RCC, ENABLE);
      GPIO_Init(USB_DP_PU_GPIO, &gpio_init);
      GPIO_WriteBit(USB_DP_PU_GPIO, USB_DP_PU_PIN, Bit_RESET);
}

char RTC_Init(void) {
	// Enable clock and power for RTC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	// Enable access to the BKP domain
	PWR_BackupAccessCmd(ENABLE);
	// Enable RTC in case it is not active
	if((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN) {
		// reset the content of the BKP registers
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);
		// switch to the LSE clock
		RCC_LSEConfig(RCC_LSE_ON);
		while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		// default prescaler for seconds 
		RTC_SetPrescaler(0x7FFF);
		// eneble RTC
		RCC_RTCCLKCmd(ENABLE);
		// waiting for synchronization
		RTC_WaitForSynchro();
 		return 1;
	}
	// Configure the TAMPER pin as an ALARM output
	BKP_RTCOutputConfig(BKP_RTCOutputSource_Alarm);
	return 0;
}

/* cheks the usb cable connection */
uint8_t CheckUSB(void) {
	//uint8_t check;
	// peripheral's initiation
        GPIO_InitTypeDef USB_DTC;

        RCC_APB2PeriphClockCmd(USB_DTC_RCC, ENABLE);
        USB_DTC.GPIO_Mode	= GPIO_Mode_IPD;
        USB_DTC.GPIO_Speed	= GPIO_Speed_2MHz;
        USB_DTC.GPIO_Pin	= USB_DTC_PIN;
        GPIO_Init(USB_DTC_GPIO, &USB_DTC);
	return GPIO_ReadInputDataBit(USB_DTC_GPIO, USB_DTC_PIN);
}

/* switches power off */
void PowerOFF(void) {
	// peripheral's initiation
        GPIO_InitTypeDef POWER_OFF;

        RCC_APB2PeriphClockCmd(POWER_OFF_RCC, ENABLE);
        POWER_OFF.GPIO_Mode       = GPIO_Mode_Out_PP;
        POWER_OFF.GPIO_Speed      = GPIO_Speed_2MHz;
        POWER_OFF.GPIO_Pin        = POWER_OFF_PIN;
        GPIO_Init(POWER_OFF_GPIO, &POWER_OFF);
	// have a nice day :)
	GPIO_SetBits(POWER_OFF_GPIO, POWER_OFF_PIN);
	vTaskDelay(100);
	GPIO_ResetBits(POWER_OFF_GPIO, POWER_OFF_PIN);
}

/* makes a measurement for the given channel */
int GetVolt(uint8_t CHAN) {
	uint8_t i, num = 36;
	uint32_t DD[num];
	int V = 0;
	float D1 = 0, D2 = 0, coef = 0;

	AD_Reset();
	AD_SetConfig(AD_CR_BM | MY_GAIN | AD_CR_RD | AD_CR_BUF | CHAN);
	AD_ReadDataCont(DD, sizeof(DD)/4, MY_FREQ);
	for (i = 0; i < sizeof(DD)/4; i++) {
		D1 = DD[i];
		coef = (1<<(MY_GAIN>>8));
	        D2 += (1250000000.0/8388608.0)*(D1 - 8388608.0)/coef;
	}
	coef = num;
	V = D2/coef;
	return V;
}


/* defines the UPDAE RATE in respect to the DATA AQUISITION FREQUENCY */
//void DefFS(void) {
//	RTC_Init();
//	ReadProgramSettings();
//	if (loggerSettings.freq < 300)
//		MY_FREQ = AD_MR_FS_12;
//	else if (loggerSettings.freq < 3600)
//		MY_FREQ = AD_MR_FS_33;
//	else
//		MY_FREQ	= AD_MR_FS_12;
//}

/* returns corrected time since last calibration in seconds */
uint32_t GetTime(void) {
	uint32_t epoch, dT1, dT2;
	double prescaler;

	RTC_Init();
	epoch = RTC_GetCounter();
	ReadTimeSettings();
	prescaler = timeSettings.prescaler;
	dT1 = epoch - timeSettings.start;
	dT2 = dT1 * prescaler;
	return timeSettings.start + dT2;
}

/* reads the current address in the FLASH from the BPR of STM */
uint32_t GetAddress(void) {
	uint16_t tmp1, tmp2;
	tmp1 = BKP_ReadBackupRegister(BKP_DR1);
	tmp2 = BKP_ReadBackupRegister(BKP_DR2);
	return ((tmp1 << 16) | tmp2);
}

/* saves the current address in the FLASH to the BPR of STM */
void SaveAddress(uint32_t addr) {
	BKP_WriteBackupRegister(BKP_DR1, addr >> 16);
	BKP_WriteBackupRegister(BKP_DR2, addr);
}

/* reads the correction number for the effective data points made since the schedule start time
   it is necessary as the start time can be placed in the past and the programm needs to calculate 
   the correctly the next WakeUp time moment */
uint32_t GetNumberCorrection(void) {
	uint16_t tmp1, tmp2;
	tmp1 = BKP_ReadBackupRegister(BKP_DR3);
	tmp2 = BKP_ReadBackupRegister(BKP_DR4);
	return ((tmp1 << 16) | tmp2);
}

/* saves the c points' correction number */
void SaveNumberCorrection(uint32_t num) {
	BKP_WriteBackupRegister(BKP_DR3, num >> 16);
	BKP_WriteBackupRegister(BKP_DR4, num);
}

/* reads the BLINK constant 
   0 - means 1/4 sec and 1 - means 1/10 sec */
uint16_t GetBlinkMode(void) {
	return BKP_ReadBackupRegister(BKP_DR5);
}

/* writes the BLINK constant */
void SaveBlinkMode(uint16_t num) {
	BKP_WriteBackupRegister(BKP_DR5, num);
}

/* makes a long measurement itself */
void MakeSinleLongMeasurement(char T[16]) {
	uint8_t i, num = 30;
	uint32_t DD[num], address;
	int V = 0;
	float D1 = 0, D2 = 0, coef = 0;

	// Get VOLT
	AD_Init();
	AD_Reset();
	AD_SetConfig(AD_CR_BM | MY_GAIN | AD_CR_RD | AD_CR_BUF | AD_CR_CH1);
	AD_ReadDataCont(DD, sizeof(DD)/4, MY_FREQ);
	AD_DeInit();
	// Get ADDRESS
	address = GetAddress();
	// write a point to the FLASH
	sFLASH_Init();
	for (i = 0; i < sizeof(DD)/4; i++) {
		if ((address & 0x00000FFF) == 0) {
			sFLASH_EraseSector(address, 0);
		}
		// converting CODE to VOLTAGE
		D1 = DD[i];
		coef = (1<<(MY_GAIN>>8));
	        D2 = (1250000000.0/8388608.0)*(D1 - 8388608.0)/coef;
		V = D2;
	        T[4] = V >> 24;
		T[5] = V >> 16;
		T[6] = V >> 8;
		T[7] = V;
		address += sFLASH_WriteData(T, address,  16, 0);
	}
	sFLASH_DeInit();
	// save the address
	SaveAddress(address);
}

/* makes long voltage measerement for self-heating tests (FOR A SINGLE CHANNEL!!!!) Head function */
void MakeMeasurementLong(void) {
	uint8_t i;
	uint32_t time;
	char T[16];

       	// Get TIME
	time = GetTime();
	T[0] = time >> 24;
	T[1] = time >> 16;
	T[2] = time >> 8;
	T[3] = time;
	for (i = 0; i < 240; i++)
		MakeSinleLongMeasurement(T);
}

/* makes a voltage measerement */
void MakeMeasurement(void) {
	int V;
	uint32_t time, address;
	char T[16];
	// Enable wrtire access to the BKP reisters
	RTC_Init();
	// Get TIME
	time = GetTime();
	T[0] = time >> 24;
	T[1] = time >> 16;
	T[2] = time >> 8;
	T[3] = time;
	// Get VOLT
	AD_Init();
	V = GetVolt(AD_CR_CH1);
	T[4] = V >> 24;
	T[5] = V >> 16;
	T[6] = V >> 8;
	T[7] = V;
	V = GetVolt(AD_CR_CH2);
	T[8]  = V >> 24;
	T[9]  = V >> 16;
	T[10] = V >> 8;
	T[11] = V;
	V = GetVolt(AD_CR_CH3);
	T[12] = V >> 24;
	T[13] = V >> 16;
	T[14] = V >> 8;
	T[15] = V;
	AD_DeInit();
	// Get ADDRESS
	address = GetAddress();
	// write the result to the FLASH
	sFLASH_Init();
	if ((address & 0x00000FFF) == 0) {
		sFLASH_EraseSector(address, 0);
	}
	address += sFLASH_WriteData(T, address,  16, 0);
	sFLASH_DeInit();
	// save the address
	SaveAddress(address);
}

/* sets the next wake up time and returns the value of the counter to rise the alarm 
   mode == 1 forces it to set the Alarm Time, while in case of mode == 0 it only returns the value */
uint32_t SetWakeUp(uint8_t mode) {
	uint32_t address, num, real_time, real_alarm_time, real_operation_time, stm_operation_time, stm_alarm_time, stm_time;
	double prescaler;
	// reading of the settings
	ReadProgramSettings();
	ReadTimeSettings();
	address = GetAddress();
	prescaler = timeSettings.prescaler;
	// get real time
	real_time = GetTime();
	if (real_time < loggerSettings.finish) { 
		// calculation of the wake up time acounting for the RTC drift
		// number of already done measurements
		num = (address/16) + GetNumberCorrection();
		// desired next wake-up time in real time-space
		real_alarm_time = loggerSettings.start + loggerSettings.freq*num;
		// duration of the time period that the DEVICE should have been operating since the TIME was set, at the next wake-up time, in real time-space :)
		real_operation_time = real_alarm_time - timeSettings.start;
		// duration of the same period in the internal STM time-space
		stm_operation_time = real_operation_time / prescaler; 
		// desired wake-up time in the STM internal time-space
		stm_alarm_time = timeSettings.start + stm_operation_time;
		// current time in STM time-space
		stm_time = RTC_GetCounter();
		// check, if the next wake-up time is in the future and move it forward, in case if not
		while (stm_alarm_time < (stm_time + 1)) {
			stm_alarm_time += loggerSettings.freq;
			SaveNumberCorrection(GetNumberCorrection() + 1);
		}
	}
	// brick the DEVICE
	else
		stm_alarm_time = 0;
	// RTC seems to produce the pulse at the END of the corresponding second :(
	stm_alarm_time = stm_alarm_time - 1;
	// set the alarm
	if (mode)
		RTC_SetAlarm(stm_alarm_time);
	
	return stm_alarm_time;
	
}

// switches MCU to 72 MHz on the fly
void SetFreqHigh(void) {
	//INCRASE THE FLASH LATTENCY
	FLASH_SetLatency(FLASH_Latency_2);
	//SET PLL SOURCE AND MULTIPLIER
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
	//ENABLE PLL, WAIT FOR IT TO BE READY, and SET SYSCLK SOURCE AS PLLCLK
	RCC_PLLCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET){};
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	//SET HCLK = SYSCLK = 72MHZ
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	//SET PCLK2 = HCLK = 72MHZ
	RCC_PCLK2Config(RCC_HCLK_Div1);
	//SET PCLK1 = HCLK/2 = 36MHZ (maximum available)
	RCC_PCLK1Config(RCC_HCLK_Div2);
	//CORE CLOCK UPDATE
	SystemCoreClockUpdate();
	//USB PIN KICK-UP
        usb_dp_set();
}

// switches MCU to 8 MHz on the fly
void SetFreqLow(void) {
	//SET HSE AS SYSCLK SOURCE, 8MHz
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
	//SET HCLK = SYSCLK / 4 = 2MHz
	RCC_HCLKConfig(RCC_SYSCLK_Div4);
	//SET PCLK1 = HCLK = 2MHZ
	RCC_PCLK1Config(RCC_HCLK_Div1);
	//SET PCLK2 = HCLK = 2MHZ
	RCC_PCLK2Config(RCC_HCLK_Div1);
	//DISABLE PLL
	RCC_PLLCmd(DISABLE);
	//DECREASE THE FLASH LATTENCY
	FLASH_SetLatency(FLASH_Latency_0);
	//CORE CLOCK UPDATE
	SystemCoreClockUpdate();
	//USB PIN RESET AND APB CLOCK DISABLE
        usb_dp_reset();
}
