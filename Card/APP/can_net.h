#ifndef __CAN_NET_H
#define __CAN_NET_H
#include "bsplib.h"

//16��6648����ͷ��
//8��3384

#define CAN_REC_TIME_OUT	15	//1.5ms(��λ100us)
#define CAN_RESEND_TIMES	5	//�ط�����

#define MAX_BOARD_NUM	10	//�Ӱ��������
#define	MAX_AXIS_NUM	40	//�Ӱ����������
#define	MAX_UNIT_NUM	((MAX_BOARD_NUM*(4*sizeof(OutPutType)+sizeof(BoardHeadType))+ MAX_AXIS_NUM*sizeof(AxisType))/sizeof(UnitObj) + 30)

#define FRAME_BID(id)	((id>>24)&0x1F)
#define FRAME_FUN(id)	((id>>23)&0x1)
#define FRAME_EPT(id)	((id>>19)&0xF)		//��������
#define FRAME_UID(id)	((id>>8)&0x7FF)	//11λ����2048
#define FRAME_SN(id)	((id>>0)&0xFF)

#define FRAME_ID		(((0x1f & m_bid)<<24)\
						|((0x1 & m_fun)<<23)\
						|((0x7ff & m_uid)<<8)\
						|((0xff & m_sn)<<0))

#define SDO_MAX		5
#define PDO_MAX		2
#define ODO_MAX		1


typedef enum {
    Cannet_State_Init 	= 1,		//��ʼ��״̬
    Cannet_State_Ready 	= 2,		//����״̬��
    Cannet_State_Err 	= 3,		//����״̬
} Cannet_StateTp;				//CAN����״̬

typedef struct {
    u8			 		state;		//can״̬,
    u8					Slave_Num;	//�������Ĵӿ�����
    u8					Axis_Num;	//������������������ģ������ڼ�¼�Ѿ����ú������ݵ���������
    u8					err_code;	//������0��OK��1���ռ䲻�㣬2��δ�������κΰ忨 3���忨���Ͳ��Ϸ�
    u32					Unit_Num;	//��Ч��������������
    u32					CommErr;	//�忨ͨѶ����ÿ��λ����һ���忨����
} Cannet_StatusType;		//��Ԫ����

//��չ֡�� ��ID��5bit��+ ����ID(6bit) + ��Ԫ������5bit�� + ����ID(1bit) + ���кţ�8bit�� + ������4bit��
//������ funID	0:����1��д
//����ID��0:��--I--Q--AD--DA,˳������
//��Ԫ����	��ÿ���������64������
typedef enum {
    PDO = 1,		//���������ݣ��������״̬�ȣ�
    ODO = 2,		//������ѯ���ݣ�����Ҫ�ܼ�ʱ����λ����Ϣ��(ÿ10ms1��100���� 1s)��
    SDO = 3,		//�����࣬��ʼ�����ݣ��������ͣ����
    SDO2 = 4,		//�����࣬���˶�����ȣ����
} UnitType;		//��Ԫ����
typedef enum {
    RO 	= 0,		//ֻ��
    WO 	= 1,		//ֻд
//	RW 	= 2,		//��д
} UnitAttr;		//��Ԫ���ԣ�RW��


typedef enum {
    Bt_NONR = 0,				//Ĭ�ϣ���Ч
    Bt_5Axis,					//5���
    Bt_8Axis,					//8���
    Bt_12Axis,					//12���
    Bt_16Axis,					//16���
    Bt_5Axis_Pro,				//5��壨fpga��
    Bt_8Axis_Pro,				//8��壨fpga��
    Bt_12Axis_Pro,				//12��壨fpga��
    Bt_16Axis_Pro,				//16��壨fpga��
    Bt_ERR,						//��Ч(����)ID
} BoardTp;		//��Ԫ���ԣ�RW��


