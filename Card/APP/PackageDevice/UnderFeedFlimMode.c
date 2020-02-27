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

#define T (*Table)
void CamCorrect3(u8 MainAxis, u8 SlaveAxis, CamTableDef *Table, LogicParaDef *LG)
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
            if(T.excute == 0)
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
                Data.EndPos =  PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, T.MainStartPos) + GUS.PackgeLength  -  ((int)GUS.SysPara.EmptySensorToCutterDis - GUS.ProductPos) % (int)GUS.PackgeLength;
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
            a2 = T.Ttime - abs(T.index - T.SlaveIndex);//
            if(T.index > T.SlaveIndex && a1 < a2)
            {
                MC_MovePP(SlaveAxis, RELMODE, 1000, 60, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, T.SlaveAxisPos[T.index] - (HZ_AxGetCurPos(SlaveAxis) - T.SlaveStartPos) % T.SlaveCirPulse); //
            }
            else if(T.index < T.SlaveIndex && a1 > a2)
            {
                MC_MovePP(SlaveAxis, RELMODE, 1000, 60, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, T.SlaveAxisPos[T.index] + T.SlaveCirPulse - (HZ_AxGetCurPos(SlaveAxis) - T.SlaveStartPos) % T.SlaveCirPulse); //Data.PackageSpd*GUS.AxisPara.FeedFilmAxisGearRate.PPR/GUS.AxisPara.FeedFilmAxisGearRate.MPR/60
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
float AverageFilter3(float value, u8 times)
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
* @author： 2019/7/19  nyz
* @Description:  色标跟踪
* @param --
* @param --
* @return --
*/

void ColorMarkCheck3(LogicParaDef *LG)
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
        if(InputGet(I_ColorMark) == ON || GetFilmFeedPosUU() - Data.MarkPosStart >= GUS.PackgeLength)
        {
//				//实际袋长
//				GUS.TestPara.PackageLTest = GetFilmFeedPosUU() - Data.MarkPosStart;
//				//获取走纸位置
//				Data.MarkPosStart = GetFilmFeedPosUU();


            //计算空包传感器和色标位置的偏差
            Data.MarkToEmtySenDif = (int)(GUS.SysPara.EmptySensorToCutterDis) % (int)(GUS.PackgeLength)  - ColorMarkPos ;
            if(Data.MarkToEmtySenDif >= 0)
            {
                if(Data.MarkToEmtySenDif <= GUS.ProductPos)
                    Data.EndPos = GetFilmFeedPosUU() + GUS.ProductPos - Data.MarkToEmtySenDif;
                else
                    Data.EndPos = GetFilmFeedPosUU() + GUS.PackgeLength + GUS.ProductPos - Data.MarkToEmtySenDif;
            }
            else
            {
                Data.EndPos = GetFilmFeedPosUU() + GUS.ProductPos + fabs(Data.MarkToEmtySenDif);
            }


            //如果凸轮在运行，过程中检测到偏差
            if(CamTable.excute == 1)
            {
                //色标偏差 = 包装袋长 - 色标位置 - 凸轮当前位置
                markPosDif = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, CamTable.MainCirPulse - CamTable.MainCurrPos % CamTable.MainCirPulse) - ColorMarkPos;
            }
            else
            {
                markPosDif = 0;
            }
            //求色标偏差的10次平均值
            Data.ColorMarkDif = AverageFilter3(markPosDif, 10);

            PackageDataArray[1] = PackageDataArray[0];
            PackageDataArray[0].StartPos = GetFilmFeedPosUU() +  ColorMarkPos - Data.ColorMarkDif;
            PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + GUS.PackgeLength;
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
* @author： 2019/7/8  nyz
* @Description:  检查物料是否在相应的位置送到位
* @param --
* @param --
* @return --
*/
void PruductCheckMark3(LogicParaDef *LG)
{
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
        FeedMove(1, Data.PackageSpd);
        LG->step = 2;
        TimerRst1(LG);
        break;

    case 2:
        if(ton(1, EmptySensor() == 1, 10))
        {
            Data.PosAdd = GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos;
            Data.StartPos = GSR.AxisUnitPos[Axis_FeedFilm];
            Data.EndPos = Data.EndPos + GUS.PackgeLength;
//					Data.ProductBuffCont++;
            LG->step = 3;
        }
        else
        {
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//停止走纸
            }

            //如果物料多长时间不到，则刀自动停到停刀角度，防止把膜烫坏
            if(TimerCnt1(LG) >= GUS.SysPara.ScaldFilmDelay && GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos && GUS.ScaldFilmEn == 1)
            {
                LG->step = 0xB0;
            }
        }
        break;

    case 0xB0: //切刀停刀停刀角度
        LogicTask.CutterStopAngleData.execute = 1;
        LG->step = 0xB1;
        break;

    case 0xB1:
        if(ton(1, EmptySensor() == 1, 10))
        {
            FeedStop();
        }

        if(LogicTask.CutterStopAngleData.execute == 0)
        {
            LG->step = 0xB2;
        }
        break;

    case 0xB2://检测到物料立马停止送料，然后把刀恢复会原来的位置
        if(ton(1, EmptySensor() == 1, 10))
        {
            FeedStop();
            LogicTask.CamCorrectData.execute = 1;
            LG->step = 0xB3;
        }
        break;

    case 0xB3:
        if(LogicTask.CamCorrectData.execute == 0 && GetFeedSta() == 0)
        {
            LG->step = 1;
        }
        break;

    case 3:
        if(GetFilmFeedSta() == 0 && (HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY))
        {
            FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        }

        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos  && GUS.ProductPosCheckEN == 1)
        {
            AlarmSetBit(2, 0, 4); //传感器一直有感应
            FeedStop();
            FilmFeedStop(50);
            break;         // YZ LG->step = 0xA0;
        }


        if(GSR.AxisUnitPos[Axis_FeedFilm] - Data.StartPos >= GUS.SysPara.ProductLength)
        {
            FeedStop();
        }

        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            LG->step = 4;
        }

        if(ton(2, EmptySensor() == 0, 10) && GSR.AxisUnitPos[Axis_FeedFilm] - Data.StartPos >= GUS.SysPara.ProductLength)
        {
            LG->step = 4;
        }
        break;


    case 0xA0:
        if(ton(2, EmptySensor() == 0, 10))
        {
            LG->step = 4;
        }
        break;

    case 4:
        if(ton(1, EmptySensor() == 1, 10))
        {
            if(GSR.AxisUnitPos[Axis_FeedFilm] < Data.EndPos)
            {
                FeedStop();
                LG->step = 5;
            }
            else
            {
                LG->step = 1;
            }
        }
        else
        {
            //如果经过一个物料位，光纤还未感应到，走纸停止
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//停止走纸
                LG->step = 1;
            }
        }
        break;

    case 5:
        FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            FeedMove(1, Data.PackageSpd);
            LG->step = 4;
        }
        break;
    }
}


