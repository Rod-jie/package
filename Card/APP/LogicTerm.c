/*
 * File: LogicTerm.c
 * File Created: 2019.5.7
 * version: v00.00.01 12轴底层
 * Description:逻辑辅组，主要用于辅组逻辑的并且需要一直运行的函数都放在这里
 * Modified By:NYZ
 * -----
 * Copyright 2019 - 2019, <<hzzh>>
 */


#include "Logic.h"
#include "FSM.h"
#include "USERTOOL.H"
#include "EXIQ.h"
#include "math.h"
#include "stdlib.h"


extern CamTableDef CamTable;  //电子凸轮表
extern GearInDataDef GearData;    //电子齿轮咬合

/**
* @author： 2019/1/9 农业照
* @Description: 触摸屏按钮操作
* @param –
* @param --
* @return --
*/

void HMI_Button()
{
//	u8 i;
    //停止按钮，回复状态
    if(GUR.RunStatus != RUN)
    {
        ResetWhichBit(&GUW.button.BitControlWord, StopBit);
    }

    if(GUR.RunStatus == RUN)
    {
        GUW.Runing = 1;
    }
    else
    {
        GUW.Runing = 0;
    }

//	//伺服报警清除
//	if(GSW.ClearAlarm == 1)
//	{
//		for(i=0;i<5;i++)
//		{
//			EN_SetSta(i,ON);//
//			HZ_AxReset(i);
//		}
//	}
//
//	//伺服报警清除
//	if(R_Trig(5,ton(5,EN_GetSta(0) == ON,400)))
//	{
//		for(i=0;i<5;i++)
//		{
//			EN_SetSta(i,OFF);//
//		}
//	}


    if(GUW.TermCtl == 0)
    {
        OutputSet(Q_TemperatureStart, ON);
    }
    else
    {
        OutputSet(Q_TemperatureStart, OFF);
    }

    GUS.SysPara.DeviceMode = 0;
    //触摸屏，控制画面切换
    if(GUW.HMI_pageChange == 1)
    {
        GUW.HMI_pageChange = 0;
        switch(GUS.SysPara.DeviceMode)
        {
        case 0:
//				GUW.HMI_page = 17;
            GUW.HMI_page = 83;
            break;

        case 1:
            GUW.HMI_page = 18;
            break;

        case 3:
            GUW.HMI_page = 20;
            break;

        default:
            GUW.HMI_page = 17;
            break;
        }
    }

}


/**
* @author： 2019/10/22 nyz
* @Description:  色标感应反应距离计算
* @param --
* @param --
* @return --
*/


void MarkReactionCaculate()
{
    GUS.TestPara.MarkReactionTime = GUS.TestPara.MarkDis / abs(GUS.TestPara.Packagespeed1 * GUS.PackgeLength / 60 - GUS.TestPara.Packagespeed2 * GUS.PackgeLength / 60);
    GUS.SysPara.MarkReactionTime = GUS.TestPara.MarkReactionTime * 1000;
}


/**
* @author： 2019/5/22 nyz
* @Description:  触摸屏手动按钮操作
* @param --
* @param --
* @return --
*/
void Teach()
{
    if(GUR.RunStatus == RUN)
    {
        GUW.TechWord = 0;
        return;
    }

    switch(GUW.TechWord)
    {
    case 1://产量清0
        GUS.Product.CurrNum = 0;
        break;

    case 2://b包装速度--
        if(GUS.PackageSpeed > 0)
            GUS.PackageSpeed--;
        break;

    case 3://包装速度++
        GUS.PackageSpeed++;
        break;

//		case 4://色标位置--
//			if(ColorMarkPos>1)
//			{
//				GUS.colorMarkPos--;
//			}
//			break;
//
//		case 5://色标位置++
//			if(ColorMarkPos< (u32)GUS.PackgeLength - 1)
//			{
//				GUS.colorMarkPos++;
//			}
//			break;
//
//		case 6://物料位置--
//			if(GUS.ProductPos>0)
//			{
//				GUS.ProductPos--;
//			}
//			break;
//
//		case 7://物料位置++
//			if(GUS.ProductPos< (u32)GUS.PackgeLength)
//			{
//				GUS.ProductPos++;
//			}
//			break;

    case 8://袋长测试
        if(LogicTask.PackageLengthTest.execute == 0)
        {
            LogicTask.PackageLengthTest.execute = 1;
        }
        else
        {
            LogicTask.PackageLengthTest.execute = 0;
        }
        break;

    case 9://包装机复位
        LogicTask.Reset.execute = 1;
        GUW.button.RunCommand = D_RESET;
        break;

    case 10://报警清除
        GSW.ClearAlarm = 1;
        OutputSet(Q_ServoAlmReset, ON);
        break;

    case 11://测试切刀每圈脉冲量
        if(LogicTask.CutterCirPulseTest.execute == 0)
        {
            LogicTask.CutterCirPulseTest.execute = 1;
        }
        else
        {
            LogicTask.CutterCirPulseTest.execute = 0;
        }
        break;

    case 12://测试走纸每圈脉冲量
        if(LogicTask.PackageLengthTest.execute == 0)
        {
            LogicTask.PackageLengthTest.execute = 2;
        }
        else
        {
            LogicTask.PackageLengthTest.execute = 0;
        }
        break;

    case 13://测试送料每圈脉冲量
        if(LogicTask.FeedCirPulseTest.execute == 0)
        {
            LogicTask.FeedCirPulseTest.execute = 1;
        }
        else
        {
            LogicTask.FeedCirPulseTest.execute = 0;
        }
        break;

    case 14:
        CamTable.TooLongFlag = 0;
        CutCamCaculate(GUS.PackgeLength);
        GUW.CamPointNum = CamTable.Ttime;
        GUW.CamCurveDraw = 1;
        break;

    case 15:
        GUW.CamCurveDraw = 2;
        GUW.CamCurveDraw1 = 2;
        break;

    case 16:
        CutCamCaculate(GUS.PackgeLength);
        GUW.CamPointNum1 = CamTable.Ttime;
        GUW.CamCurveDraw1 = 1;
        break;

    case 20://切刀走到停机位置
        LogicTask.CutterStopAngleData.execute = 1;
        break;

    case 21:
        if(GUR.RunStatus == STOP)
        {
            GUW.button.RunCommand = RUN;
        }
        break;

//		case 22:
//			if(GUR.RunStatus == RUN)
//			{
//	//			GUW.button.RunCommand = STOP;
//				Data.CylStopFlag = 1;
//			}
//			break;

    case 22:
        MarkReactionCaculate();
        break;
    }

    GUW.TechWord = 0;
}

