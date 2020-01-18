#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_flash.h"
//#include "stm32_eval_spi_flash.h"

//MX25L256 FLASH SPI Interface pins
#define sFLASH_SPI			SPI1
#define sFLASH_SPI_CLK			RCC_APB2Periph_SPI1
#define sFLASH_SPI_SCK_PIN		GPIO_Pin_5                  /* PA.05 */
#define sFLASH_SPI_SCK_GPIO_PORT	GPIOA                       /* GPIOA */
#define sFLASH_SPI_SCK_GPIO_CLK		RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MISO_PIN		GPIO_Pin_6                  /* PA.06 */
#define sFLASH_SPI_MISO_GPIO_PORT	GPIOA                       /* GPIOA */
#define sFLASH_SPI_MISO_GPIO_CLK	RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MOSI_PIN		GPIO_Pin_7                  /* PA.07 */
#define sFLASH_SPI_MOSI_GPIO_PORT	GPIOA                       /* GPIOA */
#define sFLASH_SPI_MOSI_GPIO_CLK	RCC_APB2Periph_GPIOA
#define sFLASH_CS_PIN			GPIO_Pin_3                  /* PA.03 */
#define sFLASH_CS_GPIO_PORT		GPIOA                       /* GPIOA */
#define sFLASH_CS_GPIO_CLK		RCC_APB2Periph_GPIOA

#define sFLASH_PWR_PIN			GPIO_Pin_1                  /* PB.01 */
#define sFLASH_PWR_GPIO_PORT		GPIOB                       /* GPIOB */
#define sFLASH_PWR_GPIO_CLK		RCC_APB2Periph_GPIOB

#define sFLASH_CS_LOW()			GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define sFLASH_CS_HIGH()		GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define sFLASH_PWR_DISABLE()		GPIO_ResetBits(sFLASH_PWR_GPIO_PORT, sFLASH_PWR_PIN)
#define sFLASH_PWR_ENABLE()		GPIO_SetBits(sFLASH_PWR_GPIO_PORT, sFLASH_PWR_PIN)

#define sFLASH_DUMMY_BYTE	0xA5

/* M25P SPI Flash supported commands */
#define sFLASH_CMD_WRSR		0x01  /* Write Status Register instruction */
#define sFLASH_CMD_WRITE	0x02  /* Write to Memory instruction */
#define sFLASH_CMD_READ		0x03  /* Read from Memory instruction */
#define sFLASH_CMD_WRDI		0x04  /* Write disable instruction */
#define sFLASH_CMD_RDSR		0x05  /* Read Status Register instruction  */
#define sFLASH_CMD_WREN		0x06  /* Write enable instruction */
#define sFLASH_CMD_RDID		0x9F  /* Read identification */
#define sFLASH_CMD_RDSFDP	0x5A  /* Read The Serial Flash Discoverable Parameter */
#define sFLASH_CMD_RES		0xAB  /* Read Electronic Identificator */
#define sFLASH_CMD_SE		0x20  /* 4K Sector Erase instruction */
#define sFLASH_CMD_BE		0xD8  /* 64K Block Erase instruction */
#define sFLASH_CMD_CE		0xC7  /* Chip Erase instruction */
#define sFLASH_CMD_EN4B		0xB7  /* Enable 4-byte mode for addressing */
#define sFLASH_CMD_EX4B		0xE9  /* Disable 4-byte mode for addressing */
#define sFLASH_CMD_RDSCUR	0x2B  /* Read Security Register */
#define sFLASH_CMD_WRSCUR	0x2F  /* Wright Security Register */

#define sFLASH_WIP_FLAG		0x01  /* Write In Progress (WIP) flag */

//#define sFLASH_SPI_PAGESIZE	0x100

#define	PAGE_SIZE		 256
#define	SECTOR_SIZE		4096

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void     ReadDevInfo(void);
void     WriteDevInfo(void);
void     ReadProgramSettings(void);
void     WriteProgramSettings(void);
void     ReadTimeSettings(void);
void     WriteTimeSettings(void);

void     sFLASH_Init(void);
void     sFLASH_DeInit(void);
void     sFLASH_LowLevel_Init(void);
void 	 sFLASH_EN4B(void);
void	 sFLASH_WriteEnable(void);
void	 sFLASH_WriteDisable(void);
void 	 sFLASH_WaitForWriteEnd(void);
void	 sFLASH_EraseSector(uint32_t SectorAddr, uint8_t EN4B);
void	 sFLASH_EraseBlock(uint32_t SectorAddr, uint8_t EN4B);
//void 	 sFLASH_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t EN4B);
void 	 sFLASH_ReadData(char* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t EN4B);
void 	 sFLASH_StartReadSequence(uint32_t ReadAddr, uint8_t EN4B);
uint8_t  sFLASH_SendByte(uint8_t byte);
uint8_t	 sFLASH_ReadByte(void);
uint8_t  sFLASH_DO(uint16_t CMD);
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord);
uint16_t sFLASH_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite, uint8_t EN4B);
uint32_t sFLASH_ReadID(void);
//char	 sFLASH_ReadSFDP(void);




