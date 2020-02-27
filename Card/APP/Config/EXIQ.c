#include "EXIQ.h"

#include "interlayer_config.h"



u8	EX_INPUT[EXINUM][32];     //扩展I值
u8	EX_OUTPUT[EXQNUM][32] =   //扩展Q值
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//x
};
u8 SendBuf[8] = {0};
u8 RecvBuf[8] = {0};
u32 SendId = 0;
u32 RecvId = 0;
s32 SendLen = 0;
s32 RecvLen = 0;

#define	TIME_OUT	(2*10)
EXIQparaDef EXIQpara;
void EXIQ()
{
    u8 i;
    EXIQpara.execute = 1;
    if(EXIQpara.execute == 1 && EXIQpara.step == 0)
    {
        EXIQpara.step = 1;
        EXIQpara.index = 0;
        EXIQpara.count = 0;
    }
    switch(EXIQpara.step)
    {
    case 1://输出
        if(EXQEN)
        {
            *(u32*)SendBuf = 0;
            for(i=0; i<32; i++)
            {
                *(u32*)SendBuf |= ((u32)EX_OUTPUT[EXIQpara.index][i]&1) << i;
            }
            SendId = EXQ_ID[EXIQpara.index];
            SendLen = 8;
            Can_SendData(SendId,SendLen,SendBuf);

//				CanInterface.SendID = EXQ_ID[EXIQpara.index];
//				CanInterface.SendLength = 8;
//				*(u32*)CanInterface.SendBuf = 0;
//				for(i=0; i<32; i++)
//				{
//					*(u32*)CanInterface.SendBuf |= ((u32)EX_OUTPUT[EXIQpara.index][i]&1) << i;
//				}
//				CanInterface.SendBusy = 1;
            EXIQpara.step = 2;
        }
        else
        {
            EXIQpara.index = 0;
            EXIQpara.step = 4;
        }
        break;

    case 2:
        if(0 == Can_GetSendSta())
        {
            EXIQpara.TimeStamp = SysTim_GetSys();
            EXIQpara.step = 3;
        }
        //if(CanInterface.SendBusy == 0)
//			{
//				EXIQpara.TimeStamp = TimerInterface.systimercount;
//				EXIQpara.step = 3;
//			}
        break;

    case 3:
        if(SysTim_GetSys() - EXIQpara.TimeStamp < TIME_OUT)
//			if(TimerInterface.systimercount - EXIQpara.TimeStamp < 100)
        {
            RecvLen = Can_GetRecvData(&RecvId,RecvBuf);

            if(RecvLen >= 0)
            {
//					CanInterface.RecBusy = 0;
                if((RecvId&7) == EXQ_ID[EXIQpara.index])
                {
                    EXIQpara.count = 0;
                    EXIQpara.index++;
                    if(EXIQpara.index < EXQNUM)
                    {
                        EXIQpara.step = 1;
                    }
                    else
                    {
                        EXIQpara.index = 0;
                        EXIQpara.step = 4;
                    }
                }
                else
                {
                    EXIQpara.count++;
                    if(EXIQpara.count < 5)
                    {
                        EXIQpara.step = 1;
                    }
                    else
                    {
                        EXIQpara.count = 0;
                        EXIQpara.index++;
                        if(EXIQpara.index < EXQNUM)
                        {
                            EXIQpara.step = 1;
                        }
                        else
                        {
                            EXIQpara.index = 0;
                            EXIQpara.step = 4;
                        }
                    }
                }
            }
        }
        else
        {
            EXIQpara.count++;
            if(EXIQpara.count < 5)
            {
                EXIQpara.step = 1;
            }
            else
            {
                AlarmSetBit(3,0,10);
                EXIQpara.count = 0;
                EXIQpara.index++;
                if(EXIQpara.index < EXQNUM)
                {
                    EXIQpara.step = 1;
                }
                else
                {
                    EXIQpara.index = 0;
                    EXIQpara.step = 4;
                }
            }
        }
        break;

    case 4://输入
        if(EXIEN)
        {
            SendId = EXI_ID[EXIQpara.index];
            SendLen = 0;
            Can_SendData(SendId,SendLen,SendBuf);

//				CanInterface.SendID = EXI_ID[EXIQpara.index];
//				CanInterface.SendLength = 0;
//				CanInterface.SendBusy = 1;
            EXIQpara.step = 5;
        }
        else
        {
            EXIQpara.index = 0;
            EXIQpara.step = 1;
        }
        break;

    case 5:
        if(0 == Can_GetSendSta())
        {
            EXIQpara.TimeStamp = SysTim_GetSys();
            EXIQpara.step = 6;
        }
        break;

    case 6:
        if(SysTim_GetSys() - EXIQpara.TimeStamp < TIME_OUT)
//			if(TimerInterface.systimercount - EXIQpara.TimeStamp < 100)
        {
            RecvLen = Can_GetRecvData(&RecvId,RecvBuf);

            if(RecvLen>= 0)
            {
//					CanInterface.RecBusy = 0;
                if((RecvId&7) == EXI_ID[EXIQpara.index])
                {
                    for(i=0; i<32; i++)
                    {
                        EX_INPUT[EXIQpara.index][i] = *(u32*)RecvBuf >> i & 1;
                    }
                    EXIQpara.count = 0;
                    EXIQpara.index++;
                    if(EXIQpara.index < EXINUM)
                    {
                        EXIQpara.step = 4;
                    }
                    else
                    {
                        EXIQpara.index = 0;
                        EXIQpara.step = 1;
                    }
                }
                else
                {
                    EXIQpara.count++;
                    if(EXIQpara.count < 5)
                    {
                        EXIQpara.step = 4;
                    }
                    else
                    {
                        EXIQpara.count = 0;
                        EXIQpara.index++;
                        if(EXIQpara.index < EXINUM)
                        {
                            EXIQpara.step = 4;
                        }
                        else
                        {
                            EXIQpara.index = 0;
                            EXIQpara.step = 1;
                        }
                    }
                }
            }
        }
        else
        {
            EXIQpara.count++;
            if(EXIQpara.count < 5)
            {
                EXIQpara.step = 4;
            }
            else
            {
                AlarmSetBit(3,0,10);
                EXIQpara.count = 0;
                EXIQpara.index++;
                if(EXIQpara.index < EXINUM)
                {
                    EXIQpara.step = 4;
                }
                else
                {
                    EXIQpara.index = 0;
                    EXIQpara.step = 1;
                }
            }
        }
    }
}


s32 ExInPut_GetSta(u8 cardnum,u8 num)
{
    if(num<32)
    {
        return EX_INPUT[cardnum][num];
    } else {
        return -1;
    }
}
s32 ExOutPut_SetSta(u8 cardnum, u8 num,u8 lev)
{
    if(num<32)
    {
        EX_OUTPUT[cardnum][num]=lev;
        return 0;
    } else
        return -1;
}
s32 ExOutPut_GetSta(u8 cardnum,u8 num)
{
    if(num<32)
    {
        return EX_OUTPUT[cardnum][num];
    } else
        return -1;
}












