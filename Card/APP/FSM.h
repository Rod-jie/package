#ifndef __FSM_H
#define __FSM_H

//#include "Logic.h"
#include "reset.h"
#include "axismove.h"

typedef enum
{
    INIT,	//��ʼ̬
    STOP,       //ֹ̬ͣ
    RUN	,       //����̬
    D_RESET,    //��λ̬
    SCRAM,      //��̬ͣ
    PAUSE,      //��̬ͣ
    SIGSTEP,     //��������
    SIGRUN,		//��������
} RunStatus;

extern void FSM(void);

#endif

