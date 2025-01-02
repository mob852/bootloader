#include "iap_config.h"
#include "iap.h"
#include "stmflash.h"
#include "ymodem.h"
#include "usart.h"	
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;
uint8_t tab_1024[1024] = {0};


/************************************************************************/

//static void FLASH_DisableWriteProtectionPages(void)
//{
//	uint32_t useroptionbyte = 0, WRPR = 0;
//	uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
//	FLASH_Status status = FLASH_BUSY;

//	WRPR = FLASH_GetWriteProtectionOptionByte();
//	
//	if ((WRPR & UserMemoryMask) != UserMemoryMask)
//	{
//		useroptionbyte = FLASH_GetUserOptionByte();

//		UserMemoryMask |= WRPR;

//		status = FLASH_EraseOptionBytes();

//		if (UserMemoryMask != 0xFFFFFFFF)
//		{
//			status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
//		}
//		if ((useroptionbyte & 0x07) != 0x07)
//		{ 
//			if ((useroptionbyte & 0x01) == 0x0)
//			{
//				var1 = OB_IWDG_HW;
//			}
//			if ((useroptionbyte & 0x02) == 0x0)
//			{
//				var2 = OB_STOP_RST;
//			}
//			if ((useroptionbyte & 0x04) == 0x0)
//			{
//				var3 = OB_STDBY_RST;
//			}
//			FLASH_UserOptionByteConfig(var1, var2, var3);
//		}

//		if (status == FLASH_COMPLETE)
//		{
//			SerialPutString("Write Protection Close.\r\n");
//			NVIC_SystemReset();
//		}
//		else
//		{
//			SerialPutString("Error: unprotection failed.\r\n");
//		}
//	}
//	else
//	{
//		SerialPutString("Flash not write protected.\r\n");
//	}
//}

#define FLASH_WRP_SECTORS   (OB_WRP_Sector_0|OB_WRP_Sector_1)
__IO uint32_t SectorsWRPStatus = 0xFFF;
static void FLASH_DisableWriteProtectionPages(void)
{
    FLASH_Status status = FLASH_COMPLETE;

		/* ��ȡ������д����״̬ */
		SectorsWRPStatus = FLASH_OB_GetWRP() & FLASH_WRP_SECTORS;

		if (SectorsWRPStatus == 0x00)
		{
				/* �����ѱ�д������ִ�нⱣ������*/

				/* ʹ�ܷ���OPTCR�Ĵ��� */
				FLASH_OB_Unlock();

				/* ���ö�Ӧ��nWRPλ�����д���� */
				FLASH_OB_WRPConfig(FLASH_WRP_SECTORS, DISABLE);
				status=FLASH_OB_Launch();
				/* ��ʼ��ѡ���ֽڽ��б�� */
				if (status   != FLASH_COMPLETE)
				{
					 SerialPutString("Error: unprotection failed.\r\n");
						/* User can add here some code to deal with this error */
				}
				/* ��ֹ����OPTCR�Ĵ��� */
				FLASH_OB_Lock();

				/* ��ȡ������д����״̬ */
				SectorsWRPStatus = FLASH_OB_GetWRP() & FLASH_WRP_SECTORS;

				/* ����Ƿ����óɹ� */
				if (SectorsWRPStatus == FLASH_WRP_SECTORS)
				{
						SerialPutString("Write Protection Close.\r\n");
						NVIC_SystemReset();
				}
				else
				{
					 SerialPutString("Error: unprotection failed.\r\n");
				}
		}
				
		else
				
		{ 
					FlashProtection = 0;
					SerialPutString("Flash not write protected.\r\n");
				
		}


}

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

/************************************************************************/
uint16_t IAP_ReadFlag(void)
{
#if (USE_BKP_SAVE_FLAG == 1)
	return BKP_ReadBackupRegister(IAP_FLAG_ADDR);
#else
	return STMFLASH_ReadWord(IAP_FLAG_ADDR);  
#endif	
}


/************************************************************************/
void IAP_USART_Init(void)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

//    STM_EVAL_COMInit(&USART_InitStructure);  
}

