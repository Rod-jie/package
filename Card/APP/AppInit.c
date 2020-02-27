/*
 * File: AppInit.c
 * File Created: Tuesday, 9th October 2018 2:43:29 pm
 * version:v1.1
 * Description:Ӧ�ó����ʼ������
 * param:
 * Modified By:yang
 * -----
 * Copyright 2018 - 2018 <<hzzh>>, <<hzzh>>
 */

#include "AppInit.h"

//��������
GlobalDataDef GlobalData = {0}; //�����û�ͨѶ���ݿ�
u8 uart_sendbuf[2][300];        //����ͨѶ���պͷ��ͻ���
u8 uart_recvbuf[2][300];
u8 eth_txbuf[300];
u8 eth_rxbuf[300];
u8	EXI_ID[EXINUM] = {0};     //��չI��ID
u8	EXQ_ID[EXQNUM] = {0};     //��չQ��ID

void AppInit()
{
    u8 i;
    //���õ�ַ�����modbus
    init_config(&GlobalData, GSR.ErrorCode, &GlobalData.SpaceType.Save, GLOBALDATALEN, USERWRITELEN, USERREADLEN, USERSAVELEN);
    HZ_Data_Read();//��ȡfalshֵ

    for(i = 0; i < GPO_NUM; i++)  //��ʼ�� �����
        OutPut_SetSta(i, OFF);
    for(i = 0; i < PULS_NUM; i++)  //���ʹ��
        EN_SetSta(i, OFF);

    GUR.HaveToReset = 1;
    LogicInit();

    //��ʼ��������
    AxisConfig(GSS.AxisData, GSS.AxIOconfig);

}
