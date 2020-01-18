#ifndef __chat_h
#define __chat_h

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "adc.h"
#include "cdcio.h"
#include "string.h"

//#define FLASH_SectorToErase 0x01FAF010
//#define FLASH_SectorToErase 0x1FFF000
//#define FLASH_SectorToErase 0x1FFE000

void vChatTask(void *vpars);

#endif
