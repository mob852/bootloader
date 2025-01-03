#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "stm32f4xx.h"
#include "delay.h"


void HC_SR04_Init(void);
float Calculate_Distance(void);



#endif
