#include "stdio.h"
#include "stdlib.h"
#include "chat.h"
#include "strtok.h"
#include "version.h"
#include "gpio.h"
#include "logger.h"

#define PROMPT	"> "

char data[SECTOR_SIZE];

enum {
	CMD_HELP = 0,
	CMD_HandShake,
	CMD_SetTime,
	CMD_SetTimePrescaler,
	CMD_GetTime,
	CMD_SetProgramm,
	CMD_GetProgramm,
	CMD_SetDevInfo,
	CMD_VER,
	CMD_DATE,
	CMD_GPIO,
        CMD_FLASHTEST,
        CMD_ADCTEST,
	CMD_SendDataToSTM,
	CMD_SendDataToX86,
	CMD_freq,
	CMD_USBDETECT,
	CMD_alarm,
	CMD_sleep,
	CMD_wakeup,
	CMD_LAST
};

char *cmd_list[CMD_LAST] = {
	"help",
	"hello",
	"SetTime",
	"SetTimePrescaler",
	"GetTime",
	"SetProgramm",
	"GetProgramm",
	"SetDevInfo",
	"ver",
	"date",
	"gpio",
        "TestFlash",
	"TestADC",
	"SendDataToSTM",
	"SendDataToX86",
	"freq",
	"usb",
	"alarm",
	"sleep",
	"wakeup",
};

