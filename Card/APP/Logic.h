#ifndef _LOGIC_H_
#define	_LOGIC_H_

#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"
//#include "FSM.h"
//#include "EXIQ.h"
#include "tool.h"
#include "string.h"
#include "logicdata.h"
#include "iodefine.h"
#include "AxisMove.h"


#define TimerRst(a)		 a.tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt(a)		((SysTim_GetUser()-a.tm)/10) 	//计时转成ms
#define TimerRst1(a)		 a->tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt1(a)		((SysTim_GetUser()-a->tm)/10) 	//计时转成ms

#define	ON	0
#define	OFF	1

#define PARAINIT(a)  memset(&a,0,sizeof(a));
#define INITTASK(a) if(a.execute==1&&a.step==0){a.step = 1;}

#define INITT(a)                      \
	if (a->execute == 1 && a->step == 0) \
	{                                    \
		a->step = 1;                      \
	}

#define CHECK_NUM 1
#define	VACUUM_NUM 1
#define	DOWN_NUM 1

#define	NORMALMODE	0		//正常模式
#define	STEPMODE	1		//单步调试
#define	CYCLEMODE	2		//单次调试
#define	AGINGMODE	3		//老化模式

#define AXISDELAY	5		//停轴延时
#define ENDCASE     0xff	//最后一个case




/************业务逻辑参数***********************/
typedef struct//系统任务队列
{
    LogicParaDef GoPosPara;		//走绝对位置，上位交互
    LogicParaDef RunProcessPara;
    LogicParaDef Package;
    LogicParaDef PackageJog;
    LogicParaDef FeedFilmReset;
    LogicParaDef CutterReset;
    LogicParaDef FeedReset;
    LogicParaDef AixsMove[5];
    LogicParaDef Reset;
    LogicParaDef PackageLengthTest;
    LogicParaDef CutterCirPulseTest;
    LogicParaDef FeedCirPulseTest;

    LogicParaDef PackageCamStart;
    LogicParaDef PackageCamStop;

    LogicParaDef PruductCheck;
    LogicParaDef PruductJog;

    LogicParaDef ColorMarkCheck;

    LogicParaDef CamDrawHMI;

    LogicParaDef CutterErrRst;
    LogicParaDef BlowData[4];
    LogicParaDef CodePrintData;
    LogicParaDef SprayWaterData;
    LogicParaDef CamCorrectData;
    LogicParaDef CutterStopAngleData;
    LogicParaDef EmptyCheck;
    LogicParaDef ScaldFilmAvoid;
    LogicParaDef CylStop;

    LogicParaDef FlyCut;

} Task;

extern Task LogicTask;

typedef struct//系统程序自用变量
{
    u8 initflag;
    u8 JogFlag;
    s32 ProductBuffCont;   //产品数量暂存
    float EndPos;     //当前循环位置
    float StartPos;   //开始位置

    float PosAdd;

    float FeedFlimStopPos;

    float FeedStartPos;
    u8 FirstPackageFlag;  //首次包装
    float CamDif;
    float ColorMarkDif;   //色标偏差
    float MarkPosDif[10];
    float MarkPosSum;
    float MarkPos;
    float MarkPosStart;
    float MarkPosEnd;
    float MarkToEmtySenDif;   //色标里空包检测到光纤的位置差

    u8 ProductNumRech;   //产量到达标志
    u8 ProductPosIsChange;    //物料
    int ProductPosTerm;    //物料

    u8 FeedStaTerm;   //送料运行状态暂存
    u8 CutterStuckFlag; //切刀堵转标志
    u8 CutterStuckCount; //切刀堵转标志

    u32 PackageSpd;  //包装速度，MM/Min

    s32 IsHaveProduct[10];  //0:无物料，1：有物料
    u8 CylStopFlag;   //循環停止

    u8 feedFimeAxisRunFlag;   //走膜运行标志

} SysData;
extern SysData Data;




