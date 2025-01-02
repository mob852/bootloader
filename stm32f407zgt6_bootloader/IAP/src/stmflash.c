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
//	u32 secpos;	   //扇区地址
//	u16 secoff;	   //扇区内偏移地址(16位字计算)
//	u16 secremain; //扇区内剩余地址(16位字计算)	   
// 	u16 i;    
//	u32 offaddr;   //去掉0X08000000后的地址
//	if((WriteAddr < FLASH_BASE) || (WriteAddr >= FLASH_BASE + 1024 * FLASH_SIZE))return;//非法地址
//	FLASH_Unlock();						//解锁
//	offaddr = WriteAddr - FLASH_BASE;		//实际偏移地址.
//	secpos = offaddr / PAGE_SIZE;			//扇区地址  0~127 for STM32F103RBT6
//	secoff = (offaddr % PAGE_SIZE) / 2;		//在扇区内的偏移(2个字节为基本单位.)
//	secremain = PAGE_SIZE / 2 - secoff;		//扇区剩余空间大小   
//	if(NumToWrite <= secremain)
//		secremain = NumToWrite;//不大于该扇区范围
//	while(1) 
//	{	
//		STMFLASH_Read(secpos * PAGE_SIZE + FLASH_BASE, STMFLASH_BUF, PAGE_SIZE / 2);//读出整个扇区的内容
//		for(i = 0; i < secremain; i++)//校验数据
//		{
//			if(STMFLASH_BUF[secoff + i] != 0XFFFF)break;//需要擦除  	  
//		}
//		if(i < secremain)//需要擦除
//		{
//			FLASH_EraseSector(secpos * PAGE_SIZE + FLASH_BASE,VoltageRange_3);//擦除这个扇区
//			for(i=0; i < secremain; i++)
//			{
//				STMFLASH_BUF[i + secoff] = pBuffer[i];	  
//			}
//			STMFLASH_Write_NoCheck(secpos * PAGE_SIZE + FLASH_BASE, STMFLASH_BUF, PAGE_SIZE / 2);//写入整个扇区  
//		}else 
//			STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
//		if(NumToWrite == secremain)break;//写入结束了
//		else//写入未结束
//		{
//			secpos++;				//扇区地址增1
//			secoff = 0;				//偏移位置为0 	 
//		   	pBuffer += secremain;  	//指针偏移
//			WriteAddr += secremain;	//写地址偏移	   
//		   	NumToWrite -= secremain;	//字节(16位)数递减
//			if(NumToWrite > (PAGE_SIZE / 2)) secremain = PAGE_SIZE / 2;//下一个扇区还是写不完
//			else secremain = NumToWrite;//下一个扇区可以写完了
//		}	 
//	};	
//	FLASH_Lock();//上锁
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
//		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
//		ReadAddr+=2;//偏移2个字节.	
//	}
//}


//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}  
//获取某个地址所在的flash扇区
//addr:flash地址
//返回值:0~11,即addr所在的扇区
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

// 获取下一个扇区的起始地址
// addr: 当前 Flash 地址
// 返回值: 下一个扇区的起始地址
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
    else return 0x080E0000; // 返回 Flash 最大地址或超出范围后的默认值
}




//从指定地址开始写入指定长度的数据
//特别注意:因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
//         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
//         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
//         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写. 
//该函数对OTP区域也有效!可以用来写OTP区!
//OTP区域地址范围:0X1FFF7800~0X1FFF7A0F
//WriteAddr:起始地址(此地址必须为4的倍数!!)
//pBuffer:数据指针
//NumToWrite:字(32位)数(就是要写入的32位数据的个数.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
  FLASH_Status status = FLASH_COMPLETE;
	u32 addrx=0;
	u32 endaddr=0;	
  if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
	FLASH_Unlock();									//解锁 
  FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存
 		
	addrx=WriteAddr;				//写入的起始地址
	endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
	if(addrx<0X1FFF0000)			//只有主存储区,才需要执行擦除操作!!
	{
		while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
			{   
				status=FLASH_EraseSector(STMFLASH_GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V之间!!
				if(status!=FLASH_COMPLETE)break;	//发生错误了
			}else addrx+=4;
		} 
	}
	if(status==FLASH_COMPLETE)
	{
		while(WriteAddr<endaddr)//写数据
		{
			if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//写入数据
			{ 
				break;	//写入异常
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
  FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
	FLASH_Lock();//上锁
} 

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		ReadAddr+=4;//偏移4个字节.	
	}
}















