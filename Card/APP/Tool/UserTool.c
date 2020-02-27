#include "USERTOOL.H"
#include "AxisMove.h"
/********************************************************/
/*
 * Created: 11/16 
 * Description:
 * param:  
 * Modified By:yang
 */

void SetSpdDefault(u8 i)
{

    if (i > PULS_NUM)
    {
        return;
    }
    HZ_AxSetPara(i, GSS.AxisData[i].StartSpeed, GSS.AxisData[i].Acctime, GSS.AxisData[i].RunSpeed,
                 GSS.AxisData[i].Dectime, GSS.AxisData[i].EndSpeed,
                 GSS.AxisData[i].HomeSpeedFast, GSS.AxisData[i].HomeSpeedSlow,
                 GSS.AxisData[i].HomeOffset, 0, 0);
}
/********检查轴位置*************/
s32 Check_AxisStatu()
{
    u8 i;
    for(i = 0;i<PULS_NUM;i++)
    {
        if(HZ_AxGetStatus(i)!=0)
        {
            return -1;
        }
    }
    return 0;
}

s32 Check_xyzMotor()
{
    u8 i;
    for(i=0;i<3;i++)
    {
        if(HZ_AxGetStatus(i)!=0)
        {
            return -1;
        }
    }
    return 0;
}

/******************************/





///******************************/

//void AxisRunDefault(u32 i,u32 mode,u32 Ratio,s32 pos) 
//{
//	HZ_AxSetPara(i, GSS.AxisData[i].StartSpeed, GSS.AxisData[i].Acctime, \
//		GSS.AxisData[i].StartSpeed + __fabs(GSS.AxisData[i].RunSpeed - GSS.AxisData[i].StartSpeed)*Ratio / 100, \
//		GSS.AxisData[i].Dectime, GSS.AxisData[i].EndSpeed, \
//		GSS.AxisData[i].HomeSpeedFast, GSS.AxisData[i].HomeSpeedSlow, \
//		GSS.AxisData[i].HomeOffset, 0, 0);
//	switch(mode)
//	{
//		case ABSMODE:
//			HZ_AxMoveAbs(i,pos);
//			break;
//		case RELMODE:
//			HZ_AxMoveRel(i,pos);
//			break;
//		case SPDMODE:
//			HZ_AxMoveVelocity(i,pos);
//			break;
//		case GOHOMEMODE:
//			HZ_AxHome(i);
//			break;
//	}
//}

//气缸到位信号检测
//Return 0; 无错误
//ReTurn 1; 有错误
u32	CylinderCheckTimer[CylinderCheckNum];  
u32	CylinderPinTemp[CylinderCheckNum];
u32	CylinderCheckFlag[CylinderCheckNum];
u8 CylinderCheck(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT)
{
	if(OutPut_GetSta( CylinderPin) != CylinderPinTemp[Num])
	{
		CylinderCheckFlag[Num] = 1;
		if(OutPut_GetSta( CylinderPin) == 1)
		{
			CylinderCheckTimer[Num] = OffT;
		}
		else
		{
			CylinderCheckTimer[Num] = OnT;
		}
	}
	CylinderPinTemp[Num] = OutPut_GetSta( CylinderPin);
	if(CylinderCheckFlag[Num] == 1 && CylinderCheckTimer[Num] == 0)
	{
		CylinderCheckFlag[Num] = 0;
		if((OutPut_GetSta( CylinderPin) == 1 && OffPin == 1) || (OutPut_GetSta( CylinderPin) == 0 && OnPin == 1))
		{
			return 1;
		}
	}
	return 0;
}

//气缸到位信号检测
//Return 0; 无错误
//ReTurn 1; 有错误
//u32	CylinderCheckTimer[CylinderCheckNum];  
//u32	CylinderPinTemp[CylinderCheckNum];
//u32	CylinderCheckFlag[CylinderCheckNum];
u8 CylinderCheck_NYZ(u32 Num, u32 CylinderPin, u32 OnPin, u32 OffPin, u32 OnT, u32 OffT)
{
	if(CylinderPin != CylinderPinTemp[Num])
	{
		CylinderCheckFlag[Num] = 1;
		if(OutPut_GetSta( CylinderPin) == 1)
		{
			CylinderCheckTimer[Num] = OffT;
		}
		else
		{
			CylinderCheckTimer[Num] = OnT;
		}
	}
	CylinderPinTemp[Num] = OutPut_GetSta( CylinderPin);
	if(CylinderCheckFlag[Num] == 1 && CylinderCheckTimer[Num] == 0)
	{
		CylinderCheckFlag[Num] = 0;
		if((CylinderPin == 1 && OffPin == 1) || (CylinderPin == 0 && OnPin == 1))
		{
			return 1;
		}
	}
	return 0;
}





//设定单字的某位设置为1
s8 SetWhichBit(u32 *word,u32 whichBit)
{
	if(whichBit<32)
	{
		*word |= 1<<whichBit;
		return 1;
	}
	else
	{
		return -1;
	}
}

//设定单字的某位设置为0
s8 ResetWhichBit(u32 *word,u32 whichBit)
{
	if(whichBit<32)
	{
		*word &= ~(1<<whichBit);
		return 1;
	}
	else
	{
		return -1;
	}	
}
//判断32位某一位的值
u8 WordToBit(u32 Word ,u32 whichBit)
{
	if(Word & ((u32)1<<whichBit))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//最大值
s32 MaxS32(s32 a,s32 b)
{
	if(a>b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

//最小值
s32 MinS32(s32 a,s32 b)
{
	if(a>b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

//判断数值是否在范围内
s32 RangeS32(s32 value,s32 min,s32 max)
{
	s32 minTerm,maxTerm;
	
	if(min<=max)
	{
		minTerm = min;
		maxTerm = max;
	}
	else
	{
		minTerm = max;
		maxTerm = min;		
	}
	
	if(value<minTerm)
	{
		return minTerm;
	}
	else if(value>maxTerm)
	{
		return maxTerm;
	}
	else
	{
		return value;
	}
}


unsigned char  R_trigflagbuf[TIMER_NUM];
//上升缘检测，clk由0变为1时，该函数返回值为1，当下次再执行，返回值为0; 形参 num：上升沿检测编号
unsigned char R_Trig(char num,unsigned char clk)
{
    if(num>=TIMER_NUM)
	 num = TIMER_NUM - 1;
	if((char)clk!=(R_trigflagbuf[num]))
	  {
           R_trigflagbuf[num] = clk;
		   if (clk)
		     return 1;
		}
	return 0;
}

//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num : 输入编号
//return ：0：ON 1:OFF，其他参数错误
s32 InputGet(u8 cardAdr,u8 num)
{
	if(cardAdr == 0)
	{
		return InPut_GetSta(num);
	}
	else
	{
		return ExInPut_GetSta(cardAdr - 1,num);
	}
}


//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num
//return ：0：ON 1:OFF，其他参数错误
s32 OutputGet(u8 cardAdr,u8 num)
{
	if(cardAdr == 0)
	{
		return OutPut_GetSta(num);
	}
	else
	{
		return ExOutPut_GetSta(cardAdr - 1,num);
	}
}

//获取输入口状态
//cardAdr : 扩展卡地址  0:本体  1：从卡1 ......
//num：输入编号
//return ：0：ON 1:OFF，其他参数错误
s32 OutputSet(u8 cardAdr,u8 num,u8 value)
{
	if(cardAdr == 0)
	{
		return OutPut_SetSta(num,value);
	}
	else
	{
		return ExOutPut_SetSta(cardAdr - 1,num,value);
	}
}


