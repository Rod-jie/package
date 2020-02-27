/*
 * File: AppInit.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version:v1.1
 * Description:应用程序初始化配置
 * param:
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018 <<hzzh>>, <<hzzh>>
 */

#include "AppInit.h"

//变量声明
GlobalDataDef GlobalData = {0}; //定义用户通讯数据库
u8 uart_sendbuf[2][300];        //定义通讯接收和发送缓存
u8 uart_recvbuf[2][300];
u8 eth_txbuf[300];
u8 eth_rxbuf[300];
u8	EXI_ID[EXINUM] = {0};     //扩展I板ID
u8	EXQ_ID[EXQNUM] = {0};     //扩展Q板ID

void AppInit()
{
    u8 i;
    //配置地址表完成modbus
    init_config(&GlobalData, GSR.ErrorCode, &GlobalData.SpaceType.Save, GLOBALDATALEN, USERWRITELEN, USERREADLEN, USERSAVELEN);
    HZ_Data_Read();//读取falsh值

    for(i = 0; i < GPO_NUM; i++)  //初始化 输出口
        OutPut_SetSta(i, OFF);
    for(i = 0; i < PULS_NUM; i++)  //轴口使能
        EN_SetSta(i, OFF);

    GUR.HaveToReset = 1;
    LogicInit();

    //初始化轴配置
    AxisConfig(GSS.AxisData, GSS.AxIOconfig);

}