/**
* @author： 2019/2/18 庄锐涵
* @Description: 报警提示
* @param –
* @param --
* @return --
*/

void Alarm()
{
    //无物料
    if(ton(6, GUS.NoProductDelay != 0 && GUS.SysPara.NoMaterialStop == 0 && GUS.SysPara.DeviceMode != 0 && GUR.RunStatus == RUN && EmptySensor() == 0, GUS.NoProductDelay))
    {
        AlarmSetBit(2, 0, 7); //无物料暂停
    }
}



/**
* @author： 2019/2/18 庄锐涵
* @Description: IO检测
* @param –
* @param --
* @return --
*/
void IOCheck()
{

}


/**
* @author： 2019/05/17  nyz
* @Description:  走纸运行  MM/分钟
* @param --
* @param --
* @return --
*/
void FilmFeedMove(s32 dir, float packageSpd, u32 accTime)
{
    static u32 speedTerm;
    u32 speed;
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        speed = packageSpd / GUS.AxisPara.FeedFilmAxisGearRate.MPR * GUS.AxisPara.FeedFilmAxisGearRate.PPR / 60;
        MC_MovePP(Axis_FeedFilm, SPDMODE, 1000, accTime, speed, dir);
        if(speed != speedTerm)
        {
            speedTerm = speed;
            HZ_AxReloadSpd(Axis_FeedFilm, speed);
        }
    }
    else
    {
        speed = GUS.TestPara.FeedSpeedStd * packageSpd * GUS.PackgeLength / GUS.AxisPara.FeedFilmAxisGearRate.MPR * GUS.AxisPara.FeedFilmAxisGearRate.PPR / 60;
        Dac_SetVal(0, MinS32(4096, speed));
        if(dir > 0)
        {
            OutputSet(Q_FeedFilmMotorPos, ON);
            OutputSet(Q_FeedFilmMotorNeg, OFF);
        }
        else
        {
            OutputSet(Q_FeedFilmMotorPos, OFF);
            OutputSet(Q_FeedFilmMotorNeg, ON);
        }
    }
}


/**
* @author： 2019/5/23 NYZ
* @Description: 走纸停止
* @param –
* @param --
* @return --
*/
void FilmFeedStop(u32 accTime)
{

    if(GUS.SysPara.FeedFilmAxisSel == 0 && HZ_AxGetStatus(Axis_FeedFilm) == AXSTA_CONTINUOUS)
    {
        HZ_AxSetPara(Axis_FeedFilm, 1000, accTime, \
                     200000, \
                     accTime, 1000, \
                     20000, 10000, \
                     0, 0, 0);

        if(HZ_AxGetSpd(Axis_FeedFilm) > 1500)
        {
            HZ_AxStopDec(Axis_FeedFilm);	//速度过低，会导致底层源码死循环跳不出来。
        }
        else
        {
            HZ_AxStop(Axis_FeedFilm);
        }
    }
    else
    {
        OutputSet(Q_FeedFilmMotorPos, OFF);
        OutputSet(Q_FeedFilmMotorNeg, OFF);
    }
}





/**
* @author： 2019/5/23 NYZ
* @Description: 走纸停止
* @param –
* @param --
* @return --
*/
void FilmFeedStop1()
{
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        HZ_AxStop(Axis_FeedFilm);
    }
    else
    {
        OutputSet(Q_FeedFilmMotorPos, OFF);
        OutputSet(Q_FeedFilmMotorNeg, OFF);
    }
}

/**
* @author： 2019/5/23 NYZ
* @Description: 获取走纸轴状态
* @param –
* @param --
* @return -- 0：停止  1：运行中
*/
s8 GetFilmFeedSta()
{
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        if(AXSTA_CONTINUOUS == HZ_AxGetStatus(Axis_FeedFilm) || AXSTA_STOPDEC == HZ_AxGetStatus(Axis_FeedFilm))
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
        if(OutputGet(Q_FeedFilmMotorPos) == OFF && OutputGet(Q_FeedFilmMotorNeg) == OFF)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}


/**
* @author： 2019/5/23 NYZ
* @Description: 获取当前走纸轴位置,脉冲
* @param –
* @param --
* @return --
*/
s32 GetFilmFeedPos()
{
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        return HZ_AxGetCurPos(Axis_FeedFilm) ;
    }
    else
    {
        return EncodeGetPos(FeedFilmEncode) ;
    }
}

