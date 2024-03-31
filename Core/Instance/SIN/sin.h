#ifndef __SIN_H__
#define __SIN_H__
#include "main.h"

#ifndef PI
#define PI 3.1415926
#endif

#define TIMER_PERIOD 0.001
#define PERIOD_NUM 10
#define FREQTAB_LEN 64
#define SINTAB_LEN 128
#define OPT_MAX 60.0f

typedef struct
{
	uint32_t sin_time;		//已持续时间
	uint8_t freq_idx;		//频率表索引
	uint8_t sin_period;		//每个频率已持续周期数
	uint8_t sin_idx;		//sin表索引
	float *sin_tab_pt;		//指向sin128表
	float *freq_tab_pt;		//指向频率表
}Sin_Typedef;
extern Sin_Typedef sin_struct;
extern float Freq_Tab[64];
extern float SinWave(Sin_Typedef *sin);
extern void SinInit();
#endif