//GUW.button.BitControlWord   //1506
typedef enum
{
    StartBit,							//bit0  开始按钮
    StopBit,							//bit1  停止按钮
    ResetBit,							//bit2  复位按钮
    HoldBit,							//bit3  暂停按钮
    ClearPruductNumBit,  	//bit4  产量清0按钮
    ProductNumSub1Bit,  	//bit5	产量减1按钮
    AxisPowerOn,  				//bit6	伺服使能
    PackageJogPos,				//bit7	包装正点动
    PackageJogNeg,				//bit8	包装反点动
    ColorMarkPosAdd,			//bit9	色标位置加
    ColorMarkPosSub,			//bit10	色标位置减
    ProductPosAdd,						//bit11	物料位置加
    ProductPosSub,			//bit12	物料位置减

    FilmFeedJogPos,				//bit13	走纸JOG+
    FilmFeedJogNeg,				//bit14	走纸JOG-
    CutterJogPos,				//bit15	切刀JOG+
    CutterJogNeg,               //bit16	切刀JOG-
    FeedJogPos,					//bit17	送料JOG+
    FeedJogNeg,                 //bit18	送料JOG-


} HMIButton;



typedef struct
{
    float l;		//物料长度
    float StartPos;
    float EndPos;
} PackageDataDef;

typedef struct
{
    float l;		//物料长度
    float StartPos;
    float EndPos;
} FeedDataDef;

#define BuffArrayNum 30   //待包装物料 数据长度

extern PackageDataDef PackageDataArray[BuffArrayNum];

//extern void SpeedParaInit(void);
//extern void SetSpdDefault(u8 i);
extern void Logic(void);
extern void AuxLogic(void);



/**
* @author： 2019/05/17  nyz
* @Description:  走纸运行
* @param --
* @param --
* @return --
*/
extern void FilmFeedMove(s32 dir, float packageSpd, u32 accTime);
extern void FilmFeedMoveMM(s32 dir, float packageSpd);

/**
* @author： 2019/5/23 NYZ
* @Description: 走纸停止
* @param –
* @param --
* @return --
*/
extern void FilmFeedStop(u32 accTime);
extern void FilmFeedStop1(void);

extern s8 GetFilmFeedSta(void);

extern float GetFilmFeedPosUU(void);
/**
* @author： 2019/05/23  nyz
* @Description:  送料运行
* @param --
* @param --
* @return --
*/
//设备位置
#define ColorMarkPos ((int)(GUS.PackgeLength + GUS.colorMarkPos - GUS.SysPara.MarkReactionTime * GUR.CurrPackageSpd/60/1000)%(int)GUS.PackgeLength)

#define MPM  1 //速度单位：米/分
#define PPM  0 //速度单位：包/分
extern void FeedMove(s32 dir, float packageSpd);


/**
* @author： 2019/5/23 NYZ
* @Description: 走纸停止
* @param –
* @param --
* @return --
*/
extern void FeedStop(void);
extern s8 GetFeedSta(void);
extern void CutCamCaculate(float packageLength);

extern s8 CutterStuckCheck(void);
extern void FeedFilmReset(LogicParaDef *LG);
extern void CutterReset(LogicParaDef *LG);
extern void FeedReset(LogicParaDef *LG);

extern s8 EmptySensor(void);
extern void ProductCount(void);
extern void ColorMarkLoseCheck(void);

//常规模式
extern void ConMode_Logic(void);
//不定长模式
extern void LengthDifMode_Logic(void);
//下走纸模式
extern void UnderFeedFilmMode_Logic(void);
extern void PackageReset(LogicParaDef *LG);
//extern void PackageReset1(LogicParaDef *LG);
//extern void PackageReset2(LogicParaDef *LG);
//extern void PackageReset3(LogicParaDef *LG);

//LED状态指示
typedef enum
{
    GREEN,
    YELLOW,
    RED,
} LEDSta;

extern void LedWork(u8 whichLed);

extern void LogicInit(void);

extern void FlyCut_Logic(void);





#endif


