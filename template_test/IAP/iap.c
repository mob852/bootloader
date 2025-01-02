#include "iap.h"
#include "stm32f4xx_flash.h"
#include "stmflash.h"
#include "string.h"
#include "stdio.h"	



/************************************************************************/
void IAP_WriteFlag(uint32_t flag)
{
#if (USE_BKP_SAVE_FLAG == 1)
	PWR->CR |= PWR_CR_DBP;
	BKP_WriteBackupRegister(IAP_FLAG_ADDR, flag);
	PWR->CR &= ~PWR_CR_DBP;
#else 
	FLASH_Unlock();
	
	STMFLASH_Write(IAP_FLAG_ADDR, &flag, 1);
	FLASH_Lock();
#endif	
}


void IAP_Handle(u8 * cmd)
{
	if(strcmp((char *)cmd, "update") == 0)
	{
		IAP_WriteFlag(UPDATE_FLAG_DATA);
		NVIC_SystemReset();
	}
	else if(strcmp((char *)cmd, "erase") == 0)
	{
		IAP_WriteFlag(ERASE_FLAG_DATA);
		NVIC_SystemReset();		
	}
	else if(strcmp((char *)cmd, "menu") == 0)
	{
		IAP_WriteFlag(INIT_FLAG_DATA);
		NVIC_SystemReset();	
	}
	else if(strcmp((char *)cmd, "runapp") == 0)//reset
	{
		NVIC_SystemReset();	
	}
	else
	{
		printf("÷∏¡Ó”–ŒÛ\r\n");
//		NVIC_SystemReset();	
	}
}
