#ifndef __AXIS_H
#define __AXIS_H

#include "BspLib.h"
#include "curve.h"

#define AXIS_NUM 16
//#define GPI_NUM					60	//I个数
//#define GPO_NUM					30	//Q数量

//#define AX_SPD_MAX	(200*1000)
#define AX_SPD_MIN	(20)
typedef enum 
{
	AXSTA_READY = 0,	//就绪状态
	AXSTA_STOP,			//停止状态
	AXSTA_STOPDEC,		//停止状态(减速停)
	AXSTA_DISCRETE,		//普通运动状态
	AXSTA_CONTINUOUS,	//连续运动
	AXSTA_HOMING,		//正在回原点
	AXSTA_DISABLE,		//未激活状态
	AXSTA_ERRSTOP,		//错误停
	AXSTA_SYNC,			//轴同步状态
	
}AxStateTp;

typedef enum 
{
	BACK_WARD = -1,
	FOR_WARD = 1,
}AxDirTp;

typedef enum 
{
	AX_ERR_AX_ALM	= (1<<0),		//轴报警
	AX_ERR_AX_TMOUT	= (1<<1),		//超时（回原点）
	AX_ERR_UP_LIM_H	= (1<<8),		//硬上限位
	AX_ERR_UP_LIM_S	= (1<<9),		//软上限位
	AX_ERR_DW_LIM_H	= (1<<12),		//硬下限位
	AX_ERR_DW_LIM_S	= (1<<13),		//软下限位
}AxErrBitTp;
typedef enum 
{
	HM_GO_DW	= (1<<0),			//回下限
	HM_GO_UP	= (1<<1),			//回上限
	HM_GO_HM	= (1<<2),			//回原点
	HM_GO_ZF	= (1<<3),			//回Z正转找
	HM_GO_ZB	= (1<<4),			//回z反转找
}AxHomBitTp;

//#define FORWARD		0	//正向
//#define BACKWARD	1	//反向
//#define AX_SPD_MAX	(200*1000)
//#define AX_SPD_MIN	(20)

#define A_SPD_START(n)			AxPara[n].t_StartSpd
#define A_SPD_TACC(n)			AxPara[n].TAcc
#define A_SPD_RUN(n)			AxPara[n].t_RunSpd
#define A_SPD_TDEC(n)			AxPara[n].TDec
#define A_SPD_END(n)			AxPara[n].t_EndSpd
#define A_SPD_HMFAST(n)			AxPara[n].HomeSpdFast
#define A_SPD_HMSLOW(n)			AxPara[n].HomeSpdSlow
#define A_POS_HMOFF(n)			AxPara[n].HomeOffSet
#define A_TORS(n)				AxPara[n].TorS



#define A_HM_EN(n)				AxPara[n].HomeEn
#define A_HM_SG_LV(n)			AxPara[n].HomeSigLev
#define A_HM_SG(n)				AxPara[n].HomeSig
#define A_HM_TMOUT(n)			AxPara[n].HomeTimeOut
#define A_HM_MODE(n)			AxPara[n].HomeMod

#define A_LIM_DW_S_EN(n)		AxPara[n].LimEnDw_S	
#define A_LIM_UP_S_EN(n)		AxPara[n].LimEnUp_S
#define A_LIM_DW_H_EN(n)		AxPara[n].LimEnDw_H
#define A_LIM_UP_H_EN(n)		AxPara[n].LimEnUp_H

#define A_LIM_DW_POS(n)			AxPara[n].LimPosDw		
#define A_LIM_UP_POS(n)			AxPara[n].LimPosUp
#define A_LIM_DW_SG_LV(n)		AxPara[n].LimSigDwLev
#define A_LIM_UP_SG_LV(n)		AxPara[n].LimSigUpLev
#define A_LIM_DW_SG(n)			AxPara[n].LimSigDw
#define A_LIM_UP_SG(n)			AxPara[n].LimSigUp

#define A_POS_TAR(n)			AxPara[n].TarPos
#define A_POS_CUR(n)			AxPara[n].CurPos
#define A_POS_LST(n)			AxPara[n].LastPos
#define A_SPD_CUR(n)			AxPara[n].CurSpd
#define A_STA(n)				AxPara[n].State	 //轴状态机
#define A_STU(n)				AxPara[n].Status //记录错误
#define A_DIR(n)				AxPara[n].Dir

#define A_EXCU(n)				AxPara[n].Excu
#define A_STEP(n)				AxPara[n].Step


#define A_IF_LIM_UP(n)			(A_LIM_UP_H_EN(n) && (A_LIM_UP_SG_LV(n) == AxSigGet(A_LIM_UP_SG(n))))//(A_LIM_UP_H_EN(n) && (A_LIM_UP_SG_LV(n) == InPut_GetSta(A_LIM_UP_SG(n))))
#define A_IF_LIM_DW(n)			(A_LIM_DW_H_EN(n) && (A_LIM_DW_SG_LV(n) == AxSigGet(A_LIM_DW_SG(n))))//(A_LIM_DW_H_EN(n) && (A_LIM_DW_SG_LV(n) == InPut_GetSta(A_LIM_DW_SG(n))))
#define A_IF_HOME(n)			(A_HM_EN(n) && (A_HM_SG_LV(n) == AxSigGet(A_HM_SG(n))))				//(A_HM_EN(n) && (A_HM_SG_LV(n) == InPut_GetSta(A_HM_SG(n))))

