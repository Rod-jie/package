#include "FSM.h"
#include "USERTOOL.H"
/*!
* @file  FSM.c
* @brief 机台状态机，通过运行态，停止态，复位态，急停态，暂停态等状态，控制机台安全运行。
* @version 1.1 bsp.lib 1.1 int.lib
* @author  yfs
* @date 2018/08/29
*/

extern int SysRunDelay_3S;
extern GearInDataDef GearData;    //电子齿轮咬合

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
        AlarmSetBit(2, 0, 3); //急停按下
        if(GSR.ErrorLevel > 1)
        {
            GSW.ClearAlarm = 1;
            OutPut_SetSta(Q_ServoAlmReset);
        }
    }
    else
    {
        AlarmClearBit(2, 0, 3); //急停按下
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


//读轴的报警状态
void AxisErro()
{
    u8 i;
    for(i = 0; i < PULS_NUM; i++)
    {
        if(AXSTA_ERRSTOP == HZ_AxGetStatus(i))
        {
//					//下限位
//					if( HZ_AxGetAxisErr(i) == AX_ERR_DW_LIM_H || AX_ERR_DW_LIM_S==HZ_AxGetAxisErr(i))//||(AX_ERR_DW_LIM_S&HZ_AxGetAxisErr(i))==1)
//					{
//							AlarmSetBit(3,0,i+1);
//					}
//					//上限位
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

//状态机
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

    case SIGSTEP:	//单步
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

    case SIGRUN:    //单次运行
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
        //初始态执行函数
        Logic();
        LedWork(RED);
        break;
    case RUN:
        //运行态执行函数
        Logic();
        LedWork(GREEN);
        break;
    case PAUSE:
        //暂停执行函数
        Logic();
        LedWork(YELLOW);
        break;
    case STOP:
        //停止执行函数
        Logic();
        LedWork(YELLOW);
        break;
    case D_RESET:
        //复位执行函数
        Reset();
        LedWork(RED);
        break;
    case SCRAM:
        //急停处理
        for(i = 0; i < 5; i++)
        {
            HZ_AxStop(i);
        }
        PARAINIT(LogicTask);	//所有流程清除
        CamOut(&CamTable);
        GearEnd(&GearData);
        LedWork(RED);

//        GUR.HaveToReset = 1;
        break;

    case SIGSTEP: //单步运行
        Logic();
        break;

    case SIGRUN:    //单次运行
        Logic();
        break;
    }

//无论什么状态都使用的函数

//    //复位xyz平台
//    XYZReset(&LogicTask.XYZRESETTask);
    //按键检测
    Button();
//    //上位移动接口
//    Move(GUW.AxisMove,LogicTask.AxisMoveTask);
    //轴报警处理
    AxisErro();
    AuxLogic();
    //计算编码器速度
    EnCodGetSpeed();
}


