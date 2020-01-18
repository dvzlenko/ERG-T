#include "ad779x.h"
#include "memory.h"
#include "logger.h"

/**
  * @brief  Initializes the peripherals used by the SPI driver.
  * @param  None
  * @retval None
  */
void AD_Init(void) {
	/*!< AD GPIO and SPI structures definition*/
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	/*!< SPI pins clock enable */
	RCC_APB2PeriphClockCmd(AD_CS_GPIO_CLK | AD_SPI_MOSI_GPIO_CLK | AD_SPI_MISO_GPIO_CLK | AD_SPI_SCK_GPIO_CLK, ENABLE);
	/*!< AD_SPI Periph clock enable */
	RCC_APB2PeriphClockCmd(AD_SPI_CLK, ENABLE);
	/*!< AD_PWR Periph clock enable */
	//RCC_APB2PeriphClockCmd(AD_PWR_GPIO_CLK, ENABLE);
	/*!< Configure AD_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin   = AD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(AD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure AD_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin   = AD_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(AD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure AD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin   = AD_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(AD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure AD_SPI pins: CS */
	GPIO_InitStructure.GPIO_Pin   = AD_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
       	/*!< Configure AD_PWD pin */
	//GPIO_InitStructure.GPIO_Pin   = AD_PWR_PIN;
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	//GPIO_Init(AD_PWR_GPIO_PORT, &GPIO_InitStructure);
	/*!< Configure sFLASH_SPI pins: CS */
	GPIO_InitStructure.GPIO_Pin   = sFLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
	/*!< Pull sFLASH and ADC CS high */
        sFLASH_CS_HIGH();
        AD_CS_HIGH();
	//AD_PWR_ENABLE();
	//sFLASH_PWR_ENABLE();
	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;        
	SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;      
	SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
	SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
	/*! SPI DaudRatePrescaler definition depends on the device operation mode */
	if (CheckUSB())
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
        else
                SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	/*!< Enable the AD_SPI  */
	SPI_Cmd(AD_SPI, DISABLE); // this is from the ugly multilayer wrapper from piton
	SPI_Init(AD_SPI, &SPI_InitStructure);
        SPI_SSOutputCmd(AD_SPI, ENABLE);  // not to write each time ch_high??? WTF? And people say it doesn't work
        SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set); // setting the CS high by default, as I realized, it just for fun :)
        SPI_Cmd(AD_SPI, ENABLE);
	}

/**
  * @brief  DeInitializes the peripherals used by the SPI driver.
  * @param  None
  * @retval None
  */
void AD_DeInit(void) {
	/*!< GPIO init*/
        //GPIO_InitTypeDef GPIO_InitStructure;
        /*!< Disable the AD_SPI  */
        SPI_Cmd(AD_SPI, DISABLE);
        /*!< DeInitializes the AD_SPI */
        SPI_I2S_DeInit(AD_SPI);
        /*!< sFLASH_SPI Periph clock disable */
        RCC_APB2PeriphClockCmd(AD_SPI_CLK, DISABLE);
	// PWR
	//AD_PWR_DISABLE();	
        AD_CS_HIGH();
        /*!< Configure AD_SPI pins: SCK */
        //GPIO_InitStructure.GPIO_Pin = AD_SPI_SCK_PIN;
        //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        //GPIO_Init(AD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
        /*!< Configure AD_SPI pins: MISO */
        //GPIO_InitStructure.GPIO_Pin = AD_SPI_MISO_PIN;
        //GPIO_Init(AD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
        /*!< Configure AD_SPI pins: MOSI */
        //GPIO_InitStructure.GPIO_Pin = AD_SPI_MOSI_PIN;
        //GPIO_Init(AD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
        /*!< Configure AD_CS_PIN pin: AD AD CS pin */
        //GPIO_InitStructure.GPIO_Pin = AD_CS_PIN;
        //GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
        }

