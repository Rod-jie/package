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

extern CamTableDef CamTable;  //电子凸轮表
GearInDataDef GearData;    //电子齿轮咬合
FeedDataDef FeedData[10];   //送料位置参数

#define Df_ProductPos (-GUS.ProductPos)

#define T (*Table)

/**
* @author： 2019/7/25 NYZ
* @Description:  凸轮位置矫正，当凸轮位置和凸轮点位不对应时，调用该函数矫正凸轮位置
* @param --
* @param --
* @return --
*/

void CamCorrect2(u8 MainAxis, u8 SlaveAxis, CamTableDef *Table, LogicParaDef *LG)
{
    s32 a1, a2;
//	u8 i,j;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
    }

    T.index = CamGetMainIndex(Table, MainAxis);
    switch(LG->step)
    {
    //CamIn 凸轮咬合
    case 1:
        if(GUS.ColorMarkEn == 0)
        {
            if(T.excute == 0)	//电子凸轮空闲
            {
                LG->step = 0xA0;
            }
            else if(GUR.MainAxPosUU > GUS.PackgeLength)
            {
                T.MainStartPos += UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, GUS.PackgeLength);
            }
            else if(GUR.MainAxPosUU < 0)
            {
                T.MainStartPos -= UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, GUS.PackgeLength);
            }
            else
            {
                PackageDataArray[0].StartPos = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, T.MainStartPos);
                PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + GUS.PackgeLength;
                LG->step = 0xA0;
            }
        }
        else
        {
            if(Data.FirstPackageFlag == 1)
            {
                T.MainStartPos = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackageDataArray[0].StartPos);
                LG->step = 0xA0;
            }
            else
            {

                LG->step = 3;
            }
        }
        break;

    case 0xA0:
        if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
        {
            a1 = abs(T.index - T.SlaveIndex);
            a2 = T.Ttime - abs(T.index - T.SlaveIndex);
            if(T.index > T.SlaveIndex && a1 < a2)
            {
                MC_MovePP(SlaveAxis, RELMODE, 1000, 60, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, T.SlaveAxisPos[T.index] - (HZ_AxGetCurPos(SlaveAxis) - T.SlaveStartPos) % T.SlaveCirPulse);
            }
            else if(T.index < T.SlaveIndex && a1 > a2)
            {
                MC_MovePP(SlaveAxis, RELMODE, 1000, 60, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, T.SlaveAxisPos[T.index] + T.SlaveCirPulse - (HZ_AxGetCurPos(SlaveAxis)						 - T.SlaveStartPos) % T.SlaveCirPulse);
            }

            LG->step = 2;
        }
        break;

    case 2:
        if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
        {
            LG->step = 4;
        }
        break;

    case 3:
        LG->step = 4;
        break;

    case 4:
        LG->execute = 0;
        LG->step = 0;
        break;
    }
}


/**
* @author： 2019/7/25 NYZ
* @Description:  均值滤波
* @param --     value 输入数值
* @param --			times 滤波次数
* @return --   平均值
*/
float AverageFilter2(float value, u8 times)
{
    s16 i;
    float sum;
    float avr;
    //求色标位置偏差，
    for(i = times - 1; i >= 1; i--)
    {
        Data.MarkPosDif[i] = Data.MarkPosDif[i - 1];
    }
    Data.MarkPosDif[0] = value;
    sum = 0;
    for(i = 0; i < times; i++)
    {
        sum += Data.MarkPosDif[i];
    }
    avr = sum / 10;

    return avr;
}