/**
* @author： 2019/7/19 NYZ
* @Description: 获取当前走纸轴位置,MM
* @param –
* @param --
* @return --
*/
float GetFilmFeedPosUU()
{
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        return  PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, HZ_AxGetCurPos(Axis_FeedFilm));
    }
    else
    {
        return  PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, EncodeGetPos(FeedFilmEncode));
    }
}



/**
* @author： 2019/05/23  nyz
* @Description:  送料运行
* @param --
* @param --
* @return --
*/
void FeedMove(s32 dir, float packageSpd)
{
    u32 speed;
    static u32 speedTerm;
    //送料禁用
//	if(GUS.SysPara.FeedAxisEn != 1)
//	{
//		return;
//	}

//	float speedRate;
//	speedRate = GUS.AxisPara.FeedAxisGearRate.PPR * GUS.AxisPara.FeedFilmAxisGearRate.MPR/GUS.AxisPara.FeedFilmAxisGearRate.PPR * GUS.AxisPara.FeedAxisGearRate.MPR;
    if(GUS.SysPara.FeedMotorSel == 0)
    {
        speed = packageSpd / GUS.AxisPara.FeedAxisGearRate.MPR * GUS.AxisPara.FeedAxisGearRate.PPR / 60;

        MC_MovePP(Axis_FeedProduct, SPDMODE, 1000, 200, speed, dir);
        if(speed != speedTerm)
        {
            speedTerm = speed;
            HZ_AxReloadSpd(Axis_FeedProduct, speed);
        }
    }
    else
    {
        speed = GUS.TestPara.FeedSpeedStd * packageSpd / GUS.AxisPara.FeedAxisGearRate.MPR * GUS.AxisPara.FeedAxisGearRate.PPR / 60;
        Dac_SetVal(1, MinS32(4096, speed));
        if(dir > 0)
        {
            OutputSet(Q_FeedMotorEn, ON);
        }
        else
        {
            OutputSet(Q_FeedMotorEn, ON);
        }
    }
}


/**
* @author： 2019/5/23 NYZ
* @Description: 送料停止
* @param –
* @param --
* @return --
*/
void FeedStop()
{
    if(GUS.SysPara.FeedMotorSel == 0)
    {
        if(HZ_AxGetSpd(Axis_FeedProduct) > 1500)
        {
            HZ_AxStopDec(Axis_FeedProduct);
        }
        else
        {
            HZ_AxStop(Axis_FeedProduct);
        }
    }
    else
    {
        OutputSet(Q_FeedMotorEn, OFF);
    }
}

/**
* @author： 2019/5/23 NYZ
* @Description: 获取当前走纸轴位置,脉冲
* @param –
* @param --
* @return --
*/
s32 GetFeedPos()
{
    if(GUS.SysPara.FeedMotorSel == 0)
    {
        return HZ_AxGetCurPos(Axis_FeedProduct) ;
    }
    else
    {
        return EncodeGetPos(FeedProductEncode) ;
    }
}


/**
* @author： 2019/5/23 NYZ
* @Description: 获取当前走纸轴位置,MM
* @param –
* @param --
* @return --
*/
s32 GetFeedPosUU()
{
    if(GUS.SysPara.FeedMotorSel == 0)
    {
        return  PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, HZ_AxGetCurPos(Axis_FeedProduct));
    }
    else
    {
        return PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, EncodeGetPos(FeedProductEncode)) ;
    }
}

/**
* @author： 2019/5/23 NYZ
* @Description: 获取送料轴状态
* @param –
* @param --
* @return -- 0：停止  1：运行中
*/
s8 GetFeedSta()
{
    if(GUS.SysPara.FeedMotorSel == 0)
    {
        if(HZ_AxGetStatus(Axis_FeedProduct) == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        if(OutputGet(Q_FeedMotorEn) == OFF)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
}

/**
* @author： 2019/5/23 NYZ
* @Description: 测试袋长
* @param –
* @param --
* @return --
*/
void PackageLengthTest(LogicParaDef *LG)
{
    static s32 length;
    if(LG->execute != 0 && LG->step == 0)
    {
        LG->step = 1;
        LG->count = 0;
        Data.PackageSpd = GUS.JogSpeed * GUS.PackgeLength;
    }

    if(LG->execute == 0 && LG->step != 0)
    {
        LG->step = 4;
    }

    switch(LG->step)
    {
    case 1:
        FilmFeedMove(1, Data.PackageSpd, 50);
        length = 0;
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        if(InputGet(I_ColorMark) == ON)
        {
            //测试袋长
            if(LG->execute == 1)
            {
                GUS.TestPara.PackageLTest = (u32)PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, (GetFilmFeedPos() - length));
            }

            //测试每圈脉冲数
            if(LG->execute == 2)
            {
                GUS.TestPara.FeedPackagingFilmPlusNum = GetFilmFeedPos() - length;
            }
            length = GetFilmFeedPos();
            LG->step = 3;
        }
        break;

    case 3:
        if(InputGet(I_ColorMark) == OFF)
        {
            TimerRst1(LG);
            LG->count++;
            if(LG->count >= 5)
            {
                LG->step = 4;
            }
            else
            {
                LG->step = 2;
            }
        }
        break;

    case 4:
        FilmFeedStop(50);
        LG->step = 0;
        LG->execute = 0;
        break;
    }
}


/**
* @author： 2019/5/23 NYZ
* @Description: 切刀脉冲测试
* @param –
* @param --
* @return --
*/
void CutterPulseTest(LogicParaDef *LG)
{
    static s32 pluse1, pluse2;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->count = 0;
        Data.PackageSpd = GUS.JogSpeed * GUS.PackgeLength;
    }

    if(LG->execute == 0 && LG->step != 0)
    {
        LG->step = 4;
    }

    switch(LG->step)
    {
    case 1:
        MC_MovePP(Axis_Cutter, SPDMODE, 200, 100, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, 100);
        pluse1 = 0;
        pluse2 = 0;
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        if(InputGet(I_CutOrgin) == ON)
        {
            GUS.TestPara.CutterCyclePlusNum = HZ_AxGetCurPos(Axis_Cutter) - pluse1;
            pluse1 = HZ_AxGetCurPos(Axis_Cutter);
            GUS.TestPara.CutterEncoderCyclePlusNum = EncodeGetPos(CutterEncode) - pluse2;
            pluse2 = EncodeGetPos(CutterEncode);
            LG->step = 3;
        }
        break;

    case 3:
        if(InputGet(I_CutOrgin) == OFF)
        {
            LG->count++;
            if(LG->count >= 5)
            {
                LG->step = 4;
            }
            else
            {
                LG->step = 2;
            }
        }
        break;

    case 4:
        HZ_AxStopDec(Axis_Cutter);
        LG->step = 0;
        LG->execute = 0;
        break;
    }
}