/**
  * The next two functions operate with ADC779X CONFIGURATION REGISTER
  * CR: 00 X  X  0  XXX  00  X  X   0 XXX
  *        BO UB     G       RD BUF   CH
  * BO - Set to enale the Burn Out Current (1<<6)
  * UB - Clear to enable the Bipolar Mode (0<<5)
  * G  - Gain settings:
  *    000 (0x00) -   x1 
  *    001 (0x01) -   x2 
  *    010 (0x02) -   x4
  *    011 (0x03) -   x8
  *    100 (0x04) -  x16
  *    101 (0x05) -  x32
  *    110 (0x06) -  x64
  *    111 (0x07) - x128
  * RD  - Referense Detect function (1<<5). Set enables this function and enables NOREF bit functioning in STATUS REGISTER.
  * BUF - Set enables buferization (1<<4). The Manual said that if BUF is set: "...the voltage on any input pin must be limited to 100 mV within the power supply rails." !!!
  * CH  - Defines the active Analog Input Channels:
  *            Hz     ms
  *    000 (0x00) - AIN1(+) - AIN1(-)
  *    001 (0x01) - AIN2(+) - AIN2(-)
  *    010 (0x02) - AIN3(+) - AIN3(-)
  *    011 (0x03) - AIN1(-) - AIN1(-)  What The Fuck???
  *    100 (0x04) - RESERVED 
  *    101 (0x05) - RESERVED 
  *    110 (0x06) - RESERVED 
  *    111 (0x07) - AVdd monitor.  
  */          
uint16_t AD_ReadConfig(void) {
	uint8_t tmp0, tmp1;
	/*!< Select the FLASH: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b01010000 byte to read the ADC CR */
	tmp0 = AD_SendByte((AD_RD | AD_CONFIG_REG));
	/*!< Send the DUMMY_BYTE to obtain the CR MSB byte */
	tmp0 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the CR LSB byte */
	tmp1 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the AD: Chip Select high */
	AD_CS_HIGH();
	return (tmp0 << 8) | tmp1;
	}

void AD_SetConfig(uint16_t CONF) {
	uint8_t tmp;
	/*!< Select the FLASH: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b00001000 byte to write the ADC MR */
	tmp = AD_SendByte((AD_WR | AD_CONFIG_REG));
	/*!< Send the MSB byte of MR*/
	tmp = AD_SendByte(CONF>>8);
	/*!< Send the LSB byte of MR*/
	tmp = AD_SendByte(CONF);
	/*!< Deselect the AD: Chip Select high */
	AD_CS_HIGH();
	}

/**
  * The next two functions operate with ADC779X MODE REGISTER
  * MR: X X X    X    00000000  X X X X 
  *      MD     PSW    zeros      FS
  * MD (MODE):
  *    000 (0<<5 = 0x00) - continuous conversion mode 
  *    001 (1<<5 = 0x20) - single conversion mode 
  *    010 (2<<5 = 0x40) - idle mode 
  *    011 (3<<5 = 0x60) - power down mode 
  *    100 (4<<5 = 0x80) - internal zero calibration mode 
  *    101 (5<<5 = 0xA0) - internal full-scale calibration mode 
  *    110 (6<<5 = 0xC0) - system zero calibration mode 
  *    111 (7<<5 = 0xE0) - system full-scale calibration mode 
  * FS (Filter Update):
  *                   Hz     ms
  *    0000 (0x00) - RESERVED 
  *    0001 (0x01) - 470      4
  *    0010 (0x02) - 242      8
  *    0011 (0x03) - 123     16
  *    0100 (0x04) -  62     32
  *    0101 (0x05) -  50     40
  *    0110 (0x06) -  39     48
  *    0111 (0x07) -  33.2   60
  *    1000 (0x08) -  19.6  101
  *    1001 (0x09) -  16.7  120 
  *    1010 (0x0A) -  16.7  120 
  *    1011 (0x0B) -  12.5  160
  *    1100 (0x0C) -  10    200
  *    1101 (0x0D) -  8.33  240
  *    1110 (0x0E) -  6.25  320
  *    1111 (0x0F) -  4.17  480
  */          
uint16_t AD_ReadMode(void) {
	uint8_t tmp0, tmp1;
	/*!< Select the FLASH: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b01001000 byte to push ADC place the MR into the DR */
	AD_SendByte((AD_RD | AD_MODE_REG));
	/*!< Send the DUMMY_BYTE to obtain the MR MSB byte */
	tmp0 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the MR LSB byte */
	tmp1 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the AD: Chip Select high */
	AD_CS_HIGH();
	return (tmp0 << 8) | tmp1;
	}

