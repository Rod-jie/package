/*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 12轴底层
 * Description:用户逻辑
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018, <<hzzh>>
 */

#include "Logic.h"
#include "FSM.h"
#include "USERTOOL.H"
#include "EXIQ.h"
#include "math.h"
#include "stdlib.h"

/*全局变量做统一处理*/
Task LogicTask = { 0 }; //统一的任务类
SysData Data = { 0 };   //统一的数据类

int SysRunDelay_3S ;
int sysRun_et;



CamTableDef CamTable = {0};  //电子凸轮表
extern GearInDataDef GearData;    //电子齿轮咬合


PackageDataDef PackageDataArray[BuffArrayNum] = {0};

/**
* @author： 2019/05/17  nyz
* @Description:  切刀凸轮曲线计算
* @param --
* @param --
* @return --
*/
void CutCamCaculate(float packageLength)
{
	int i;
	float mainMpp,slaveMpp;
	float synL;
	CamTable.MainCirPulse = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate,packageLength);
	if(GUS.SysPara.CutterNum>0)
	{
		CamTable.SlaveCirPulse = GUS.AxisPara.CutterAxisGearRate.PPR/GUS.SysPara.CutterNum;
	}
	else
	{
		CamTable.SlaveCirPulse = GUS.AxisPara.CutterAxisGearRate.PPR;
	}
	synL = GUS.SysPara.CutterCloseAngle*GUS.AxisPara.CutterAxisGearRate.MPR/360.0f;
	CamTable.SynPulse = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate,synL);
	
	GUS.SysPara.CamPNum = CAMPointNum;
	CamTable.Ttime = GUS.SysPara.CamPNum - 1;
	if(CamTable.Ttime>1900)
	{
		CamTable.Ttime = 1900;
	}
	mainMpp = (float)GUS.AxisPara.FeedFilmAxisGearRate.PPR/ GUS.AxisPara.FeedFilmAxisGearRate.MPR;
	slaveMpp = (float)GUS.AxisPara.CutterAxisGearRate.PPR/ GUS.AxisPara.CutterAxisGearRate.MPR;
	CamTable.SpeedMul =  slaveMpp/mainMpp*GUS.SysPara.CutterSpeedCorrect;	
	
	//判断是伺服电机还是
	if(GUS.SysPara.FeedFilmAxisSel == 0)
	{
		CamTable.MainAxisMode = REALAXIS;	
	}
	else
	{
		CamTable.MainAxisMode =  ENCODERAXIS;
		CamTable.MainEncodeNum = FeedFilmEncode;	
	}
	CamTable.MainStartPos  =  HZ_AxGetCurPos(Axis_FeedFilm);// - UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate,Data.ColorMarkDif);
	CamTable.SlaveStartPos =  0;	
	
	CamTableCaculate(&CamTable);	
	
	
	/***************主要用于在触摸屏上显示凸轮曲线*******************/
//	k = 300.0/(float)CamTable.SlaveAxisPos[CamTable.Ttime -1];
	for(i=0;i<=CamTable.Ttime/2;i++)
	{
		if(2*i<500-2)
		{
			GUS.CamCurve[i] = CamTable.SlaveAxisPos[2*i+2] - CamTable.SlaveAxisPos[2*i];
			GUS.YTable[i] = CamTable.SlaveAxisPos[2*i] + 1;			
//			GUS.YTable[i] = CamTable.SlaveAxisPos[i+1] - CamTable.SlaveAxisPos[i];
		}		
	}
	/*******************end**********************/
}


/**
* @author： 2019/05/22  nyz
* @Description:  切刀堵转
* @param --
* @param --
* @return --
*/
s8 CutterStuckCheck()
{
	s32 cutterAngle;
	cutterAngle = (int)GUR.SlaveAxPosUU%(360/GUS.SysPara.CutterNum);
//	//切到堵转判断，
	
	if(GUS.SysPara.CutterStuckMode == 1)
	{
		//通过力矩报警输入点判断切刀是否堵转	
		if(cutterAngle >= (360 + GUS.SysPara.TorqueCheckAngleMin) ||  (cutterAngle > 0 && cutterAngle < GUS.SysPara.TorqueCheckAngleMax))
		{
			if(InputGet(I_TorqueAlm) == ON )
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
		  return 0;
		}
	}
	else
	{
		return 0;
	}
}