/**
* @author： 2019/9/6 NYZ
* @Description:  计算物料在在包装袋内的位置，
* @param --
* @param --
* @return --
*/
void ProductPosCaculat()
{
    if(GUS.ColorMarkEn == 0)
    {
        GUR.ProductCurrPos = ((int)GUS.SysPara.EmptySensorToCutterDis + (int)Df_ProductPos % (int)GUS.PackgeLength) % (int)GUS.PackgeLength;
    }
    else
    {
        if((int)GUS.SysPara.EmptySensorToCutterDis >= (int)Df_ProductPos % (int)GUS.PackgeLength)
        {
            GUR.ProductCurrPos = GUS.RealPackgeLength - ((int)GUS.SysPara.EmptySensorToCutterDis - (int)GUS.colorMarkPos % (int)GUS.PackgeLength) % (int)GUS.PackgeLength;
        }
        else
        {
            GUR.ProductCurrPos = (int)GUS.colorMarkPos % (int)GUS.RealPackgeLength - (int)GUS.SysPara.EmptySensorToCutterDis;
        }
    }
}

/**
* @author： 2019/7/19  nyz
* @Description:  色标跟踪
* @param --
* @param --
* @return --
*/

void ColorMarkCheck2(LogicParaDef *LG)
{
    float markPosDif;  //色标偏差
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    //连续丢标检测
    if(LG->execute == 1)
    {
        ColorMarkLoseCheck();
    }

    switch(LG->step)
    {
    case 1:
        //|| GetFilmFeedPosUU() - Data.MarkPosStart >= GUS.PackgeLength
        if(InputGet(I_ColorMark) == ON )
        {
//            //计算空包传感器和色标位置的偏差
//            Data.MarkToEmtySenDif = (int)(GUS.SysPara.EmptySensorToCutterDis) % (int)(GUS.PackgeLength)  - ColorMarkPos;
//            if(Data.MarkToEmtySenDif >= 0)
//            {
//                if(Data.MarkToEmtySenDif <= Df_ProductPos)
//                    Data.EndPos = GetFilmFeedPosUU() + Df_ProductPos - Data.MarkToEmtySenDif;
//                else
//                    Data.EndPos = GetFilmFeedPosUU() + GUS.PackgeLength + Df_ProductPos - Data.MarkToEmtySenDif;
//            }
//            else
//            {
//                Data.EndPos = GetFilmFeedPosUU() + Df_ProductPos + fabs(Data.MarkToEmtySenDif);
//            }
					 

            //如果凸轮在运行，过程中检测到偏差
            if(CamTable.excute == 1)
            {
                //色标偏差 = 包装袋长 - 色标位置 - 凸轮当前位置
                markPosDif = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, CamTable.MainCirPulse - CamTable.MainCurrPos) - ColorMarkPos;
            }
            else
            {
                markPosDif = 0;
            }

            if(fabs(markPosDif) > 5)
            {
                markPosDif = 0;
            }

            //求色标偏差的10次平均值
            Data.ColorMarkDif = AverageFilter2(markPosDif, 10);
            PackageDataArray[1] = PackageDataArray[0];
            PackageDataArray[0].StartPos = GetFilmFeedPosUU() +  ColorMarkPos - Data.ColorMarkDif;
            PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + GUS.PackgeLength;
						
						Data.EndPos = PackageDataArray[0].EndPos + (float)(Df_ProductPos % (int)GUS.PackgeLength);
            Data.ProductBuffCont++;
            LG->step = 2;
        }
        break;

    case 2:
        if(InputGet(I_ColorMark) == OFF)
        {
            LG->step = 1;
        }
        break;
    }
}

/**
* @author： 2019/10/17  nyz
* @Description:  切点检测（不追色标）
* @param --
* @param --
* @return --
*/

void CutPointCheck2(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    switch(LG->step)
    {
    case 1:
        //
        if(CamTable.excute == 0 )
        {
//            //计算空包传感器和色标位置的偏差
//            Data.MarkToEmtySenDif = (float)(((int)GUS.SysPara.EmptySensorToCutterDis + Df_ProductPos) % (int)GUS.PackgeLength) ;
//            Data.EndPos = GetFilmFeedPosUU() + Data.MarkToEmtySenDif ;
            PackageDataArray[1] = PackageDataArray[0];
            PackageDataArray[0].StartPos = GSR.AxisUnitPos[Axis_FeedFilm];
            Data.PosAdd = 0;
            PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + GUS.PackgeLength;						
            Data.ProductBuffCont++;
						Data.EndPos = PackageDataArray[0].EndPos + (float)(Df_ProductPos % (int)GUS.PackgeLength);
            LG->step = 2;
        }
        break;

    case 2:
        if(CamTable.excute == 1 || (CamTable.excute == 0 && GetFilmFeedPosUU() - PackageDataArray[0].StartPos >= GUS.PackgeLength))
        {
            LG->step = 1;
        }
        break;

    }
}

