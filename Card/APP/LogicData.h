#ifndef __LOGICDATA_H
#define __LOGICDATA_H
#include "bsplib.h"

typedef struct
{
    u8 execute;
    u8 step;
    u8 done;
    u8 count;
    u8 index;
    u32 tm;
} LogicParaDef;

typedef struct
{
    u32 execute;
    u32 mode;
    s32 pos;
    u32 speed;
} GoPosPara;	//��λ�����˶��ӿ�

/************ҵ���߼�����***********************/


/***********����������������**************/


#endif

