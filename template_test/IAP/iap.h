#ifndef __IAP_H
#define __IAP_H
#include "sys.h"


#define INIT_FLAG_DATA      0xFFFF   //Ĭ�ϱ�־������(��Ƭ�ӵ����)
#define UPDATE_FLAG_DATA      0xEEEE   //���ر�־������
#define UPLOAD_FLAG_DATA      0xDDDD   //�ϴ���־������
#define ERASE_FLAG_DATA       0xCCCC   //������־������
#define APPRUN_FLAG_DATA      0x5A5A   //APP����Ҫ���κδ���ֱ������״̬

#define IAP_FLAG_ADDR   (uint32_t)(0x08004000)//App�����Bootloader��������Ϣ�ĵ�ַ(�ݶ���СΪsector1)

void IAP_Handle(u8 * cmd);

#endif