void AD_SetMode(uint16_t MODE) {
	/*!< Select the FLASH: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b00001000 byte to write the ADC MR */
	AD_SendByte((AD_WR | AD_MODE_REG));
	/*!< Send the MSB byte of MR*/
	AD_SendByte(MODE>>8);
	/*!< Send the LSB byte of MR*/
	AD_SendByte(MODE);
	/*!< Deselect the AD: Chip Select high */
	AD_CS_HIGH();
	}


/**
  * @brief  Sends a byte 0b01000000 through the SPI interface to the ADC.
  * @retval STATUS REGISTER value:
  * SR:  X   X    X    0  1   X X X
  *     RDY ERR NOREF    SR3   CH
  * RDY   - Ready Bit. Set when the DR is not ready
  * ERR   - Set when the result in DR is clamped to 0 or 1
  * NOREF - Set when there is a problem with the reference voltage
  * SR3   - Is set to 1 for AD7799 and to zero for AD7798. The same is for AD7794 and AD7795?
  * CH    - Channel selected: 000 - AIN1
  *                           001 - AIN2
  *                           010 - AIN3
  */
uint8_t AD_ReadStatus(void) {
	uint8_t tmp;
	/*!< Select the FLASH: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b01000000 byte to push ADC place the SR into the DR */
	tmp = AD_SendByte((AD_RD | AD_STATUS_REG));
	/*!< Send the DUMMY_BYTE to obtain the SR from DR */
	tmp = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the AD: Chip Select high */
	AD_CS_HIGH();
	return tmp;
	}

uint8_t AD_CheckStatus(void) {
	/*!< Send the 0b01000000 byte to push ADC place the SR into the DR */
	AD_SendByte((AD_RD | AD_STATUS_REG));
	/*!< Send the DUMMY_BYTE to obtain the SR from DR */
	return AD_SendByte(AD_DUMMY_BYTE);
	}


/**
  * @brief Reads ID
  */
uint8_t AD_ReadID(void) {
	uint8_t tmp;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send the 0b01010000 byte to push ADC place the ID into the DR */
	tmp = AD_SendByte((AD_RD | AD_ID_REG));
	/*!< Send the DUMMY_BYTE to obtain the ID from DR */
	tmp = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	return tmp;
	}
/**
  * @brief Writes OFFSET REGISTER
  */
void AD_WriteOFF(uint32_t OFF_REG_VAL) {
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send the a command to read Offset Register */
	AD_SendByte((AD_WR | AD_OFF_REG));
	/*!< Send the OFF_REG_VAL */
	AD_SendByte(OFF_REG_VAL>>16);
	AD_SendByte(OFF_REG_VAL>>8);
	AD_SendByte(OFF_REG_VAL);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	}

/**
  * @brief Reads OFFSET REGISTER
  */
uint32_t AD_ReadOFF(void) {
	uint8_t tmp0, tmp1, tmp2;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send the a command to read Offset Register */
	AD_SendByte((AD_RD | AD_OFF_REG));
	/*!< Send the DUMMY_BYTE to obtain the 1-st bit of the OFF_REG from DR */
	tmp0 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 2-nd bit of the OFF_REG from DR */
	tmp1 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 3-rd bit of the OFF_REG from DR */
	tmp2 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	return (tmp0 << 16) | (tmp1 << 8) | tmp2;
	}

/**
  * @brief Reads FULL SCALE REGISTER
  */
uint32_t AD_ReadFS(void) {
	uint8_t tmp0, tmp1, tmp2;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send the a command to read Offset Register */
	AD_SendByte((AD_RD | AD_FS_REG));
	/*!< Send the DUMMY_BYTE to obtain the 1-st bit of the OFF_REG from DR */
	tmp0 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 2-nd bit of the OFF_REG from DR */
	tmp1 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 3-rd bit of the OFF_REG from DR */
	tmp2 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	return (tmp0 << 16) | (tmp1 << 8) | tmp2;
	}

/**
  * @brief Reads the single conversion result from ADC
  */
uint32_t AD_ReadDataSingle(uint8_t FS) {
	uint8_t tmp0, tmp1, tmp2;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send the command to enable a single conversion mode */
	AD_SendByte((AD_WR | AD_MODE_REG));
	AD_SendByte(0x20);
	AD_SendByte(FS);
	/*!< Wait for the SR RDY bit to clear */
	while (AD_CheckStatus() & 0x80);
	/*!< Send the command to push ADC place the ID into the DR */
	AD_SendByte((AD_RD | AD_DATA_REG));
	/*!< Send the DUMMY_BYTE to obtain the 1-st bit of the DATA from DR */
	tmp0 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 2-nd bit of the DATA from DR */
	tmp1 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Send the DUMMY_BYTE to obtain the 3-rd bit of the DATA from DR */
	tmp2 = AD_SendByte(AD_DUMMY_BYTE);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	return (tmp0 << 16) | (tmp1 << 8) | tmp2;
	}