/**
* @author： 2019/5/23 NYZ
* @Description: 送料脉冲测试
* @param –
* @param --
* @return --
*/
void FeedPulseTest(LogicParaDef *LG)
{
    static s32 pluse;
    if(LG->execute == 1 && LG->step == 0)
    {
        Data.PackageSpd = GUS.JogSpeed * GUS.PackgeLength;
        LG->step = 1;
        LG->count = 0;
    }

    if(LG->execute == 0 && LG->step != 0)
    {
        LG->step = 4;
    }

    switch(LG->step)
    {
    case 1:
        FeedMove(1, Data.PackageSpd);
        pluse = 0;
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        if(InputGet(I_FeedProductOrgin) == ON)
        {
            //测试每圈脉冲数
            GUS.TestPara.FeedProductPlusNum = GetFeedPos() - pluse;
            pluse = GetFeedPos();
            LG->step = 3;
        }
        break;

    case 3:
        if(InputGet(I_FeedProductOrgin) == OFF)
        {
            TimerRst1(LG);
            LG->count++;
            if(LG->count >= 5)
            {
                LG->step = 4;
            }
            else
            {
                LG->step = 2;
            }
        }
        break;

    case 4:
        FeedStop();
        LG->step = 0;
        LG->execute = 0;
        break;
    }
}

void ManualJog()
{
    u32 speed;
    if(GUR.RunStatus == RUN)
    {
        return;
    }
    //走纸点动
    if(WordToBit(GUW.button.BitControlWord, FilmFeedJogPos))
    {
        CamEnd(&CamTable);//走纸点动前要先结束凸轮
        FilmFeedMove(1, GUS.JogSpeed * GUS.PackgeLength, 50);
        if(GUS.SysPara.JogMiddlePackageSwitch == 0)
        {
            OutputSet(Q_MiddleClose	, OFF); //合上中封
        }
    }
    else if(WordToBit(GUW.button.BitControlWord, FilmFeedJogNeg))
    {
        CamEnd(&CamTable);//走纸点动前要先结束凸轮
        FilmFeedMove(-1, GUS.JogSpeed * GUS.PackgeLength, 50);
        if(GUS.SysPara.JogMiddlePackageSwitch == 0)
        {
            OutputSet(Q_MiddleClose	, OFF); //合上中封
        }
    }


    if(R_Trig(1, WordToBit(GUW.button.BitControlWord, FilmFeedJogPos) == 0 && WordToBit(GUW.button.BitControlWord, FilmFeedJogNeg) == 0))
    {
        FilmFeedStop(50);
    }

    //送料点动
    if(WordToBit(GUW.button.BitControlWord, FeedJogPos))
    {
        FeedMove(1, GUS.JogSpeed * GUS.PackgeLength);
    }
    else if(WordToBit(GUW.button.BitControlWord, FeedJogNeg))
    {
        FeedMove(-1, GUS.JogSpeed * GUS.PackgeLength);
    }

    if(R_Trig(2, WordToBit(GUW.button.BitControlWord, FeedJogPos) == 0 && WordToBit(GUW.button.BitControlWord, FeedJogNeg) == 0))
    {
        FeedStop();
    }

    //切刀点动
    if(WordToBit(GUW.button.BitControlWord, CutterJogPos))
    {
        speed = GUS.JogSpeed * GUS.PackgeLength * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60;
        MC_MovePP(Axis_Cutter, SPDMODE, 1000, 100, speed, 100);
    }
    else if(WordToBit(GUW.button.BitControlWord, CutterJogNeg))
    {
        speed = GUS.JogSpeed * GUS.PackgeLength * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60;
        MC_MovePP(Axis_Cutter, SPDMODE, 1000, 100, speed, -100);
    }

    if(R_Trig(3, WordToBit(GUW.button.BitControlWord, CutterJogPos) == 0 && WordToBit(GUW.button.BitControlWord, CutterJogNeg) == 0))
    {
//		HZ_AxStopDec(Axis_Cutter);
        if(HZ_AxGetSpd(Axis_Cutter) > 1500)
        {
            HZ_AxStopDec(Axis_Cutter);
        }
        else
        {
            HZ_AxStop(Axis_Cutter);
        }
    }

}

