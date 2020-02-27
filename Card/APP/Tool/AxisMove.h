#ifndef __AXISMOVE_H
#define __AXISMOVE_H
#include "Logic.h"
#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"

#define AXIS_NUM 12

enum {
    GOHOMETYPE1 ,	    //直接回原点(平常模式)
    GOHOMETYPE2,		//先到上限位再回原点
    GOHOMETYPE3,		//先回下限位再回原点
	GOHOMETYPE4,        //z向找原点正向
	GOHOMETYPE5,        //z向找原点反向
};

typedef enum
{
    AX_ERR_AX_ALM = (1 << 0),		//轴报警
    AX_ERR_AX_TMOUT = (1 << 1),		//超时（回原点）
    AX_ERR_UP_LIM_H = (1 << 8),		//硬上限位
    AX_ERR_UP_LIM_S = (1 << 9),		//软上限位
    AX_ERR_DW_LIM_H = (1 << 12),		//硬下限位
    AX_ERR_DW_LIM_S = (1 << 13),		//软下限位
} AxErrBitTp;

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

//typedef enum	//轴号定义
//{
//    XMOTOR,
//    YMOTOR,
//    ZMOTOR,
//    R1MOTOR,
//    R2MOTOR,
//} AxisNum;

typedef enum	//轴动模式选择
{
    ABSMODE,//0
    RELMODE,//1
    SPDMODE,//2
    GOHOMEMODE,//3
    STOPMODE,	//4
} MoveMode;


typedef enum	//轴动类型
{
    REALAXIS,    	//0   实轴
    ENCODERAXIS,	//1 编码器轴
} AxisMode;

extern void Move(GoPosPara Para[], LogicParaDef T[]);
extern s32 MotorMove(u32 num,u32 speed,s32 pos,u32 mode);
extern void SetAxisSpdRadio(u8 i,u8 spd);
extern s32 SetSpd(u8 i, u32 RunSpeed);
extern void SetSpdDefault(u8 i);
extern void  SpeedParaInit(void);

extern float PulseToUserUnit(AxisConversion *p,s32 Dat);   
extern s32	UserUnitToPulse(AxisConversion *p,float Dat);
extern void MotorMoveUU(u8 i,u32 mode,u32 Ratio,float pos);
extern void MC_MoveUU(u8 i,u32 mode,u32 StartSpeed,u32 Acctime, u32 TargtSpeed,float TargetPos);   
extern void MC_MovePP(u8 i,u32 mode,u32 StartSpeed,u32 Acctime, u32 TargtSpeed,s32 TargetPos);   

#define CAMPointNum 500    //凸轮点数
typedef struct
{
//	float MasCirLength;			//主轴长度
//	u32	 MasCirPulse;				//主轴每圈脉冲
//					  
//	float	SlvCirLength;			//从轴长度
//	u32 SlvCirPulse;				//从轴每圈脉冲					    
//	float SynLength;				//同步区长度
//	float MasCycLength;				//料长
	u8 excute;
	u8 step;
	u8 done;
	u8 busy;
	
	u8 isCanIn;
	
	s32 index;   
	s32 MainIndex;
	s32 SlaveIndex;
	
	s32 indexTerm;
	
	u16 flyT;
	
	u32 t;
	u32 tt;
	u32 ttt;
	u32 tttt;
	
	s32 MainStartPos;    //主轴凸轮开始位置
	s32 SlaveStartPos;   //从轴凸轮开始位置
	s32 MainSpeed;    //主轴凸轮开始位置
	s32 SlaveSpeed;   //从轴凸轮开始位置	
	
	s32 MainCurrPos;    //主轴当前位置
	s32 SlaveCurrPos;   //从轴当前位置
	
	s32 MainTargetPos;    //主轴目标位置
	s32 SlaveTargetPos;   //从轴目标位置	
	
	
	
	u32 RealMainPulse;  //实际计算出来的主轴脉冲
	u32 RealSlavePulse; //实际计算出来的从轴脉冲	
	//凸轮输入参数
	u32 MainCirPulse;				//主轴脉冲数
	u32 SlaveCirPulse;			//从轴脉冲数
	u32 SynPulse;					//同步区长度
	s32 Ttime; 							//凸轮点数
	float SpeedMul;         //速度倍率，从轴/主轴	
	u8 MainAxisMode;       //主轴类型 0：实轴 1：编码器
	u8 MainEncodeNum;       //主轴编码器编号
	//end
	float MainAxisTPulse;   //主轴等分脉冲数
	float SlaveAxisTPulse;   //从轴同步区等分脉冲数
	
	u32 MainPlus1;     //主轴脉冲过大时，第一段凸轮结束
	u32 MainPlus2;     //主轴脉冲过大时，第二段起点
	u8 TooLongFlag;    //主轴过长标志
	u8 LongEndFlag;    //主轴过长标志
	s32 LongStopPos;   //
	
	s32 yDif;     //从轴位置偏差
	s32 yCyDif;     //从轴周期偏差
	
	u32 TT[20];
	float Vel[20];
//	s32 MainAxisPos[500];  //凸轮表格
	s32 SlaveAxisPos[CAMPointNum];	//从轴凸轮表格
//	s32 MainAxisRelPos[500];	 //主轴每段脉冲数，方便监控，不参与计算
	s32 SlaveAxisRelPos[CAMPointNum];	
	
	u32 mainAxSynSpeed;      //主轴同步速度
	u32 SlaveAxSynSpeed;		//从轴同步速度
	float SynSpeedRate;     //主从速度比例
}CamTableDef;