typedef struct
{
	u32	StartSpd;		//起始速度
	u32	t_StartSpd;		//供下面使用
//	u32	AccSpd;			//加速度
	u32 TAcc;		//加速时间
//	u32 Jerk;			//加加速度
	u32	RunSpd;			//运行速度
	u32	t_RunSpd;		//运行速度
	u32 TDec;		//减速时间
//	u32	DecSpd;			//减速度
//	u32 DJerk;			//加减速度
	u32	EndSpd;			//末速度
	u32	t_EndSpd;		//末速度
	u32 TorS;			//0:梯形1：s形
	u32	HomeSpdFast;	//回原点速度快
	u32	HomeSpdSlow;	//会原点速度慢
	s32 HomeOffSet;		//回原点偏移量，默认为0
	
	u8	HomeEn;			//原点使能 0不使能 
	u8	HomeSigLev;		//原点信号有效电平
	u16	HomeSig;		//原点信号
	u32 HomeTimeOut;	//原点超时时间，单位ms	（为0时无效）
	u32	HomeMod;		//回原点模式
	
	u8	LimAlmLev;		//轴口报警信号电平 0 ，1 ， 其他表示不生效
	u8	LimEnUp_H;		//硬上限使能
	u8	LimEnDw_H;		//硬下限使能
	u8	LimEnUp_S;		//软上限使能
	u8	LimEnDw_S;		//软下限使能
	u8	LimSigUpLev;	//上限信号有效电平
	u8	LimSigDwLev;	//下限信号有效电平
	u8	LimSigUp;		//上限信号
	u8	LimSigDw;		//下限信号
	s32	LimPosUp;		//软上限位置
	s32	LimPosDw;		//软下限位置
	
	s32	TarPos;			//目标位置
	s32	LastPos;		//运行前的位置
	s32	CurPos;			//当前位置
	u32	CurSpd;			//当前速度
	u16 Excu;			//0：空闲	其他：忙
	u16 Step;			//0：空闲	其他：忙
	AxStateTp	State;	//轴状态机（描述，处于某个状态）
	u32	Status;			//轴状态（暂时用于记录错误）
	AxDirTp	Dir;		//方向
}AxParaDef;

//
extern u32 GetSpd(u8 axNum);
extern u32 GetDecSteps(u8 axNum);


extern s32 Multi_AxGetStop(u8 AxNum);
extern s32 Multi_AxSetMulti(u8 AxNum);
extern s32 Multi_AxMoveRel(u8 AxNum, s32 pos);

extern void HZ_AxMotion(void);

//对外接口
//01绝对定位
extern s32 HZ_AxMoveAbs(u8 AxNum,s32 pos);
//02相对定位
extern s32 HZ_AxMoveRel(u8 AxNum,s32 pos);
//03叠加定位
extern s32 HZ_AxMoveAdd(u8 AxNum,s32 pos);
//重新定位
//extern s32 HZ_AxMoveSuperImposed();
//恒速运行()
//AxNum :轴号，从0开始
//pos	:大于0正向，小于0反向 0：错误
extern s32 HZ_AxMoveVelocity(u8 AxNum,s32 pos);
extern s32 HZ_AxChSpd(u8 AxNum,u32 Spd);
//停止轴
extern s32 HZ_AxStop(u8 AxNum);

//////*****//////////////
//减速停
extern s32 HZ_AxStopDec(u8 AxNum);
//回原点
extern s32 HZ_AxHome(u8 AxNum);


//复位轴(复位错误及把轴状态归为就绪)
extern s32 HZ_AxReset(u8 AxNum);
//读轴状态
extern s32 HZ_AxGetStatus(u8 AxNum);
//读轴错误
extern s32 HZ_AxGetAxisErr(u8 AxNum);
//读当前位置
extern s32 HZ_AxGetCurPos(u8 AxNum);
//写当前位置
extern s32 HZ_AxSetCurPos(u8 AxNum,s32 pos);
//设置参数
extern s32 HZ_AxSetPara(u8 AxNum,\
				u32 Start,u32 TAcc,u32 Run,u32 TDec,u32 End,\
				u32 HomFast,u32 HomSlow,u32 HomOffset,\
				u32 TorS,u32 HomTimeOut);
//设置轴信号参数
extern s32 HZ_AxSetAxiSig(u8 AxNum,\
					u8 HomSigEn,u8 HomSigNum,u8 HomSigLev,\
					u8 UpSigEn,u8 UpSigNum,u8 UpSigLev,\
					u8 DwSigEn,u8 DwSigNum,u8 DwSigLev);
//设置上下软限位
extern s32 HZ_AxSetLimSoft(u8 AxNum,\
					u8 UpEn,s32 UpPos,\
					u8 DwEn,s32 DwPos);
//设置回原点模式
//AxNum :轴号
//Mod	:模式
//		0：反向找原点
//		1：先正向找上限位，再反向找原点
//		2：先反向找下限位，再正向找原点
extern s32 HZ_AxSetHomMod(u8 AxNum,u8 Mod);

#endif