void IAP_Init(void)
{
    // IAP_USART_Init();
	uart_init(115200);

#if (USE_BKP_SAVE_FLAG == 1)
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN , ENABLE); 
#endif
}
/************************************************************************/
int8_t IAP_RunApp(void)
{
	if (((*(__IO uint32_t*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{   
		SerialPutString("\r\n Run to app.\r\n");
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_MSP(*(__IO uint32_t*) ApplicationAddress);
		Jump_To_Application();
		return 0;
	}
	else
	{
		SerialPutString("\r\n Run to app error.\r\n");
		return -1;
	}
}
//void SerialPutHex(uint32_t value)
//{
//    char hex_str[9];  // ���ڴ洢 8 λʮ�������ַ�����+1 �ַ����ڽ�������
//    snprintf(hex_str, sizeof(hex_str), "%08X", value);  // ��ʽ��Ϊ 8 λʮ������
//    SerialPutString(hex_str);  // ͨ�����ڴ�ӡʮ�������ַ���
//}

int8_t IAP_RunApp_debug(void)
{
    uint32_t msp_value;       // ��ջָ�루MSP��ֵ
    uint32_t reset_handler;   // ��λ�������ĵ�ַ

    // ��ȡ MSP ֵ�� Reset_Handler ��ַ
    msp_value = *(__IO uint32_t*)ApplicationAddress;
    reset_handler = *(__IO uint32_t*)(ApplicationAddress + 4);

    // ��ӡ������Ϣ
    SerialPutString("\r\n[DEBUG] Checking Application...\r\n");
    SerialPutString("[DEBUG] ApplicationAddress MSP Value: ");
		printf("%08X",msp_value); // ��ӡ MSP ֵ
    SerialPutString("\r\n[DEBUG] ApplicationAddress Reset_Handler: ");
    printf("%08X",msp_value); // ��ӡ Reset_Handler ��ַ

    // У�� MSP �Ƿ�ָ�� SRAM����Ч�Լ�飩
    if ((msp_value & 0x2FFE0000) == 0x20000000) // У�� MSP �Ƿ��� SRAM ��Χ��
    {   
        SerialPutString("\r\nable to Run to app.\r\n");

//        // ������ת��ַ
//        JumpAddress = reset_handler;
//        Jump_To_Application = (pFunction)JumpAddress;

//        // ������ջָ�루MSP��
//        __set_MSP(msp_value);

//        // ��ת��Ӧ�ó���
//        Jump_To_Application();

        return 0;  // ��ת�ɹ�
    }
    else
    {
        // ��ӡ������Ϣ
        SerialPutString("\r\nRun to app error: Invalid MSP.\r\n");
        return -1;  // ��תʧ��
    }
}


/************************************************************************/
void IAP_Main_Menu(void)
{
	uint8_t cmdStr[CMD_STRING_SIZE] = {0};
	BlockNbr = (FlashDestination - 0x08000000) >> 12;
	
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)
    UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
#else /* USE_STM3210E_EVAL */
	if (BlockNbr < 62)
	{
		UserMemoryMask = ((uint32_t)~((1 << BlockNbr) - 1));
	}
	else
	{
		UserMemoryMask = ((uint32_t)0x80000000);
	}
#endif 
	
	if ((FLASH_OB_GetWRP() & UserMemoryMask) != UserMemoryMask)
	{
		FlashProtection = 1;
	}
	else
	{
		FlashProtection = 0;
	}

	while (1)
	{
		SerialPutString("\r\n IAP Main Menu (V 0.2.0)\r\n");
		SerialPutString(" update\r\n");
		SerialPutString(" upload\r\n");
		SerialPutString(" erase\r\n");
		SerialPutString(" menu\r\n");
		SerialPutString(" runapp\r\n");
		if(FlashProtection != 0)//There is write protected
		{
			SerialPutString(" diswp\r\n");
		}
		
		GetInputString(cmdStr);

		if(strcmp((char *)cmdStr, CMD_UPDATE_STR) == 0)
		{
			IAP_WriteFlag(UPDATE_FLAG_DATA);
			return;
		}
		else if(strcmp((char *)cmdStr, CMD_UPLOAD_STR) == 0)
		{
			IAP_WriteFlag(UPLOAD_FLAG_DATA);
			return;
		}
		else if(strcmp((char *)cmdStr, CMD_ERASE_STR) == 0)
		{
			IAP_WriteFlag(ERASE_FLAG_DATA);
			return;
		}
		else if(strcmp((char *)cmdStr, CMD_MENU_STR) == 0)
		{
			IAP_WriteFlag(INIT_FLAG_DATA);
		}
		else if(strcmp((char *)cmdStr, CMD_RUNAPP_STR) == 0)
		{
			IAP_WriteFlag(APPRUN_FLAG_DATA);
			return;
		}
		else if(strcmp((char *)cmdStr, CMD_DISWP_STR) == 0)
		{
			FLASH_DisableWriteProtectionPages();
		}
		else
		{
			SerialPutString(" Invalid CMD !\r\n");
		}
	}
}


/************************************************************************/
int8_t IAP_Update(void)
{
	uint8_t Number[10] = "";
	int32_t Size = 0;
	Size = Ymodem_Receive(&tab_1024[0]);
	if (Size > 0)
	{
		SerialPutString("\r\n Update Over!\r\n");
		SerialPutString(" Name: ");
		SerialPutString(file_name);
		Int2Str(Number, Size);
		SerialPutString("\r\n Size: ");
		SerialPutString(Number);
		SerialPutString(" Bytes.\r\n");
		
		IAP_RunApp_debug();
		
		return 0;
	}
	else if (Size == -1)
	{
		SerialPutString("\r\n Image Too Big!\r\n");
		return -1;
	}
	else if (Size == -2)
	{
		SerialPutString("\r\n Update failed!\r\n");
		return -2;
	}
	else if (Size == -3)
	{
		SerialPutString("\r\n Aborted by user.\r\n");
		return -3;
	}
	else
	{
		SerialPutString(" Receive Filed.\r\n");
		return -4;
	}
}


/************************************************************************/
int8_t IAP_Upload(void)
{
	uint32_t status = 0; 
	SerialPutString("\n\n\rSelect Receive File ... (press any key to abort)\n\r");
	if (GetKey() == CRC16)
	{
		status = Ymodem_Transmit((uint8_t*)ApplicationAddress, (const uint8_t*)"UploadedFlashImage.bin", FLASH_IMAGE_SIZE);
		if (status != 0) 
		{
			SerialPutString("\n\rError Occured while Transmitting File\n\r");
			return -1;
		}
		else
		{
			SerialPutString("\n\rFile Trasmitted Successfully \n\r");
			return -2;
		}
	}
	else
	{
		SerialPutString("\r\n\nAborted by user.\n\r");  
		return 0;
	}
}


/************************************************************************/
//int8_t IAP_Erase(void)
//{
//	uint8_t erase_cont[3] = {0};
//	Int2Str(erase_cont, FLASH_IMAGE_SIZE / PAGE_SIZE);
//	SerialPutString(" @");//Ҫ�пո񣬱���bug
//	SerialPutString(erase_cont);
//	SerialPutString("@");
//	if(EraseSomePages(FLASH_IMAGE_SIZE, 1))
//		return 0;
//	else
//		return -1;
//}

/*����app��*/

int8_t IAP_Erase(void)
{
	uint8_t erase_cont[3] = {0};
	// Int2Str(erase_cont, FLASH_IMAGE_SIZE / PAGE_SIZE);
	
	//��ʾҪ��������ʼ��ַ ������sector
//	Int2Str(erase_cont,STMFLASH_GetFlashSector(ApplicationAddress));
//	SerialPutString(" application start:");//Ҫ�пո񣬱���bug
//	printf("%x",ApplicationAddress);
//	SerialPutString(erase_cont);
//	SerialPutString("@");

	if(EraseSomeSectors(ApplicationAddress, ApplicationEndAddress)==1)
		return 0;
	else
		return -1;
}

	

