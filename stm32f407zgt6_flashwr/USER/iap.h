#ifndef __IAP_H
#define __IAP_H
#include "sys.h"


#define INIT_FLAG_DATA      0xFFFF   //默认标志的数据(空片子的情况)
#define UPDATE_FLAG_DATA      0xEEEE   //下载标志的数据
#define UPLOAD_FLAG_DATA      0xDDDD   //上传标志的数据
#define ERASE_FLAG_DATA       0xCCCC   //擦除标志的数据
#define APPRUN_FLAG_DATA      0x5A5A   //APP不需要做任何处理，直接运行状态

#define IAP_FLAG_ADDR   (uint32_t)(0x08004000)//App区域和Bootloader区域共享信息的地址(暂定大小为sector1)

void IAP_Handle(u8 * cmd);

#endif