/**
* @author： 2019/9/4 nyz
* @Description:  空包感应，看有没有料经过
* @param --
* @param --
* @return --
*/

void EmptyCheck(LogicParaDef *LG)
{
    u8 i;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        ton_et[1] = 0;
    }

    switch(LG->step)
    {
    case 1:
//				FeedMove(1,Data.PackageSpd*GUS.PackgeLength/GUS.AxisPara.FeedAxisGearRate.MPR);
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        //防空包传感器，检测有没有料流过
        if(InputGet(I_EmptyCheck) == ON)
        {
            GUS.FeedStartPos = GSR.AxisUnitPos[Axis_FeedProduct];
//				 Data.IsHaveProduct[0] = 1;
            LG->step = 0xA0;
        }
        //检测拔插有没有感应到
        if(InputGet(I_FeedProductOrgin) == ON)
        {
            LG->step = 3;
        }
        break;

    case 0xA0:
        if(InputGet(I_EmptyCheck) == OFF)
        {
            GUS.FeedEndPos = GSR.AxisUnitPos[Axis_FeedProduct];
            if(GUS.FeedEndPos - GUS.FeedStartPos > 100)
            {
                LG->step = 3;
                Data.IsHaveProduct[0] = 1;
            }
        }

        if(InputGet(I_FeedProductOrgin) == ON)
        {
            LG->step = 3;
        }
        break;

    case 3:
        if(InputGet(I_FeedProductOrgin) == ON)
        {
            for(i = 9; i > 0; i--)
            {
                Data.IsHaveProduct[i] = Data.IsHaveProduct[i - 1];
            }
            Data.IsHaveProduct[0] = 0;
            LG->step = 4;
        }
        break;

    case 4:
        if(InputGet(I_FeedProductOrgin) == OFF)
        {
            LG->step = 2;
        }
        break;
    }
}



///**
//* @author： 2019/7/8  nyz
//* @Description:  检查物料是否在相应的位置送到位
//* @param --
//* @param --
//* @return --
//*/
float PackgeLengthTerm;
void PruductCheck2(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        ton_et[1] = 0;
			  PackgeLengthTerm = GUS.PackgeLength;
    }

    switch(LG->step)
    {
    case 1:
        TimerRst1(LG);
        OutputSet(Q_MiddleClose	, OFF); //闭合中封
        LG->step = 2;
        break;

    case 2:
        if(TimerCnt1(LG) > GUS.MiddleCloseDelay || Data.JogFlag == 1)
        {
            LG->step = 3;
        }
        break;

    case 3:
					GearIn(Axis_FeedFilm, Axis_FeedProduct,
								 UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackgeLengthTerm),//GUS.PackgeLength - Data.PosAdd),
								 GUS.AxisPara.FeedAxisGearRate.PPR,
								 Data.PackageSpd * GUS.PackgeLength / GUS.AxisPara.FeedAxisGearRate.MPR / GUS.AxisPara.FeedAxisGearRate.MPR * GUS.AxisPara.FeedAxisGearRate.PPR / 60, //主轴速度
								 &GearData);	
        //判断停机前走膜是否在运行状态
        if(Data.feedFimeAxisRunFlag == 1)
        {				
            //判断走膜是否需要快速启动
            if(GUS.FastStopEn == 1)
            {
                FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
            }
            else
            {
                FilmFeedMove(1, Data.PackageSpd, 1000); //开始走纸
            }
        }
        else
        {
            GearOut(&GearData);           //电子齿轮咬合脱离
        }
        LG->step = 4;
        break;

    case 4:
        if(InputGet(I_FeedProductOrgin) == ON)
        {
            LG->step = 8;
        }
        break;

    case 8:
