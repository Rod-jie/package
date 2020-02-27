/*
 * File: LogicTerm.c
 * File Created: 2019.5.7
 * version: v00.00.01 12��ײ�
 * Description:�߼����飬��Ҫ���ڸ����߼��Ĳ�����Ҫһֱ���еĺ�������������
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


extern CamTableDef CamTable;  //����͹�ֱ�
extern GearInDataDef GearData;    //���ӳ���ҧ��

/**
* @author�� 2019/1/9 ũҵ��
* @Description: ��������ť����
* @param �C
* @param --
* @return --
*/

void HMI_Button()
{
//	u8 i;
    //ֹͣ��ť���ظ�״̬
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

//	//�ŷ��������
//	if(GSW.ClearAlarm == 1)
//	{
//		for(i=0;i<5;i++)
//		{
//			EN_SetSta(i,ON);//
//			HZ_AxReset(i);
//		}
//	}
//
//	//�ŷ��������
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
    //�����������ƻ����л�
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
* @author�� 2019/10/22 nyz
* @Description:  ɫ���Ӧ��Ӧ�������
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
* @author�� 2019/5/22 nyz
* @Description:  �������ֶ���ť����
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
    case 1://������0
        GUS.Product.CurrNum = 0;
        break;

    case 2://b��װ�ٶ�--
        if(GUS.PackageSpeed > 0)
            GUS.PackageSpeed--;
        break;

    case 3://��װ�ٶ�++
        GUS.PackageSpeed++;
        break;

//		case 4://ɫ��λ��--
//			if(ColorMarkPos>1)
//			{
//				GUS.colorMarkPos--;
//			}
//			break;
//
//		case 5://ɫ��λ��++
//			if(ColorMarkPos< (u32)GUS.PackgeLength - 1)
//			{
//				GUS.colorMarkPos++;
//			}
//			break;
//
//		case 6://����λ��--
//			if(GUS.ProductPos>0)
//			{
//				GUS.ProductPos--;
//			}
//			break;
//
//		case 7://����λ��++
//			if(GUS.ProductPos< (u32)GUS.PackgeLength)
//			{
//				GUS.ProductPos++;
//			}
//			break;

    case 8://��������
        if(LogicTask.PackageLengthTest.execute == 0)
        {
            LogicTask.PackageLengthTest.execute = 1;
        }
        else
        {
            LogicTask.PackageLengthTest.execute = 0;
        }
        break;

    case 9://��װ����λ
        LogicTask.Reset.execute = 1;
        GUW.button.RunCommand = D_RESET;
        break;

    case 10://�������
        GSW.ClearAlarm = 1;
        OutputSet(Q_ServoAlmReset, ON);
        break;

    case 11://�����е�ÿȦ������
        if(LogicTask.CutterCirPulseTest.execute == 0)
        {
            LogicTask.CutterCirPulseTest.execute = 1;
        }
        else
        {
            LogicTask.CutterCirPulseTest.execute = 0;
        }
        break;

    case 12://������ֽÿȦ������
        if(LogicTask.PackageLengthTest.execute == 0)
        {
            LogicTask.PackageLengthTest.execute = 2;
        }
        else
        {
            LogicTask.PackageLengthTest.execute = 0;
        }
        break;

    case 13://��������ÿȦ������
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

    case 20://�е��ߵ�ͣ��λ��
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
* @author�� 2019/2/18 ׯ��
* @Description: ������ʾ
* @param �C
* @param --
* @return --
*/

void Alarm()
{
    //������
    if(ton(6, GUS.NoProductDelay != 0 && GUS.SysPara.NoMaterialStop == 0 && GUS.SysPara.DeviceMode != 0 && GUR.RunStatus == RUN && EmptySensor() == 0, GUS.NoProductDelay))
    {
        AlarmSetBit(2, 0, 7); //��������ͣ
    }
}



/**
* @author�� 2019/2/18 ׯ��
* @Description: IO���
* @param �C
* @param --
* @return --
*/
void IOCheck()
{

}


/**
* @author�� 2019/05/17  nyz
* @Description:  ��ֽ����  MM/����
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
* @author�� 2019/5/23 NYZ
* @Description: ��ֹֽͣ
* @param �C
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
            HZ_AxStopDec(Axis_FeedFilm);	//�ٶȹ��ͣ��ᵼ�µײ�Դ����ѭ������������
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
* @author�� 2019/5/23 NYZ
* @Description: ��ֹֽͣ
* @param �C
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
* @author�� 2019/5/23 NYZ
* @Description: ��ȡ��ֽ��״̬
* @param �C
* @param --
* @return -- 0��ֹͣ  1��������
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
* @author�� 2019/5/23 NYZ
* @Description: ��ȡ��ǰ��ֽ��λ��,����
* @param �C
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
* @author�� 2019/7/19 NYZ
* @Description: ��ȡ��ǰ��ֽ��λ��,MM
* @param �C
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
* @author�� 2019/05/23  nyz
* @Description:  ��������
* @param --
* @param --
* @return --
*/
void FeedMove(s32 dir, float packageSpd)
{
    u32 speed;
    static u32 speedTerm;
    //���Ͻ���
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
* @author�� 2019/5/23 NYZ
* @Description: ����ֹͣ
* @param �C
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
* @author�� 2019/5/23 NYZ
* @Description: ��ȡ��ǰ��ֽ��λ��,����
* @param �C
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
* @author�� 2019/5/23 NYZ
* @Description: ��ȡ��ǰ��ֽ��λ��,MM
* @param �C
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
* @author�� 2019/5/23 NYZ
* @Description: ��ȡ������״̬
* @param �C
* @param --
* @return -- 0��ֹͣ  1��������
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
* @author�� 2019/5/23 NYZ
* @Description: ���Դ���
* @param �C
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
            //���Դ���
            if(LG->execute == 1)
            {
                GUS.TestPara.PackageLTest = (u32)PulseToUserUnit(&GUS.AxisPara.FeedFilmAxisGearRate, (GetFilmFeedPos() - length));
            }

            //����ÿȦ������
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
* @author�� 2019/5/23 NYZ
* @Description: �е��������
* @param �C
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
* @author�� 2019/5/23 NYZ
* @Description: �����������
* @param �C
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
            //����ÿȦ������
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
    //��ֽ�㶯
    if(WordToBit(GUW.button.BitControlWord, FilmFeedJogPos))
    {
        CamEnd(&CamTable);//��ֽ�㶯ǰҪ�Ƚ���͹��
        FilmFeedMove(1, GUS.JogSpeed * GUS.PackgeLength, 50);
        if(GUS.SysPara.JogMiddlePackageSwitch == 0)
        {
            OutputSet(Q_MiddleClose	, OFF); //�����з�
        }
    }
    else if(WordToBit(GUW.button.BitControlWord, FilmFeedJogNeg))
    {
        CamEnd(&CamTable);//��ֽ�㶯ǰҪ�Ƚ���͹��
        FilmFeedMove(-1, GUS.JogSpeed * GUS.PackgeLength, 50);
        if(GUS.SysPara.JogMiddlePackageSwitch == 0)
        {
            OutputSet(Q_MiddleClose	, OFF); //�����з�
        }
    }


    if(R_Trig(1, WordToBit(GUW.button.BitControlWord, FilmFeedJogPos) == 0 && WordToBit(GUW.button.BitControlWord, FilmFeedJogNeg) == 0))
    {
        FilmFeedStop(50);
    }

    //���ϵ㶯
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

    //�е��㶯
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
* @author�� 2019/5/23 NYZ
* @Description: ��װλ�ø���
* @param �C
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

    //λ���ݴ棬ȷ���ϵ㲻��Ҫ��ԭ��
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
* @author�� 2019/7/11 NYZ
* @Description: ��������͹������
* @param �C
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
* @author�� 2019/05/22  nyz
* @Description:  ��ֽ����ɫ��λ��
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
* @author�� 2019/05/22  nyz
* @Description:  �е��Ḵλ
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
        //�ж��е��Ƿ��ת
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
* @author�� 2019/05/22  nyz
* @Description:  �����Ḵλ
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
* @author�� 2019/05/22  nyz
* @Description:  �е����� ��λ,����ͣ���е��Ƕ�
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
    //�����ŷ��Զ���λ
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
* @author�� 2019/07/26  nyz
* @Description:  ����
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
            //�ж��Ƿ���Ҫ����
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
* @author�� 2019/07/26  nyz
* @Description:  ����
* @param --
* @param --
* @return --
*/
void CodePrint(LogicParaDef *LG)
{
    //���������ã�����͹�����������������������
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
* @author�� 2019/07/26  nyz
* @Description:  ��ƾ�
* @param --
* @param --
* @return --
*/
void SprayWater(LogicParaDef *LG)
{
    //���������ã�����͹�����������������������
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
* @author�� 2019/07/26  nyz
* @Description:  �е�ֹͣ�Ƕȣ����ݵ�ǰ����λ�ã�ȷ����Ӧ�����Ǳ�ͣ���������е��պϽǶ� �� ͣ���Ƕ����ж�
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
            //����ǰ����λ��С�ڱպϽǶ�ʱ����������ͣ��
            if(cutterAngle < 30)
            {
                MC_MoveUU(Axis_Cutter, RELMODE, 1000, 100, speed, -cutterAngle - GUS.CutterStopAngle);
            }
            else if(cutterAngle >= 30 && cutterAngle < GUS.CutterStopAngle)
            {
                //������λ�ô��ڱպϽǶȣ���С��ͣ���Ƕ�ʱ����������ͣ��
                MC_MoveUU(Axis_Cutter, RELMODE, 1000, 100, speed, GUS.CutterStopAngle - GUR.SlaveAxPosUU);
            }
            else if(cutterAngle > (360 / GUS.SysPara.CutterNum - GUS.CutterStopAngle))
            {
                //������λ�ô��ڵ�360 - ͣ���Ƕ�ʱ����������ͣ��
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
* @author�� 2019/07/26  nyz
* @Description:  �հ���������Ӧ
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
* @author�� 2019/07/26  nyz
* @Description:  ��������
* @param --
* @param --
* @return --
*/
void ProductCount()
{
    //��������
    GUS.Product.CurrNum++;
    if(GUS.Product.StopEn == 0)
    {
        if(GUS.Product.CurrNum >= GUS.Product.SetNum)
        {
            AlarmSetBit(2, 0, 2); //��������
        }
    }
}


/**
* @author�� 2019/07/26  nyz
* @Description:  ����������
* @param --
* @param --
* @return --
*/
void ColorMarkLoseCheck()
{
    static u8 count;
    if(R_Trig(4, InputGet(I_ColorMark) == ON))
    {
        //ʵ�ʴ���
        GUS.TestPara.PackageLTest = GetFilmFeedPosUU() - Data.MarkPosStart;
        //��ȡ��ֽλ��
        Data.MarkPosStart = GetFilmFeedPosUU();
        if(abs(GUS.TestPara.PackageLTest - GUS.PackgeLength) > 5)
        {
            count++;
            if(count >= 3)
            {
                count = 0;
                AlarmSetBit(2, 0, 6); //��װ�������ô���
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
        //���������ж�
        if(GUS.SysPara.LoseColorMarkTimes != 0)
        {
            AlarmSetBit(2, 0, 5); //��������
        }
    }
}

/**
* @author�� 2019/08/2  nyz
* @Description:  ָʾ��
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
* @author�� 2019/08/2  nyz
* @Description:  ���ݱ���
* @param --
* @param --
* @return --
*/


void ParaProtect()
{
    //��װ�ٶȱ���
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
//	//��װ�ٶȱ���
//	if(GUS.JogSpeed > GUS.SysPara.MaxPackageSpeed)
//	{
//		GUS.JogSpeed = GUS.SysPara.MaxPackageSpeed;
//	}
//	else if(GUS.JogSpeed < GUS.SysPara.MinPackageSpeed)
//	{
//		GUS.JogSpeed = GUS.SysPara.MinPackageSpeed;
//	}



//	//��װ���ȱ���
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
* @author�� 2019/5/7 NYZ
* @Description: ������������Ҫ�����ڸ��߼��޹�����Ҫһֱ���еĺ���������������
* @param �C
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

    //���ϵ������ֽ�����Ĭ��Ϊ�ŷ�
    GUS.SysPara.FeedFilmAxisSel = 0;
    GUS.SysPara.FeedMotorSel = 0;

    FeedFilmReset(&LogicTask.FeedFilmReset);
    CutterReset(&LogicTask.CutterReset);
    FeedReset(&LogicTask.FeedReset);
    CutteErrorRst(&LogicTask.CutterErrRst);
}

