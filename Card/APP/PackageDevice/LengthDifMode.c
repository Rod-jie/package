/*
 * File: Logic.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version: v00.00.01 12��ײ�
 * Description:�û��߼�
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

/*ȫ�ֱ�����ͳһ����*/

extern CamTableDef CamTable;  //����͹�ֱ�

#define T (*Table)
void CamCorrect1(u8 MainAxis, u8 SlaveAxis, CamTableDef *Table, LogicParaDef *LG)
{
    s32 a1, a2;
    u8 i;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    T.index = CamGetMainIndex(Table, MainAxis);
    switch(LG->step)
    {
    //CamIn ͹��ҧ��
    case 1:
        if(Data.ProductBuffCont > 0)
        {
            //�жϵ����ĸ�λ��
            if(GetFilmFeedPosUU() > PackageDataArray[0].StartPos && (GetFilmFeedPosUU() <= PackageDataArray[0].EndPos || (CamTable.TooLongFlag == 1 && CamTable.LongEndFlag == 0)))
            {

                LG->step = 0xA0;
                Data.EndPos = PackageDataArray[0].EndPos + GUS.ProductDistance - GUS.SysPara.EmptySensorToCutterDis - ((float)GUS.ProductDistance / 2 - GUS.CutPosAdjust);
                CutCamCaculate(fabs(PackageDataArray[0].EndPos - PackageDataArray[0].StartPos));
                T.MainStartPos = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackageDataArray[0].StartPos);
            }
            else
            {
                //͹�����ڽ���,�ѵ�1��������Ƴ�
                for(i = 0; i < BuffArrayNum; i++)
                {
                    if(i < Data.ProductBuffCont)
                    {
                        PackageDataArray[i] = PackageDataArray[i + 1];
                    }
                    else
                    {
                        PARAINIT(PackageDataArray[i]);
                    }
                }
                Data.ProductBuffCont--;
            }
        }
        else
        {
            LG->step = 4;
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
                MC_MovePP(SlaveAxis, RELMODE, 1000, 60, Data.PackageSpd * GUS.AxisPara.FeedFilmAxisGearRate.PPR / GUS.AxisPara.FeedFilmAxisGearRate.MPR / 60, T.SlaveAxisPos[T.index] + T.SlaveCirPulse - (HZ_AxGetCurPos(SlaveAxis) - T.SlaveStartPos) % T.SlaveCirPulse);
            }
            LG->step = 2;
        }
        break;

    case 2:
        if(HZ_AxGetStatus(SlaveAxis) == AXSTA_READY)
        {
            LG->step = 3;
        }
        break;




    case 3:
        CutCamCaculate(fabs(PackageDataArray[0].EndPos - PackageDataArray[0].StartPos));
        T.MainStartPos = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackageDataArray[0].StartPos);
        LogicTask.CamDrawHMI.execute = 1;
        CamIn(&CamTable);
        LG->step = 4;
        break;

    case 4:
        LG->execute = 0;
        LG->step = 0;
        break;
    }
}


