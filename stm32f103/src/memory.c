#include <memory.h>
#include <ad779x.h>
#include <chat.h>
#include <logger.h>

/* reads Device Info from the MCU FLASH */
void ReadDevInfo(void) {
	uint8_t i;
	uint32_t *source_addr = (uint32_t *)DEV_INFO_ADDR;
	uint32_t *dest_addr = (void *)&deviceInfo;
	for (i = 0; i < sizeof(deviceInfo)/4; i++) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
}

/* writes Device Info to the MCU FLASH */
void WriteDevInfo(void) {
	uint8_t i;
	uint32_t *source_addr = (void *)&deviceInfo;
	uint32_t *dest_addr = (uint32_t *) DEV_INFO_ADDR;
	FLASH_Unlock();
	FLASH_ErasePage(DEV_INFO_ADDR);
	for (i = 0; i < sizeof(deviceInfo)/4; i++) {
	    FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
	    source_addr++;
	    dest_addr++;
	}
	FLASH_Lock();
}

/* reads ProgrammSettings from 1 kByte page of the MCU FLASH */
void ReadProgramSettings(void) {
	uint8_t i;
	uint32_t *source_addr = (uint32_t *)PROG_SET_ADDR;
	uint32_t *dest_addr = (void *)&loggerSettings;
	for (i = 0; i < 3; i++) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
}

/* writes ProgrammSettings in one 1 kByte page of the MCU FLASH */
void WriteProgramSettings(void) {
	uint8_t i;
	uint32_t *source_addr = (void *)&loggerSettings;
	uint32_t *dest_addr = (uint32_t *) PROG_SET_ADDR;
	FLASH_Unlock();
	FLASH_ErasePage(PROG_SET_ADDR);
	for (i = 0; i < 3; i++) {
	    FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
	    source_addr++;
	    dest_addr++;
	}
	FLASH_Lock();
}

/* reads TimeSettings from 1 kByte page of the MCU FLASH */
void ReadTimeSettings(void) {
	uint8_t i;
	uint32_t *source_addr = (uint32_t *)TIME_SET_ADDR;
	uint32_t *dest_addr = (void *)&timeSettings;
	for (i = 0; i < 2; i++) {
		*dest_addr = *(__IO uint32_t*)source_addr;
		source_addr++;
		dest_addr++;
	}
}

/* writes Time Settings in one 1 kByte page of the MCU FLASH */
void WriteTimeSettings(void) {
	uint8_t i;
	uint32_t *source_addr = (void *)&timeSettings;
	uint32_t *dest_addr = (uint32_t *) TIME_SET_ADDR;
	FLASH_Unlock();
	FLASH_ErasePage(TIME_SET_ADDR);
	for (i = 0; i < 2; i++) {
	    FLASH_ProgramWord((uint32_t)dest_addr, *source_addr);
	    source_addr++;
	    dest_addr++;
	}
	FLASH_Lock();
}

/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_DeInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	/*!< Disable the sFLASH_SPI  */
	SPI_Cmd(sFLASH_SPI, DISABLE);
	/*!< DeInitializes the sFLASH_SPI */
	SPI_I2S_DeInit(sFLASH_SPI);
	/*!< sFLASH_SPI Periph clock disable */
	RCC_APB2PeriphClockCmd(sFLASH_SPI_CLK, DISABLE);
	/*!< Configure all pins used by the SPI as input floating *******************/
	/*!< SCK */ 
	GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	/*!< MISO */
	GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
	GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	/*!< MOSI */
	GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
	GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
	/*!< CS it should not be configured!!!*/
	//GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
	//GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	/*!< sFLASH_CS HIGH and sFLASH_PWR_ENABLE LOW */
	//sFLASH_PWR_DISABLE();
	}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_LowLevel_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	/*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO  and sFLASH_SPI_SCK_GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK | sFLASH_SPI_SCK_GPIO_CLK, ENABLE);
	/*!< sFLASH_SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);
	/*!< sFLASH_PWR Periph clock enable */
	//RCC_APB2PeriphClockCmd(sFLASH_PWR_GPIO_CLK, ENABLE);
	/*!< Configure sFLASH_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin	= sFLASH_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure sFLASH_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin	= sFLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
	GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure sFLASH_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin	= sFLASH_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
	GPIO_InitStructure.GPIO_Pin	= sFLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure AD_CS_PIN pin */
	GPIO_InitStructure.GPIO_Pin	= AD_CS_PIN;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure sFLASH_PWR_PIN pin */
	//GPIO_InitStructure.GPIO_Pin	= sFLASH_PWR_PIN;
	//GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	//GPIO_Init(sFLASH_PWR_GPIO_PORT, &GPIO_InitStructure);
	/*!< Pull AD_CS, sFLASH_CS, and sFLASH_PWR_ENABLE pins high */
	AD_CS_HIGH();
	sFLASH_CS_HIGH();
	//AD_PWR_ENABLE();
	//sFLASH_PWR_ENABLE();
	}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sFLASH_Init(void) {
	SPI_InitTypeDef  SPI_InitStructure;

	sFLASH_LowLevel_Init();

	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode		= SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize		= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL		= SPI_CPOL_High;	
	SPI_InitStructure.SPI_CPHA		= SPI_CPHA_2Edge;	
	SPI_InitStructure.SPI_NSS		= SPI_NSS_Soft;
	SPI_InitStructure.SPI_FirstBit 		= SPI_FirstBit_MSB;
	/*! SPI DaudRatePrescaler definition depends on the device operation mode */
        if (CheckUSB())
                SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; /* PLLCLK -> HCLK -> PCLK2 -> SCK = 48MHz -> 24MHz -> 6MHz -> 1.5MHz */
        else
                SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  /* I need it to be as fast as possible in case if MCU operates at 1 MHz*/
	//SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_Init(sFLASH_SPI, &SPI_InitStructure);
	/*!< Enable the sFLASH_SPI  */
	SPI_Cmd(sFLASH_SPI, ENABLE);
	}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void sFLASH_WriteEnable(void) {
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Write Enable" instruction */
	sFLASH_SendByte(sFLASH_CMD_WREN);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	}