void AxisParaSet()
{
    int i;
    for(i = 0; i < 8; i++)
    {
        GSS.AxIOconfig[i].HomeMode = 0;
        GSS.AxIOconfig[i].alarmmode = 0;
        GSS.AxIOconfig[i].OrgNum = i;
        GSS.AxIOconfig[i].Orglev = 1;
        GSS.AxIOconfig[i].limitMode = 0;
    }
//	GSS.AxIOconfig[Axis_Cutter].alarmmode = 2;
    GSS.AxIOconfig[0].alarmmode = 1;
    GSS.AxIOconfig[1].alarmmode = 1;
    GSS.AxIOconfig[2].alarmmode = 1;
    GSS.Conversion[Axis_Cutter] =  GUS.AxisPara.CutterAxisGearRate;
    GSS.Conversion[Axis_Cutter].MPR = 360;
    GSS.Conversion[Axis_FeedFilm] =  GUS.AxisPara.FeedFilmAxisGearRate;
    GSS.Conversion[Axis_FeedProduct] =  GUS.AxisPara.FeedAxisGearRate;
}

/**
* @author： 2019/5/23 NYZ
* @Description: 包装位置更新
* @param –
* @param --
* @return --
*/
void PackagePosRefresh()
{
    GUR.CurrPosPulse[0] = GetFilmFeedPos();
    GUR.CurrPosUU[0] = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, GUR.CurrPosPulse[0]);
    GUR.CurrPosPulse[1] = HZ_AxGetCurPos(Axis_Cutter);
    GUR.CurrPosUU[1] = (GUR.CurrPosPulse[1] % GSS.Conversion[Axis_Cutter].PPR) * 360.0 / GSS.Conversion[Axis_Cutter].PPR;
    GUR.CurrPosPulse[2] = GetFeedPos();
    GUR.CurrPosUU[2] = PulseToUserUnit(&GUS.AxisPara.FeedAxisGearRate, GUR.CurrPosPulse[2]);

    GUR.MainAxPosPP = CamTable.MainCurrPos;
    GUR.SlaveAxPosPP = CamTable.SlaveCurrPos % GSS.Conversion[Axis_Cutter].PPR;

    GUR.MainAxPosUU = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, GUR.MainAxPosPP);
    GUR.SlaveAxPosUU = PulseToUserUnit(&GSS.Conversion[Axis_Cutter], GUR.SlaveAxPosPP);


    GUR.SlaveEncoderPP = EncodeGetPos(CutterEncode) % GUS.AxisPara.CutttorAxisEncodePPR;
    GUR.SlaveEncoderUU = GUR.SlaveEncoderPP * 360.0 / GUS.AxisPara.CutttorAxisEncodePPR;

    GUR.FeedAxPosPP = HZ_AxGetCurPos(Axis_FeedProduct) % GUS.AxisPara.FeedAxisGearRate.PPR;

    //位置暂存，确保上点不需要会原点
    GUS.FeedAxPosTerm = HZ_AxGetCurPos(Axis_FeedProduct);
    GUS.FeedFilmAxPosTerm = GUR.MainAxPosPP;
    GUS.CutterAxPosTerm = GUR.SlaveAxPosPP;
    GUS.CutterEncoderPosTerm = GUR.SlaveEncoderPP;
    GUS.CamExcuteStatusTerm = CamTable.excute;



    GUS.GearSCurrPosPP =  GearData.sCurrPlus;
    GUS.GearMCurrPosPP =  GearData.mCurrPlus;
    GUS.GearStatus   = Data.feedFimeAxisRunFlag;
}

/**
* @author： 2019/7/11 NYZ
* @Description: 触摸屏画凸轮曲线
* @param –
* @param --
* @return --
*/
void CamDrawHMI(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->count = 0;

    }

    switch(LG->step)
    {
    case 1:
        GUW.CamPointNum = CamTable.Ttime;
        GUW.CamCurveDraw = 2;
        GUW.CamCurveDraw1 = 2;
        LG->step = 2;
        break;

    case 2:
        if(GUW.CamCurveDraw == 0 ||  GUW.CamCurveDraw1 == 0)
        {
            GUW.CamCurveDraw = 1;
            GUW.CamCurveDraw1 = 1;
            LG->step = 3;
        }
        break;

    case 3:
        if(GUW.CamCurveDraw == 0 ||  GUW.CamCurveDraw1 == 0)
        {
            LG->execute = 0;
            LG->step = 0;
        }
        break;
    }
}


/**
* @author： 2019/05/22  nyz
* @Description:  走纸轴找色标位置
* @param --
* @param --
* @return --
*/
void FeedFilmReset(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }

    switch(LG->step)
    {
    case 1:
        FilmFeedMove(1, 5000, 50);
        LG->step = 2;
        break;

    case 2:
        if(InputGet(I_ColorMark) == ON)
        {
            FilmFeedStop1();
            LG->step = 3;
        }
        break;

    case 3:
        if(GetFilmFeedSta() == 0)
        {
            HZ_AxSetCurPos(Axis_FeedFilm, 0);
            EncodeSetPos(FeedFilmEncode, 0);
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        }
        break;
    }
}