/**
* @author： 2019/05/22  nyz
* @Description:  包装机复位
* @param --
* @param --
* @return --
*/
void PackageReset(LogicParaDef *LG)
{
	static float feedFilmPosTerm;
	if(LG->execute == 1 && LG->step == 0)
	{
		PARAINIT(LogicTask);
		GUR.HaveToReset = 1;
		LG->execute = 1;
		LG->step = 1;
		LG->count = 0;
		LG->done = 0;	
	}	
	
	//伺服报警，则系统不允许复位
    for(int i=0; i<PULS_NUM; i++)
    {				
				if( AX_ERR_AX_ALM == HZ_AxGetAxisErr(i))
				{
					LG->execute = 0;
					LG->step = 0;
					LG->done = 0;
					if(LG->execute == 1)
					{
						HZ_AxStop(Axis_FeedFilm);
						HZ_AxStop(Axis_Cutter);
						HZ_AxStop(Axis_FeedProduct);
					}
				}
    }
		
		//回原点过程中拍停止，回原点流程退出
		if(LG->execute == 1)
		{
			if(InputGet(I_EmergencyBt) == ON)
			{
				LG->execute = 0;
				LG->step = 0;
				LG->done = 0;	
				GUW.button.RunCommand = STOP;
				GUR.RunStatus = INIT;	
				GUR.HaveToReset = 1;
				for(int i=0; i<PULS_NUM; i++)
				{				
					HZ_AxStop(i);
				}		
			}
		}
	
	switch(LG->step)
	{
		case 1:
			FilmFeedMove(1,5000,50);		
			LogicTask.CutterReset.execute = 1;
			LogicTask.FeedReset.execute = 1;
			LG->step = 2;
			break;	
		
		case 2:
			if(InputGet(I_CutOrgin) == ON)
			{
				FilmFeedStop1();
			}
			
			if(LogicTask.CutterReset.execute == 0)
			{		
				FilmFeedStop1();
				LG->step = 3;
				TimerRst1(LG);
			}
			
			if(InputGet(I_TorqueAlm) == ON )
			{
				HZ_AxStop(Axis_Cutter);
				PARAINIT(LogicTask.CutterReset);
				FilmFeedStop1();
				LG->step = 0xA0;
			}
			break;					

		case 0xA0:
			if(HZ_AxGetStatus(Axis_Cutter) == 0)
			{
				MC_MoveUU(Axis_Cutter,RELMODE,1000,100, 2000*GUS.AxisPara.FeedFilmAxisGearRate.PPR/GUS.AxisPara.FeedFilmAxisGearRate.MPR/60,-GUS.CutterStopAngle); 
				if(GUS.SysPara.CutterStuckMode != 1)
				{
					LG->execute = 0;
					LG->step = 0;
					LG->done = 1;	
					GUW.button.RunCommand = STOP;
					GUR.RunStatus = INIT;	
					AlarmSetBit(2, 0, 1); //切刀堵转
				}
				else
				{
					LG->step = 0xA1;
				}
			}
			break;
			
		case 0xA1:
			if(HZ_AxGetStatus(Axis_Cutter) == 0)
			{
				LG->count++;
				if(LG->count>=2)
				{
					LG->execute = 0;
					LG->step = 0;
					LG->done = 1;	
					GUW.button.RunCommand = STOP;
					GUR.RunStatus = INIT;	
					AlarmSetBit(2, 0, 1); //切刀堵转
				}
				else
				{
					if(InputGet(I_TorqueAlm) == ON)
					{
										LG->execute = 0;
				LG->step = 0;
				LG->done = 1;	
						GUW.button.RunCommand = STOP;
						GUR.RunStatus = INIT;	
						AlarmSetBit(2, 0, 1); //切刀堵转
					}
					else
					{
						FilmFeedMove(1,5000,50); 
						feedFilmPosTerm =  GSR.AxisUnitPos[Axis_FeedFilm];
						LG->step = 0xA2;	
					}	
				}
			}
			break;
			
		case 0xA2:		
			if(GSR.AxisUnitPos[Axis_FeedFilm] - feedFilmPosTerm>=GUS.PackgeLength*GUS.SysPara.PassPackageNum)
			{
				LG->step = 1;
			}
			break;
			
			
		case 3:
			if(TimerCnt1(LG)>200 && GetFilmFeedSta() == 0)
			{
				FilmFeedMove(1,5000,50);	
				MC_MovePP(Axis_Cutter,RELMODE,1000,100, 3000*GUS.AxisPara.FeedFilmAxisGearRate.PPR/GUS.AxisPara.FeedFilmAxisGearRate.MPR/60,GUS.CutterStopAngle*GUS.AxisPara.CutterAxisGearRate.PPR/360);
				LG->step = 4;
			}
			break;
			
		case 4:
			if(InputGet(I_TorqueAlm) == ON )
			{
				HZ_AxStop(Axis_Cutter);
				PARAINIT(LogicTask.CutterReset);
				FilmFeedStop1();
				LG->step = 0xA0;
			}			
			else if(HZ_AxGetStatus(Axis_Cutter) == 0 )
			{
				FilmFeedStop1();
				LG->step = 5;
			}
			break;
		
		case 5:
			if(GetFilmFeedSta() == 0)
			{
				LG->step = 6;
			}
			break;
			
			
		case 6:
			if(LogicTask.FeedReset.execute == 0 && LogicTask.FeedFilmReset.execute == 0 && HZ_AxGetStatus(Axis_Cutter) == 0 && HZ_AxGetStatus(Axis_FeedFilm) == 0 && HZ_AxGetStatus(Axis_FeedProduct) == 0)
			{			
				LG->execute = 0;
				LG->step = 0;
				LG->done = 1;	
				HZ_AxSetCurPos(Axis_FeedFilm,0);
				EncodeSetPos(FeedFilmEncode,0);	
				HZ_AxSetCurPos(Axis_FeedProduct,0);
				EncodeSetPos(FeedProductEncode,0);					
				Data.ProductBuffCont = 0;	
				Data.FirstPackageFlag = 0;
				PARAINIT(PackageDataArray);
				PARAINIT(LogicTask);
				PARAINIT(CamTable);
				PARAINIT(Data);
				GUR.HaveToReset = 0;
				GUR.HaveToRepower = 0;
				GSW.ClearAlarm = 1;			
				GUR.RunStatus = STOP;	
				GUW.button.RunCommand = STOP;
				GUS.IsReset = 1;
			}
			break;
	}	
}