/**
* @author： 2019/7/8  nyz
* @Description:  检查物料是否在相应的位置送到位
* @param --
* @param --
* @return --
*/
void PruductCheck3(LogicParaDef *LG)
{
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
        FeedMove(1, Data.PackageSpd);
        LG->step = 0xC0;
        TimerRst1(LG);
        break;

    case 0xC0:
        if(ton(2, EmptySensor() == 0, 10) )
        {
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//停止走纸
            }
        }

        if(ton(1, EmptySensor() == 1, 10))
        {
            Data.PosAdd = GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos;
            if(Data.PosAdd > GUS.PackgeLength / 2)
            {
                Data.PosAdd = 0;
            }
            Data.EndPos = GSR.AxisUnitPos[Axis_FeedFilm]  +  GUS.PackgeLength - Data.PosAdd;
            Data.StartPos = GSR.AxisUnitPos[Axis_FeedFilm];
            PackageDataArray[1] = PackageDataArray[0];
            PackageDataArray[0].StartPos =  ((int)GUS.SysPara.EmptySensorToCutterDis  - GUS.ProductPos) % (int)GUS.PackgeLength - Data.PosAdd;
            if(PackageDataArray[0].StartPos < 0)
            {
                PackageDataArray[0].StartPos += GUS.PackgeLength;
            }
            PackageDataArray[0].StartPos += GSR.AxisUnitPos[Axis_FeedFilm];
            PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + GUS.PackgeLength;
            Data.ProductBuffCont++;
            if(Data.ProductPosTerm != GUS.ProductPos)
            {
                Data.ProductPosIsChange = 1;
            }
            else
            {
                Data.ProductPosIsChange = 0;
            }
            Data.ProductPosTerm = GUS.ProductPos;
            LG->step = 2;
        }
        else
        {
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//停止走纸
            }

            //如果物料多长时间不到，则刀自动停到停刀角度，防止把膜烫坏
            if(TimerCnt1(LG) >= GUS.SysPara.ScaldFilmDelay && GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos && GUS.ScaldFilmEn == 1)
            {
                LG->step = 0xB0;
            }
        }
        break;

    case 0xB0: //切刀停刀停刀角度
        LogicTask.CutterStopAngleData.execute = 1;
        LG->step = 0xB1;
        break;

    case 0xB1:
        if(ton(1, EmptySensor() == 1, 10))
        {
            FeedStop();
        }

        if(LogicTask.CutterStopAngleData.execute == 0)
        {
            LG->step = 0xB2;
        }
        break;

    case 0xB2://检测到物料立马停止送料，然后把刀恢复会原来的位置
        if(ton(1, EmptySensor() == 1, 10))
        {
            FeedStop();
            LogicTask.CamCorrectData.execute = 1;
            LG->step = 0xB3;
        }

        break;

    case 0xB3:
        if(LogicTask.CamCorrectData.execute == 0 && GetFeedSta() == 0)
        {
            LG->step = 1;
        }
        break;


    case 2:
        if(GetFilmFeedSta() == 0)
        {
            FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        }



        //如果大于物料长度，还感应到物料，就停止皮带，避免那种物料连在一起的情况
        if(GSR.AxisUnitPos[Axis_FeedFilm] - Data.StartPos >= GUS.SysPara.ProductLength)
        {
            FeedStop();
        }

        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            LG->step = 3;
        }
        else if(ton(2, EmptySensor() == 0, 10) && GSR.AxisUnitPos[Axis_FeedFilm] - Data.StartPos >= GUS.SysPara.ProductLength)
        {
            LG->step = 3;
        }