/**
* @author： 2019/05/22  nyz
* @Description:  切刀轴复位
* @param --
* @param --
* @return --
*/
void CutterReset(LogicParaDef *LG)
{
//	s32 stopAngle;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }

    switch(LG->step)
    {
    case 1:
        MC_MovePP(Axis_Cutter, SPDMODE, 200, 100, 2000 * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, 100);
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        //判断切刀是否堵转
        if(InputGet(I_CutOrgin) == OFF)
        {
            LG->step = 3;
        }
        break;

    case 3:
        if(InputGet(I_CutOrgin) == ON)
        {
            TimerRst1(LG);
            HZ_AxStopDec(Axis_Cutter);
            LG->step = 4;
        }
        break;

    case 4:
        if(HZ_AxGetStatus(Axis_Cutter) == 0 && TimerCnt1(LG) > 200)
        {
            MC_MovePP(Axis_Cutter, SPDMODE, 200, 100, 2000 * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, -100);
            LG->step = 5;
        }
        break;

    case 5:
        if(InputGet(I_CutOrgin) == OFF)
        {
            HZ_AxStop(Axis_Cutter);
            LG->step = 6;
        }
        break;

    case 6:
        if(HZ_AxGetStatus(Axis_Cutter) == 0  )
        {
            TimerRst1(LG);
            LG->step = 7;
        }
        break;

    case 7:
        if(TimerCnt1(LG) > 200)
        {
            MC_MovePP(Axis_Cutter, RELMODE, 200, 100, 2000 * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, GUS.SysPara.CutterHomeOffset * (int)GUS.AxisPara.FeedFilmAxisGearRate.PPR / 360);
            LG->step = 8;
        }
        break;

    case 8:
        if(HZ_AxGetStatus(Axis_Cutter) == 0  )
        {
            TimerRst1(LG);
            LG->step = 9;
        }
        break;

    case 9:
        if(TimerCnt1(LG) > 300)
        {
            HZ_AxSetCurPos(Axis_Cutter, 0);
            EncodeSetPos(CutterEncode, 0);
            LG->step = 10;
        }
        break;

    case 10:
        if(HZ_AxGetStatus(Axis_Cutter) == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        }
        break;
    }
}


/**
* @author： 2019/05/22  nyz
* @Description:  送料轴复位
* @param --
* @param --
* @return --
*/
void FeedReset(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }

    switch(LG->step)
    {
    case 1:
        FeedMove(1, 5000);
        LG->step = 2;
        break;

    case 2:
        if(InputGet(I_FeedProductOrgin) == ON)
        {
            FeedStop();
            LG->step = 3;
        }
        break;

    case 3:
        LG->execute = 0;
        LG->step = 0;
        LG->done = 1;
        break;
    }
}



/**
* @author： 2019/05/22  nyz
* @Description:  切刀报警 复位,并且停刀切刀角度
* @param --
* @param --
* @return --
*/
void CutteErrorRst(LogicParaDef *LG)
{
    float k;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }
    //报警伺服自动复位
    //R_Trig(6,AX_ERR_AX_ALM == HZ_AxGetAxisErr(Axis_Cutter)) ||
//	if((InputGet(I_TorqueAlm) == ON && GUS.CutProductEN == 1) )
    if(R_Trig(6, AX_ERR_AX_ALM == HZ_AxGetAxisErr(Axis_Cutter)))
    {
        LG->execute = 1;
    }

    switch(LG->step)
    {
    case 1:
        TimerRst1(LG);
        LG->step = 2;
        break;

    case 2:
        if(TimerCnt1(LG) > 100)
        {
            EN_SetSta(Axis_Cutter, ON); //
            HZ_AxReset(Axis_Cutter);
            TimerRst1(LG);
            LG->step = 3;
        }
        break;

    case 3:
        if(TimerCnt1(LG) > 50)
        {
            EN_SetSta(Axis_Cutter, OFF); //
            if(AX_ERR_AX_ALM == HZ_AxGetAxisErr(Axis_Cutter))
            {
                LG->count++;
                if(LG->count >= 3)
                {
                    LG->step = 0;
                    LG->execute = 0;
                }
                else
                {
                    LG->step = 1;
                }
            }
            else
            {
                LG->step = 4;
            }
            TimerRst1(LG);
        }
        break;

    case 4:
        if(TimerCnt1(LG) > 200)
        {
            k = (float)GUS.AxisPara.CutttorAxisEncodePPR / (float)GUS.AxisPara.CutterAxisGearRate.PPR;
            HZ_AxSetCurPos(Axis_Cutter, (float)Encode[CutterEncode].CurrPos / k);
            LG->step = 5;
        }
        break;

    case 5:
        LogicTask.CutterStopAngleData.execute = 1;
        LG->step = 6;
        break;

    case 6:
        if(LogicTask.CutterStopAngleData.execute == 0)
        {
            LG->step = 7;
            TimerRst1(LG);
        }
        break;

    case 7:
        if(HZ_AxGetStatus(Axis_Cutter) == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
        }
        break;
    }
}


/**
* @author： 2019/07/26  nyz
* @Description:  吹气
* @param --
* @param --
* @return --
*/
u8 BlowOutput[4] = {Y11, Y11, Y11, Y11};
void Blow()
{
    u8 i;
    LogicParaDef *LG;

    for(i = 0; i < 1; i++)
    {

        LG = &LogicTask.BlowData[i];

        if(CamTable.excute == 1 && GetFilmFeedSta() == 1)
        {
            LG->execute = 1;
        }

        if(LG->execute == 1 && LG->step == 0)
        {
            LG->step = 1;
            LG->done = 0;
        }

        switch(LG->step)
        {
        case 1:
            //判断是否需要吹气
            if(GUS.SysPara.BlowEn & (1 << i))
            {
                if(GUR.SlaveAxPosUU >= (float)GUS.SysPara.BlowAngle[i])
                {
                    OutputSet(LOCAL, BlowOutput[i], ON);
                    LG->step = 2;
                }
            }
            else
            {
                LG->step = 3;
            }
            break;

        case 2:
            if(GetFilmFeedSta() == 0 || CamTable.excute == 0 || GUR.SlaveAxPosUU >= ((float)GUS.SysPara.BlowAngle[i] + (float)GUS.SysPara.BlowLength[i]))
            {
                OutputSet(LOCAL, BlowOutput[i], OFF);
                LG->step = 3;
            }
            break;

        case 3:
            if(CamTable.excute == 0)
            {
                LG->execute = 0;
                LG->step = 0;
                LG->done = 0;
            }
            break;
        }
    }

}