typedef struct {
    u8				Size;
    u8				Flg;			//���±�־1������2�����ڷ���(�����ظ���1����)
    u8				err;			//�������
    u8				bid;			//��id

//	u8				oid;			//����ID
    u16				uid;			//��ԪID(������)
    UnitType		Type;
    UnitAttr		Attr;

    u8				Data[8];		//���8���ֽ�
//	u8				ept[2];			//������4�ֽڶ��룩
} UnitObj;


typedef struct
{
    UnitObj CurPos;
    UnitObj Start;
    UnitObj TAcc;
    UnitObj Run;
    UnitObj TDec;
    UnitObj End;
    UnitObj TorS;
//	UnitObj HomeMod;
    UnitObj HomFast;
    UnitObj HomSlow;
    UnitObj HomOffSet;
    UnitObj HomTimeOut;
    UnitObj HomAttr;			//u8 en,u8 sig,u8 siglev,u8 Mod

    UnitObj LimUp_H;		//u8 en,u8 sig,u8 lev
    UnitObj LimDw_H;		//u8 en,u8 sig,u8 lev
    UnitObj LimUp_S;		//s32 pos,u8 en
    UnitObj LimDw_S;		//s32 pos,u8 en

//	UnitObj Move;			//�����˶���Ϣ��TorS(1bit(msb)),Mode(4bit(lsb)),spdlev(1Byte)pos(4Byte)��
    UnitObj Cmd;			//�����˶���Ϣ��pos(4Byte),Cmd(1Byte)),Spdlev(1Byte)��cmdsn(1Byte)����
} AxisType;

typedef struct
{
    UnitObj BoardType;	//data[0]:�����ͣ�data[1]:��num��data[2]:Inum��data[3]:Onum
    UnitObj reserved[3];
} BoardHeadType;


#define AxisTypeInitializer(bid)	{\
	/*size,flg,err,bid,uid,type,attr,data,ept*/\
	{6,0,0,bid,0,ODO,RO,{0}},/*CurPos + 2ByteErr ���������һ��*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*StartSpd*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*TAcc*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*RunSpd*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*TDec*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*EndSpd*/\
	{1,0,0,bid,0,SDO,WO,{0}},/*TorS*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*HmSpdFast*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*HmSpdSlow*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*HmOffSet*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*HmTimeOut*/\
	{4,0,0,bid,0,SDO,WO,{0}},/*HmAttr,u8 en,u8 sig,u8 siglev,u8 Mod*/\
	{3,0,0,bid,0,SDO,WO,{0}},/*LimUp_H,u8 en,u8 sig,u8 lev*/\
	{3,0,0,bid,0,SDO,WO,{0}},/*LimDw_H,u8 en,u8 sig,u8 lev*/\
	{5,0,0,bid,0,SDO,WO,{0}},/*LimUp_S,s32 pos,u8 en*/\
	{5,0,0,bid,0,SDO,WO,{0}},/*LimDw_S,s32 pos,u8 en*/\
	{7,0,0,bid,0,SDO2,WO,{0}} /*Move pos(4Byte),Mode(1Byte)),spdlev(1Byte),cmdsn(1Byte)��*/\
}

typedef struct
{
    UnitObj			Data;
} CmdSnType;		//ÿ����һ��

#define CmdSnTypeTypeInitializer(bid,AxisNum)	{\
	{(3*AxisNum +7)/8,0,0,bid,0,PDO,RO,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}}\
}

typedef struct {
    UnitObj			Data;
} InPutType;		//

//���64��
#define InPutTypeInitializer(bid,num)	{\
	{((num + 7)/8),0,0,bid,0,PDO,RO,{0}}\
}
typedef struct {
    UnitObj			Data;
} OutPutType;		//

//���64��
#define OutPutTypeInitializer(bid,num)	{\
	{((num + 7)/8),0,0,bid,0,SDO,WO,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}}\
}


typedef struct {
    UnitObj			Data;
} D32InPutType;		//

