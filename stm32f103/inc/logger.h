#include "hw_config.h"
#include "usb_lib.h"
#include "stdio.h"
#include "stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "memory.h"
#include "ad779x.h"

#define MY_FREQ		AD_MR_FS_123	
#define MY_GAIN		AD_CR_Gain64
#define PROG_SET_ADDR	0x0800F000	// 60-th kByte of the STM32F102 medium density devices
#define TIME_SET_ADDR	0x0800F400	// 61-st kByte of the STM32F102 medium density devices
#define DEV_INFO_ADDR	0x0800F800	// 62-th kByte of the STM32F102 medium density devices

// USB cable detection stuff
#define USB_DTC_RCC	RCC_APB2Periph_GPIOA
#define USB_DTC_GPIO	GPIOA
#define USB_DTC_PIN	GPIO_Pin_8

// USB autoenable stuff
#define USB_DP_PU_RCC	RCC_APB2Periph_GPIOA
#define USB_DP_PU_GPIO	GPIOA
#define USB_DP_PU_PIN	GPIO_Pin_9

// POWER OFF stuff
#define POWER_OFF_RCC	RCC_APB2Periph_GPIOA
#define POWER_OFF_GPIO	GPIOA
#define POWER_OFF_PIN	GPIO_Pin_10

typedef struct {
	uint32_t freq; 
	uint32_t start;
	uint32_t finish; 
	} LoggerSettings;
LoggerSettings loggerSettings;

typedef struct {
	uint32_t start; 
	float prescaler;
	} TimeSettings;
TimeSettings timeSettings;

typedef struct {
	char	model[32]; 
	char	serial[32]; 
	char	mcu[32]; 
	char	adc[32]; 
	char	flash[32];
	char	sensor[32]; 
	char	A10[32];
	char	A11[32];
	char	A12[32];
	char	R1[32];
	char	A20[32];
	char	A21[32];
	char	A22[32];
	char	R2[32];
	char	A30[32];
	char	A31[32];
	char	A32[32];
	char	R3[32];
	} DeviceInfo;
DeviceInfo deviceInfo;

//+++++++++++++++++++++++++++++++++++++++++++++++

//void		DefFS(void);
void		PowerOFF(void);
void		MakeMeasurement(void);
void		MakeMeasurementLong(void);
void		SetFreqLow(void);
void		SetFreqHigh(void);
void 		SaveAddress(uint32_t addr);
void		SaveNumberCorrection(uint32_t num);
void		SaveBlinkMode(uint16_t num);
void		MakeSinleLongMeasurement(char T[16]);
int		GetVolt(uint8_t CHAN);
char		RTC_Init(void);
uint8_t		CheckUSB(void);
uint16_t	GetBlinkMode(void);
uint32_t	GetTime(void);
uint32_t	GetTime2(void);
uint32_t	GetAddress(void);
uint32_t	GetNumberCorrection(void);
uint32_t	SetWakeUp(uint8_t mode);

void usb_dp_set();
void usb_dp_reset();