/**
* @author： 2019/10/08 nyz
* @Description:  逻辑初始化函数
* @param --
* @param --
* @return --
*/
void LogicInit()
{
	sysRun_et = SysTim_GetUser();
	SysRunDelay_3S = 0;
	
	OutputSet(Q_TemperatureStart	,ON);//打开加热
	//2020.2.21 nyz 上电后不需回零，根据上次断电的当前轴位置设置轴当前位置，
  ton_et[10] = 0;
	//判断上电是否允许不用复位
	GUR.HaveToReset = 1;
	if(GUS.IsReset == 1)
	{
		HZ_AxSetCurPos(Axis_FeedProduct,GUS.FeedAxPosTerm%UserUnitToPulse(&GUS.AxisPara.FeedAxisGearRate,GUS.AxisPara.FeedAxisGearRate.MPR));
		HZ_AxSetCurPos(Axis_FeedFilm,GUS.FeedFilmAxPosTerm);
		HZ_AxSetCurPos(Axis_Cutter,GUS.CutterAxPosTerm);	
		
		EncodeSetPos(FeedFilmEncode,GUS.CutterEncoderPosTerm);
		CutCamCaculate(GUS.RealPackgeLength);	
		CamTable.MainStartPos = 0;		
		if(GUS.CamExcuteStatusTerm == 1)
		{
			CamTable.excute = 1;
		}
		GUR.HaveToReset = 0;
		GUR.HaveToRepower = 0;
		GSW.ClearAlarm = 1;			
		GUR.RunStatus = STOP;	
		GUW.button.RunCommand = STOP;	
		
		GearData.mStartPos = HZ_AxGetCurPos(Axis_FeedFilm) - GUS.GearMCurrPosPP;
		GearData.sStartPos = HZ_AxGetCurPos(Axis_FeedProduct) - GUS.GearSCurrPosPP;
		Data.feedFimeAxisRunFlag = GUS.GearStatus;
 }
	//***************************//
	
}


void Logic()
{
	switch(GUS.SysPara.DeviceMode)
	{
		case 0://常规模式	
				ConMode_Logic();
			break;
		
		case 1://不定长模式					
				LengthDifMode_Logic();		
			break;
		
		case 3:
				UnderFeedFilmMode_Logic();
			break;
		
		default:
				ConMode_Logic();
			break;
	}
	
	if( SysTim_GetUser() - sysRun_et> 50000)
	{
		SysRunDelay_3S = 1;
	}
	
//FlyCut_Logic();
	GUR.CurrPackageSpd = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate,HZ_AxGetSpd(Axis_FeedFilm))*60;	
}


