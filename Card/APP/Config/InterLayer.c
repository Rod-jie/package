#include "InterLayer.h"


void Axis_pos(void);

//每次轮询的接口函数
void InterLayer()
{
    //获取rtc时间
    RTC_Get_Time(&GSR.time.hour, &GSR.time.min, &GSR.time.sec, &GSR.time.ampm);	
		//每次更新轴参数
		AxisConfig(GSS.AxisData,GSS.AxIOconfig);
    //底层轴动流程
    HZ_AxMotion();
    //modbus通讯流程
    HZ_ModBus(uart_232,1);
		HZ_ModBus(uart_485,1);
    HZ_ModBus(eth,1);
    //软件定时器
    HZ_TimerThread();
    //配合地址表中的flash操作
    HZ_FlashOperate();
    //底层点动函数
		HZ_JogOperate(5);
    //地址表中的报警函数
    HZ_Alarm();
    //获取轴当前状态
    Axis_pos();
    //状态机
    FSM();
}
/**
* @author  yang
* @Description: 获取轴当前状态
* @param -- 用户自己定义内部需要获取的内容
* @param --
* @return --
*/
void Axis_pos()
{
	int i;
	for(i=0; i<PULS_NUM; i++)
	{
		GSR.AxisPosition[i]= HZ_AxGetCurPos(i);
		GSR.AXSTA[i] = HZ_AxGetStatus(i);
		GSR.AxisUnitPos[i] = PulseToUserUnit(&GSS.Conversion[i],GSR.AxisPosition[i]); 
	}
	
	GSR.AxisUnitPos[Axis_Cutter] = PulseToUserUnit(&GSS.Conversion[Axis_Cutter],GSR.AxisPosition[Axis_Cutter]%GSS.Conversion[Axis_Cutter].PPR); 

	for(i=0;i<4;i++)
	{
		GSR.AxisEncoder[i] = EnCode_Get32(i);
		Encode[i].CurrPos = EncodeGetPos(i);
	}
//    for(i = 0; i < 4; i++)
//    {
////        GSR.AxisEncoder[i] = EnCode_Get32(i);
//    }
//    for(i = 0; i<PULS_NUM; i++)
//    {
//        
//    }
//		
//	for(i=0;i<5;i++)
//	{
//		
//	}
}

/**
* @author  yang
* @Description: 自带点动调用函数。当执行点动时调用
* @param --
* @param --
* @return --
*/
void JogGo(u8 axisnum,s32 pos,u8 spd)
{
    SetAxisSpdRadio(axisnum,spd);
    if(axisnum<PULS_NUM)	//只有主卡部分点动函数
    {
        if(pos > 0)	//正向点动
        {
            if(AXSTA_ERRSTOP == HZ_AxGetStatus(axisnum))
            {
                //只有下限错误
                if(0 == (0x0fff & HZ_AxGetAxisErr(axisnum)))
                {
                    HZ_AxReset(axisnum);
                    HZ_AxMoveRel(axisnum,pos);
                }
            } else {
                //没有报警,正常运动
                HZ_AxMoveRel(axisnum,pos);
            }
        }
        else	//反向点动
        {
            if(AXSTA_ERRSTOP == HZ_AxGetStatus(axisnum))
            {
                //只有上限错误
                if(0 == (0xf0ff & HZ_AxGetAxisErr(axisnum)))
                {
                    HZ_AxReset(axisnum);
                    HZ_AxMoveRel(axisnum,pos);
                }
            } else {
                //没有报警,正常运动
                HZ_AxMoveRel(axisnum,pos);
            }
        }
    }
}
//系统点动停止调用函数
void jogstop(u32 axisnum)
{
    HZ_AxStop(axisnum);
}
//系统回零调用函数
void joghome(u32 axisnum)
{
    HZ_AxReset(axisnum);
    HZ_AxHome(axisnum);
}
/**
* @author  yang
* @Description: 扩展卡输入输出状态的Modbus更新，当Modbus咨询到指定地址后调用
* @param --
* @param --
* @return --
*/
void ex_inputupdata()
{
    u8 i;
    //扩展卡1
    GSR.InputStatus[4] = 0;
    GSR.InputStatus[5] = 0;
    GSR.InputStatus[6] = 0;
    GSR.InputStatus[7] = 0;
    for(i=0; i<32; i++)
    {
        GSR.InputStatus[4] |=(u32) EX_INPUT[0][i] <<i;
    }
    //扩展卡2
    GSR.InputStatus[8] = 0;
    GSR.InputStatus[9] = 0;
    GSR.InputStatus[10] = 0;
    GSR.InputStatus[11] = 0;
    for(i=0; i<32; i++)
    {
        GSR.InputStatus[8] |=(u32) EX_INPUT[1][i] <<i;
    }
    //扩展卡3
    GSR.InputStatus[12] = 0;
    GSR.InputStatus[13] = 0;
    GSR.InputStatus[14] = 0;
    GSR.InputStatus[15] = 0;
    for(i=0; i<32; i++)
    {
        GSR.InputStatus[12] |=(u32) EX_INPUT[2][i] <<i;
    }
}

void ex_outputstatusupdata()
{
    u8 i;
    //扩展输出板1
    GSW.OutputStatus[4] = 0;
    for(i = 0; i < 32; i ++)
        GSW.OutputStatus[4]	|= (u32)	EX_OUTPUT[0][i]	<<	i;
    //扩展输出板2
    GSW.OutputStatus[8] = 0;
    for(i = 0; i < 32; i ++)
        GSW.OutputStatus[8]	|= (u32)	EX_OUTPUT[1][i]	<<	i;
}

void ex_outputupdata()
{
    u8 i;
    //扩展输出板1
    for(i = 0; i < 32; i ++)
        EX_OUTPUT[0][i]=(GSW.OutputStatus[4]>>i&1);
    //扩展输出板2
    for(i = 0; i < 32; i ++)
        EX_OUTPUT[1][i]=(GSW.OutputStatus[8]>>i&1);
}

