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
    //CamIn ͹��ҧ��
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
* @author�� 2019/7/25 NYZ
* @Description:  ��ֵ�˲�
* @param --     value ������ֵ
* @param --			times �˲�����
* @return --   ƽ��ֵ
*/
float AverageFilter3(float value, u8 times)
{
    s16 i;
    float sum;
    float avr;
    //��ɫ��λ��ƫ�
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
* @author�� 2019/7/19  nyz
* @Description:  ɫ�����
* @param --
* @param --
* @return --
*/

void ColorMarkCheck3(LogicParaDef *LG)
{
    float markPosDif;  //ɫ��ƫ��
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;
        LG->count = 0;
    }

    //����������
    if(LG->execute == 1)
    {
        ColorMarkLoseCheck();
    }

    switch(LG->step)
    {
    case 1:
        if(InputGet(I_ColorMark) == ON || GetFilmFeedPosUU() - Data.MarkPosStart >= GUS.PackgeLength)
        {
//				//ʵ�ʴ���
//				GUS.TestPara.PackageLTest = GetFilmFeedPosUU() - Data.MarkPosStart;
//				//��ȡ��ֽλ��
//				Data.MarkPosStart = GetFilmFeedPosUU();


            //����հ���������ɫ��λ�õ�ƫ��
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


            //���͹�������У������м�⵽ƫ��
            if(CamTable.excute == 1)
            {
                //ɫ��ƫ�� = ��װ���� - ɫ��λ�� - ͹�ֵ�ǰλ��
                markPosDif = PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, CamTable.MainCirPulse - CamTable.MainCurrPos % CamTable.MainCirPulse) - ColorMarkPos;
            }
            else
            {
                markPosDif = 0;
            }
            //��ɫ��ƫ���10��ƽ��ֵ
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
* @author�� 2019/7/8  nyz
* @Description:  ��������Ƿ�����Ӧ��λ���͵�λ
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
                FilmFeedStop(50);     					//ֹͣ��ֽ
            }

            //������϶೤ʱ�䲻�������Զ�ͣ��ͣ���Ƕȣ���ֹ��Ĥ�̻�
            if(TimerCnt1(LG) >= GUS.SysPara.ScaldFilmDelay && GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos && GUS.ScaldFilmEn == 1)
            {
                LG->step = 0xB0;
            }
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
            LG->step = 1;
        }
        break;

    case 3:
        if(GetFilmFeedSta() == 0 && (HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY))
        {
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        }

        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos  && GUS.ProductPosCheckEN == 1)
        {
            AlarmSetBit(2, 0, 4); //������һֱ�и�Ӧ
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
            //�������һ������λ�����˻�δ��Ӧ������ֹֽͣ
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//ֹͣ��ֽ
                LG->step = 1;
            }
        }
        break;

    case 5:
        FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            FeedMove(1, Data.PackageSpd);
            LG->step = 4;
        }
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
                FilmFeedStop(50);     					//ֹͣ��ֽ
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
                FilmFeedStop(50);     					//ֹͣ��ֽ
            }

            //������϶೤ʱ�䲻�������Զ�ͣ��ͣ���Ƕȣ���ֹ��Ĥ�̻�
            if(TimerCnt1(LG) >= GUS.SysPara.ScaldFilmDelay && GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos && GUS.ScaldFilmEn == 1)
            {
                LG->step = 0xB0;
            }
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
            LG->step = 1;
        }
        break;


    case 2:
        if(GetFilmFeedSta() == 0)
        {
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        }



        //����������ϳ��ȣ�����Ӧ�����ϣ���ֹͣƤ��������������������һ������
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
//						AlarmSetBit(2,0,4);//���ϴ�λ
//						FeedStop();
//						FilmFeedStop(50);     					//ֹͣ��ֽ
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
            //�������һ������λ�����˻�δ��Ӧ������ֹֽͣ
            if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
            {
                FilmFeedStop(50);     					//ֹͣ��ֽ
                LG->step = 1;
            }
        }
        break;

    case 5:
        FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
        if(GSR.AxisUnitPos[Axis_FeedFilm] >= Data.EndPos)
        {
            FeedMove(1, Data.PackageSpd);
            LG->step = 3;
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
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
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
                //��������
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
        //��ʾÿ�����ڵ��е��ĸ������
        GUW.CutterDif = PulseToUserUnit(&GUS.AxisPara.CutterAxisGearRate, CamTable.yCyDif);
        LG->step = 2;
        break;
    }
}





/**
* @author�� 2019/05/17  nyz
* @Description:  ��װ�㶯
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
        FilmFeedStop(50);    //ֹͣ��ֽ
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
            if(GUS.ColorMarkEn == 1)
            {
                LogicTask.ColorMarkCheck.execute = 1;
            }
            FilmFeedMove(1, Data.PackageSpd, 50); //��ʼ��ֽ
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

            //��������
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
        //��ʾÿ�����ڵ��е��ĸ������
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
* @author�� 2019/1/9 ũҵ��
* @Description: ���������߼�
* @param �C
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
            CamOut(&CamTable);
//				HZ_AxStop(Axis_Cutter);
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
        if(HZ_AxGetStatus(Axis_Cutter) == AXSTA_READY  && GetFilmFeedSta() == 0 && LogicTask.CutterErrRst.execute == 0)
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
* @author�� 2019/7/20 ũҵ��
* @Description: ��������������
* @param �C
* @param --
* @return --
*/
void UnderFeedFilmMode_Logic()
{
    RunProcess3(&LogicTask.RunProcessPara);  //�ܵ�����

    if(GUS.SysPara.FeedFilmAxisSel == 0)
    {
        CamRun(Axis_FeedFilm, Axis_Cutter, &CamTable); //͹������
    }
    else
    {
        CamRun(FeedFilmEncode, Axis_Cutter, &CamTable); //͹������
    }

    //�ж��е��Ƿ��ת
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


