#ifndef __AXISMOVE_H
#define __AXISMOVE_H
#include "Logic.h"
#include "DataDefine.h"
#include "bsplib.h"
#include "interlayer_config.h"

#define AXIS_NUM 12

enum {
    GOHOMETYPE1 ,	    //ֱ�ӻ�ԭ��(ƽ��ģʽ)
    GOHOMETYPE2,		//�ȵ�����λ�ٻ�ԭ��
    GOHOMETYPE3,		//�Ȼ�����λ�ٻ�ԭ��
	GOHOMETYPE4,        //z����ԭ������
	GOHOMETYPE5,        //z����ԭ�㷴��
};

typedef enum
{
    AX_ERR_AX_ALM = (1 << 0),		//�ᱨ��
    AX_ERR_AX_TMOUT = (1 << 1),		//��ʱ����ԭ�㣩
    AX_ERR_UP_LIM_H = (1 << 8),		//Ӳ����λ
    AX_ERR_UP_LIM_S = (1 << 9),		//������λ
    AX_ERR_DW_LIM_H = (1 << 12),		//Ӳ����λ
    AX_ERR_DW_LIM_S = (1 << 13),		//������λ
} AxErrBitTp;

typedef enum 
{
	AXSTA_READY = 0,	//����״̬
	AXSTA_STOP,			//ֹͣ״̬
	AXSTA_STOPDEC,		//ֹͣ״̬(����ͣ)
	AXSTA_DISCRETE,		//��ͨ�˶�״̬
	AXSTA_CONTINUOUS,	//�����˶�
	AXSTA_HOMING,		//���ڻ�ԭ��
	AXSTA_DISABLE,		//δ����״̬
	AXSTA_ERRSTOP,		//����ͣ
	AXSTA_SYNC,			//��ͬ��״̬
}AxStateTp;

//typedef enum	//��Ŷ���
//{
//    XMOTOR,
//    YMOTOR,
//    ZMOTOR,
//    R1MOTOR,
//    R2MOTOR,
//} AxisNum;

typedef enum	//�ᶯģʽѡ��
{
    ABSMODE,//0
    RELMODE,//1
    SPDMODE,//2
    GOHOMEMODE,//3
    STOPMODE,	//4
} MoveMode;


