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

FlyCutCamTableDef FlyCutCamTable;  //�ɼ�����͹�ֱ�

void FlyCutLogic(LogicParaDef *LG)
{
    if(LG->execute == 1 && LG->step == 0)
    {
        LG->step = 1;
        LG->done = 0;

    }

    switch(LG->step)
    {
    case 1:
        FilmFeedMove(1, 10000, 50);
        PARAINIT(FlyCutCamTable);
        LG->step = 2;
        break;

    case 2:
        if(InputGet(0, X4) == ON)
        {
            FlyCutCamIn(1, 0, Axis_FeedFilm, Axis_Cutter, &FlyCutCamTable);
            LG->step = 3;
        }
        break;

    case 3:
        if(HZ_AxGetCurPos(Axis_Cutter) - FlyCutCamTable.SlaveStartPos  >= 10000)
        {
            FlyCutCamStop(&FlyCutCamTable);
            LG->step = 4;
        }
        break;

    case 4:
        if(FlyCutCamGetSta(&FlyCutCamTable) == 0)
        {
            LG->step = 2;
        }
        break;
    }
}

/**
* @author�� 2019/10/6 ũҵ��
* @Description: �ɼ�
* @param �C
* @param --
* @return --
*/
void FlyCut_Logic()
{
    FlyCutLogic(&LogicTask.FlyCut);
    CamFlyCutRun(&FlyCutCamTable);
}