//        if(GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos >  GUS.PackgeLength)
//        {
//            Data.EndPos +=  GUS.PackgeLength;
//        }
//			  else if(GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos <  -GUS.PackgeLength)
//				{
//						Data.EndPos -=  GUS.PackgeLength;
//				}
//				Data.PosAdd = GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos;

//				if(Data.PosAdd<GUS.PackgeLength && Data.PosAdd>0)
//				{
//					Data.PosAdd = -Data.PosAdd;
//				}
				if(Data.EndPos < GSR.AxisUnitPos[Axis_FeedFilm])
				{
					Data.EndPos += GUS.PackgeLength;
				}
				
				PackgeLengthTerm = Data.EndPos - GSR.AxisUnitPos[Axis_FeedFilm];
		    
				if(PackgeLengthTerm < GUS.PackgeLength*3/4 && PackgeLengthTerm > 0)
				{
					PackgeLengthTerm = GUS.PackgeLength + PackgeLengthTerm;
				}
				
				
				//2020.2.21 注释掉，如果执行这个判断的话，送料拨抓位置会有变化
//				if(GUS.ColorMarkEn)
//				{				
//					if(Data.PosAdd >= 20 || Data.PosAdd <= -20)
//					{
//							Data.PosAdd = 0;
//					}
//				}
				
        Data.EndPos = Data.EndPos + GUS.PackgeLength;
				//**********************************//
        LG->step = 9;
        break;

    case 9:
        GearIn(Axis_FeedFilm, Axis_FeedProduct,
               UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackgeLengthTerm),
               GUS.AxisPara.FeedAxisGearRate.PPR,
               Data.PackageSpd * GUS.PackgeLength / GUS.AxisPara.FeedAxisGearRate.MPR / GUS.AxisPara.FeedAxisGearRate.MPR * GUS.AxisPara.FeedAxisGearRate.PPR / 60, //主轴速度
               &GearData);
        GearData.mStartPos = GSR.AxisPosition[Axis_FeedFilm];
        GearData.sStartPos = GSR.AxisPosition[Axis_FeedProduct];
        if(Data.IsHaveProduct[GUS.SysPara.EmptyCount] == 1 || Data.JogFlag == 1 || GUS.EmptyCheckEn == 0)
        {
            Data.IsHaveProduct[GUS.SysPara.EmptyCount] = 0;
//					Data.ProductBuffCont++;
            if(Data.feedFimeAxisRunFlag == 0)
            {
                FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
            }
            Data.feedFimeAxisRunFlag = 1;
            LG->step = 10;
        }
        else
        {
            //如果经过一个物料位，没有物料,走纸停止
            FilmFeedStop(50);     					//停止走纸
            Data.feedFimeAxisRunFlag = 0;
            GearOut(&GearData);           //电子齿轮咬合脱离
            LG->step = 0xA0;
        }
        break;

    case 0xA0:
        if(GetFilmFeedSta() == 0)
        {
            LG->step = 0xA1;
        }
        break;

    case 0xA1:
        if(InputGet(I_FeedProductOrgin) == OFF)
        {
            LG->step = 0xA2;
        }
        break;

    case 0xA2:
        if(InputGet(I_FeedProductOrgin) == ON)
        {

            LG->step = 9;
        }
        break;

    case 0xB0:
        GearEnd(&GearData);
        LogicTask.ScaldFilmAvoid.execute = 2;
        LG->step = 0xB1;
        break;

    case 0xB1:
        if(LogicTask.ScaldFilmAvoid.execute == 0)
        {
            LG->step = 9;
        }
        break;

    case 10:
        if(InputGet(I_FeedProductOrgin) == OFF)
        {
            LG->step = 3;
        }
        break;
    }
}

