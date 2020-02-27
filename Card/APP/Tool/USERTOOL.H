#ifndef _USERTOOL_H_
#define	_USERTOOL_H_

#include "Logic.h"
#include "EXIQ.h"
//#include "DataDefine.h"
//#include "bsplib.h"
//#include "interlayer_config.h"

#define	CylinderCheckNum	10
	
//extern void SetAxisSpdRadio(u8 i,u8 spd);
//extern s32 SetSpd(u8 i, u32 RunSpeed);

extern s32 Check_AxisStatu(void);
extern s32 Check_xyzMotor(void);


extern s8 SetWhichBit(u32 *word,u32 whichBit);
extern s8 ResetWhichBit(u32 *word,u32 whichBit);
extern u8 WordToBit(u32 Word ,u32 whichBit);
extern s32 MaxS32(s32 a,s32 b);
extern s32 MinS32(s32 a,s32 b);
extern s32 RangeS32(s32 value,s32 min,s32 max);
extern u8 CylinderCheck(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT);
extern u8 CylinderCheck_NYZ(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT);
extern unsigned char R_Trig(char num,unsigned char clk);

//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num : 输入编号
//return ：0：ON 1:OFF，其他参数错误
extern s32 InputGet(u8 cardAdr,u8 num);


//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num
//return ：0：ON 1:OFF，其他参数错误
extern s32 OutputGet(u8 cardAdr,u8 num);

//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num：输入编号
//return ：0：ON 1:OFF，其他参数错误
extern s32 OutputSet(u8 cardAdr,u8 num,u8 value);
#endif


