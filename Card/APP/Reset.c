#include "reset.h"
LogicParaDef ResetTask = {0};
void Reset()
{
    if(GUR.RunStatus == D_RESET)
    {
        PackageReset(&LogicTask.Reset);
    }
}

/**
* @author�� 2018/08/21  yang
* @Description:  xyz��λ����
* @param --
* @param --
* @return --
*/
void XYZReset(LogicParaDef* Task)
{
    INITT(Task)
    switch(Task->step)
    {
    case 1:
        break;

    }
}