//			else if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos )
//			{
//				if(Data.FirstPackageFlag == 1 && GUS.ProductPosCheckEN == 1)
//				{
//						AlarmSetBit(2,0,4);//物料错位
//						FeedStop();
//						FilmFeedStop(50);     					//停止走纸
//						LG->step = 0xA0;
//				}
//			}
        break;


    case 0xA0:
        if(ton(2, EmptySensor() == 0, 10))
        {
            LG->step = 3;
        }
        break;

    case 3:
        if(ton(1, EmptySensor() == 1, 10))
        {
            if(GSR.AxisUnitPos[Axis_FeedFilm] < Data.EndPos)
            {
                FeedStop();
                LG->step = 5;
            }
            else
            {
                LG->step = 1;
            }
        }
        else
        {
            //如果经过一个物料位，光纤还未感应到，走纸停止
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//停止走纸
                LG->step = 1;
            }
        }
        break;

    case 5:
        FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            FeedMove(1, Data.PackageSpd);
            LG->step = 3;
        }
        break;
    }
}



/**
* @author： 2019/05/17  nyz
* @Description:  包装
* @param --
* @param --
* @return --
*/
void Package3(LogicParaDef *LG)
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
        if(GUS.ColorMarkEn == 0)
        {
            LogicTask.PruductCheck.execute = 1;
            Data.FirstPackageFlag = 1;
            FeedMove(1, Data.PackageSpd);
            PARAINIT(LogicTask.ColorMarkCheck);
        }
        else
        {
            if(Data.FirstPackageFlag == 1)
            {
                LogicTask.PruductCheck.execute = 1;
            }
            LogicTask.ColorMarkCheck.execute = 1;
            FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
        }
        LG->step = 2;
        break;

    case 2:
        if(CamTable.excute == 0 && Data.ProductBuffCont > 0  && GSR.AxisUnitPos[Axis_FeedFilm] >= PackageDataArray[0].StartPos)
        {
            GUS.RealPackgeLength = PackageDataArray[0].EndPos - GSR.AxisUnitPos[Axis_FeedFilm];
            if(GUS.RealPackgeLength >=	(GUS.PackgeLength - 10))
            {
                CutCamCaculate(GUS.RealPackgeLength );
                LogicTask.CamDrawHMI.execute = 1;
                CamIn(&CamTable);
                LG->step = 3;
                LogicTask.PruductCheck.execute = 1;
            }
        }
        else if(CamTable.excute == 1)
        {
            LG->step = 3;
            CamIn(&CamTable);
        }

        if(GetFilmFeedSta() == 0 && GUR.RunStatus != RUN)
        {
            LG->step = 0;
            LG->execute = 0;
        }
        break;

    case 3:
        if(CamTable.excute == 0)
        {
            if(Data.FirstPackageFlag == 1)
            {
                if(Data.ProductBuffCont > 0)
                {
                    Data.ProductBuffCont--;
                }
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
        Data.CutterStuckCount = 0;
        //显示每个周期的切刀的跟随误差
        GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
        LG->step = 2;
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
void PackageJog3(LogicParaDef *LG)
{
    if(InputGet(I_JogBt) == ON && GUR.RunStatus != RUN)
    {
        LG->execute = 1;
    }
    else
    {
        LG->execute = 0;
    }

    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        Data.PackageSpd = GUS.JogSpeed * GUS.PackgeLength;
    }

    if(LG->execute == 0  && LG->step != 0)
    {
        FilmFeedStop(50);    //停止走纸
    }

    switch(LG->step)
    {
    case 1:
        //包装开始
        LogicTask.CamCorrectData.execute = 1;
        Data.CutterStuckFlag = 0;
        LG->step = 0xA0;
        break;

    case 0xA0:
        if(LogicTask.CamCorrectData.execute  == 0)
        {
            if(GUS.ColorMarkEn == 1)
            {
                LogicTask.ColorMarkCheck.execute = 1;
            }
            FilmFeedMove(1, Data.PackageSpd, 50); //开始走纸
            LG->step = 2;
        }
        break;

    case 2:
        if(GUS.ColorMarkEn == 0)
        {
            if(CamTable.excute == 0 )
            {
                GUS.RealPackgeLength = GUS.PackgeLength;
                CutCamCaculate(GUS.RealPackgeLength);
                LogicTask.CamDrawHMI.execute = 1;
                CamIn(&CamTable);
                LG->step = 3;
            }
            else if(CamTable.excute == 1)
            {
                LG->step = 3;
                CamIn(&CamTable);
            }
        }
        else
        {
            if(CamTable.excute == 0 && Data.ProductBuffCont > 0  && GSR.AxisUnitPos[Axis_FeedFilm] >= PackageDataArray[0].StartPos)
            {

                GUS.RealPackgeLength = PackageDataArray[0].EndPos - GSR.AxisUnitPos[Axis_FeedFilm];
//					  GUS.RealPackgeLength = GUS.PackgeLength;
                if(GUS.RealPackgeLength >= (GUS.PackgeLength - 10))
                {
                    CutCamCaculate(GUS.RealPackgeLength );
                    LogicTask.CamDrawHMI.execute = 1;
                    CamIn(&CamTable);
                    LG->step = 3;
                }
            }
            else if(CamTable.excute == 1)
            {
                LG->step = 3;
                CamIn(&CamTable);
            }
        }

        if(GetFilmFeedSta() == 0)
        {
            LG->step = 6;
        }
        break;

    case 3:
        if(CamTable.excute == 0)
        {
            if(Data.ProductBuffCont > 0)
            {
                Data.ProductBuffCont--;
            }

            //产量计数
            ProductCount();

            LG->step = 4;
        }

        if(GetFilmFeedSta() == 0)
        {
            LG->step = 6;
        }
        break;

    case 4:
        Data.CutterStuckCount = 0;
        //显示每个周期的切刀的跟随误差
        GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
        LG->step = 1;
        break;

    case 5:
        LG->step = 6;
        break;

    case 6:
        if(GetFilmFeedSta() == 0)
        {
            LG->step = 0;
            LG->execute = 0;
            CamOut(&CamTable);
        }
        break;
    }
}


/**
* @author： 2019/1/9 农业照
* @Description: 主调函数逻辑
* @param –
* @param --
* @return --
*/
void RunProcess3(LogicParaDef *LG)
{
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        Data.CutterStuckCount = 0;
        Data.PackageSpd = GUS.PackageSpeed * GUS.PackgeLength;
    }

    switch(LG->step)
    {
    case 1:
        //包装开始
        LogicTask.CamCorrectData.execute = 1;
        Data.CutterStuckFlag = 0;
        LG->step = 0xA0;
        break;

    case 0xA0:
        if(LogicTask.CamCorrectData.execute  == 0)
        {
            LogicTask.Package.execute = 1;
            LG->step = 2;
        }
        break;

    case 2:
        //判断是否有堵刀
        if(CutterStuckCheck() == 1)
        {
            Data.CutterStuckFlag = 1;
            Data.FeedStaTerm = GetFeedSta();
            //包装立马停止
            FeedStop();
            FilmFeedStop(50);
            CamOut(&CamTable);
//				HZ_AxStop(Axis_Cutter);
            LG->step = 3;
        }
        else if(LG->execute == 0)
        {
            LG->step = 5;
            //包装循环停止
            FeedStop();
            FilmFeedStop(50);
            CamOut(&CamTable);
//				PARAINIT(LogicTask.Package);
            PARAINIT(LogicTask.PruductCheck);
        }
        break;

    case 3:
        if(GetFilmFeedSta() == 0 && GetFeedSta() == 0 && HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY)
        {
            LogicTask.CutterErrRst.execute = 1;
            LG->step = 4;
        }
        break;

    case 4:
        if(LogicTask.CutterErrRst.execute == 0)
        {
            Data.CutterStuckCount++;
            if(Data.CutterStuckCount >= GUS.SysPara.CutterReversalTimes)
            {
                PARAINIT(LogicTask.Package);
                PARAINIT(LogicTask.PruductCheck);
                AlarmSetBit(2, 0, 1); //切刀堵转
                Data.CutterStuckCount = 0;
                Data.CutterStuckFlag = 0;
                LG->step = 5;
            }
            else
            {
                //如果停止前传送带是运行的，那么切刀恢复后继续运行
                if(Data.FeedStaTerm == 1)
                {
                    FeedMove(1, Data.PackageSpd);
                }
                CamIn(&CamTable);
                LG->step = 1;
            }
        }
        break;

    case 5:
        //切刀反转角度
        if(HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY  && GetFilmFeedSta() == 0 && LogicTask.CutterErrRst.execute == 0)
        {
            //走到停刀角度
            if(GUS.ScaldFilmEn == 1)
            {
                LogicTask.CutterStopAngleData.execute = 1;
            }
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
            FeedStop();
            FilmFeedStop(50);
            CamOut(&CamTable);
//				PARAINIT(LogicTask.Package);
            PARAINIT(LogicTask.PruductCheck);

            LG->execute = 0;
            LG->step = 0;
            LG->done = 1;
            GUR.RunStatus = STOP;
            GUW.button.RunCommand = STOP;
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
void UnderFeedFilmMode_Logic()
{
    RunProcess3(&LogicTask.RunProcessPara);  //总调函数

    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        CamRun(Axis_FeedFilm, Axis_Cutter, &CamTable); //凸轮运行
    }
    else
    {
        CamRun(FeedFilmEncode, Axis_Cutter, &CamTable); //凸轮运行
    }

    //判断切刀是否堵转
    if(Data.CutterStuckFlag == 0)
    {
        if(GUS.ColorMarkEn == 0)
        {
            PruductCheck3(&LogicTask.PruductCheck);
        }
        else
        {
            PruductCheckMark3(&LogicTask.PruductCheck);
        }
        Package3(&LogicTask.Package);
        ColorMarkCheck3(&LogicTask.ColorMarkCheck);
    }

    CamCorrect3(Axis_FeedFilm, Axis_Cutter, &CamTable, &LogicTask.CamCorrectData);

    PackageJog3(&LogicTask.PackageJog);


    GUS.MiddleOpenDelay = 1000;
    //当走纸轴停止后，中封延时打开
    if(ton(4, GetFilmFeedSta() == 0, GUS.MiddleOpenDelay))
    {
        OutputSet(Q_MiddleClose	, ON); //打开中封
    }

    if(GetFilmFeedSta() == 1 && (GUR.RunStatus == RUN || LogicTask.PackageJog.execute == 1))
    {
        OutputSet(Q_MiddleClose	, OFF); //合上中封
    }
}