//���64��
#define D32InPutTypeInitializer(bid)	{\
	{4,0,0,bid,0,PDO,RO,{0}}\
}
typedef struct {
    UnitObj			Data;
} D32OutPutType;		//

//���64��
#define D32OutPutTypeInitializer(bid)	{\
	{4,0,0,bid,0,SDO,WO,{0}}\
}


//Ĭ�����5��ͷ��32 ���ᣬ20��IO��Ԫ��������ͨIO����IO
typedef struct {
    UnitObj data[MAX_UNIT_NUM];
} CommDataDef;


typedef struct {
    int exec;
    int step;
} StepParaDef;

//typedef struct{
//	u32 flg;		//Ϊ1ʱΪδ���ͣ�0Ϊ�ѷ������
//	UnitObj* Unit;	//��Ч��Ԫ
//}TxUnitDef;
typedef struct {
    u8			Head;		//��δ��������
    u8			Tail;		//��δ��������
    u8			Len;		//��Ч���ݸ���
    u8			ept;		//����
    UnitObj* 	Data[5];	//��Ч��Ԫ
} TxListDef;

//#define UNIT_NUM (sizeof(CommData)/sizeof(UnitObj))

//extern CommDataDef CommData;



typedef struct
{
    AxisType* Data;			//���ݽṹ��
    CmdSnType * CmdSnData;	//����ɵ��������
    s32		Bid;			//���ڰ�ID
    u8		LimSUpEn;		//������ʹ��
    u8		LimSDwEn;		//������
    u8		LimHUpEn;		//Ӳ����
    u8		LimHDwEn;		//Ӳ����
    s32		LimSUp;			//������
    s32		LimSDw;			//������
    u8		LimHUp;			//Ӳ�����ź�
    u8		LimHDw;			//Ӳ�����ź�
    u8		LimHUpLev;		//Ӳ�����ź�
    u8		LimHDwLev;		//Ӳ�����ź�
    u8		Cmdsn;			//����λ��Ч
    u8		Axisn;			//��һ����ϵ������
} AxListDef;

typedef struct
{
    InPutType * Gpi;
    InPutType * Axi;
    OutPutType * Gpo;
    OutPutType * Axo;
} IOListDef;
typedef struct
{
    u8 BoardType;
    u8 Anum;
    u8 Inum;
    u8 Onum;
    u8 D32Inum;
    u8 D32Onum;
    UnitObj * start;
    D32InPutType	*D32IHead;
    D32OutPutType	*D32OHead;
} BoardListDef;

//extern AxListDef AxList[];


typedef struct
{
    Cannet_StatusType	Cannet_Status;
    BoardListDef		BoardList[MAX_BOARD_NUM];
    AxListDef			AxList[MAX_AXIS_NUM];
    IOListDef			IOList[MAX_BOARD_NUM];
    CommDataDef 		CommData;
    TxListDef 			PDOList;
    TxListDef 			SDOList;
    TxListDef 			ODOList;
} CanNetDataDef;

//Cmd:
typedef enum {
    ABSCMD = 1,
    RELCMD,
    SPDCMD,
    HOMCMD,
    STOPDECCMD = 0X80,
    STOPCMD = 0X90,
} ExAxActDef;

//extern StepParaDef CAN_send_onePara;
extern s32  HZ_ExAxGetStatus(u8 ExAxNum);


//20190515
/****************************************************************************************************************************************/
//�����Ǹ��ⲿ���õĺ���

//can������Ҫ��ѯ�ĺ���
extern void can_netLoop(void);
//�忨ͨѶ����ÿ��λ����һ���忨����
extern u32 HZ_ExCanComErrGet(void);
//ֻ�е�����ֵΪ2��������ɣ��󷽿ɶ���չ����в���
extern s32 HZ_ExCanNetStateGet(void);
//���ò���
extern s32 HZ_ExAxSetPara(u8 ExAxNum, \
                          u32 Start, u32 TAcc, u32 Run, u32 TDec, u32 End, \
                          u32 HomFast, u32 HomSlow, u32 HomOffset, \
                          u32 TorS, u32 HomTimeOut);