void vChatTask(void *vpars)
{
	char s[64];
	char cmd[64];
	char *c;
	char *tk;
	int i = 0;
	int adc_ret = -1;

	gpio_init();

	while (1) {
		// PROMPT printing was disabled due to the puculiarities of the device startup!!!
		//cdc_write_buf(&cdc_out, PROMPT, sizeof(PROMPT) - 1, 1);

		memset(cmd, 0, sizeof(cmd));
		c = cmd;

		while (1) {
			i = cdc_read_buf(&cdc_in, c, 1);
			if (i)
				cdc_write_buf(&cdc_out, c, 1, 1);
			else {
				vTaskDelay(10);
				continue;
			}
			if (*c == '\r') {
				cdc_write_buf(&cdc_out, "\n", 1, 1);
				break;
			}
			if (*c == 8) { /* backspace */
				*c = 0;
				if (c > cmd)
					c -= 1;
				continue;
			}
			if (c + 1 < cmd + sizeof(cmd))
				c += 1;
		};

		sniprintf(s, sizeof(s), "OK\r\n");
		tk = _strtok(cmd, " \n\r");

		if (strcmp(tk, cmd_list[CMD_VER]) == 0) {
			sniprintf(s, sizeof(s), "%s\r\n", __VERSION);

		} else if (strcmp(tk, cmd_list[CMD_HandShake]) == 0) {
			uint32_t coef;
			// hello reply, just a huge and unique number
			cdc_write_buf(&cdc_out, "167321907\r\n", 0, 1);
			// device info
			ReadDevInfo();
			// MODEL
			for (i = 0; i < sizeof(deviceInfo.model); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.model[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// Serial Number
			for (i = 0; i < sizeof(deviceInfo.serial); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.serial[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// MCU unit
			for (i = 0; i < sizeof(deviceInfo.mcu); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.mcu[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// ADC unit
			for (i = 0; i < sizeof(deviceInfo.adc); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.adc[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// FLASH unit
			for (i = 0; i < sizeof(deviceInfo.flash); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.flash[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// SENSOR unit
			for (i = 0; i < sizeof(deviceInfo.sensor); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.sensor[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// alphas for the 1-st sensor
			for (i = 0; i < sizeof(deviceInfo.A10); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A10[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A11); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A11[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A12); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A12[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// R for the 1-st sensor
			for (i = 0; i < sizeof(deviceInfo.R1); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.R1[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// alphas for the 2-nd sensor
			for (i = 0; i < sizeof(deviceInfo.A20); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A20[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A21); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A21[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A22); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A22[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// R for the 2-nd sensor
			for (i = 0; i < sizeof(deviceInfo.R2); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.R2[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// alphas for the 3-rd sensor
			for (i = 0; i < sizeof(deviceInfo.A30); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A30[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A31); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A31[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			for (i = 0; i < sizeof(deviceInfo.A32); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.A32[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// R for the 3-rd sensor
			for (i = 0; i < sizeof(deviceInfo.R3); i++) {
				sniprintf(s, sizeof(s), "%c", deviceInfo.R3[i]);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			// OK
			sniprintf(s, sizeof(s), "OK\n\r");	

		} else if (strcmp(tk, cmd_list[CMD_SetTime]) == 0) {
			uint8_t i = 0;
			uint32_t epoch;
			ReadTimeSettings();
			// initialize RTC
			RTC_Init();
			cdc_write_buf(&cdc_out, "ready\r\n", 0, 1);
			// real time receive form the host
			while (i < 4)
				i += cdc_read_buf(&cdc_in, &data[i], 4);
			// set RTC counter value
			epoch = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]);
			RTC_SetCounter(epoch);
			timeSettings.start = epoch;
			// save TimeSettings
			WriteTimeSettings();

		} else if (strcmp(tk, cmd_list[CMD_SetTimePrescaler]) == 0) {
			ReadTimeSettings();
			// obtain the TimePrescaler form the host
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err - Provide the integer value for TimePrescaler in ppb - parts per billion\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			timeSettings.prescaler = atof(tk);
			// save new TimePrescaler value
			WriteTimeSettings();

		} else if (strcmp(tk, cmd_list[CMD_GetTime]) == 0) {
			uint32_t epoch, corr_epoch, prscl;

			RTC_Init();
			ReadTimeSettings();
			epoch = RTC_GetCounter();
			corr_epoch = GetTime();
			prscl = 10000000*timeSettings.prescaler;

			sniprintf(s, sizeof(s), "%d\r\n", timeSettings.start);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "%d\r\n", epoch);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "%d\r\n", corr_epoch);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "%d\r\n", prscl);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);

			sniprintf(s, sizeof(s), "");

		} else if (strcmp(tk, cmd_list[CMD_SetProgramm]) == 0) {
			// RTC enable
			RTC_Init();
			// address for the first data bytes write in the FLASH
			uint32_t address = 0x000000;
			// get the frequency of data acquire as "every X minutes"
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err - Provide the data acquiring frequency\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			loggerSettings.freq = atoi(tk);

			// get the start time for data acquire in seconds since epoch 
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err - Provide the data acquiring start time\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			loggerSettings.start = atoi(tk);

			// get the finish time for data acquire in seconds since epoch 
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err - Provide the data acquiring finish time\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			loggerSettings.finish = atoi(tk);

			// save the address to the BKP registers
			SaveAddress(address);
			// save the settings to the MCU FLASH
			WriteProgramSettings();
			// reset number of corrrections. Needed in case if user operated the device in the middle of the programm, and then made a new program starting somewhere in future
			SaveNumberCorrection(0);

		} else if (strcmp(tk, cmd_list[CMD_GetProgramm]) == 0) {
			uint32_t address;
			// RTC enable
			RTC_Init();
			// get the address from the BKP registers
			address = GetAddress();
			// get the settings to the MCU FLASH
			ReadProgramSettings();
			// ADDRESS
			sniprintf(s, sizeof(s), "%x\r\n", address);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			// START point
			sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.start);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			// FINISH point
			sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.finish);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			// FREQUENCY
			sniprintf(s, sizeof(s), "%d\r\n", loggerSettings.freq);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "OK\r\n");


		} else if (strcmp(tk, cmd_list[CMD_SetDevInfo]) == 0) {
			uint8_t i, rs;
			ReadDevInfo();
			// get the DEVICE model
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.model[rs], 32);
			// get the DEVICE serial number
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.serial[rs], 32);
			// get the MCU name
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.mcu[rs], 32);
			// get the ADC name
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.adc[rs], 32);
			// get the FLASH name
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.flash[rs], 32);
			// get the SENSOR name
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.sensor[rs], 32);
			// get the A10 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A10[rs], 32);
			// get the A11 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A11[rs], 32);
			// get the A12 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A12[rs], 32);
			// get the R1 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.R1[rs], 32);
			// get the A20 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A20[rs], 32);
			// get the A21 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A21[rs], 32);
			// get the A22 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A22[rs], 32);
			// get the R2 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.R2[rs], 32);
			// get the A30 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A30[rs], 32);
			// get the A31 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A31[rs], 32);
			// get the A32 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.A32[rs], 32);
			// get the R3 coefficient
			rs = 0;
			while (rs < 16) 
				rs += cdc_read_buf(&cdc_in, &deviceInfo.R3[rs], 32);
			// OK
			sniprintf(s, sizeof(s), "OK\n\r");	
			// write it to the memory
			WriteDevInfo();

		} else if (strcmp(tk, cmd_list[CMD_HELP]) == 0) {
			int i;

			for (i = 0; i < CMD_LAST; i++) {
				char *_s = cmd_list[i];

				cdc_write_buf(&cdc_out, _s, strlen(_s), 1);
				cdc_write_buf(&cdc_out, "\r\n", 2, 1);
			}

		} else if (strcmp(tk, cmd_list[CMD_DATE]) == 0) {
			sniprintf(s, sizeof(s), "%d\r\n", xTaskGetTickCount());

		} else if (strcmp(tk, cmd_list[CMD_GPIO]) == 0) {
			int i = 0, v;

			tk = _strtok(NULL, " \n\r");
			if (tk)
				i = atoi(tk);
			tk = _strtok(NULL, " \n\r");
			if (tk) {
				unsigned int timeout = 1000;

				v = !!atoi(tk);

				tk = _strtok(NULL, " \n\r");
				if (tk)
					timeout = atoi(tk);
				if (!timeout)
					timeout = 1000;

				gpio_set_val_timeout(i, v, timeout);
			} else {
				v = gpio_out_get(i);
			}
			sniprintf(s, sizeof(s), "%d: %d\r\n", i, v);
		} else if (strcmp(tk, cmd_list[CMD_SendDataToSTM]) == 0) {
			uint32_t sz = 0, rsz = 0, wr = 0, rs = 0;
			uint32_t address;
			// data ammount to receive from the host
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err1 - Provide the data amount in bytes!\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			sz = atoi(tk);
			// address to write in
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err2 - Provide the address to write to!\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			address = strtol(tk, NULL, 16); // it must point at the sector begining!!! like this XXX000 or XXXXX000 for MX25L256
			// turn the FLASH on
			sFLASH_Init();
			while (rsz < sz) {
				// erasing if adress points to the sector begining
				if ((address & 0x000FFF) == 0) {
					sFLASH_EraseSector(address, 0);
				}
				// number of bytes to write to FLASH. MX25L256 allows writing of not more than one page per cycle
				wr = PAGE_SIZE*(rsz/PAGE_SIZE < sz/PAGE_SIZE) + sz%PAGE_SIZE*(rsz/PAGE_SIZE == sz/PAGE_SIZE);
				// declare readiness
				cdc_write_buf(&cdc_out, "ready\r\n", 0, 1);
				// read data fron USB IO bufer
				rs = 0;
				while (rs < wr) 
					rs += cdc_read_buf(&cdc_in, &data[rs], wr);
				rsz += rs;
				// write the data to the flash 
				sFLASH_WriteData(data, address, wr, 0);
				// increment the address
				address += wr;
			}
			// turn the FLASH off
			sFLASH_DeInit();
			// declare finish
			cdc_write_buf(&cdc_out, "done\r\n", 0, 1);
			sniprintf(s, sizeof(s), "");
		} else if (strcmp(tk, cmd_list[CMD_SendDataToX86]) == 0) {
			uint32_t sz = 0, sd = 0, rd = 0;
			uint32_t address;
			// set fast blink mode
			SaveBlinkMode(1);
			// amount of data to send to the host, bytes
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err1 - Provide the data amount in bytes!\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			sz = atoi(tk); 
			// address to read from
			tk = _strtok(NULL, " \n\r");
			if (!tk) {
				cdc_write_buf(&cdc_out, "Err2 - Provide the address to read from!\r\n", 0, 1);
				sniprintf(s, sizeof(s), "");
				goto out;
			}
			address = strtol(tk, NULL, 16);
			// turn the FLASH on
			sFLASH_Init();
			while (sd < sz) {
				// number of bytes to read from the FLASH
				rd = SECTOR_SIZE*(sd/SECTOR_SIZE < sz/SECTOR_SIZE) + sz%SECTOR_SIZE*(sd/SECTOR_SIZE == sz/SECTOR_SIZE);
				// read data from the FLASH to the data bufer
				sFLASH_ReadData(data, address, rd, 0);
				// write the data to the USB IO bufer
				sd += cdc_write_buf(&cdc_out, data, rd, 1);
				// increment the address 
				address += rd;
			}
			// turn the FLASH off
			sFLASH_DeInit();
			sniprintf(s, sizeof(s), "");
			// set slow blink mode
			SaveBlinkMode(0);
		} else if (strcmp(tk, cmd_list[CMD_ADCTEST]) == 0) {
			uint16_t i, j, ID = 0; 
			uint16_t MR = 0, CR = 0;
			uint32_t DD[25], J, SR = 0, OR = 0, FR = 0;
			int V;
			float D,coef;

			AD_Init();
			AD_Reset();
			
			ID = AD_ReadID();
			SR = AD_ReadStatus();
			MR = AD_ReadMode();
			CR = AD_ReadConfig();
			OR = AD_ReadOFF();
			FR = AD_ReadFS();
			sniprintf(s, sizeof(s), "INIT: ID: %2x, SR: %2x, MR: %4x, ", ID, SR, MR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "CR: %4x; ZSR: %6x; FSR: %6x;\r\n", CR, OR, FR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			
			sniprintf(s, sizeof(s), "FOR CF: %x\r\n", AD_CR_BM | AD_CR_Gain128 | AD_CR_RD | AD_CR_BUF | AD_CR_CH1);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "FOR MD: %x\r\n", AD_MR_MD_IM | AD_MR_FS_123);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);

			AD_SetConfig(AD_CR_BM | AD_CR_Gain128 | AD_CR_RD | AD_CR_BUF | AD_CR_CH1);
			AD_SetMode(AD_MR_MD_IM | AD_MR_FS_123);

			ID = AD_ReadID();
			SR = AD_ReadStatus();
			MR = AD_ReadMode();
			CR = AD_ReadConfig();
			OR = AD_ReadOFF();
			FR = AD_ReadFS();
			sniprintf(s, sizeof(s), "CONF: ID: %2x, SR: %2x, MR: %4x, ", ID, SR, MR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "CR: %4x; ZSR: %6x; FSR: %6x;\r\n", CR, OR, FR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			
			for (i=0; i < 10; i++) {
				AD_ReadDataCont(DD,5,AD_MR_FS_62);
				AD_ReadDataCont(DD,sizeof(DD)/4,AD_MR_FS_62);
				D = 0;
				for (j = 0; j < sizeof(DD)/4; j++) {
			                D += (1250000000.0/8388608.0)*(DD[j] - 8388608.0)/(1<<(AD_CR_Gain128>>8));
				}
				
				V = 4*D/sizeof(DD);	
				sniprintf(s, sizeof(s), "%d, ", V);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			cdc_write_buf(&cdc_out, "\r\n\r\n", 0, 1);
			AD_SetConfig(AD_CR_BM | AD_CR_Gain128 | AD_CR_RD | AD_CR_BUF | AD_CR_CH1);
			for (i = 0; i < 10; i++) {
				J = 0;
				for (j = 0; j < 25; j++) {
					J += AD_ReadDataSingle(AD_MR_FS_62);
				}
				D = J;
				D = D/25.0;
				coef = 1<<(AD_CR_Gain128>>8);
				V = (1200000000.0/8388608.0)*(D - 8388608.0)/coef;
				sniprintf(s, sizeof(s), "%d, ", V);
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
				
			}
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);

			ID = AD_ReadID();
			SR = AD_ReadStatus();
			MR = AD_ReadMode();
			CR = AD_ReadConfig();
			OR = AD_ReadOFF();
			FR = AD_ReadFS();
			sniprintf(s, sizeof(s), "FIN:  ID: %2x, SR: %2x, MR: %4x, ", ID, SR, MR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "CR: %4x; ZSR: %6x; FSR: %6x;\r\n", CR, OR, FR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);

			AD_Reset();
			AD_DeInit();
			sniprintf(s, sizeof(s), "");

		} else if (strcmp(tk, cmd_list[CMD_FLASHTEST]) == 0) {
			char DUMMY_BYTE = 0xA5;
			uint32_t ID = 0, SR = 0;
			sFLASH_Init();
			vTaskDelay(1);
			sFLASH_WriteEnable();
			ID  = sFLASH_ReadID();
			SR  = sFLASH_DO(sFLASH_CMD_RDSR);
			sFLASH_DeInit();
			sniprintf(s, sizeof(s), "ID: %x; SR: %x\r\n", ID, SR);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "");

		} else if (strcmp(tk, cmd_list[CMD_freq]) == 0) {
			uint16_t i;
			RTC_Init();
			SaveAddress(0x000000);
			SetFreqLow();
			MakeMeasurementLong();
			SetFreqHigh();
		} else if (strcmp(tk, cmd_list[CMD_USBDETECT]) == 0) {
			uint8_t a1=12,a2=12,a3=12;
			a1 = CheckUSB();
			a2 = CheckUSB();
			a3 = CheckUSB();
			sniprintf(s, sizeof(s), "USB: %d, %d, %d\r\n", a1, a2, a3 );
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			if (CheckUSB()) {
				sniprintf(s, sizeof(s), "USB detected\r\n");
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			else {
				sniprintf(s, sizeof(s), "USB not detected\r\n");
				cdc_write_buf(&cdc_out, s, strlen(s), 1);
			}
			sniprintf(s, sizeof(s), "");


		} else if (strcmp(tk, cmd_list[CMD_alarm]) == 0) {
			uint32_t rtm, tm, wkptm, num;
			rtm = GetTime();
			tm = RTC_GetCounter();
			wkptm = SetWakeUp(0);
			num = GetNumberCorrection();
			sniprintf(s, sizeof(s), "   rtm = %d, tm: %d, wkptm: %d, ", rtm, tm, wkptm);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);
			sniprintf(s, sizeof(s), "diff: %d, num: %d\r\n", (wkptm-tm), num);
			cdc_write_buf(&cdc_out, s, strlen(s), 1);

		} else if (strcmp(tk, cmd_list[CMD_sleep]) == 0) {
			PowerOFF();
		} else if (strcmp(tk, cmd_list[CMD_wakeup]) == 0) {
			BKP_DeInit();
		} else {
			cdc_write_buf(&cdc_out, "\r\n", 0, 1);
			sniprintf(s, sizeof(s), "E: try `help`\r\n");
			}
out:
		cdc_write_buf(&cdc_out, s, strlen(s), 1);
	}
}

