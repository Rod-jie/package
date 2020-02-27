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
#define TimerCnt(a)		((SysTim_GetUser()-a.tm)/10) 	//��ʱת��ms
#define TimerRst1(a)		 a->tm = SysTim_GetUser()		  //0.1 ms
#define TimerCnt1(a)		((SysTim_GetUser()-a->tm)/10) 	//��ʱת��ms

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

#define	NORMALMODE	0		//����ģʽ
#define	STEPMODE	1		//��������
#define	CYCLEMODE	2		//���ε���
#define	AGINGMODE	3		//�ϻ�ģʽ

#define AXISDELAY	5		//ͣ����ʱ
#define ENDCASE     0xff	//���һ��case




/************ҵ���߼�����***********************/
typedef struct//ϵͳ�������
{
    LogicParaDef GoPosPara;		//�߾���λ�ã���λ����
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

typedef struct//ϵͳ�������ñ���
{
    u8 initflag;
    u8 JogFlag;
    s32 ProductBuffCont;   //��Ʒ�����ݴ�
    float EndPos;     //��ǰѭ��λ��
    float StartPos;   //��ʼλ��

    float PosAdd;

    float FeedFlimStopPos;

    float FeedStartPos;
    u8 FirstPackageFlag;  //�״ΰ�װ
    float CamDif;
    float ColorMarkDif;   //ɫ��ƫ��
    float MarkPosDif[10];
    float MarkPosSum;
    float MarkPos;
    float MarkPosStart;
    float MarkPosEnd;
    float MarkToEmtySenDif;   //ɫ����հ���⵽���˵�λ�ò�

    u8 ProductNumRech;   //���������־
    u8 ProductPosIsChange;    //����
    int ProductPosTerm;    //����

    u8 FeedStaTerm;   //��������״̬�ݴ�
    u8 CutterStuckFlag; //�е���ת��־
    u8 CutterStuckCount; //�е���ת��־

    u32 PackageSpd;  //��װ�ٶȣ�MM/Min

    s32 IsHaveProduct[10];  //0:�����ϣ�1��������
    u8 CylStopFlag;   //ѭ�hֹͣ

    u8 feedFimeAxisRunFlag;   //��Ĥ���б�־

} SysData;
extern SysData Data;




//GUW.button.BitControlWord   //1506
typedef enum
{
    StartBit,							//bit0  ��ʼ��ť
    StopBit,							//bit1  ֹͣ��ť
    ResetBit,							//bit2  ��λ��ť
    HoldBit,							//bit3  ��ͣ��ť
    ClearPruductNumBit,  	//bit4  ������0��ť
    ProductNumSub1Bit,  	//bit5	������1��ť
    AxisPowerOn,  				//bit6	�ŷ�ʹ��
    PackageJogPos,				//bit7	��װ���㶯
    PackageJogNeg,				//bit8	��װ���㶯
    ColorMarkPosAdd,			//bit9	ɫ��λ�ü�
    ColorMarkPosSub,			//bit10	ɫ��λ�ü�
    ProductPosAdd,						//bit11	����λ�ü�
    ProductPosSub,			//bit12	����λ�ü�

    FilmFeedJogPos,				//bit13	��ֽJOG+
    FilmFeedJogNeg,				//bit14	��ֽJOG-
    CutterJogPos,				//bit15	�е�JOG+
    CutterJogNeg,               //bit16	�е�JOG-
    FeedJogPos,					//bit17	����JOG+
    FeedJogNeg,                 //bit18	����JOG-


} HMIButton;



typedef struct
{
    float l;		//���ϳ���
    float StartPos;
    float EndPos;
} PackageDataDef;

typedef struct
{
    float l;		//���ϳ���
    float StartPos;
    float EndPos;
} FeedDataDef;

#define BuffArrayNum 30   //����װ���� ���ݳ���

extern PackageDataDef PackageDataArray[BuffArrayNum];

//extern void SpeedParaInit(void);
//extern void SetSpdDefault(u8 i);
extern void Logic(void);
extern void AuxLogic(void);



/**
* @author�� 2019/05/17  nyz
* @Description:  ��ֽ����
* @param --
* @param --
* @return --
*/
extern void FilmFeedMove(s32 dir, float packageSpd, u32 accTime);
extern void FilmFeedMoveMM(s32 dir, float packageSpd);

/**
* @author�� 2019/5/23 NYZ
* @Description: ��ֹֽͣ
* @param �C
* @param --
* @return --
*/
extern void FilmFeedStop(u32 accTime);
extern void FilmFeedStop1(void);

extern s8 GetFilmFeedSta(void);

extern float GetFilmFeedPosUU(void);
/**
* @author�� 2019/05/23  nyz
* @Description:  ��������
* @param --
* @param --
* @return --
*/
//�豸λ��
#define ColorMarkPos ((int)(GUS.PackgeLength + GUS.colorMarkPos - GUS.SysPara.MarkReactionTime * GUR.CurrPackageSpd/60/1000)%(int)GUS.PackgeLength)

#define MPM  1 //�ٶȵ�λ����/��
#define PPM  0 //�ٶȵ�λ����/��
extern void FeedMove(s32 dir, float packageSpd);


/**
* @author�� 2019/5/23 NYZ
* @Description: ��ֹֽͣ
* @param �C
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

//����ģʽ
extern void ConMode_Logic(void);
//������ģʽ
extern void LengthDifMode_Logic(void);
//����ֽģʽ
extern void UnderFeedFilmMode_Logic(void);
extern void PackageReset(LogicParaDef *LG);
//extern void PackageReset1(LogicParaDef *LG);
//extern void PackageReset2(LogicParaDef *LG);
//extern void PackageReset3(LogicParaDef *LG);

//LED״ָ̬ʾ
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