//���û�ԭ��ģʽ
//AxNum :���
//en	:ʹ��
//Sig	:ԭ���ź�
//Mod	:ģʽ
//		0��������ԭ��
//		1��������������λ���ٷ�����ԭ��
//		2���ȷ���������λ����������ԭ��
extern s32 HZ_ExAxSetHomMod(u8 ExAxNum, u8 En, u8 Sig, u8 Lev, u8 Mod);
//����Ӳ��λ
extern s32 HZ_ExAxSetLimit_H(u8 ExAxNum, u8 en_Up, u8 sig_Up, u8 lev_Up, u8 en_Dw, u8 sig_Dw, u8 lev_Dw);
//��������λ
extern s32 HZ_ExAxSetLimit_S(u8 ExAxNum, s32 pos_Up, u8 en_Up, s32 pos_Dw, u8 en_Dw);
//���ö���
extern s32 HZ_ExAxSetAct(u8 ExAxNum, s32 pos, ExAxActDef Cmd, u8 Spd);

//��ȡλ��
extern s32 HZ_ExAxGetCurPos(u8 ExAxNum);
//��ȡ�ᣨæ��״̬
extern s32 HZ_ExAxGetExe(u8 ExAxNum);
//���λ��ģʽ
extern void HZ_ExAxMoveRel(u8 ExAxNum, s32 pos);
//����λ��ģʽ
extern void HZ_ExAxMoveAbs(u8 ExAxNum, s32 pos);
//�ٶ�ģʽ
extern void HZ_ExAxMoveVelocity(u8 ExAxNum, s32 pos);
;
extern void HZ_ExAxStopDec(u8 ExAxNum);
extern void HZ_ExAxStop(u8 ExAxNum);
extern void HZ_ExAxHome(u8 ExAxNum);
//��ȡ����״̬
//return bit8: Ӳ����bit9:������bit12��Ӳ����bit13��������
extern u32 HZ_ExAxGetErr(u8 ExAxNum);

//�������
//bid:�忨�ţ���0��ʼ��
//num:�ڼ�������0��ʼ��
//val:��Ҫд���ֵ
//return:��Ҫ��ȡ��ֵ
extern s32 HZ_ExOutPutSet(u8 bid, u8 num, u8 val);
extern s32 HZ_ExOutPutGet(u8 bid, u8 num);
extern s32 HZ_ExEnSet(u8 bid, u8 num, u8 val);
extern s32 HZ_ExEnGet(u8 bid, u8 num);
extern s32 HZ_ExInPutGet(u8 bid, u8 num);
extern s32 HZ_ExAlmGet(u8 bid, u8 num);
extern s32 HZ_ExLimGet(u8 bid, u8 num);

//��ȡ32λ����ֵ
//bid:�忨�ţ���0��ʼ��
//num:�ڼ�������0��ʼ��
//val:��Ҫд���ֵ
//return:��Ҫ��ȡ��ֵ
extern u32 HZ_ExD32IGet(u32 bid, u32 num);
//��ȡ32λ���ֵ
//bid:�忨�ţ���0��ʼ��
//num:�ڼ�������0��ʼ��
//val:��Ҫд���ֵ
//return:��Ҫ��ȡ��ֵ
extern u32 HZ_ExD32OGet(u32 bid, u32 num);
//����32λ���ֵ
//bid:�忨�ţ���0��ʼ��
//num:�ڼ�������0��ʼ��
//val:��Ҫд���ֵ
//return:��Ҫ��ȡ��ֵ
extern s32 HZ_ExD32OSet(u32 bid, u32 num, u32 val);


/****************************************************************************************************************************************/
#endif