void sFLASH_WriteDisable(void) {
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Write Disable" instruction */
	sFLASH_SendByte(sFLASH_CMD_WRDI);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	}

/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
void sFLASH_WaitForWriteEnd(void) {
	uint8_t flashstatus = 0;
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Read Status Register" instruction */
	sFLASH_SendByte(sFLASH_CMD_RDSR);
	/*!< Loop as long as the memory is busy with a write cycle */
	do {
		/*!< Send a dummy byte to generate the clock needed by the FLASH
		and put the value of the status register in FLASH_Status variable */
 		flashstatus = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	}
	while ((flashstatus & sFLASH_WIP_FLAG) == SET); /* Write in progress */
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sFLASH_SendByte(uint8_t byte) {
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(sFLASH_SPI, byte);
	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(sFLASH_SPI);
	}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sFLASH_SendHalfWord(uint16_t HalfWord) {
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	/*!< Send Half Word through the sFLASH peripheral */
	SPI_I2S_SendData(sFLASH_SPI, HalfWord);
	/*!< Wait to receive a Half Word */
	while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	/*!< Return the Half Word read from the SPI bus */
	return SPI_I2S_ReceiveData(sFLASH_SPI);
	}

/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t sFLASH_ReadID(void) {
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "RDID " instruction */
	sFLASH_SendByte(0x9F);
	/*!< Read a byte from the FLASH */
	Temp0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	/*!< Read a byte from the FLASH */
	Temp1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	/*!< Read a byte from the FLASH */
	Temp2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return Temp;
	}

char sFLASH_ReadSFDP(void) {
	uint64_t  T, T0 = 0, T1 = 0, T2 = 0, T3 = 0, T4 = 0, T5 = 0, T6 = 0, T7 = 0, T8 = 0;
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "RDID " instruction */
	sFLASH_SendByte(0x5A);
	/*!< Read bytes from the FLASH */
	T0 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T1 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T2 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T3 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T4 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T5 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T6 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	T7 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	//T8 = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	//T = (T0 << 16) | (T1 << 8) | T2;
	//T = (T0 << 64) | (T1 << 56) | (T2 << 48) | (T3 << 40) | (T4 << 32) | (T5 << 24) | (T6 << 16) | (T7 << 8) | T8;
	T = (T0 << 56) | (T1 << 48) | (T2 << 40) | (T3 << 32) | (T4 << 24) | (T5 << 16) | (T6 << 8) | T7;
	return T;
	}

void sFLASH_EN4B(void) {
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "RDSR " instruction */
	sFLASH_SendByte(sFLASH_CMD_EN4B);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	}

uint8_t sFLASH_DO(uint16_t CMD) {
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "RDSR " instruction */
	sFLASH_SendByte(CMD);
	/*!< Read a byte from the FLASH */
	uint32_t SR = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	return SR;
	}

/**
  * @brief  Erases the specified FLASH sector.
  * @param  SectorAddr: address of the sector to erase.
  * @retval None
  */
void sFLASH_EraseSector(uint32_t SectorAddr, uint8_t EN4B) {
	/*!< Set four-byte address mode if needed */
	if (EN4B)
		sFLASH_EN4B();
	/*!< Send write enable instruction */
	sFLASH_WriteEnable();
	/*!< Sector Erase */
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send Sector Erase instruction */
	sFLASH_SendByte(sFLASH_CMD_SE);
	/*!< Send SectorAddr highest nibble address byte */
	if (EN4B)
		sFLASH_SendByte((SectorAddr & 0xFF000000) >> 24);
	/*!< Send SectorAddr high nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/*!< Send SectorAddr medium nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/*!< Send SectorAddr low nibble address byte */
	sFLASH_SendByte(SectorAddr & 0xFF);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	/*!< Wait for write end */
	sFLASH_WaitForWriteEnd();
	}

void sFLASH_EraseBlock(uint32_t SectorAddr, uint8_t EN4B) {
	/*!< Set four-byte address mode if needed */
	if (EN4B)
		sFLASH_EN4B();
	/*!< Send write enable instruction */
	sFLASH_WriteEnable();
	/*!< Sector Erase */
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send Sector Erase instruction */
	sFLASH_SendByte(sFLASH_CMD_BE);
	/*!< Send SectorAddr highest nibble address byte */
	if (EN4B)
		sFLASH_SendByte((SectorAddr & 0xFF000000) >> 24);
	/*!< Send SectorAddr high nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/*!< Send SectorAddr medium nibble address byte */
	sFLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/*!< Send SectorAddr low nibble address byte */
	sFLASH_SendByte(SectorAddr & 0xFF);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	/*!< Wait for write end */
	sFLASH_WaitForWriteEnd();
	}

/**
  * @brief  Writes more than one byte to the FLASH with a single WRITE cycle
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the FLASH page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the FLASH.
  * @param  WriteAddr: FLASH's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the FLASH, must be equal
  *         or less than "sFLASH_PAGESIZE" value.
  * @retval None
  */
uint16_t sFLASH_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite, uint8_t EN4B) {
	uint16_t rt = NumByteToWrite;
	/*!< Set four-byte address mode if needed */
	if (EN4B)
		sFLASH_EN4B();
	/*!< Enable the write access to the FLASH */
	sFLASH_WriteEnable();
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Write to Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_WRITE);
	/*!< Send WriteAddr highest nibble address byte to write to */
	if (EN4B)
		sFLASH_SendByte((WriteAddr & 0xFF000000) >> 24);
	/*!< Send WriteAddr high nibble address byte to write to */
	sFLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/*!< Send WriteAddr medium nibble address byte to write to */
	sFLASH_SendByte((WriteAddr & 0xFF00) >> 8);
	/*!< Send WriteAddr low nibble address byte to write to */
	sFLASH_SendByte(WriteAddr & 0xFF);
	/*!< while there is data to be written on the FLASH */
	while (NumByteToWrite--) {
		/*!< Send the current byte */
		sFLASH_SendByte(*pBuffer);
		/*!< Point on the next byte to be written */
		pBuffer++;
		}
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	/*!< Wait for write end */
	sFLASH_WaitForWriteEnd();
	return rt;
	}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the FLASH.
  * @retval None
  */