/**
* @author�� 2019/7/8  nyz
* @Description:  ��������Ƿ�����Ӧ��λ���͵�λ
* @param --
* @param --
* @return --
*/
void PruductCheck1(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        ton_et[1] = 0;
    }
    //�е�΢��λ����С��λ��һ������ϼ��
    if(GUS.CutPosAdjust > GUS.ProductDistance / 2)
    {
        GUS.CutPosAdjust = GUS.ProductDistance / 2;
    }
    else if(GUS.CutPosAdjust < -GUS.ProductDistance / 2)
    {
        GUS.CutPosAdjust = -GUS.ProductDistance / 2;
    }


    switch(LG->step)
    {
    case 1:
        FeedMove(1, Data.PackageSpd);
        LG->step = 0xA0;
        TimerRst1(LG);
        break;

    case 0xA0:
        if(ton(1, EmptySensor() == 1, 10))
        {
            LG->step = 2;
            if(Data.ProductBuffCont == 0)
            {
                PackageDataArray[0].StartPos = GSR.AxisUnitPos[Axis_FeedFilm] - Data.PosAdd;
                PackageDataArray[0].EndPos = PackageDataArray[0].StartPos + fabs(GUS.SysPara.EmptySensorToCutterDis - ((float)GUS.ProductDistance / 2 + GUS.CutPosAdjust));
                PackageDataArray[0].l =  PackageDataArray[0].EndPos - PackageDataArray[0].StartPos;
                Data.ProductBuffCont++;
            }
            PackageDataArray[Data.ProductBuffCont].StartPos = GSR.AxisUnitPos[Axis_FeedFilm] + GUS.SysPara.EmptySensorToCutterDis - ((float)GUS.ProductDistance / 2 + GUS.CutPosAdjust) - Data.PosAdd;
        }

        //������϶೤ʱ�䲻�������Զ�ͣ��ͣ���Ƕȣ���ֹ��Ĥ�̻�
        if(TimerCnt1(LG) >= GUS.SysPara.ScaldFilmDelay && GUS.ScaldFilmEn == 1 && LogicTask.PackageJog.execute == 0)
        {
            LG->step = 0xB0;
        }
        break;

    case 0xB0: //�е�ͣ��ͣ���Ƕ�
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

    case 0xB2://��⵽��������ֹͣ���ϣ�Ȼ��ѵ��ָ���ԭ����λ��
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
            LG->step = 0xB4;
            TimerRst1(LG);
        }
        break;

    case 0xB4:
        if(TimerCnt1(LG) > 100)
        {
            LG->step = 1;
        }
        break;

    case 2:
        if(Data.ProductBuffCont == 0 && CamTable.excute == 0)
        {
            CamTable.TooLongFlag = 1;
            CamTable.LongEndFlag = 0;
            CutCamCaculate(PackageDataArray[0].l);
            LogicTask.CamDrawHMI.execute = 1;
            CamIn(&CamTable);
        }

        if(GetFilmFeedSta() == 0)
        {
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        }

        if(ton(1, EmptySensor() == 0, 10))
        {
            PackageDataArray[Data.ProductBuffCont].EndPos = GSR.AxisUnitPos[Axis_FeedFilm] + GUS.SysPara.EmptySensorToCutterDis + ((float)GUS.ProductDistance / 2 - GUS.CutPosAdjust) - Data.PosAdd;
            PackageDataArray[Data.ProductBuffCont].l = PackageDataArray[Data.ProductBuffCont].EndPos -  PackageDataArray[Data.ProductBuffCont].StartPos;
            if(PackageDataArray[Data.ProductBuffCont].l > GUS.SysPara.MinDifPackageLength)
            {
                if(CamTable.TooLongFlag == 1 && Data.ProductBuffCont == 0)
                {
                    CamTable.LongStopPos = UserUnitToPulse(&GUS.AxisPara.FeedFilmAxisGearRate, PackageDataArray[Data.ProductBuffCont].EndPos);
                    CamTable.LongEndFlag = 1;
                    GUR.LengthArr[1] = GUR.LengthArr[0];
                    GUS.RealPackgeLength = PackageDataArray[Data.ProductBuffCont].l;
                    GUR.LengthArr[0] = GUS.RealPackgeLength - GUS.ProductDistance;
                }
                Data.EndPos = GSR.AxisUnitPos[Axis_FeedFilm] + GUS.ProductDistance - Data.PosAdd;
                Data.ProductBuffCont++;
                LG->step = 3;
            }
            else
            {
                LG->step = 0xD0;
            }
        }
        break;

    case 0xD0:
        PackageDataArray[Data.ProductBuffCont].EndPos = GSR.AxisUnitPos[Axis_FeedFilm] + GUS.SysPara.EmptySensorToCutterDis + ((float)GUS.ProductDistance / 2 - GUS.CutPosAdjust);
        PackageDataArray[Data.ProductBuffCont].l = PackageDataArray[Data.ProductBuffCont].EndPos -  PackageDataArray[Data.ProductBuffCont].StartPos;
        if(PackageDataArray[Data.ProductBuffCont].l > GUS.SysPara.MinDifPackageLength)
        {
            Data.EndPos = GSR.AxisUnitPos[Axis_FeedFilm] + GUS.ProductDistance  - Data.PosAdd;
            Data.ProductBuffCont++;
            LG->step = 0xD1;
        }

        if(ton(1, EmptySensor() == 1, 10))
        {
            FeedStop();
        }

        break;

    case 0xD1:
        LG->step = 3;
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
            //�������һ������λ�����˻�δ��Ӧ������ֹֽͣ
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                if(LogicTask.PackageJog.execute == 0)
                {
                    FilmFeedStop(50);     					//ֹͣ��ֽ
                }
                LG->step = 0xE0;
            }
        }
        break;

    case  0xE0:
        if(GetFilmFeedSta() == 0 || LogicTask.PackageJog.execute == 1)
        {
            Data.PosAdd = GSR.AxisUnitPos[Axis_FeedFilm] - Data.EndPos;
            LG->step = 1;
        }
        break;

    case 5:
        FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            if(GetFeedSta() == 0)
            {
                FeedMove(1, Data.PackageSpd);
                LG->step = 3;
            }
        }
        break;
    }
}