/**
* @author： 2019/07/26  nyz
* @Description:  打码
* @param --
* @param --
* @return --
*/
void CodePrint(LogicParaDef *LG)
{
    //当打码启用，并且凸轮正在运行是启动打码程序
    if(GUS.SysPara.CodePrintEn == 1 && CamTable.excute == 1 && GetFilmFeedSta() == 1)
    {
        LG->execute = 1;
    }

    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }


    switch(LG->step)
    {
    case 1:
        if(GUR.SlaveAxPosUU >= (float)GUS.SysPara.CodePrintPos)
        {
            OutputSet(Q_Code, ON);
            TimerRst1(LG);
            LG->step = 2;
        }
        break;

    case 2:
        if(TimerCnt1(LG) > 50)
        {
            OutputSet(Q_Code, OFF);
            LG->step = 3;
        }
        break;

    case 3:
        if(CamTable.excute == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 0;
        }
        break;

    }
}

/**
* @author： 2019/07/26  nyz
* @Description:  喷酒精
* @param --
* @param --
* @return --
*/
void SprayWater(LogicParaDef *LG)
{
    //当打码启用，并且凸轮正在运行是启动打码程序
    if(GUS.SysPara.SprayWaterEn == 1 && CamTable.excute == 1 && GetFilmFeedSta() == 1)
    {
        LG->execute = 1;
    }

    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }


    switch(LG->step)
    {
    case 1:
        if(GUR.SlaveAxPosUU >= (float)GUS.SysPara.SprayWaterPos)
        {
            OutputSet(Q_Code, ON);
            TimerRst1(LG);
            LG->step = 2;
        }
        break;

    case 2:
        if(TimerCnt1(LG) > GUS.SysPara.SprayWaterDelay)
        {
            OutputSet(Q_Code, OFF);
            LG->step = 3;
        }
        break;

    case 3:
        if(CamTable.excute == 0)
        {
            LG->execute = 0;
            LG->step = 0;
            LG->done = 0;
        }
        break;

    }
}


/**
* @author： 2019/07/26  nyz
* @Description:  切刀停止角度，根据当前刀的位置，确定刀应该往那边停刀，根据切刀闭合角度 和 停刀角度做判断
* @param --
* @param --
* @return --
*/

void CutterStopAngle(LogicParaDef *LG)
{
    u32 speed;
    s32 cutterAngle;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
    }

    cutterAngle = (int)GUR.SlaveAxPosUU % (360 / GUS.SysPara.CutterNum);

    switch(LG->step)
    {
    case 1:
        speed = Data.PackageSpd * GUS.AxisPara.CutterAxisGearRate.PPR / GUS.AxisPara.CutterAxisGearRate.MPR / 60;
        if(HZ_AxGetStatus(Axis_Cutter) == 0)
        {
            //当当前刀的位置小于闭合角度时，往反方向停刀
            if(cutterAngle < 30)
            {
                MC_MoveUU(Axis_Cutter, RELMODE, 1000, 100, speed, -cutterAngle - GUS.CutterStopAngle);
            }
            else if(cutterAngle >= 30 && cutterAngle < GUS.CutterStopAngle)
            {
                //当刀的位置大于闭合角度，又小于停刀角度时，往正方向停刀
                MC_MoveUU(Axis_Cutter, RELMODE, 1000, 100, speed, GUS.CutterStopAngle - GUR.SlaveAxPosUU);
            }
            else if(cutterAngle > (360 / GUS.SysPara.CutterNum - GUS.CutterStopAngle))
            {
                //当刀的位置大于刀360 - 停刀角度时，往反方向停刀
                MC_MoveUU(Axis_Cutter, RELMODE, 1000, 100, speed, (360 / GUS.SysPara.CutterNum - GUS.CutterStopAngle) - GUR.SlaveAxPosUU);
            }
            LG->step = 2;
        }
        break;

    case 2:
        if(HZ_AxGetStatus(Axis_Cutter) == 0)
        {
            LG->step = 3;
        }
        break;

    case 3:
        LG->execute = 0;
        LG->step = 0;
        LG->done = 0;
        break;
    }
}


