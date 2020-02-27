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
* @author£º 2018/08/21  yang
* @Description:  xyz¸´Î»½ø³Ì
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


