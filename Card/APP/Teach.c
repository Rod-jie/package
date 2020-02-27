#include "Teach.h"

extern SysData Data;
void Teach()
{
    u8 i;
    if(GUR.RunStatus == RUN)
    {
        return;
    }
    switch(GSW.TeachWord)
    {
    case 1:	//�������
        Data.GlueData.delay = GUS.gluePara.Data.delay;
        Data.GlueData.times = GUS.gluePara.Data.times;
        Data.GlueData.postDelay = GUS.gluePara.Data.postDelay;
        LogicTask.glueTask.execute = 1;
        //LogicTask.glueMode.execute = 1;
        break;
    case 2:	//�Ž�
        LogicTask.exhaustTask.execute = 1;
        break;
    case 3:	//�Ž��ر�
        PARAINIT(LogicTask.exhaustTask);
        OutPut_SetSta(Q_Glue, OFF);
        break;
    case 4: //ȡ�ϲ���
        for(i = 0; i <= 12; i++)
        {
            Nozzle_set(i, 0, ON);
        }
        break;
    case 5://����
        for(i = 0; i <= 12; i++)
        {
            Nozzle_set(i, 0, OFF);
        }
        break;
    case 6:

        break;

    default:
        break;
    }
    GSW.TeachWord = 0;
}