typedef struct
{
	u8 excute;
	u8 step;
	u8 done;
	u8 busy;	
	
	s32 index;   	
	s32 indexTerm;
	
	s32 MainStartPos;    //主轴凸轮开始位置
	s32 SlaveStartPos;   //从轴凸轮开始位置

	
	s32 MainCurrPos;    //主轴当前位置
	s32 SlaveCurrPos;   //从轴当前位置
	s32 MainSpeed;    //主轴速度
	s32 SlaveSpeed;   //从轴速度
	
	u8 mType;   //主轴类型 0：脉冲轴  1：编码器
	u8 mAx;     //主轴ID号
	u8 sAx;			//从轴ID号
	
	s32 Ttime; 							//凸轮点数
	float SpeedMul;         //速度倍率，从轴/主轴	


	float MainAxisTPulse;   //主轴等分脉冲数
	float SlaveAxisTPulse;   //从轴同步区等分脉冲数
	
	
	s32 flyStep;   //追剪步骤：0：追剪区，1：同步区，2：减速区
	s32 flyPos[3]; //每个阶段的位置
	u8 flyEndflag; //同步结束标志，该标志一结束，从轴就会减速停止
	
	s32 SlaveAxisPos[CAMPointNum];	//从轴凸轮表格
	s32 SlaveAxisRelPos[CAMPointNum];	
}FlyCutCamTableDef;

extern CamTableDef CamTable;
//extern s8 CamTableCaculate(CamTableDef *Table);
extern s8 CamTableCaculate(CamTableDef *Table);
extern s8 CamTableSetPara(CamTableDef *Table);
extern s8 CamRun(u8 MainAxis,u8 SlaveAxis,CamTableDef *Table);
extern s8 CamIn(CamTableDef *Table);
extern s8 CamOut(CamTableDef *Table);
extern s8 CamSetPos(s32 mainStartPos,s32 slaveStartPos,CamTableDef *Table);
extern u32 CamGetMainIndex(CamTableDef *Table,u8 MainAxis);
extern u32 CamGetSlaveIndex(CamTableDef *Table);
extern s8 CamEnd(CamTableDef *Table);

typedef struct
{
	s32 StartPos;
	s32 CurrPos;
}EncodeParaDef;
extern EncodeParaDef Encode[];

/**
* @author 2019/5/23  NYZ
* @Description: 设定编码器位置
* @param --  编码器编号
* @param --  编码器设定位置
* @return -- -1：错误 1：正确
*/
extern s8 EncodeSetPos(u8 encodeNum,s32 pos);

/**
* @author 2019/5/23  NYZ
* @Description: 设定编码器位置
* @param --  编码器编号
* @return -- -1：错误 1：正确
*/
extern s32 EncodeGetPos(u8 encodeNum);
extern  void EnCodGetSpeed(void);	
extern  s32 GetSpeed(u8 mode,u8 axisNum);


typedef struct
{
	u8 execute;
	u8 step;
	u8 busy;
	u8 done;
	u8 gearOut;
	u8 mAx;  //主轴
	u8 sAx;  //从轴
	s32 mStartPos;
	s32 sStartPos;
	s32 mCylPlus;
	s32 sCylPlus;	
	s32 mCurrPlus;
	s32 sCurrPlus;
	s32 mSpd;
	s32 sSpd;
	s32 sTargetPlus;
	u32 mTgSpd;	
	float rate;  //比列
}GearInDataDef;



#define GEARDIV 50   //齿轮咬合，脉冲阈值
//电子齿轮比咬合
extern s8 GearRun(GearInDataDef *G);
extern s8 GearIn(u8 mAx,u8 sAx,u32 mCylPlus,u32 sCylPlus,u32 mTgSpd,GearInDataDef *G);
extern s8 GearEnd(GearInDataDef *G);
extern s8 GearOut(GearInDataDef *G);



/*********************  追剪功能 *************************************/
extern s8 FlyCutCamTableCaculate(FlyCutCamTableDef *Table);
extern s8 CamFlyCutRun(FlyCutCamTableDef *Table);
extern s8 FlyCutCamIn(float ratio,u8 mType,u8 mAx,u8 sAx,FlyCutCamTableDef *Table);
extern s8 FlyCutCamStop(FlyCutCamTableDef *Table);
extern s8 FlyCutCamGetSta(FlyCutCamTableDef *Table);
#endif