/**
* @author�� 2019/05/17  nyz
* @Description:  ��װ
* @param --
* @param --
* @return --
*/
void Package1(LogicParaDef *LG)
{
    u8 i;
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;

    }

    switch(LG->step)
    {
    case 1:
        FeedMove(1, Data.PackageSpd);
        LogicTask.PruductCheck.execute = 1;
        LG->step = 2;
        break;

    case 2:
        if(CamTable.excute == 0)
        {
            if(Data.ProductBuffCont > 0 && GSR.AxisUnitPos[Axis_FeedFilm] >= PackageDataArray[0].StartPos)
            {
                GUR.LengthArr[1] = GUR.LengthArr[0];
                GUS.RealPackgeLength = PackageDataArray[0].l;
                GUR.LengthArr[0] = GUS.RealPackgeLength - GUS.ProductDistance;
                CutCamCaculate(fabs(PackageDataArray[0].EndPos - GSR.AxisUnitPos[Axis_FeedFilm]));
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
        break;

    case 3:
        if(CamTable.excute == 0)
        {
            LG->step = 4;

            //͹�����ڽ���,�ѵڸ�������Ƴ�
            for(i = 0; i < BuffArrayNum; i++)
            {
                if(i < Data.ProductBuffCont)
                {
                    PackageDataArray[i] = PackageDataArray[i + 1];
                }
                else
                {
                    PARAINIT(PackageDataArray[i]);
                }
            }
            Data.ProductBuffCont--;

            if(Data.FirstPackageFlag == 1)
            {
                //��������
                ProductCount();
            }
            Data.FirstPackageFlag = 1;
        }
        break;

    case 4:
        //��ʾÿ�����ڵ��е��ĸ������
        GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
        LG->step = 2;
        break;
    }
}


/**
* @author�� 2019/05/17  nyz
* @Description:  ��װ
* @param --
* @param --
* @return --
*/
void PackageJog1(LogicParaDef *LG)
{
    u8 i;
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
        Data.PackageSpd =  GUS.JogSpeed * GUS.PackgeLength;
    }

    if(LG->execute == 0 && LG->step < 5 && LG->step != 0)
    {
        LG->step = 5;
    }


    switch(LG->step)
    {
    case 1:
        LogicTask.CamCorrectData.execute = 1;
        LG->step = 0xA0;
        break;

    case 0xA0:
        if(LogicTask.CamCorrectData.execute == 0)
        {
            LogicTask.PruductCheck.execute = 1;
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
            LG->step = 2;
        }
        break;

    case 2:
        if(CamTable.excute == 0)
        {
            if(Data.ProductBuffCont > 0 && GSR.AxisUnitPos[Axis_FeedFilm] >= PackageDataArray[0].StartPos)
            {
                GUR.LengthArr[1] = GUR.LengthArr[0];
                GUS.RealPackgeLength = PackageDataArray[0].l;
                GUR.LengthArr[0] = GUS.RealPackgeLength - GUS.ProductDistance;
                CutCamCaculate(fabs(PackageDataArray[0].EndPos - GSR.AxisUnitPos[Axis_FeedFilm]));
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

        break;

    case 3:
        if(CamTable.excute == 0 && HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY)
        {
            LG->step = 4;
            //͹�����ڽ���,�ѵڸ�������Ƴ�
            for(i = 0; i < BuffArrayNum; i++)
            {
                if(i < Data.ProductBuffCont)
                {
                    PackageDataArray[i] = PackageDataArray[i + 1];
                }
                else
                {
                    PARAINIT(PackageDataArray[i]);
                }
            }
            Data.ProductBuffCont--;
        }
        break;

    case 4:
        //��������
        if(HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY)
        {
            if(Data.ProductBuffCont == 0)
            {
                MC_MoveUU(Axis_Cutter, RELMODE, 500, 100, Data.PackageSpd * GUS.AxisPara.CutterAxisGearRate.PPR / GUS.AxisPara.CutterAxisGearRate.MPR / 60, (float)GUS.CutterStopAngle);
            }
            ProductCount();
            //��ʾÿ�����ڵ��е��ĸ������
            GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
            LG->step = 2;
        }
        break;

    case 5:
        FilmFeedStop(50);   //ֹͣ��ֽ
        FeedStop();
        LG->step = 6;
        break;

    case 6:
        if(GetFilmFeedSta() == 0)
        {
            CamOut(&CamTable);
            PARAINIT(LogicTask.PruductCheck);
            LG->step = 0;
            LG->execute = 0;
        }
        break;
    }
}


/**
* @author�� 2019/1/9 ũҵ��
* @Description: ���������߼�
* @param �C
* @param --
* @return --
*/
void RunProcess1(LogicParaDef *LG)
{
    if(LG->step == 0 && LG->execute == 1)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
        Data.PackageSpd = GUS.PackageSpeedMPS * 1000;
    }

    switch(LG->step)
    {
    case 1:
        //��װ��ʼ
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
        //�ж��Ƿ��жµ�
        if(CutterStuckCheck() == 1)
        {
            Data.CutterStuckFlag = 1;
            Data.FeedStaTerm = GetFeedSta();
            //��װ����ֹͣ
            FeedStop();
            FilmFeedStop(50);
            HZ_AxStop(Axis_Cutter);
            CamOut(&CamTable);
            LG->step = 3;
        }
        else if(LG->execute == 0)
        {
            LG->step = 5;
            //��װѭ��ֹͣ
            FeedStop();
            FilmFeedStop(50);
            CamOut(&CamTable);
//				PARAINIT(LogicTask.Package);
            PARAINIT(LogicTask.PruductCheck);
        }
        break;

    case 3:
        if(LogicTask.PackageCamStop.execute == 0)
        {
            //�е���ת�Ƕ�
//				MC_MovePP(Axis_Cutter,RELMODE,1000,60,5000,-(int)(GUS.SysPara.CutterReversalAngle/360.0*GUS.AxisPara.CutterAxisGearRate.PPR));
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
                AlarmSetBit(2, 0, 1); //�е���ת
                Data.CutterStuckCount = 0;
                Data.CutterStuckFlag = 0;
                LG->step = 5;
            }
            else
            {
                //���ֹͣǰ���ʹ������еģ���ô�е��ָ����������
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
        //�е���ת�Ƕ�
        if(HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY && GetFilmFeedSta() == 0 && LogicTask.CutterErrRst.execute == 0)
        {
            //�ߵ�ͣ���Ƕ�
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
            //��װѭ��ֹͣ
            FeedStop();
            FilmFeedStop(50);
            CamOut(&CamTable);
            PARAINIT(LogicTask.Package);
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
* @author�� 2019/7/20 ũҵ��
* @Description: ��������������
* @param �C
* @param --
* @return --
*/
void LengthDifMode_Logic()
{
    RunProcess1(&LogicTask.RunProcessPara);  //�ܵ�����
    Package1(&LogicTask.Package);
    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        CamRun(Axis_FeedFilm, Axis_Cutter, &CamTable); //͹������
    }
    else
    {
        CamRun(FeedFilmEncode, Axis_Cutter, &CamTable); //͹������
    }
    PackageJog1(&LogicTask.PackageJog);
    PruductCheck1(&LogicTask.PruductCheck);
    CamCorrect1(Axis_FeedFilm, Axis_Cutter, &CamTable, &LogicTask.CamCorrectData);


    GUS.MiddleOpenDelay = 1000;
    //����ֽ��ֹͣ���з���ʱ��
    if(ton(4, GetFilmFeedSta() == 0, GUS.MiddleOpenDelay))
    {
        OutputSet(Q_MiddleClose	, ON); //���з�
    }

    if(GetFilmFeedSta() == 1 && (GUR.RunStatus == RUN || LogicTask.PackageJog.execute == 1))
    {
        OutputSet(Q_MiddleClose	, OFF); //�����з�
    }
}