/**
* @author： 2019/05/17  nyz
* @Description:  包装，追标
* @param --
* @param --
* @return --
*/
void Package2(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    switch(LG->step)
    {
    case 1:
        if(GUS.SysPara.FeedAxisEn == 1)
        {
            LogicTask.PruductCheck.execute = 1;
            LogicTask.EmptyCheck.execute = 1;
        }
        else
        {
            OutputSet(Q_MiddleClose	, OFF); //闭合中封
            FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        }

        LogicTask.ColorMarkCheck.execute = 1;

        LG->step = 2;
        break;

    case 2:
        if(GetFilmFeedSta() == 0 && GUR.RunStatus != RUN)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        if(GUS.SysPara.CutterAxisEn != 1)
        {
            return;
        }

        if(GSR.AxisUnitPos[Axis_FeedFilm] - PackageDataArray[0].StartPos >= GUS.PackgeLength)
        {
            PackageDataArray[0].StartPos += GUS.PackgeLength;
            PackageDataArray[0].EndPos =  PackageDataArray[0].StartPos  + GUS.PackgeLength;
        }
        else if(PackageDataArray[0].StartPos - GSR.AxisUnitPos[Axis_FeedFilm]   >= GUS.PackgeLength)
        {
            PackageDataArray[0].StartPos -= GUS.PackgeLength;
            PackageDataArray[0].EndPos =  PackageDataArray[0].StartPos  + GUS.PackgeLength;
        }



        if(CamTable.excute == 0 && Data.ProductBuffCont > 0 && GSR.AxisUnitPos[Axis_FeedFilm] >= PackageDataArray[0].StartPos)
        {
            GUS.RealPackgeLength = PackageDataArray[0].EndPos - GSR.AxisUnitPos[Axis_FeedFilm];
//				if(GUS.RealPackgeLength >=	(GUS.PackgeLength - 10))
            if(GUS.RealPackgeLength >=	(GUS.PackgeLength) / 2 )
            {
                CutCamCaculate(GUS.RealPackgeLength);
                LogicTask.CamDrawHMI.execute = 1;
                CamIn(&CamTable);
                OutputSet(Q_CutSign, ON); //切刀信号输出
                TimerRst1(LG);
                LG->step = 3;
            }
            else
            {
                LG->step = 2;
            }

        }
        else if(CamTable.excute == 1)
        {
            LG->step = 3;
            CamIn(&CamTable);
        }
        break;

    case 3:
        if(TimerCnt1(LG) > 100)
        {
            OutputSet(Q_CutSign, OFF); //切刀信号输出
        }

        if(CamTable.excute == 0)
        {
            //计算凸轮偏差
            //凸轮周期结束,把第0个缓存点移出
            if(Data.FirstPackageFlag == 1)
            {
                Data.ProductBuffCont--;
                //产量计数
                ProductCount();
            }
            Data.FirstPackageFlag = 1;
            LG->step = 4;
        }

        if(GetFilmFeedSta() == 0 && GUR.RunStatus != RUN)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    case 4:
        //显示每个周期的切刀的跟随误差
        GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
        LG->step = 2;
        TimerRst1(LG);
        break;
    }
}




/**
* @author： 2019/05/17  nyz
* @Description:  包装点动
* @param --
* @param --
* @return --
*/
void PackageJog2(LogicParaDef *LG)
{
    if(R_Trig(8, InputGet(I_JogBt) == ON) && GUR.HaveToReset == 0 && GUR.RunStatus == STOP)
    {
        Data.JogFlag = 1;
        GUW.button.RunCommand = RUN;
    }

    if(R_Trig(9, InputGet(I_JogBt) == OFF) && GUR.HaveToReset == 0 && Data.JogFlag == 1)
    {
        GUW.button.RunCommand = STOP;
        FilmFeedStop1();
    }
}

/**
* @author： 2019/1/9 农业照
* @Description: 主调函数逻辑
* @param –
* @param --
* @return --
*/