/**
  * @brief Continuously reads the conversion results from ADC
  * num - is nuber of conversions to read
  * FS - is a Frame Set frequency
  */
void AD_ReadDataCont(uint32_t* data, uint16_t num, uint8_t FS) {
	uint8_t i,j=1, tmp0, tmp1, tmp2;
	uint32_t tmp;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Calibrations!!! */
	//AD_SendByte((AD_WR | AD_MODE_REG));
	//AD_SendByte(0x80);
	//AD_SendByte(FS);
	/*!< Wait for the SR RDY bit to clear */
	//while (AD_CheckStatus() & 0x80);
	//AD_SendByte((AD_WR | AD_MODE_REG));
	//AD_SendByte(0xA0);
	//AD_SendByte(FS);
	/*!< Wait for the SR RDY bit to clear */
	//while (AD_CheckStatus() & 0x80);
	/*!< Send the command to enable a continuous conversion mode */
	AD_SendByte((AD_WR | AD_MODE_REG));
	AD_SendByte(0x00);
	AD_SendByte(FS);
	for (i = 0; i < num; i++) {
		/*!< Wait for the SR RDY bit to clear */
		while (AD_CheckStatus() & 0x80);
		/*!< Send the 0x58 byte to declare read from DATA REGISTER */
		AD_SendByte(AD_RD | AD_DATA_REG);
		/*!< Send the DUMMY_BYTE to obtain the 1-st bit of the DATA from DR */
		tmp0 = AD_SendByte(AD_DUMMY_BYTE);
		/*!< Send the DUMMY_BYTE to obtain the 2-nd bit of the DATA from DR */
		tmp1 = AD_SendByte(AD_DUMMY_BYTE);
		/*!< Send the DUMMY_BYTE to obtain the 3-rd bit of the DATA from DR */
		tmp2 = AD_SendByte(AD_DUMMY_BYTE);
		/*!< Fill the buffer data with the converted value */
		*data = ((tmp0 << 16) | (tmp1 << 8) | tmp2);
		data++;
	}
	/*!< Send the command to put ADC in Idle Mode */
	AD_SendByte((AD_WR | AD_MODE_REG));
	AD_SendByte(0x40);
	AD_SendByte(FS);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
	}


/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */    
uint8_t AD_SendByte(uint8_t byte) {
        /*!< Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_TXE) == RESET);
        /*!< Send byte through the SPI1 peripheral */
        SPI_I2S_SendData(AD_SPI, byte);
        /*!< Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_RXNE) == RESET);
        /*!< Return the byte read from the SPI bus */
        return SPI_I2S_ReceiveData(AD_SPI);
}


/**
  * @brief Resets the ADC
  */    
void AD_Reset(void) {
	uint8_t i;
	/*!< Select the ADC: Chip Select low */
	AD_CS_LOW();
	/*!< Send 32 ones to ADC */
	for (i = 0; i < 4; i++)
		AD_SendByte(0xFF);
	/*!< Deselect the ADC: Chip Select high */
	AD_CS_HIGH();
}


/*
void AD_ReadDataCont(uint32_t* data, uint8_t num) {
	uint8_t i,j=1, tmp0, tmp1, tmp2;
	uint32_t tmp;
	AD_CS_LOW();
	AD_SendByte((AD_RD | AD_DATA_REG | AD_CRE));
	for (i = 0; i < num; i++) {
		while (AD_CheckStatus() & 0x80);
			j++;
		tmp0 = AD_SendByte(AD_DUMMY_BYTE);
		tmp1 = AD_SendByte(AD_DUMMY_BYTE);
		tmp2 = AD_SendByte(AD_DUMMY_BYTE);
		*data = ((tmp0 << 16) | (tmp1 << 8) | tmp2);
		data++;
		j = 1;
	}
	AD_SendByte((AD_RD | AD_DATA_REG | AD_CRD));
	AD_CS_HIGH();
	}
*/


