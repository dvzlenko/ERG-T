#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"


/*<! AD7799 SPI Interface pins */
#define AD_SPI				SPI1
#define AD_SPI_CLK			RCC_APB2Periph_SPI1
#define AD_SPI_SCK_PIN			GPIO_Pin_5                  /* PA.05 */
#define AD_SPI_SCK_GPIO_PORT		GPIOA                       /* GPIOA */
#define AD_SPI_SCK_GPIO_CLK		RCC_APB2Periph_GPIOA
#define AD_SPI_MISO_PIN			GPIO_Pin_6                  /* PA.06 */
#define AD_SPI_MISO_GPIO_PORT		GPIOA                       /* GPIOA */
#define AD_SPI_MISO_GPIO_CLK		RCC_APB2Periph_GPIOA
#define AD_SPI_MOSI_PIN			GPIO_Pin_7                  /* PA.07 */
#define AD_SPI_MOSI_GPIO_PORT		GPIOA                       /* GPIOA */
#define AD_SPI_MOSI_GPIO_CLK		RCC_APB2Periph_GPIOA
#define AD_CS_PIN			GPIO_Pin_4                  /* PA.04 */
#define AD_CS_GPIO_PORT			GPIOA                       /* GPIOA */
#define AD_CS_GPIO_CLK			RCC_APB2Periph_GPIOA

#define AD_PWR_PIN			GPIO_Pin_0                  /* PB.00 */
#define AD_PWR_GPIO_PORT		GPIOB                       /* GPIOB */
#define AD_PWR_GPIO_CLK			RCC_APB2Periph_GPIOB

#define AD_CS_LOW()			GPIO_ResetBits(AD_CS_GPIO_PORT, AD_CS_PIN)
#define AD_CS_HIGH()			GPIO_SetBits(AD_CS_GPIO_PORT, AD_CS_PIN)
#define AD_PWR_DISABLE()		GPIO_ResetBits(AD_PWR_GPIO_PORT, AD_PWR_PIN)
#define AD_PWR_ENABLE()			GPIO_SetBits(AD_PWR_GPIO_PORT, AD_PWR_PIN)

#define AD_DUMMY_BYTE			0xA5

/*<! IO operations definition for communication register */
#define AD_WR		(0<<6)  /* 0b0000000 - write to */
#define AD_RD		(1<<6)  /* 0b1000000 - read from */
#define AD_CRD		(0<<2)  /* 0b100 - disable a continuous read mode */
#define AD_CRE		(1<<2)  /* 0b100 - enable a continuous read mode */

/*<! Registers address description */
#define AD_STATUS_REG	(0<<3)  /*!< 0b0000;   8-bit. Status Register During a Read Operation (RO) */
#define AD_MODE_REG	(1<<3)  /*!< 0b1000;   16-bit. Mode Register (R/w) */
#define AD_CONFIG_REG	(2<<3)  /*!< 0b10000;  16-bit. Configuration Register (R/W) */
#define AD_DATA_REG	(3<<3)  /*!< 0b11000;  16/24-bit. Data Register (RO) */
#define AD_ID_REG	(4<<3)  /*!< 0b100000; 8-bit. ID Register (RO) */
#define AD_IO_REG	(5<<3)  /*!< 0b100000; 8-bit. IO Register (RW) */
#define AD_OFF_REG	(6<<3)  /*!< 0b100000; 24-bit. OFFSET REGISTER Register (RW) */
#define AD_FS_REG	(7<<3)  /*!< 0b100000; 24-bit. FULL SCALE Register (RW) */

/*<! MODE REGISTER constants */
#define AD_MR_MD_CC	(0<<13)	// Continuous Conversion mode
#define AD_MR_MD_SC	(1<<13)	// Single Conversion mode 
#define AD_MR_MD_IM	(2<<13)	// Idle Mode 
#define AD_MR_MD_PD	(3<<13)	// Power Down mode 
#define AD_MR_MD_IZC	(4<<13)	// Internal Zero Calibration mode 
#define AD_MR_MD_IFC	(5<<13)	// Internal Full-scale Calibration mode 
#define AD_MR_MD_SZC	(6<<13)	// System Zero Calibration mode 
#define AD_MR_MD_SFC	(7<<13)	// System Full-scale Calibration mode 
#define AD_MR_FS_470	0x01	// 470 Hz,   4 ms
#define AD_MR_FS_242	0x02	// 242 Hz,   8 ms
#define AD_MR_FS_123	0x03 	// 123 Hz,  16 ms
#define AD_MR_FS_62	0x04 	//  62 Hz,  32 ms
#define AD_MR_FS_50	0x05	//  50 Hz,  40 ms
#define AD_MR_FS_39	0x06	//  39 Hz,  48 ms
#define AD_MR_FS_33	0x07	//  33 Hz,  60 ms
#define AD_MR_FS_19	0x08	//  19 Hz, 101 ms
#define AD_MR_FS_16	0x0A	//  16 Hz, 120 ms
#define AD_MR_FS_12	0x0B	//  12 Hz, 160 ms
#define AD_MR_FS_10	0x0C	//  10 Hz, 200 ms
#define AD_MR_FS_8	0x0D	//   8 Hz, 240 ms
#define AD_MR_FS_6	0x0E	//   6 Hz, 320 ms
#define AD_MR_FS_4	0x0F	//   4 Hz, 480 ms

/*<! CONFIGURATION REGISTER constants */
#define AD_CR_BO	(1<<14)	// Enable Burn-Out current
#define AD_CR_BM	(0<<13)	// Enable Bipollar operation Mode
#define AD_CR_UM	(1<<13)	// Enable Unipolar operation Mode
#define AD_CR_Gain1	(0<<8)	// Gain   x1
#define AD_CR_Gain2	(1<<8)	// Gain   x2
#define AD_CR_Gain4	(2<<8)	// Gain   x4
#define AD_CR_Gain8	(3<<8)	// Gain   x8
#define AD_CR_Gain16	(4<<8)	// Gain  x16
#define AD_CR_Gain32	(5<<8)	// Gain  x32
#define AD_CR_Gain64	(6<<8)	// Gain  x64
#define AD_CR_Gain128	(7<<8)	// Gain x128
#define AD_CR_RD	(1<<5)	// Enable Reference Detect Function
#define AD_CR_BUF	(1<<4)	// Enable data Buferization
#define AD_CR_CH1	0x00	// AIN1(+) - AIN1(-)
#define AD_CR_CH2	0x01	// AIN2(+) - AIN2(-)
#define AD_CR_CH3	0x02	// AIN3(+) - AIN3(-)

/*<! FUNCTIONS */
void		AD_Init(void);
void		AD_DeInit(void);
void		AD_Reset(void);
void		AD_SetMode(uint16_t MODE);
void		AD_SetConfig(uint16_t CONF);
void		AD_ReadDataCont(uint32_t* data, uint16_t num, uint8_t FS);
void 		AD_WriteOFF(uint32_t OFF_REG_VAL);
uint8_t		AD_SendByte(uint8_t byte);
uint8_t		AD_ReadID(void);
uint8_t		AD_ReadStatus(void);
uint8_t		AD_CheckStatus(void);
uint16_t	AD_ReadMode(void);
uint16_t	AD_ReadConfig(void);
uint32_t	AD_ReadOFF(void);
uint32_t	AD_ReadFS(void);
uint32_t	AD_ReadDataSingle(uint8_t FS);