/**
* @author： 2019/10/19 NYZ
* @Description: 包装循环减速停止
* @param –
* @param --
* @return --
*/
void CylStop(LogicParaDef *LG)
{
    static u32 packageSpeed;
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        packageSpeed = GUS.PackageSpeed;
    }

    if(Data.CylStopFlag == 1)
    {
            if(GUS.FastStopEn == 1)
            {
                //循环快速停机
                if(GUS.SysPara.CutterAxisEn == 1)
                {
                    if((GUR.SlaveAxPosUU >= GUS.CutterStopAngle && GUR.SlaveAxPosUU < GUS.CutterStopAngle + 4) || GetFilmFeedSta() == 0)
                    {
                        Data.CylStopFlag = 0;
                        GUW.button.RunCommand = STOP;
                    }
                }
                else
                {
                    GUW.button.RunCommand = STOP;
                }
            }
            else
            {
                //循环慢速停机
                if(GUS.SysPara.CutterAxisEn == 1 && GetFilmFeedSta() == 1 )
                {
                    LG->execute  = 1;
                }
                else
                {
                    GUW.button.RunCommand = STOP;
                }
            }
    }

    switch(LG->step)
    {
    case 1:
        if(GetFilmFeedSta() == 1)
        {
            TimerRst1(LG);
            if(packageSpeed > 50)
            {
                packageSpeed -= 5;
            }
            Data.PackageSpd = packageSpeed * GUS.PackgeLength;
            LG->step = 2;
        }
        else
        {
            LG->step = 4;
        }
        break;

    case 2:
        if(TimerCnt1(LG) > 10)
        {
            if(packageSpeed > 50)
            {
                LG->step = 1;
            }
            else
            {
                LG->step = 3;
            }
        }
        break;

    case 3:
        if((GUR.SlaveAxPosUU >= GUS.CutterStopAngle && GUR.SlaveAxPosUU < GUS.CutterStopAngle + 4) || GUS.SysPara.CutterAxisEn != 1)
        {
            LG->step = 4;
            FilmFeedStop1();
        }
        break;

    case 4:
        Data.CylStopFlag = 0;
        GUW.button.RunCommand = STOP;
        LG->execute = 0;
        LG->step = 0;
        break;
    }
}


