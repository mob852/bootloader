#include "stmflash.h"
#include "iap_config.h"
///**
//  * @brief  Read half words (16-bit data) of the specified address
//  * @note   This function can be used for all STM32F10x devices.
//  * @param  faddr: The address to be read (the multiple of the address, which is 2)
//  * @retval Value of specified address
//  */
//u16 STMFLASH_ReadHalfWord(u32 faddr)
//{
//	return *(vu16*)faddr; 
//}


///**
//  * @brief  There is no check writing.
//  * @note   This function can be used for all STM32F10x devices.
//  * @param  WriteAddr: The starting address to be written.
//  * @param  pBuffer: The pointer to the data.
//  * @param  NumToWrite:  The number of half words written
//  * @retval None
//  */
//static void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
//{ 			 		 
//	u16 i;
//	for(i=0;i<NumToWrite;i++)
//	{
//		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
//	    WriteAddr+=2;//add addr 2.
//	}  
//} 

//u16 STMFLASH_BUF[PAGE_SIZE / 2];//Up to 2K bytes

///**
//  * @brief  Write data from the specified address to the specified length.
//  * @note   This function can be used for all STM32F10x devices.
//  * @param  WriteAddr: The starting address to be written.(The address must be a multiple of two)
//  * @param  pBuffer: The pointer to the data.
//  * @param  NumToWrite:  The number of half words written
//  * @retval None
//  */
//void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
//{
//	u32 secpos;	   //������ַ
//	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
//	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
// 	u16 i;    
//	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
//	if((WriteAddr < FLASH_BASE) || (WriteAddr >= FLASH_BASE + 1024 * FLASH_SIZE))return;//�Ƿ���ַ
//	FLASH_Unlock();						//����
//	offaddr = WriteAddr - FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
//	secpos = offaddr / PAGE_SIZE;			//������ַ  0~127 for STM32F103RBT6
//	secoff = (offaddr % PAGE_SIZE) / 2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
//	secremain = PAGE_SIZE / 2 - secoff;		//����ʣ��ռ��С   
//	if(NumToWrite <= secremain)
//		secremain = NumToWrite;//�����ڸ�������Χ
//	while(1) 
//	{	
//		STMFLASH_Read(secpos * PAGE_SIZE + FLASH_BASE, STMFLASH_BUF, PAGE_SIZE / 2);//������������������
//		for(i = 0; i < secremain; i++)//У������
//		{
//			if(STMFLASH_BUF[secoff + i] != 0XFFFF)break;//��Ҫ����  	  
//		}
//		if(i < secremain)//��Ҫ����
//		{
//			FLASH_EraseSector(secpos * PAGE_SIZE + FLASH_BASE,VoltageRange_3);//�����������
//			for(i=0; i < secremain; i++)
//			{
//				STMFLASH_BUF[i + secoff] = pBuffer[i];	  
//			}
//			STMFLASH_Write_NoCheck(secpos * PAGE_SIZE + FLASH_BASE, STMFLASH_BUF, PAGE_SIZE / 2);//д����������  
//		}else 
//			STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
//		if(NumToWrite == secremain)break;//д�������
//		else//д��δ����
//		{
//			secpos++;				//������ַ��1
//			secoff = 0;				//ƫ��λ��Ϊ0 	 
//		   	pBuffer += secremain;  	//ָ��ƫ��
//			WriteAddr += secremain;	//д��ַƫ��	   
//		   	NumToWrite -= secremain;	//�ֽ�(16λ)���ݼ�
//			if(NumToWrite > (PAGE_SIZE / 2)) secremain = PAGE_SIZE / 2;//��һ����������д����
//			else secremain = NumToWrite;//��һ����������д����
//		}	 
//	};	
//	FLASH_Lock();//����
//}

///**
//  * @brief  Start reading the specified data from the specified address.
//  * @note   This function can be used for all STM32F10x devices.
//  * @param  ReadAddr: Start addr
//  * @param  pBuffer: The pointer to the data.
//  * @param  NumToWrite:  The number of half words written(16bit)
//  * @retval None
//  */
//void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
//{
//	u16 i;
//	for(i=0;i<NumToRead;i++)
//	{
//		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
//		ReadAddr+=2;//ƫ��2���ֽ�.	
//	}
//}


//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

// ��ȡ��һ����������ʼ��ַ
// addr: ��ǰ Flash ��ַ
// ����ֵ: ��һ����������ʼ��ַ
u32 STMFLASH_GetNextSectorStart(u32 addr)
{
    if (addr < ADDR_FLASH_SECTOR_1) return ADDR_FLASH_SECTOR_1;
    else if (addr < ADDR_FLASH_SECTOR_2) return ADDR_FLASH_SECTOR_2;
    else if (addr < ADDR_FLASH_SECTOR_3) return ADDR_FLASH_SECTOR_3;
    else if (addr < ADDR_FLASH_SECTOR_4) return ADDR_FLASH_SECTOR_4;
    else if (addr < ADDR_FLASH_SECTOR_5) return ADDR_FLASH_SECTOR_5;
    else if (addr < ADDR_FLASH_SECTOR_6) return ADDR_FLASH_SECTOR_6;
    else if (addr < ADDR_FLASH_SECTOR_7) return ADDR_FLASH_SECTOR_7;
    else if (addr < ADDR_FLASH_SECTOR_8) return ADDR_FLASH_SECTOR_8;
    else if (addr < ADDR_FLASH_SECTOR_9) return ADDR_FLASH_SECTOR_9;
    else if (addr < ADDR_FLASH_SECTOR_10) return ADDR_FLASH_SECTOR_10;
    else if (addr < ADDR_FLASH_SECTOR_11) return ADDR_FLASH_SECTOR_11;
    else return 0x080E0000; // ���� Flash ����ַ�򳬳���Χ���Ĭ��ֵ
}




//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FFF7800~0X1FFF7A0F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
	FLASH_Unlock();									//���� 
  FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���
 		
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
	if(addrx<0X1FFF0000)			//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
				if(status!=FLASH_COMPLETE)break;	//����������
			}else addrx+=4;
		} 
	}
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)//д����
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
  FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
	FLASH_Lock();//����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}















