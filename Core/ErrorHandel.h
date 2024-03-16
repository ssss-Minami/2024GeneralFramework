#ifndef __ERRORHANDEL_H__
#define __ERRORHANDEL_H__

#include "main.h"

#define MOTOR0LOST 0
#define MOT1R1LOST 1
#define MOT2R0LOST 2
#define MOT3R0LOST 3
#define MOT4R0LOST 4
#define MOT5R0LOST 5
#define MOT6R0LOST 6
#define MOT7R0LOST 7
#define MOT8R0LOST 8
#define MOT9R0LOST 9

#define OUT_OF_ENUM 20
#define NULL_POINTER 21






extern uint8_t error_flag;
extern void sErrorHandel(uint8_t flag);

#endif