void RunProcess2(LogicParaDef *LG)
{
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        Data.CutterStuckCount = 0;
    }

    if(Data.JogFlag == 0)	//点动标志位：   1：点动启动   0：点动未启动
    {
        if(Data.CylStopFlag == 0)	//循停标志位：	1：循停   0：正常工作
        {
            Data.PackageSpd = GUS.PackageSpeed * GUS.PackgeLength;	//包装速度换算成包每分
        }
    }
    else
    {
        Data.PackageSpd = GUS.JogSpeed * GUS.PackgeLength;
    }

    switch(LG->step)
    {
    case 1:
        //包装开始
        if(Data.CutterStuckFlag == 1)
        {
            LogicTask.CamCorrectData.execute = 1;
        }
        GUS.IsReset = 0;
        LG->step = 0xA0;
        break;

    case 0xA0:
        if(LogicTask.CamCorrectData.execute  == 0)
        {
            LogicTask.Package.execute = 1;
						Data.CutterStuckFlag = 0;
            LG->step = 2;
        }
        break;

    case 2:
        //判断是否有堵刀
        if(CutterStuckCheck() == 1)
        {
            Data.CutterStuckFlag = 1;
            Data.FeedStaTerm = GetFeedSta();
//				FilmFeedStop(50);
            HZ_AxStop(Axis_FeedFilm);
            CamOut(&CamTable);
            PARAINIT(CamTable);
            PARAINIT(LogicTask.Package);
            PARAINIT(LogicTask.PruductCheck);
            if(GUS.SysPara.PassPackageNum > 10)
            {
                GUS.SysPara.PassPackageNum = 10;
            }

            if(GUS.ColorMarkEn == 0)
            {
                Data.ProductBuffCont = 1;
            }
            else
            {
                Data.ProductBuffCont = 2;
            }

            Data.ProductBuffCont -= GUS.SysPara.PassPackageNum;
						
            LG->step = 3;
        }
        else if(LG->execute == 0)
        {
            LG->step = 5;
            //包装循环停止
            FilmFeedStop(50);
//					CamOut(&CamTable);
        }
        break;

    case 3:
        if(GetFilmFeedSta() == 0 && GetFeedSta() == 0 && HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY)
        {
            LogicTask.CutterStopAngleData.execute = 1;
            LG->step = 4;
        }
        break;

    case 4:
        if(LogicTask.CutterStopAngleData.execute == 0)
        {
            LogicTask.Package.count = 0;
            Data.CutterStuckCount++;
            if(Data.CutterStuckCount >= 3 || CutterStuckCheck() == 1)// GUS.SysPara.CutterReversalTimes)
            {
                PARAINIT(LogicTask.Package);
                PARAINIT(LogicTask.PruductCheck);
                AlarmSetBit(2, 0, 1); //切刀堵转
                Data.CutterStuckCount = 0;
//                Data.CutterStuckFlag = 0;
                LG->step = 5;
            }
            else
            {
                LG->step = 0xA0;
            }
        }
        break;
				
		case 0xB0:
			break;

    case 5:
        //切刀反转角度
        if(HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY  && GetFilmFeedSta() == 0)
        {
//					//走到停刀角度
//					if(GUS.ScaldFilmEn == 1)
//					{
//            LogicTask.CutterStopAngleData.execute = 1;
//					}
            LG->step = 6;
        }
        break;

    case 6:
        if(LogicTask.CutterStopAngleData.execute == 0)
        {
            LG->step = 7;
        }
        break;

    case 7:
        if(HZ_AxGetStatus(Axis_Cutter) == 0)
        {
            //包装循环停止
            FilmFeedStop(50);
            CamOut(&CamTable);
            GearEnd(&GearData);
//				PARAINIT(LogicTask.Package);
            PARAINIT(LogicTask.PruductCheck);
            Data.JogFlag = 0;
            Data.CylStopFlag = 0;
            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
            GUR.RunStatus = STOP;
            GUW.button.RunCommand = STOP;
						GUS.IsReset = 1;
        }
        break;
    }
}
/**
* @author： 2019/7/20 农业照
* @Description: 不定长函数调用
* @param –
* @param --
* @return --
*/
void ConMode_Logic()
{
    RunProcess2(&LogicTask.RunProcessPara);  //总调函数
    CamRun(Axis_FeedFilm, Axis_Cutter, &CamTable); //凸轮运行

    EmptyCheck(&LogicTask.EmptyCheck);
    CylStop(&LogicTask.CylStop);

    if(GUS.ColorMarkEn == 0)
    {
        CutPointCheck2(&LogicTask.ColorMarkCheck);
    }
    else
    {
        ColorMarkCheck2(&LogicTask.ColorMarkCheck);

    }
    PruductCheck2(&LogicTask.PruductCheck);
    Package2(&LogicTask.Package);

    CamCorrect2(Axis_FeedFilm, Axis_Cutter, &CamTable, &LogicTask.CamCorrectData);
    PackageJog2(&LogicTask.PackageJog);
    GearRun(&GearData);
    GUS.ScaldFilmEn = 0;	//防烫膜禁用

    GUS.MiddleOpenDelay = 1000;
    //当走纸轴停止后，中封延时打开
    if(ton(4, GUR.RunStatus != RUN, GUS.MiddleOpenDelay))
    {
        OutputSet(Q_MiddleClose	, ON); //打开中封
    }

    if(GUR.RunStatus == RUN)
    {
        //色标信号输出
        if(R_Trig(11, InputGet(I_ColorMark) == ON))
        {
            OutputSet(Q_MarkSign, ON); //色标信号输出
        }

        if(ton(8, OutputGet(Q_MarkSign) == ON, 100))
        {
            OutputSet(Q_MarkSign, OFF); //色标信号输出
        }
    }

}