/**
* @author： 2019/07/26  nyz
* @Description:  空包传感器感应
* @param --
* @param --
* @return --
*/
s8 EmptySensor()
{
    if(InputGet(I_EmptyCheck) == ON || GUS.SysPara.FeedAxisEn != 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
* @author： 2019/07/26  nyz
* @Description:  产量计数
* @param --
* @param --
* @return --
*/
void ProductCount()
{
    //产量计数
    GUS.Product.CurrNum++;
    if(GUS.Product.StopEn == 0)
    {
        if(GUS.Product.CurrNum >= GUS.Product.SetNum)
        {
            AlarmSetBit(2, 0, 2); //产量到达
        }
    }
}


/**
* @author： 2019/07/26  nyz
* @Description:  连续丢标检测
* @param --
* @param --
* @return --
*/
void ColorMarkLoseCheck()
{
    static u8 count;
    if(R_Trig(4, InputGet(I_ColorMark) == ON))
    {
        //实际袋长
        GUS.TestPara.PackageLTest = GetFilmFeedPosUU() - Data.MarkPosStart;
        //获取走纸位置
        Data.MarkPosStart = GetFilmFeedPosUU();
        if(abs(GUS.TestPara.PackageLTest - GUS.PackgeLength) > 5)
        {
            count++;
            if(count >= 3)
            {
                count = 0;
                AlarmSetBit(2, 0, 6); //包装长度设置错误
            }
        }
        else
        {
            count = 0;
        }

    }

    if(fabs(GetFilmFeedPosUU() - Data.MarkPosStart) >= MaxS32(2, GUS.SysPara.LoseColorMarkTimes)*GUS.PackgeLength)
    {
        Data.MarkPosStart = GetFilmFeedPosUU();
        //连续丢标判断
        if(GUS.SysPara.LoseColorMarkTimes != 0)
        {
            AlarmSetBit(2, 0, 5); //连续丢标
        }
    }
}

/**
* @author： 2019/08/2  nyz
* @Description:  指示灯
* @param --
* @param --
* @return --
*/


void LedWork(u8 whichLed)
{
    switch(whichLed)
    {
    case 0: //run
        OutputSet(Q_Red	, OFF);
        OutputSet(Q_Green, ON);
        OutputSet(Q_Yellow, OFF);
        break;

    case 1://Stop
        OutputSet(Q_Red	, OFF);
        OutputSet(Q_Green, OFF);
        OutputSet(Q_Yellow, ON);
        break;

    case 2://ERROR
        OutputSet(Q_Red	, ON);
        OutputSet(Q_Green, OFF);
        OutputSet(Q_Yellow, OFF);
        break;


    }
}




/**
* @author： 2019/08/2  nyz
* @Description:  数据保护
* @param --
* @param --
* @return --
*/


void ParaProtect()
{
    //包装速度保护
    if(GUS.PackageSpeed > GUS.SysPara.MaxPackageSpeed)
    {
        GUS.PackageSpeed = GUS.SysPara.MaxPackageSpeed;
    }
//	else if(GUS.PackageSpeed < GUS.SysPara.MinPackageSpeed)
//	{
//		GUS.PackageSpeed = GUS.SysPara.MinPackageSpeed;
//	}
//
//	if(GUS.PackageSpeedMPS > GUS.SysPara.MaxDifPackageSpeed)
//	{
//		GUS.PackageSpeedMPS = GUS.SysPara.MaxDifPackageSpeed;
//	}
//
//	//包装速度保护
//	if(GUS.JogSpeed > GUS.SysPara.MaxPackageSpeed)
//	{
//		GUS.JogSpeed = GUS.SysPara.MaxPackageSpeed;
//	}
//	else if(GUS.JogSpeed < GUS.SysPara.MinPackageSpeed)
//	{
//		GUS.JogSpeed = GUS.SysPara.MinPackageSpeed;
//	}



//	//包装长度保护
//	if(GUS.PackgeLength < (float)GUS.SysPara.MinPackageLength)
//	{
//		GUS.PackgeLength = (float)GUS.SysPara.MinPackageLength;
//	}
//	//
//	if(GUS.SysPara.ProductLength > GUS.PackgeLength)
//	{
//		 GUS.SysPara.ProductLength =  GUS.PackgeLength ;
//	}

    if(GUS.ProductPos > (s32)GUS.PackgeLength)
    {
        GUS.ProductPos = GUS.PackgeLength;
    }
    else if(GUS.ProductPos < -(s32)GUS.PackgeLength)
    {
        GUS.ProductPos = -(s32)GUS.PackgeLength;
    }

    if(GUS.colorMarkPos > (s32)GUS.PackgeLength)
    {
        GUS.colorMarkPos = GUS.PackgeLength;
    }
    else if(GUS.colorMarkPos < -(s32)GUS.PackgeLength)
    {
        GUS.colorMarkPos = -(s32)GUS.PackgeLength;
    }


//	GUS.SysPara.SpeedMarkPosComp = 6;
}

/**
* @author： 2019/5/7 NYZ
* @Description: 辅助函数，主要是用于跟逻辑无关且需要一直运行的函数，都放在这里
* @param –
* @param --
* @return --
*/
void AuxLogic()
{

    Alarm();
    IOCheck();
    AxisParaSet();
    SpeedParaInit();
    Teach();
    HMI_Button();
    PackagePosRefresh();
    CamDrawHMI(&LogicTask.CamDrawHMI);
    Blow();
    CodePrint(&LogicTask.CodePrintData);
    SprayWater(&LogicTask.SprayWaterData);
    CutterStopAngle(&LogicTask.CutterStopAngleData);
    ParaProtect();
    if(GUR.RunStatus != RUN)
    {
        PackageLengthTest(&LogicTask.PackageLengthTest);
        CutterPulseTest(&LogicTask.CutterCirPulseTest);
        FeedPulseTest(&LogicTask.FeedCirPulseTest);
        ManualJog();
    }

    //送料电机和走纸电机都默认为伺服
    GUS.SysPara.FeedFilmAxisSel = 0;
    GUS.SysPara.FeedMotorSel = 0;

    FeedFilmReset(&LogicTask.FeedFilmReset);
    CutterReset(&LogicTask.CutterReset);
    FeedReset(&LogicTask.FeedReset);
    CutteErrorRst(&LogicTask.CutterErrRst);
}

