#ifndef _EXIQ_H_
#define	_EXIQ_H_

#include "bsplib.h"
//#include "timer.h"

typedef struct
{
    u8	execute;
    u8	step;
    u8	index;
    u8	count;
    u32	TimeStamp;
} EXIQparaDef;

//扩展卡配置
#define	EXIEN		0
#define	EXQEN		1

#define	EXINUM	1			  //扩展I板数量
#define	EXQNUM	1            //扩展Q板数量

extern u8	EXI_ID[];     //扩展I板ID
extern u8	EXQ_ID[];     //扩展Q板ID

extern u8	EX_INPUT[][32];     //扩展I值
extern u8	EX_OUTPUT[][32];    //扩展Q值

extern void EXIQ(void);
extern s32 ExInPut_GetSta(u8 cardnum,u8 num);
extern s32 ExOutPut_SetSta(u8 cardnum, u8 num,u8 lev);
extern s32 ExOutPut_GetSta(u8 cardnum,u8 num);

#endif
