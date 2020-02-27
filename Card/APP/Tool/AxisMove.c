#include "axismove.h"
#include "USERTOOL.H"
#include <stdio.h>
#include "stdlib.h"
/**
* @author 19/02/25  yang
* @Description: 上位机轴移动接口
* @param -- Para 移动参数
* @param -- T 任务名
* @return --
*/
void Move(GoPosPara Para[], LogicParaDef T[])
{
    for(int i = 0; i < AXIS_NUM; i++)
    {
        if (Para[i].execute == 1&&Para[i].mode != STOPMODE) //该轴运动
        {
            T[i].execute = 1;
        }
        if(Para[i].execute == 1&& Para[i].mode == STOPMODE)  //立即停轴
        {
            PARAINIT(T[i]);
            PARAINIT(Para[i]);
            HZ_AxStop(i);
        }
        INITTASK(T[i])
        switch (T[i].step)
        {
        case 1:
            if (HZ_AxGetStatus(i) == AXSTA_READY)
            {
                MotorMove(i, Para[i].speed, Para[i].pos, Para[i].mode);
            }
            T[i].step = 2;
            break;
        case 2:
            if (HZ_AxGetStatus(i) == AXSTA_READY||HZ_AxGetStatus(i)==AXSTA_ERRSTOP)
            {
                PARAINIT(T[i]);
                PARAINIT(Para[i]);
            }
            break;
        default:
            break;
        }
    }
}
/**
* @author 19/02/19  yang
* @Description: 轴移动接口
* @param -- 轴号 速度 位置 模式
* @param --
* @return --
*/
s32 MotorMove(u32 num,u32 speed,s32 pos,u32 mode)
{
    if(num>PULS_NUM)
        return -1;
    SetSpd(num, speed);	//设置速度
    switch(mode)
    {
    case ABSMODE:
        HZ_AxMoveAbs(num, pos);
        break;
    case RELMODE:
        HZ_AxMoveRel(num, pos);
        break;
    case SPDMODE:
        HZ_AxMoveVelocity(num, pos);
        break;
    case GOHOMEMODE:
        //回零模式下速度为回零快速
        GSS.AxisData[num].RunSpeed = speed;
        HZ_AxSetPara(num, GSS.AxisData[num].StartSpeed, GSS.AxisData[num].Acctime,
                     GSS.AxisData[num].StartSpeed ,
                     GSS.AxisData[num].Dectime, GSS.AxisData[num].EndSpeed,
                     GSS.AxisData[num].RunSpeed, GSS.AxisData[num].HomeSpeedSlow,
                     GSS.AxisData[num].HomeOffset, 0, 0);
        HZ_AxHome(num);
        break;
    default:
        break;
    }
    return 0;
}
/*
 * Created: 11/16
 * Description:
 * param:
 * Modified By:yang
 */
void SetAxisSpdRadio(u8 i,u8 spd)
{
    if(spd>=100||spd<=0)
    {
        spd = 100;
    }
    HZ_AxSetPara(i, GSS.AxisData[i].StartSpeed, GSS.AxisData[i].Acctime,
                 GSS.AxisData[i].StartSpeed + __fabs(GSS.AxisData[i].RunSpeed - GSS.AxisData[i].StartSpeed)*spd / 100,
                 GSS.AxisData[i].Dectime, GSS.AxisData[i].EndSpeed,
                 GSS.AxisData[i].HomeSpeedFast, GSS.AxisData[i].HomeSpeedSlow,
                 GSS.AxisData[i].HomeOffset, 0, 0);
}

