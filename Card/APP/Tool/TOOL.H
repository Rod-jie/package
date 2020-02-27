#ifndef _TOOL_H_
#define	_TOOL_H_

#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"


extern float ConvertToUserUnit(u32 num,u32 pulse);
extern u32 ConvertToMachineUnit(u32 num,float MM);
extern s32 Check_AxisStatu(void);
extern s32 Check_xyzMotor(void);
#endif