typedef enum	//�ᶯ����
{
    REALAXIS,    	//0   ʵ��
    ENCODERAXIS,	//1 ��������
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

#define CAMPointNum 500    //͹�ֵ���
typedef struct
{
//	float MasCirLength;			//���᳤��
//	u32	 MasCirPulse;				//����ÿȦ����
//					  
//	float	SlvCirLength;			//���᳤��
//	u32 SlvCirPulse;				//����ÿȦ����					    
//	float SynLength;				//ͬ��������
//	float MasCycLength;				//�ϳ�
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
	
	s32 MainStartPos;    //����͹�ֿ�ʼλ��
	s32 SlaveStartPos;   //����͹�ֿ�ʼλ��
	s32 MainSpeed;    //����͹�ֿ�ʼλ��
	s32 SlaveSpeed;   //����͹�ֿ�ʼλ��	
	
	s32 MainCurrPos;    //���ᵱǰλ��
	s32 SlaveCurrPos;   //���ᵱǰλ��
	
	s32 MainTargetPos;    //����Ŀ��λ��
	s32 SlaveTargetPos;   //����Ŀ��λ��	
	
	
	
	u32 RealMainPulse;  //ʵ�ʼ����������������
	u32 RealSlavePulse; //ʵ�ʼ�������Ĵ�������	
	//͹���������
	u32 MainCirPulse;				//����������
	u32 SlaveCirPulse;			//����������
	u32 SynPulse;					//ͬ��������
	s32 Ttime; 							//͹�ֵ���
	float SpeedMul;         //�ٶȱ��ʣ�����/����	
	u8 MainAxisMode;       //�������� 0��ʵ�� 1��������
	u8 MainEncodeNum;       //������������
	//end
	float MainAxisTPulse;   //����ȷ�������
	float SlaveAxisTPulse;   //����ͬ�����ȷ�������
	
	u32 MainPlus1;     //�����������ʱ����һ��͹�ֽ���
	u32 MainPlus2;     //�����������ʱ���ڶ������
	u8 TooLongFlag;    //���������־
	u8 LongEndFlag;    //���������־
	s32 LongStopPos;   //
	
	s32 yDif;     //����λ��ƫ��
	s32 yCyDif;     //��������ƫ��
	
	u32 TT[20];
	float Vel[20];
//	s32 MainAxisPos[500];  //͹�ֱ��
	s32 SlaveAxisPos[CAMPointNum];	//����͹�ֱ��
//	s32 MainAxisRelPos[500];	 //����ÿ���������������أ����������
	s32 SlaveAxisRelPos[CAMPointNum];	
	
	u32 mainAxSynSpeed;      //����ͬ���ٶ�
	u32 SlaveAxSynSpeed;		//����ͬ���ٶ�
	float SynSpeedRate;     //�����ٶȱ���
}CamTableDef;

typedef struct
{
	u8 excute;
	u8 step;
	u8 done;
	u8 busy;	
	
	s32 index;   	
	s32 indexTerm;
	
	s32 MainStartPos;    //����͹�ֿ�ʼλ��
	s32 SlaveStartPos;   //����͹�ֿ�ʼλ��

	
	s32 MainCurrPos;    //���ᵱǰλ��
	s32 SlaveCurrPos;   //���ᵱǰλ��
	s32 MainSpeed;    //�����ٶ�
	s32 SlaveSpeed;   //�����ٶ�
	
	u8 mType;   //�������� 0��������  1��������
	u8 mAx;     //����ID��
	u8 sAx;			//����ID��
	
	s32 Ttime; 							//͹�ֵ���
	float SpeedMul;         //�ٶȱ��ʣ�����/����	


	float MainAxisTPulse;   //����ȷ�������
	float SlaveAxisTPulse;   //����ͬ�����ȷ�������
	
	
	s32 flyStep;   //׷�����裺0��׷������1��ͬ������2��������
	s32 flyPos[3]; //ÿ���׶ε�λ��
	u8 flyEndflag; //ͬ��������־���ñ�־һ����������ͻ����ֹͣ
	
	s32 SlaveAxisPos[CAMPointNum];	//����͹�ֱ��
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
* @Description: �趨������λ��
* @param --  ���������
* @param --  �������趨λ��
* @return -- -1������ 1����ȷ
*/
extern s8 EncodeSetPos(u8 encodeNum,s32 pos);

/**
* @author 2019/5/23  NYZ
* @Description: �趨������λ��
* @param --  ���������
* @return -- -1������ 1����ȷ
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
	u8 mAx;  //����
	u8 sAx;  //����
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
	float rate;  //����
}GearInDataDef;



#define GEARDIV 50   //����ҧ�ϣ�������ֵ
//���ӳ��ֱ�ҧ��
extern s8 GearRun(GearInDataDef *G);
extern s8 GearIn(u8 mAx,u8 sAx,u32 mCylPlus,u32 sCylPlus,u32 mTgSpd,GearInDataDef *G);
extern s8 GearEnd(GearInDataDef *G);
extern s8 GearOut(GearInDataDef *G);



/*********************  ׷������ *************************************/
extern s8 FlyCutCamTableCaculate(FlyCutCamTableDef *Table);
extern s8 CamFlyCutRun(FlyCutCamTableDef *Table);
extern s8 FlyCutCamIn(float ratio,u8 mType,u8 mAx,u8 sAx,FlyCutCamTableDef *Table);
extern s8 FlyCutCamStop(FlyCutCamTableDef *Table);
extern s8 FlyCutCamGetSta(FlyCutCamTableDef *Table);
#endif