s32 SetSpd(u8 i, u32 RunSpeed)
{
    if (i > PULS_NUM)
    {
        return -1;
    }
    if (RunSpeed < GSS.AxisData[i].StartSpeed)
    {
        RunSpeed = GSS.AxisData[i].StartSpeed + 1;
    }
    if (HZ_AxSetPara(i, GSS.AxisData[i].StartSpeed, GSS.AxisData[i].Acctime, RunSpeed,
                     GSS.AxisData[i].Dectime, GSS.AxisData[i].EndSpeed,
                     GSS.AxisData[i].HomeSpeedFast, GSS.AxisData[i].HomeSpeedSlow,
                     GSS.AxisData[i].HomeOffset, 0, 0) == 0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}


/**
* @author： 2018/08/20  Administrator
* @Description: 初始化速度
* @param –
* @param --
* @return --
*/
void SpeedParaInit()
{
	u8 i;
	if (Data.initflag)
	{
        Data.initflag = 0;
        for(i=0;i<PULS_NUM;i++)
        {
            GSS.AxisData[i].StartSpeed = 2000;
            GSS.AxisData[i].Acctime = 50;
            GSS.AxisData[i].RunSpeed = 10000;
            GSS.AxisData[i].Dectime = 50;
            GSS.AxisData[i].EndSpeed = 2000;
            GSS.AxisData[i].HomeSpeedFast = 1000;
            GSS.AxisData[i].HomeSpeedSlow = 700;
            GSS.AxisData[i].HomeOffset = 0;
            GSS.AxisData[i].SoftMaxLimit = 99999;
            GSS.AxisData[i].SoftMinLimit = -99999;
        }
		HZ_Data_Write();
	}
}

/**
* @author 18/12/1  NYZ
* @Description: 把脉冲数转换程用户单位，例如，脉冲转换程毫米
* @param -- 轴号 脉冲数
* @param --
* @return -- 距离或角度 等用户单位
*/
float PulseToUserUnit(AxisConversion *p,s32 Dat)   
{
	return (((float)Dat*(float)p->MPR)/(float)p->PPR);
}

/**
* @author 18/12/1  NYZ
* @Description: 把用户单位转换成脉冲数，例如，毫米->脉冲数
* @param -- 轴号 单位距离
* @param --
* @return -- 脉冲数
*/
s32	UserUnitToPulse(AxisConversion *p,float Dat)
{
	return (s32)(Dat*(float)p->PPR/p->MPR);
}


/**
* @author 18/12/1  NYZ
* @Description: 以用户单位移动轴
* @param -- 轴号 模式 速度比例 位置 
* @param --
* @return --
*/
void MotorMoveUU(u8 i,u32 mode,u32 Ratio,float pos)   
{
	HZ_AxSetPara(i, GSS.AxisData[i].StartSpeed, GSS.AxisData[i].Acctime, \
		GSS.AxisData[i].StartSpeed + __fabs(GSS.AxisData[i].RunSpeed - GSS.AxisData[i].StartSpeed)*Ratio / 100, \
		GSS.AxisData[i].Dectime, GSS.AxisData[i].EndSpeed, \
		GSS.AxisData[i].HomeSpeedFast, GSS.AxisData[i].HomeSpeedSlow, \
		GSS.AxisData[i].HomeOffset, 0, 0);
	switch(mode)
	{
		case ABSMODE:
			HZ_AxMoveAbs(i,UserUnitToPulse(&GSS.Conversion[i],pos));
			break;
		case RELMODE:
			HZ_AxMoveRel(i,UserUnitToPulse(&GSS.Conversion[i],pos));
			break;
		case SPDMODE:
			HZ_AxMoveVelocity(i,UserUnitToPulse(&GSS.Conversion[i],pos));
			break;
		case GOHOMEMODE:
			HZ_AxReset(i);
			HZ_AxHome(i);
			break;
	}
}


/**
* @author 18/12/1  NYZ
* @Description: 以用户单位移动轴
* @param -- 轴号 模式 速度比例 位置 
* @param --
* @return --
*/
void MC_MoveUU(u8 i,u32 mode,u32 StartSpeed,u32 Acctime, u32 TargtSpeed,float TargetPos)   
{
	if(StartSpeed>TargtSpeed)
	{	
		TargtSpeed = StartSpeed + 5;
	}	
	
	HZ_AxSetPara(i, StartSpeed, Acctime, \
		TargtSpeed, \
		Acctime,StartSpeed, \
		StartSpeed, TargtSpeed, \
		UserUnitToPulse(&GSS.Conversion[i],TargetPos), 0, 0);
	
	switch(mode)
	{
		case ABSMODE:
			HZ_AxMoveAbs(i,UserUnitToPulse(&GSS.Conversion[i],TargetPos));
			break;
		case RELMODE:
			HZ_AxMoveRel(i,UserUnitToPulse(&GSS.Conversion[i],TargetPos));
			break;
		case SPDMODE:
			HZ_AxMoveVelocity(i,UserUnitToPulse(&GSS.Conversion[i],TargetPos));
			break;
		case GOHOMEMODE:
			HZ_AxReset(i);
			HZ_AxHome(i);
			break;
	}
}

/**
* @author 18/12/1  NYZ
* @Description: 以脉冲单位移动轴
* @param -- 轴号 模式 速度 位置 
* @param --
* @return --
*/
void MC_MovePP(u8 i,u32 mode,u32 StartSpeed,u32 Acctime, u32 TargtSpeed,s32 TargetPos)   
{
	if(StartSpeed>TargtSpeed)
	{	
		TargtSpeed = StartSpeed + 5;
	}	
	HZ_AxSetPara(i, StartSpeed, Acctime, \
		TargtSpeed, \
		Acctime,StartSpeed, \
		StartSpeed, TargtSpeed, \
		TargetPos, 0, 0);
	
 
	
	switch(mode)
	{
		case ABSMODE:
			HZ_AxMoveAbs(i,TargetPos);
			break;
		case RELMODE:
			HZ_AxMoveRel(i,TargetPos);
			break;
		case SPDMODE:
			HZ_AxMoveVelocity(i,TargetPos);
			break;
		case GOHOMEMODE:
			HZ_AxReset(i);
			HZ_AxHome(i);
			break;
	}
}

/**
* @author 2019/5/17  NYZ
* @Description: 计算旋切电子凸轮表
* @param -- 
* @param --
* @return --
*/

#define T (*Table)
s8 CamTableCaculate(CamTableDef *Table)
{
	s32 i,j;
	u32 synT,flyT;   //同步区点数,飞剪区点数
	float slaveFlyPulse;   //从轴追赶区脉冲数
//	float slaveSynPulse;   //从轴同步区脉冲数
	float v,acc,jeck;
	float slaveSynVel;		//从轴同步区等分脉冲数
	float term1,term2; 		//		
	float accTRate;      //从轴加速段比例
	float	vTRate; 				//从轴匀速端比例
	float t[5];
	int term;
	float mainCirPulse;
	
	for(i=0;i<CAMPointNum;i++)
	{
		T.SlaveAxisPos[i] = 0;
		T.SlaveAxisRelPos[i] = 0;
	}
	
	if(1)
	{	
		if((T.MainCirPulse - T.SynPulse)* T.SpeedMul > 2*(T.SlaveCirPulse - (float)T.SynPulse*T.SpeedMul) || T.TooLongFlag == 1)
		{
			accTRate = 0.48f;
			vTRate = 0.04f;			
			mainCirPulse = 2*T.SlaveCirPulse / T.SpeedMul - T.SynPulse;	
			T.MainPlus1 = (mainCirPulse - T.SynPulse)/2;	
			T.MainPlus2 = mainCirPulse - T.MainPlus1;	
			if(T.TooLongFlag == 0)
			{
				T.LongStopPos = T.MainCirPulse + T.MainStartPos;
				T.LongEndFlag = 1;
			}
			T.TooLongFlag = 1;
		}
		else
		{
			accTRate = 0.45f;
			vTRate = 0.1f;			
			mainCirPulse = T.MainCirPulse;
			T.TooLongFlag = 0;
			T.LongEndFlag = 0;
		}
				
		T.MainAxisTPulse = mainCirPulse/(float)T.Ttime;	
		synT = T.SynPulse/T.MainAxisTPulse;
		flyT = T.Ttime - synT;
		slaveFlyPulse = T.SlaveCirPulse - (float)T.SynPulse*T.SpeedMul;
//		slaveSynPulse = (float)T.SynPulse*T.SpeedMul;
		slaveSynVel = T.MainAxisTPulse*T.SpeedMul;
		T.flyT = flyT;
		v = (slaveFlyPulse - slaveSynVel*flyT)/((1.0f - accTRate)*(float)flyT);
		
		//合成速度不允许小与0，如果小与0，择调整分配比例
		if((v + slaveSynVel)<0)
		{
			v = -1.0*slaveSynVel;
			accTRate = 1.0f - (slaveFlyPulse - slaveSynVel*(float)flyT)/v/(float)flyT;
			vTRate = 1.0f - 2.0f*accTRate;
		}		
					
			t[0] = accTRate*(float)flyT/2;
			t[1] = accTRate*(float)flyT/2 + t[0];
			t[2] = vTRate*(float)flyT    +  t[1];
			t[3] = accTRate*(float)flyT/2 + t[2];
			t[4] = accTRate*(float)flyT/2 + t[3];	


			T.TT[0] = t[0];
			T.TT[1] = t[1];
			T.TT[2] = t[2];
			T.TT[3] = t[3];
			T.TT[4] = t[4];	


			acc = 2.0f*v/(accTRate*(float)flyT);
			jeck = acc/(float)t[0];
			term1 = 0;
			term2 = 0;
		
		for(j=0;j<=T.Ttime;j++)
		{
			if(j<=synT)
			{
				term2 = slaveSynVel*j;
			}
			else
			{
					i = j-synT;
					if( i>0 && i<=T.TT[0])
					{
						term1 = 0.5f*jeck*i*i ;
						T.Vel[0] = 0.5f*jeck*i*i;
					}
					else if(i>T.TT[0] && i <= T.TT[1])
					{
						term1 = T.Vel[0] + jeck*t[0]*((float)i-t[0]) - 0.5f*jeck*((float)i-t[0])*((float)i-t[0]) ;
						T.Vel[1] = T.Vel[0] + jeck*t[0]*((float)i-t[0]) - 0.5f*jeck*((float)i-t[0])*((float)i-t[0]) ;
					}
					else if(i>T.TT[1] && i <= T.TT[2])
					{
						term1 = T.Vel[1];
						T.Vel[2] = T.Vel[1];				
					}
					else if(i>T.TT[2] && i <= T.TT[3])
					{
						term1 = T.Vel[2] - 0.5f*jeck*((float)i-t[2])*((float)i-t[2]);
						T.Vel[3] = T.Vel[2] - 0.5f*jeck*((float)i-t[2])*((float)i-t[2]);					
					}
					else if(i>T.TT[3] && i <= T.TT[4])
					{
						term1 = T.Vel[3] - jeck*((float)i-t[3])*(t[3]-t[2])  + 0.5f*jeck*((float)i-t[3])*((float)i-t[3]);
						T.Vel[4] = T.Vel[3] - jeck*((float)i-t[3])*(t[3]-t[2])  + 0.5f*jeck*((float)i-t[3])*((float)i-t[3]);
					}
					else
					{
						term1 = 0;
					}
					term2 = term1 + term2 + slaveSynVel;						
				}
			T.SlaveAxisPos[j]  = term2;
		}

			//*******************************从轴分配多余脉冲数*****************************//	
				term = T.SlaveCirPulse - T.SlaveAxisPos[T.Ttime];
				term1 = (float)term/(float)flyT;
				for(i=1; i<= flyT;i++)
				{
					T.SlaveAxisPos[i+synT]= T.SlaveAxisPos[i+synT] + (float)i*term1;
					if(T.SlaveAxisPos[i+synT]<T.SlaveAxisPos[i+synT-1])
					{
						T.SlaveAxisPos[i+synT] = T.SlaveAxisPos[i+synT-1];
					}
				}
			//*******************************End*****************************///
				
			//方便监控，不参与计算
			T.SlaveAxisRelPos[0] = T.SlaveAxisPos[0];	
			for(i=0;i<T.Ttime;i++)
			{				
				T.SlaveAxisRelPos[i] = T.SlaveAxisPos[i+1] - T.SlaveAxisPos[i];
			}

			T.RealMainPulse = 0;			
			T.RealSlavePulse = 0;
			for(i=0;i<T.Ttime;i++)
			{
				T.RealSlavePulse += T.SlaveAxisRelPos[i];
			}						
		}
	return 1;
}





/**
* @author 2019/5/17  NYZ
* @Description: 获取主轴当前凸轮位置
* @param -- 凸轮表
* @param --
* @return -- 主轴凸轮位置
*/
u32 CamGetMainIndex(CamTableDef *Table,u8 MainAxis)
{
	static u32 index;
	if(CamTable.TooLongFlag == 0)
	{
		index = T.MainCurrPos/T.MainAxisTPulse;
	}
	else
	{
		if(HZ_AxGetCurPos(MainAxis) <= (T.MainStartPos + T.MainPlus1))
		{

			index = (HZ_AxGetCurPos(MainAxis) - T.MainStartPos)/T.MainAxisTPulse;
		}	
		else
		{
			if(CamTable.LongEndFlag == 1)
			{
				if(HZ_AxGetCurPos(MainAxis)>= T.LongStopPos)
				{	
					index = T.Ttime;
				}				
				else if(HZ_AxGetCurPos(MainAxis) >= (T.LongStopPos - T.MainPlus2))
				{
					index =T.MainPlus1/T.MainAxisTPulse + (HZ_AxGetCurPos(MainAxis)- (T.LongStopPos - T.MainPlus2))/T.MainAxisTPulse;
				}
				else
				{
					index = T.MainPlus1/T.MainAxisTPulse;
				}
			}
			else
			{
				index =T.MainPlus1/T.MainAxisTPulse;
			}
		}							
	}
	
	if(index>T.Ttime)
	{
		index = T.Ttime;
	}	
	return index;
}

/**
* @author 2019/5/17  NYZ
* @Description: 获取从轴当前凸轮位置
* @param -- 凸轮表
* @return -- 从轴凸轮位置
*/
u32 CamGetSlaveIndex(CamTableDef *Table)
{
	u32 index,i;
	index = 0;
	//判断凸轮是否在运行状态
	for(i=0;i<=T.Ttime;i++)
	{
		if(T.SlaveCurrPos >= T.SlaveAxisPos[i])
		{
			index = i;
		}
	}
	return index%T.Ttime;	
}


/**
* @author 2019/5/17  NYZ
* @Description: 获取从轴当前凸轮位置
* @param -- 凸轮表
* @return -- 0：凸轮没运行  1：凸轮正在运行
*/
u32 CamGetIndex(CamTableDef *Table)
{
	u32 index,i;
	index = 0;
	//判断凸轮是否在运行状态
	if(T.busy == 1)
	{
		for(i=1;i<=T.Ttime;i++)
		{
			if(T.SlaveCurrPos > T.SlaveAxisPos[i-0] && T.SlaveCurrPos<=T.SlaveAxisPos[i])
			{
				index = i;
				return index;
			}
		}
	}
	return index;	
}

/**
* @author 2019/5/17  NYZ
* @Description: 实时更新凸轮位置
* @param -- 主轴
* @param -- 从轴
* @param -- 凸轮表
* @return -- 
*/
s8 CamCurrPosRef(u8 MainAxis,u8 SlaveAxis,CamTableDef *Table)
{
	int mainCurrPlus,slaveCurrPlus;	
	u16 i;
/////************************************循环计脉冲*************************************//////
	//判断是主轴是实轴，还是编码器轴
	if(T.MainAxisMode == 0)
	{
		mainCurrPlus = HZ_AxGetCurPos(MainAxis) - T.MainStartPos;	//主轴当前位置 = 主轴当前位置 - 起始位置 
	}
	else
	{
		
		mainCurrPlus = (EncodeGetPos(MainAxis) - T.MainStartPos);
	}	
	
	slaveCurrPlus = (HZ_AxGetCurPos(SlaveAxis) - T.SlaveStartPos)%T.SlaveCirPulse;
	
	//判断当前脉冲数是否小于0，如果小与0,那就加上周期脉冲数，保证脉冲计数是循环计数
	if(mainCurrPlus>= 0)
	{
		T.MainCurrPos = mainCurrPlus;	
	}
	else
	{
		T.MainCurrPos = mainCurrPlus + T.MainCirPulse;		
	}
	
	if(slaveCurrPlus >= 0)
	{
		T.SlaveCurrPos = slaveCurrPlus;
	}
	else
	{
		T.SlaveCurrPos = slaveCurrPlus + T.SlaveCirPulse;
	}
	
//	T.MainIndex = T.MainCurrPos/T.MainAxisTPulse;
	for(i=0;i<=T.Ttime;i++)
	{
		if(T.SlaveCurrPos < T.SlaveAxisPos[i])
		{
			T.SlaveIndex = i;
			break;
		}
	}	
/////********************************************end************************************//////	
	return 1;
}

/**
* @author 2019/5/17  NYZ
* @Description: 设置凸轮位置
* @param -- 
* @param -- 主轴类型，0：实轴 1：编码器轴
* @param --
* @return --
*/
s8 CamSetPos(s32 mainStartPos,s32 slaveStartPos,CamTableDef *Table)
{
		T.MainStartPos = mainStartPos;
		T.MainStartPos = slaveStartPos;
		return 1;	
}

/**
* @author 2019/7/17  NYZ
* @Description: 从轴速度计算
* @param -- 
* @param -- 主轴类型，0：实轴 1：编码器轴
* @param --
* @return --
*/
u32 SlaveSpeedCaculate(u32 MainAxis,CamTableDef *Table)
{
	float yPulseNum;
	float k1;
	static float k;
	yPulseNum = T.SlaveAxisPos[T.index] - T.SlaveCurrPos;	
	if(yPulseNum > (s32)T.SlaveCirPulse/2)
	{
		yPulseNum  = (s32)T.SlaveCirPulse - yPulseNum;
	}
	
	if(yPulseNum < 0 && (-yPulseNum < (s32)T.SlaveCirPulse/2))
	{
//						yPulseNum  = 0;
	}					
	else if(yPulseNum < 0 && (-yPulseNum >= (s32)T.SlaveCirPulse/2))
	{
		yPulseNum += T.SlaveCirPulse;
	}
	
	T.MainSpeed = GetSpeed(T.MainAxisMode,MainAxis);
		
	if(T.index < CAMPointNum - 1)
	{					
		k = (float)abs(T.SlaveAxisPos[T.index + 1] - T.SlaveAxisPos[T.index])/T.MainAxisTPulse;
	}
	else
	{
		k = (float)abs(T.SlaveAxisPos[T.index] - T.SlaveAxisPos[T.index-1])/T.MainAxisTPulse;
	}
	
	if(yPulseNum > 10)
	{
		k1 = 1.02;
	}
	else if(yPulseNum<-10)
	{
		k1 = 0.98;
	}
	else
	{
		k1 = 1;
	}					
	T.yDif = yPulseNum;
		
	T.SlaveSpeed = T.MainSpeed* k * k1;
	if(T.SlaveSpeed>400000)
	{
		T.SlaveSpeed = 400000;
	}
	return T.SlaveSpeed;	
}

/**
* @author 2019/5/17  NYZ
* @Description: 运行凸轮曲线，间歇凸轮
* @param -- 
* @param --
* @return --
*/
float k1 = 1.3;
u32 t = 100;
#define FRNUN 10
u32 speed[FRNUN];
s8 CamRun(u8 MainAxis,u8 SlaveAxis,CamTableDef *Table)
{
  float xPulseNum;
	float yPulseNum;
	static float k;
//	u16 i;
	
	s32 a1,a2;
	CamCurrPosRef(MainAxis,SlaveAxis,Table);
	if(T.excute == 1 && T.step == 0)
	{
		T.step = 0xA0;
		T.busy = 1;
		
//		T.MainStartPos = HZ_AxGetCurPos(MainAxis);
//		T.SlaveStartPos = 0;
		//计算凸轮表
		CamTableCaculate(Table);
	}
	
	T.index = CamGetMainIndex(Table,MainAxis);	
	switch(T.step)
	{
		//CamIn 凸轮咬合
		
		case 0xA0:
			if(HZ_AxGetStatus(SlaveAxis) != AXSTA_READY)
			{
				HZ_AxStop(SlaveAxis);			
			}
			T.step = 0xA1;
			break;
		
		case 0xA1:
			if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
			{			
				a1 = abs(T.index - T.SlaveIndex);
				a2 = T.Ttime - abs(T.index - T.SlaveIndex);
				T.step = 0xA2;
			}
			break;

		case 0xA2:
			a1 = abs(T.index - T.SlaveIndex);
			a2 = T.Ttime - abs(T.index - T.SlaveIndex);			
			if(a1<10 || a2<10)
			{	
				T.step = 1;
			}
			else
			{
				T.step = 0xA2;
			}
			break;			
		
		case 1:
			if(HZ_AxGetStatus(MainAxis) != AXSTA_READY)
			{					
				T.step = 2;
			}
			break;
				
		case 2:							
				T.indexTerm = T.index;		
				//判断凸轮旋转方向
					xPulseNum = T.MainAxisTPulse;				
					yPulseNum = T.SlaveAxisPos[T.index+1] - T.SlaveCurrPos;	
					if(yPulseNum > (s32)T.SlaveCirPulse/2)
					{
						yPulseNum  = (s32)T.SlaveCirPulse - yPulseNum;
					}
					
					if(yPulseNum < 0 && (-yPulseNum < (s32)T.SlaveCirPulse/2))
					{
						yPulseNum  = 0;
					}
					else if(yPulseNum < 0 && (-yPulseNum >= (s32)T.SlaveCirPulse/2))
					{
						 yPulseNum += T.SlaveCirPulse;
					}
					
				T.MainSpeed = GetSpeed(T.MainAxisMode,MainAxis);
				
				k = (float)abs(yPulseNum)/(float)abs(xPulseNum);	
				T.SlaveSpeed = T.MainSpeed * k * k1;	
				T.SlaveSpeed = 	MinS32(T.SlaveSpeed,200000);
				if(T.SlaveSpeed<100)
				{
					T.SlaveSpeed = 100;
				}
				
				T.SlaveSpeed = T.SlaveSpeed;
				
				HZ_AxSetPara(SlaveAxis,\
				T.SlaveSpeed ,20,T.SlaveSpeed ,20,T.SlaveSpeed ,\
				0,0,0,\
				0,0);				
				HZ_AxMoveRel(SlaveAxis,yPulseNum);		
				T.step = 0xB0;
			break;
				
		case 0xB0:
				T.step = 3;
			break;
			
		case 3:
			//判断主轴是否停止
			if(HZ_AxGetStatus(MainAxis) == AXSTA_READY)
			{	
				//主轴停止，从轴也停止
				HZ_AxStop(SlaveAxis);
				T.step = 0xC0;								
			}	
			else
			{					
					if(T.TooLongFlag == 0)
					{
						//判断凸轮点有没有变化
						if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
						{
							T.indexTerm = T.index;
							T.step = 2;
						}
						
						if(HZ_AxGetCurPos(MainAxis) > (T.MainStartPos + T.MainCirPulse))
						{
							//判断当前凸周期是否结束
							//主轴停止，从轴也停止
//							HZ_AxStop(SlaveAxis);
							T.mainAxSynSpeed = HZ_AxGetSpd(MainAxis);
							T.SlaveAxSynSpeed =  HZ_AxGetSpd(SlaveAxis);
							T.SynSpeedRate = (float)T.SlaveAxSynSpeed/(float)T.mainAxSynSpeed; 
							T.TooLongFlag = 0;
							T.LongEndFlag = 0;							
							T.excute = 0;
							T.step = 0;
							T.busy = 0;
						}
					}
					else
					{						
						if(HZ_AxGetCurPos(MainAxis) < (T.MainStartPos + T.MainPlus1))
						{
								//判断凸轮点有没有变化
							if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
							{
								T.indexTerm =T.index;
								T.step = 2;
							}									
						}	
						else
						{
							if(T.LongEndFlag == 1)
							{
								 if(HZ_AxGetCurPos(MainAxis)>= T.LongStopPos)
								 {
										//判断当前凸周期是否结束
										//主轴停止，从轴也停止
//										HZ_AxStop(SlaveAxis);
										T.mainAxSynSpeed = HZ_AxGetSpd(MainAxis);
										T.SlaveAxSynSpeed =  HZ_AxGetSpd(SlaveAxis);
										T.SynSpeedRate = (float)T.SlaveAxSynSpeed/(float)T.mainAxSynSpeed; 
										T.TooLongFlag = 0;
										T.LongEndFlag = 0;
										T.excute = 0;
										T.step = 0;
										T.busy = 0;
								 }
								else if(HZ_AxGetCurPos(MainAxis) >= (T.LongStopPos - T.MainPlus2))
								{
										//判断凸轮点有没有变化
									if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
									{
										T.indexTerm =T.index;
										T.step = 2;
									}	
								}
								else
								{
									HZ_AxStop(SlaveAxis);
									T.step = 4;
								}
							}
							else
							{
								HZ_AxStop(SlaveAxis);
								T.step = 4;
							}
						}						
					}
				}		
			
			if(T.excute == 2)
			{
				T.step = 5;
			}
			break;
			
		case 0xC0:
			if(HZ_AxGetStatus(MainAxis) == AXSTA_READY)
			{					
				if(T.TooLongFlag == 0)
				{
					if(HZ_AxGetCurPos(MainAxis) > (T.MainStartPos + T.MainCirPulse))
					{
						//判断当前凸周期是否结束
						//主轴停止，从轴也停止
//							HZ_AxStop(SlaveAxis);
						T.mainAxSynSpeed = HZ_AxGetSpd(MainAxis);
						T.SlaveAxSynSpeed =  HZ_AxGetSpd(SlaveAxis);
						T.SynSpeedRate = (float)T.SlaveAxSynSpeed/(float)T.mainAxSynSpeed; 
						T.TooLongFlag = 0;
						T.LongEndFlag = 0;							
						T.excute = 0;
						T.step = 0;
						T.busy = 0;
					}
				}
				else
				{						
					if(HZ_AxGetCurPos(MainAxis) < (T.MainStartPos + T.MainPlus1))
					{
							
					}	
					else
					{
						if(T.LongEndFlag == 1)
						{
							 if(HZ_AxGetCurPos(MainAxis)>= T.LongStopPos)
							 {
									//判断当前凸周期是否结束
									//主轴停止，从轴也停止
//										HZ_AxStop(SlaveAxis);
									T.mainAxSynSpeed = HZ_AxGetSpd(MainAxis);
									T.SlaveAxSynSpeed =  HZ_AxGetSpd(SlaveAxis);
									T.SynSpeedRate = (float)T.SlaveAxSynSpeed/(float)T.mainAxSynSpeed; 
									T.TooLongFlag = 0;
									T.LongEndFlag = 0;
									T.excute = 0;
									T.step = 0;
									T.busy = 0;
							 }
						}
					}						
				}					
			}
			else
			{
				if(T.isCanIn == 1)
				{
					T.step = 0xA0;
				}
			}
			break;

		case 4:
			if(CamTable.LongEndFlag == 1)
			{
				if(HZ_AxGetCurPos(MainAxis) >= (T.LongStopPos - T.MainPlus2))
				{
					T.step = 0xA0;
				}
			}

			if(T.excute == 2)
			{
				T.step = 5;
			}			
			break;

		case 5:
			if(HZ_AxGetStatus(MainAxis) == AXSTA_READY)
			{
				//主轴停止，从轴也停止
				HZ_AxStop(SlaveAxis);
//				HZ_AxStopDec(SlaveAxis);
				T.excute = 0;
				T.step = 0;
				T.busy = 0;								
			}
			break;
	}
	return 1;
}




/**
* @author 2019/5/17  NYZ
* @Description: 凸轮咬合
* @param -- 
* @param --
* @return --
*/
s8 CamIn(CamTableDef *Table)
{
//	if(T.step== 0 && T.excute == 0)
//	{
//		T.excute = 1;		
//	}
	T.excute = 1;	
	T.step = 0; 
	T.isCanIn = 1;
	return 1;
}

/**
* @author 2019/5/17  NYZ
* @Description: 凸轮脱离凸轮
* @param -- 
* @param --
* @return -- 0:凸轮结束 1：凸轮运行
*/
s8 CamOut(CamTableDef *Table)
{
//	if(T.excute == 1)
//	{
//		T.excute = 2;
//	}
	T.isCanIn = 0;
	return T.busy;
}

/**
* @author 2020/2/24  NYZ
* @Description: 凸轮结束
* @param -- 
* @param --
* @return -- 0:凸轮结束 1：凸轮运行
*/
s8 CamEnd(CamTableDef *Table)
{
	T.excute = 0;
	T.step = 0;
	T.busy = 0;	
	return 0;
}


EncodeParaDef Encode[4];
/**
* @author 2019/5/23  NYZ
* @Description: 设定编码器位置
* @param --  编码器编号
* @param --  编码器设定位置
* @return -- -1：错误 1：正确
*/
s8 EncodeSetPos(u8 encodeNum,s32 pos)
{
	if(encodeNum<4)
	{
		Encode[encodeNum].StartPos = EnCode_Get32(encodeNum);
		Encode[encodeNum].StartPos -= pos;		
		return 1;
	}
	else
	{
		return -1;
	}
}

/**
* @author 2019/5/23  NYZ
* @Description: 设定编码器位置
* @param --  编码器编号
* @return -- -1：错误 1：正确
*/
s32 EncodeGetPos(u8 encodeNum)
{
	if(encodeNum<4)
	{
		Encode[encodeNum].CurrPos =  EnCode_Get32(encodeNum) - Encode[encodeNum].StartPos;
		return Encode[encodeNum].CurrPos;
	}
	else
	{
		return 0;
	}	
}

typedef struct 
{
	s32 speed;
	s32 count;
	u32 k;
	s32 countt;
	s32 countTerm[10];
	u32 t;
	u32 tt;
}EncoderDef;
EncoderDef Encoder[4] = {0};

void EnCodGetSpeed()
{
	u8 i,j;
	for(i=0;i<4;i++)
	{
		Encoder[i].t = SysTim_GetUser() - Encoder[i].tt;
		if(Encoder[i].t>=1000)
		{
			Encoder[i].tt = SysTim_GetUser();
			Encoder[i].countTerm[Encoder[i].k] = EnCode_Get32(i) - Encoder[i].count;
			Encoder[i].k++;
			if(Encoder[i].k>=10)
			{
				Encoder[i].k = 0;
			}
			Encoder[i].count = EnCode_Get32(i);
			Encoder[i].countt = 0;			
			for(j=0;j<10;j++)
			{
				Encoder[i].countt += Encoder[i].countTerm[j];
			}
			Encoder[i].speed = abs(Encoder[i].countt);
		}
	}
}

s32 GetSpeed(u8 mode,u8 axisNum)
{
	if(mode == 0)
	{
		return HZ_AxGetSpd(axisNum);
	}
	else
	{
		return Encoder[axisNum].speed;
	}
}


//应用底层函数
typedef struct
{
	u8 excute;
	u8 step;
	s8 done;
	u8 busy;
	
	s32 tarPos;	
	u32 tarSpd;
	u32 acc;
	u32 jeck;
	u32 startSpd;
	u32 endSpd;
	
	s32 curPos;
	s32 currSpd;
	
	s32 PosTerm;
	
	u8 sta;
	u8 mode;
	
	u8 crrMode;
	
	u8 dir;  //方向
	
	u8 axisNum;
	
	char errMsg[10];  //错误信息
	u32 errCode;			//错误码	
}AxisDataDef;

#define AxisNum 5
AxisDataDef AxisData[AxisNum] = {0};


s8 MC_MoveVel(u8 axisNum,s32 startSpd,u32 acc,u32 jeck,s32 tarSpd)
{
		AxisData[axisNum].startSpd = abs(startSpd);
		if(tarSpd > 0 )
		{
			AxisData[axisNum].dir = 1;
		}
		else
		{
			AxisData[axisNum].dir = 0;			
		}	
		AxisData[axisNum].mode = SPDMODE;
		AxisData[axisNum].jeck = jeck;
		AxisData[axisNum].tarSpd = abs(tarSpd);
		AxisData[axisNum].excute  = 1;
		AxisData[axisNum].axisNum = axisNum;
		return 1;
}



s8 SpdMode(AxisDataDef *p,u8 axNum)
{

 if(p->excute == 1 && p->step == 0)
 {
	 p->step = 1;
	 p->busy = 1;
 }
 
 switch(p->step)
 {
	 case 1:
		  Puls_SetSpd(p->axisNum,p->tarSpd);
			Puls_Start(p->axisNum,0xFFFFFFFF);
			Dir_SetSta(p->axisNum,p->dir);
			p->step = 2;
		 break;
	 
	 case 2:
			p->done = 1;
			p->excute = 0;
			p->step = 0;
		 break;
 }
 
 if(Puls_GetSta(p->axisNum) == 1)
 {
	 if(Dir_GetSta(p->axisNum))
	 {
		p->curPos = Puls_GetRan(p->axisNum);
	 }
 }
 
 return 1;
}

void GetCurrPulse()
{
	u32 i;	
	for(i=0;i<AxisNum;i++)
	{	
	 if(Puls_GetSta(i) == 1)
	 {
		 if(Dir_GetSta(i))
		 {
			 AxisData[i].curPos = AxisData[i].curPos + (s32)Puls_GetRan(i) - AxisData[i].PosTerm;
			 AxisData[i].PosTerm = Puls_GetRan(i);			 
		 }
		 else
		 {
			 AxisData[i].curPos = AxisData[i].curPos - ((s32)Puls_GetRan(i) - AxisData[i].PosTerm);
			 AxisData[i].PosTerm = Puls_GetRan(i);				 
		 }
	 }		
	}
}


/********************* 电子齿轮比咬合*************************************/
#define region_2   
#ifdef region_2
//电子齿轮比咬合
s8 GearRun(GearInDataDef *G)
{
	if(G->execute == 1 && G->step == 0)
	{
		G->mStartPos = HZ_AxGetCurPos(G->mAx);
		G->sStartPos = HZ_AxGetCurPos(G->sAx);
		G->step = 1;
		G->busy = 1;
		G->done = 0;		
	}
  G->mCurrPlus = (HZ_AxGetCurPos(G->mAx) - G->mStartPos)%G->mCylPlus;
	G->sCurrPlus = (HZ_AxGetCurPos(G->sAx) - G->sStartPos)%G->sCylPlus;
	switch(G->step)
	{		
		case 1:
			G->rate = (float)G->sCylPlus/(float)G->mCylPlus;
			if(abs((int)(G->mCurrPlus*G->rate) - G->sCurrPlus)> GEARDIV || G->gearOut)
			{					
				G->step= 2;
			}			
			break;

		case 2:
			if(HZ_AxGetStatus(G->sAx) == AXSTA_READY)
			{	
				if(G->gearOut)
				{			
					G->sSpd = G->mTgSpd*G->rate*1.5f;	
				}
				else
				{
					G->mSpd = HZ_AxGetSpd(G->mAx);				
					G->sSpd = G->mSpd*G->rate*1.5f;	
				}
				HZ_AxSetPara(G->sAx,\
				G->sSpd ,20,G->sSpd ,20,G->sSpd,\
				0,0,0,\
				0,0);	
				
				HZ_AxMoveRel(G->sAx,GEARDIV);		
				G->step = 3;
			}			
			break;			
		
		case 3:
			if(HZ_AxGetStatus(G->sAx) == AXSTA_READY)
			{
				G->step = 1;
			}
			break;
				
		}
	
		return 1;
}

/**
* @author 2019/9/5  NYZ
* @Description: 电子齿轮咬合
* @param -- 
* @param --
* @return --
*/
s8 GearIn(u8 mAx,u8 sAx,u32 mCylPlus,u32 sCylPlus,u32 mTgSpd ,GearInDataDef *G)
{
	G->mCylPlus = mCylPlus;
	G->sCylPlus = sCylPlus;	
	G->mAx = mAx;
	G->sAx = sAx;
	G->execute = 1;
	G->gearOut = 0;
	G->mTgSpd = mTgSpd;
	return 1;
}



/**
* @author 2019/9/5  NYZ
* @Description: 电子齿轮脱离
* @param -- 
* @param --
* @return --
*/
s8 GearOut(GearInDataDef *G)
{
	G->gearOut = 1;
	return 1;
}


/**
* @author 2019/9/5  NYZ
* @Description: 电子齿轮结束
* @param -- 
* @param --
* @return --
*/
s8 GearEnd(GearInDataDef *G)
{
	G->execute = 0;
	G->step = 0;
	G->busy = 0;
	return 1;
}
#endif




/*********************  追剪功能 *************************************/
#define region_1   
#ifdef region_1

/**
* @author 2019/5/17  NYZ
* @Description: 计算追剪电子凸轮表
* @param -- 
* @param --
* @return --
*/

s8 FlyCutCamTableCaculate(FlyCutCamTableDef *Table)
{
	s32 i,j;
	float acc;
	float slaveSynVel;		//从轴同步区等分脉冲数
	//清空
	for(i=0;i<CAMPointNum;i++)
	{
		T.SlaveAxisPos[i] = 0;
		T.SlaveAxisRelPos[i] = 0;
	}
		
	slaveSynVel = T.MainAxisTPulse*T.SpeedMul;
	T.SlaveAxisTPulse = slaveSynVel;
	acc = slaveSynVel/(float)(T.Ttime/2);
	for(i=0;i<T.Ttime;i++)
	{
		if(i<(T.Ttime/2))
		{
			T.SlaveAxisRelPos[i] = i*acc;
			if(T.SlaveAxisRelPos[i]> slaveSynVel)
			{
				T.SlaveAxisRelPos[i] = slaveSynVel;
			}
		}
		else if(i == T.Ttime/2)
		{
			T.SlaveAxisRelPos[i] = slaveSynVel;
		}
		else
		{
			T.SlaveAxisRelPos[i] = slaveSynVel - (i-T.Ttime/2)*acc;
			if(T.SlaveAxisRelPos[i]< 0)
			{
				T.SlaveAxisRelPos[i] = 0;
			}				
		}
	}				
	//方便监控，不参与计算
//	T.SlaveAxisRelPos[0] = T.SlaveAxisPos[0];	
	for(i=0;i<T.Ttime;i++)
	{	
		for(j=0;j<=i;j++)
		{
			T.SlaveAxisPos[i] += T.SlaveAxisRelPos[j];
		}
	}				
	return 1;
}

s8 CamFlyCutRun(FlyCutCamTableDef *Table)
{
  float xPulseNum;
	float yPulseNum;
	static float k;	
	if(T.excute == 1 && T.step == 0)
	{
		T.step = 1;
		T.busy = 1;
		T.flyEndflag = 0;

	}		
	T.SlaveCurrPos = HZ_AxGetCurPos(T.sAx) - T.SlaveStartPos;
	
	if(T.mType == 0)
	{
		T.MainCurrPos = HZ_AxGetCurPos(T.mAx) - T.MainStartPos;
	}
	else
	{
		T.MainCurrPos = EncodeGetPos(T.mAx) - T.MainStartPos;
	}
	T.index = abs(T.MainCurrPos)/T.MainAxisTPulse;	
	switch(T.step)
	{				
		case 1:
				//计算追剪凸轮表
				FlyCutCamTableCaculate(Table);			
				T.step = 2;
			break;
				
		case 2:	//追剪区											
				//判断凸轮旋转方向
				xPulseNum = T.MainAxisTPulse;				
				yPulseNum = T.SlaveAxisPos[T.index+1] - T.SlaveCurrPos;			
				if(yPulseNum < 0)
				{
					yPulseNum  = 0;
				}					
				T.MainSpeed = GetSpeed(T.mType,T.mAx);				
				k = __fabs(yPulseNum)/__fabs(xPulseNum);	
				T.SlaveSpeed = T.MainSpeed * k * 1.3f;	
				T.SlaveSpeed = 	RangeS32(T.SlaveSpeed,100,200000);	
					
				HZ_AxSetPara(T.sAx,\
				T.SlaveSpeed ,20,T.SlaveSpeed ,20,T.SlaveSpeed ,\
				0,0,0,\
				0,0);				
				HZ_AxMoveRel(T.sAx,yPulseNum);				
				T.step = 3;
			break;
							
		case 3:	
				//判断凸轮点有没有变化
				if(HZ_AxGetStatus(T.sAx) == AXSTA_READY)
				{
					T.index++;					
					if(T.index>=T.Ttime/2)
					{
						T.flyPos[0] = T.SlaveAxisPos[T.index];
						T.step = 4;
						T.indexTerm = 0;	
					}
					else
					{
						T.step = 2;
					}
				}																		
			break;
			
			
		case 4://同步区					
				//判断凸轮旋转方向
			{
				xPulseNum = T.MainAxisTPulse;				
				yPulseNum = T.flyPos[0] + T.SlaveAxisTPulse*(T.index - T.Ttime/2) - T.SlaveCurrPos;		
				if(yPulseNum < 0)
				{
					yPulseNum  = 0;
				}				
				T.MainSpeed = GetSpeed(T.mType,T.mAx);				
				k = __fabs(yPulseNum)/__fabs(xPulseNum);	
				T.SlaveSpeed = T.MainSpeed * k * 1.3f;	
				T.SlaveSpeed = 	RangeS32(T.SlaveSpeed,100,200000);			
				HZ_AxSetPara(T.sAx,\
				T.SlaveSpeed ,20,T.SlaveSpeed ,20,T.SlaveSpeed ,\
				0,0,0,\
				0,0);				
				HZ_AxMoveRel(T.sAx,yPulseNum);		
				T.step = 5;
			}
			break;
				
		case 5:		
				//判断凸轮点有没有变化
				if(HZ_AxGetStatus(T.sAx) == AXSTA_READY)
				{				
					if(T.flyEndflag == 1)
					{
						T.flyEndflag = 0;
						T.step = 6;
						T.flyPos[1] = T.flyPos[0] + T.SlaveAxisTPulse*(T.index - T.Ttime/2);
						T.indexTerm = T.index;	
					}
					else
					{
						T.step = 4;
					}
				}																	
			break;	

		case 6:	//减速区				
				xPulseNum = T.MainAxisTPulse;				
				yPulseNum = T.SlaveAxisPos[T.index + 1 - T.indexTerm + T.Ttime/2] + T.flyPos[1] - T.flyPos[0] - T.SlaveCurrPos;		
				if(yPulseNum < 0)
				{
					yPulseNum  = 0;
				}					
				T.MainSpeed = GetSpeed(T.mType,T.mAx);				
				k = __fabs(yPulseNum)/__fabs(xPulseNum);	
				T.SlaveSpeed = T.MainSpeed * k * 1.3f;	
				T.SlaveSpeed = 	RangeS32(T.SlaveSpeed,100,200000);	
					
				HZ_AxSetPara(T.sAx,\
				T.SlaveSpeed ,20,T.SlaveSpeed ,20,T.SlaveSpeed ,\
				0,0,0,\
				0,0);				
				HZ_AxMoveRel(T.sAx,yPulseNum);			
				T.step = 7;
			break;
							
		case 7:
				//判断凸轮点有没有变化
				if(HZ_AxGetStatus(T.sAx) == AXSTA_READY)
				{					
					if(T.index  - T.indexTerm > T.Ttime/2)
					{
						T.step = 8;
						T.indexTerm = 0;	
					}
					else
					{
						T.step = 6;
					}
				}																	
			break;
				
		case 8:
			MC_MovePP(T.sAx,ABSMODE,1000,100,50000,T.SlaveStartPos);	
			T.step = 9;
			break;
		
		case 9:
			if(HZ_AxGetStatus(T.sAx) == AXSTA_READY)
			{
				T.excute = 0;
				T.step = 0;
				T.busy = 0;	
			}					
			break;
	}
	return 1;
}

s8 FlyCutCamIn(float ratio,u8 mType,u8 mAx,u8 sAx,FlyCutCamTableDef *Table)
{
	T.SpeedMul = ratio;
	T.mType = mType;
	T.mAx = mAx;
	T.sAx = sAx;
	if(T.mType == 0)
	{
		T.MainStartPos = HZ_AxGetCurPos(mAx);
	}
	else
	{
		T.MainStartPos = EncodeGetPos(mAx);
	}
	T.SlaveStartPos = HZ_AxGetCurPos(sAx);
	T.Ttime = 200;
	T.MainAxisTPulse = 50;
	T.excute = 1;
	return 1;
}

s8 FlyCutCamStop(FlyCutCamTableDef *Table)
{
	T.flyEndflag = 1;
	return 1;
}


s8 FlyCutCamGetSta(FlyCutCamTableDef *Table)
{
	if(T.excute == 0)
	{
		return 0;
	}
	else
	{	
		return 1;
	}
}

#endif

