#include "FSM.h"
#include "USERTOOL.H"
/*!
* @file  FSM.c
* @brief ��̨״̬����ͨ������̬��ֹ̬ͣ����λ̬����̬ͣ����̬ͣ��״̬�����ƻ�̨��ȫ���С�
* @version 1.1 bsp.lib 1.1 int.lib
* @author  yfs
* @date 2018/08/29
*/

extern int SysRunDelay_3S;
extern GearInDataDef GearData;    //���ӳ���ҧ��

void Button()
{
    if(R_Trig(7, InputGet(I_StartBt) == ON) && GUR.HaveToReset == 0)
    {
        if(GUR.RunStatus == STOP)
        {
            GUW.button.RunCommand = RUN;
        }
    }

    if(InputGet(I_StartBt) == ON && GUR.HaveToReset == 1)
    {
        GUW.button.RunCommand = D_RESET;
    }

    if(InputGet(I_StopBt) == ON || GUS.SysPara.CylStopButton == 1)
    {
        if(GUR.RunStatus == RUN)
        {
//			GUW.button.RunCommand = STOP;
            Data.CylStopFlag = 1;
        }
        if(GSR.ErrorLevel > 1)
        {
            GSW.ClearAlarm = 1;
            OutPut_SetSta(Q_ServoAlmReset);
        }
        GUS.SysPara.CylStopButton = 0;
    }

    if(InputGet(I_EmergencyBt) == ON)
    {
//		GUW.button.RunCommand = SCRAM;
        GUW.button.RunCommand = STOP;
        AlarmSetBit(2, 0, 3); //��ͣ����
        if(GSR.ErrorLevel > 1)
        {
            GSW.ClearAlarm = 1;
            OutPut_SetSta(Q_ServoAlmReset);
        }
    }
    else
    {
        AlarmClearBit(2, 0, 3); //��ͣ����
    }


    if(R_Trig(10, InputGet(I_EmergencyBt) == OFF))
    {
        if(GUW.button.RunCommand == SCRAM)
        {
            GUW.button.RunCommand = STOP;
        }
    }

    if(ton(7, OutputGet(Q_ServoAlmReset) == ON, 1000))
    {
        OutputSet(Q_ServoAlmReset, OFF);
    }
}


//����ı���״̬
void AxisErro()
{
    u8 i;
    for(i = 0; i < PULS_NUM; i++)
    {
        if(AXSTA_ERRSTOP == HZ_AxGetStatus(i))
        {
//					//����λ
//					if( HZ_AxGetAxisErr(i) == AX_ERR_DW_LIM_H || AX_ERR_DW_LIM_S==HZ_AxGetAxisErr(i))//||(AX_ERR_DW_LIM_S&HZ_AxGetAxisErr(i))==1)
//					{
//							AlarmSetBit(3,0,i+1);
//					}
//					//����λ
//					if( AX_ERR_UP_LIM_H == HZ_AxGetAxisErr(i)|| AX_ERR_UP_LIM_S==HZ_AxGetAxisErr(i))
//					{
//							AlarmSetBit(3,1,i+1);
//					}

            if( AX_ERR_AX_ALM == HZ_AxGetAxisErr(i))
            {						
                AlarmSetBit(2, 0, 10 + i);
								if(SysRunDelay_3S)
								{
									GUS.IsReset = 0;
								}
            }
        }
    }
}

//״̬��
void FSM()
{
    u8 i;
    if(GSR.ErrorLevel > 0)//
    {
        if(GUR.RunStatus == RUN)
        {
            GUW.button.RunCommand = STOP;
        }
    }
    if(GSR.ErrorLevel > 3)//
    {
        GUR.HaveToRepower = 1;
    }
    else
    {
        if(GSR.ErrorLevel == 3)//
        {
            GUR.HaveToReset = 1;
        }
    }
    if(GUR.HaveToReset == 1)
    {
        AlarmSetBit(3, 0, 0); //
    }
    if(GUR.HaveToRepower == 1)
    {
        AlarmSetBit(4, 0, 0); //
    }
    switch(GUW.button.RunCommand)
    {
    case RUN:
        if(GUR.HaveToRepower == 0 && GUR.HaveToReset == 0 &&
                (GUR.RunStatus == STOP || GUR.RunStatus == PAUSE) && GSR.ErrorLevel < 2)
        {
            GUR.RunStatus = RUN;
            LogicTask.RunProcessPara.execute = 1;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    case STOP:
        if(GUR.RunStatus != INIT)
        {
            GUR.RunStatus = STOP;
            LogicTask.RunProcessPara.execute = 0;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;
    case PAUSE:
        if(GUR.RunStatus == RUN)
        {
            GUR.RunStatus = PAUSE;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;
    case D_RESET:
        if( (GUR.RunStatus != RUN) && (GUR.RunStatus != PAUSE) )
        {
            GUR.RunStatus = D_RESET;
            LogicTask.Reset.execute = 1;
        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    case SCRAM:
        GUR.RunStatus = SCRAM;
        break;

    case SIGSTEP:	//����
        if(GUR.HaveToRepower == 0 && GUR.HaveToReset == 0 &&
                (GUR.RunStatus == STOP || GUR.RunStatus == PAUSE) && GSR.ErrorLevel < 2)
        {
            GUR.RunStatus = SIGSTEP;

        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    case SIGRUN:    //��������
        if(GUR.HaveToRepower == 0 && GUR.HaveToReset == 0 &&
                (GUR.RunStatus == STOP || GUR.RunStatus == PAUSE) && GSR.ErrorLevel < 2)
        {
            GUR.RunStatus = SIGRUN;

        }
        else
        {
            GUW.button.RunCommand = GUR.RunStatus;
        }
        break;

    }


    switch(GUR.RunStatus)
    {
    case INIT:
        //��ʼִ̬�к���
        Logic();
        LedWork(RED);
        break;
    case RUN:
        //����ִ̬�к���
        Logic();
        LedWork(GREEN);
        break;
    case PAUSE:
        //��ִͣ�к���
        Logic();
        LedWork(YELLOW);
        break;
    case STOP:
        //ִֹͣ�к���
        Logic();
        LedWork(YELLOW);
        break;
    case D_RESET:
        //��λִ�к���
        Reset();
        LedWork(RED);
        break;
    case SCRAM:
        //��ͣ����
        for(i = 0; i < 5; i++)
        {
            HZ_AxStop(i);
        }
        PARAINIT(LogicTask);	//�����������
        CamOut(&CamTable);
        GearEnd(&GearData);
        LedWork(RED);

//        GUR.HaveToReset = 1;
        break;

    case SIGSTEP: //��������
        Logic();
        break;

    case SIGRUN:    //��������
        Logic();
        break;
    }

//����ʲô״̬��ʹ�õĺ���

//    //��λxyzƽ̨
//    XYZReset(&LogicTask.XYZRESETTask);
    //�������
    Button();
//    //��λ�ƶ��ӿ�
//    Move(GUW.AxisMove,LogicTask.AxisMoveTask);
    //�ᱨ������
    AxisErro();
    AuxLogic();
    //����������ٶ�
    EnCodGetSpeed();
}