//void sFLASH_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t EN4B) {
void sFLASH_ReadData(char* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead, uint8_t EN4B) {
	/*!< Set four-byte address mode if needed */
	if (EN4B)
		sFLASH_EN4B();
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_READ);
	/*!< Send ReadAddr highest nibble address byte to read from */
	if (EN4B)
		sFLASH_SendByte((ReadAddr & 0xFF000000) >> 24);
	/*!< Send ReadAddr high nibble address byte to read from */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte to read from */
	sFLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	sFLASH_SendByte(ReadAddr & 0xFF);
	/*!< while there is data to be read */ 
	while (NumByteToRead--) {
		/*!< Read a byte from the FLASH */
		*pBuffer = sFLASH_SendByte(sFLASH_DUMMY_BYTE);
		/*!< Point to the next location where the byte read will be saved */
		pBuffer++;
		}
	//vTaskDelay(10);
	/*!< Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	}

/**
  * @brief  Initiates a read data byte (READ) sequence from the Flash.
  *   This is done by driving the /CS line low to select the device, then the READ
  *   instruction is transmitted followed by 3 bytes address. This function exit
  *   and keep the /CS line low, so the Flash still being selected. With this
  *   technique the whole content of the Flash is read with a single READ instruction.
  * @param  ReadAddr: FLASH's internal address to read from.
  * @retval None
  */
void sFLASH_StartReadSequence(uint32_t ReadAddr, uint8_t EN4B) {
	/*!< Set four-byte address mode if needed */
	if (EN4B)
		sFLASH_EN4B();
	/*!< Select the FLASH: Chip Select low */
	sFLASH_CS_LOW();
	/*!< Send "Read from Memory " instruction */
	sFLASH_SendByte(sFLASH_CMD_READ);

	/*!< Send the 24- or 32-bit address of the address to read from */
	/*!< Send ReadAddr highest nibble address byte to read from */
	if (EN4B)
		sFLASH_SendByte((ReadAddr & 0xFF000000) >> 24);
	/*!< Send ReadAddr high nibble address byte */
	sFLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr medium nibble address byte */
	sFLASH_SendByte((ReadAddr & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte */
	sFLASH_SendByte(ReadAddr & 0xFF);
	}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
uint8_t sFLASH_ReadByte(void)
{
  return (sFLASH_SendByte(sFLASH_DUMMY_BYTE));
}





// ====================================================
 
