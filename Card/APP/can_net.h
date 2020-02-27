#ifndef __CAN_NET_H
#define __CAN_NET_H
#include "bsplib.h"

//16轴6648（无头）
//8轴3384

#define CAN_REC_TIME_OUT	15	//1.5ms(单位100us)
#define CAN_RESEND_TIMES	5	//重发次数

#define MAX_BOARD_NUM	10	//从板最大数量
#define	MAX_AXIS_NUM	40	//从板最大轴数量
#define	MAX_UNIT_NUM	((MAX_BOARD_NUM*(4*sizeof(OutPutType)+sizeof(BoardHeadType))+ MAX_AXIS_NUM*sizeof(AxisType))/sizeof(UnitObj) + 30)

#define FRAME_BID(id)	((id>>24)&0x1F)
#define FRAME_FUN(id)	((id>>23)&0x1)
#define FRAME_EPT(id)	((id>>19)&0xF)		//保留，空
#define FRAME_UID(id)	((id>>8)&0x7FF)	//11位，最2048
#define FRAME_SN(id)	((id>>0)&0xFF)

#define FRAME_ID		(((0x1f & m_bid)<<24)\
						|((0x1 & m_fun)<<23)\
						|((0x7ff & m_uid)<<8)\
						|((0xff & m_sn)<<0))

#define SDO_MAX		5
#define PDO_MAX		2
#define ODO_MAX		1


typedef enum {
    Cannet_State_Init 	= 1,		//初始化状态
    Cannet_State_Ready 	= 2,		//就绪状态，
    Cannet_State_Err 	= 3,		//错误状态
} Cannet_StateTp;				//CAN网络状态

typedef struct {
    u8			 		state;		//can状态,
    u8					Slave_Num;	//搜索到的从卡个数
    u8					Axis_Num;	//从轴个数（已搜索到的）（用于记录已经设置好轴数据的轴数量）
    u8					err_code;	//错误码0：OK，1：空间不足，2：未搜索到任何板卡 3：板卡类型不合法
    u32					Unit_Num;	//有效个数，搜索到的
    u32					CommErr;	//板卡通讯错误，每个位代表一个板卡错误
} Cannet_StatusType;		//单元类型

//扩展帧， 板ID（5bit）+ 对象ID(6bit) + 单元索引（5bit） + 功能ID(1bit) + 序列号（8bit） + 保留（4bit）
//功能码 funID	0:读，1：写
//对象ID，0:轴--I--Q--AD--DA,顺序排列
//单元索引	，每个对象最多64个数据
typedef enum {
    PDO = 1,		//过程类数据，轴的运行状态等，
    ODO = 2,		//其他轮询数据，不需要很及时，轴位置信息等(每10ms1条100个轴 1s)，
    SDO = 3,		//触发类，初始化数据，触发发送，输出
    SDO2 = 4,		//触发类，轴运动命令等，输出
} UnitType;		//单元类型
typedef enum {
    RO 	= 0,		//只读
    WO 	= 1,		//只写
//	RW 	= 2,		//读写
} UnitAttr;		//单元属性（RW）


typedef enum {
    Bt_NONR = 0,				//默认，无效
    Bt_5Axis,					//5轴板
    Bt_8Axis,					//8轴板
    Bt_12Axis,					//12轴板
    Bt_16Axis,					//16轴板
    Bt_5Axis_Pro,				//5轴板（fpga）
    Bt_8Axis_Pro,				//8轴板（fpga）
    Bt_12Axis_Pro,				//12轴板（fpga）
    Bt_16Axis_Pro,				//16轴板（fpga）
    Bt_ERR,						//无效(错误)ID
} BoardTp;		//单元属性（RW）


typedef struct {
    u8				Size;
    u8				Flg;			//更新标志1：触发2：正在发送(可以重复置1触发)
    u8				err;			//错误次数
    u8				bid;			//板id

//	u8				oid;			//对象ID
    u16				uid;			//单元ID(子索引)
    UnitType		Type;
    UnitAttr		Attr;

    u8				Data[8];		//最大8个字节
//	u8				ept[2];			//保留（4字节对齐）
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

//	UnitObj Move;			//包含运动信息（TorS(1bit(msb)),Mode(4bit(lsb)),spdlev(1Byte)pos(4Byte)）
    UnitObj Cmd;			//包含运动信息（pos(4Byte),Cmd(1Byte)),Spdlev(1Byte)，cmdsn(1Byte)））
} AxisType;

typedef struct
{
    UnitObj BoardType;	//data[0]:版类型，data[1]:轴num，data[2]:Inum，data[3]:Onum
    UnitObj reserved[3];
} BoardHeadType;


#define AxisTypeInitializer(bid)	{\
	/*size,flg,err,bid,uid,type,attr,data,ept*/\
	{6,0,0,bid,0,ODO,RO,{0}},/*CurPos + 2ByteErr 与轴错误定义一致*/\
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
	{7,0,0,bid,0,SDO2,WO,{0}} /*Move pos(4Byte),Mode(1Byte)),spdlev(1Byte),cmdsn(1Byte)）*/\
}

typedef struct
{
    UnitObj			Data;
} CmdSnType;		//每个板一个

#define CmdSnTypeTypeInitializer(bid,AxisNum)	{\
	{(3*AxisNum +7)/8,0,0,bid,0,PDO,RO,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}}\
}

typedef struct {
    UnitObj			Data;
} InPutType;		//

//最多64个
#define InPutTypeInitializer(bid,num)	{\
	{((num + 7)/8),0,0,bid,0,PDO,RO,{0}}\
}
typedef struct {
    UnitObj			Data;
} OutPutType;		//

//最多64个
#define OutPutTypeInitializer(bid,num)	{\
	{((num + 7)/8),0,0,bid,0,SDO,WO,{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}}\
}


typedef struct {
    UnitObj			Data;
} D32InPutType;		//

//最多64个
#define D32InPutTypeInitializer(bid)	{\
	{4,0,0,bid,0,PDO,RO,{0}}\
}
typedef struct {
    UnitObj			Data;
} D32OutPutType;		//

//最多64个
#define D32OutPutTypeInitializer(bid)	{\
	{4,0,0,bid,0,SDO,WO,{0}}\
}


//默认最大5个头，32 个轴，20个IO单元，包括普通IO和轴IO
typedef struct {
    UnitObj data[MAX_UNIT_NUM];
} CommDataDef;


typedef struct {
    int exec;
    int step;
} StepParaDef;

//typedef struct{
//	u32 flg;		//为1时为未发送，0为已发送完成
//	UnitObj* Unit;	//有效单元
//}TxUnitDef;
typedef struct {
    u8			Head;		//还未填充的数据
    u8			Tail;		//还未发的数据
    u8			Len;		//有效数据个数
    u8			ept;		//保留
    UnitObj* 	Data[5];	//有效单元
} TxListDef;

//#define UNIT_NUM (sizeof(CommData)/sizeof(UnitObj))

//extern CommDataDef CommData;



typedef struct
{
    AxisType* Data;			//数据结构体
    CmdSnType * CmdSnData;	//已完成的命令序号
    s32		Bid;			//所在板ID
    u8		LimSUpEn;		//阮上限使能
    u8		LimSDwEn;		//软下限
    u8		LimHUpEn;		//硬上限
    u8		LimHDwEn;		//硬下限
    s32		LimSUp;			//阮上限
    s32		LimSDw;			//软下限
    u8		LimHUp;			//硬上限信号
    u8		LimHDw;			//硬下限信号
    u8		LimHUpLev;		//硬上限信号
    u8		LimHDwLev;		//硬下限信号
    u8		Cmdsn;			//低三位有效
    u8		Axisn;			//在一块板上的轴序号
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
//下面是给外部调用的函数

//can网络需要轮询的函数
extern void can_netLoop(void);
//板卡通讯错误，每个位代表一个板卡错误
extern u32 HZ_ExCanComErrGet(void);
//只有当返回值为2（配置完成）后方可对扩展板进行操作
extern s32 HZ_ExCanNetStateGet(void);
//设置参数
extern s32 HZ_ExAxSetPara(u8 ExAxNum, \
                          u32 Start, u32 TAcc, u32 Run, u32 TDec, u32 End, \
                          u32 HomFast, u32 HomSlow, u32 HomOffset, \
                          u32 TorS, u32 HomTimeOut);
//设置回原点模式
//AxNum :轴号
//en	:使能
//Sig	:原点信号
//Mod	:模式
//		0：反向找原点
//		1：先正向找上限位，再反向找原点
//		2：先反向找下限位，再正向找原点
extern s32 HZ_ExAxSetHomMod(u8 ExAxNum, u8 En, u8 Sig, u8 Lev, u8 Mod);
//设置硬限位
extern s32 HZ_ExAxSetLimit_H(u8 ExAxNum, u8 en_Up, u8 sig_Up, u8 lev_Up, u8 en_Dw, u8 sig_Dw, u8 lev_Dw);
//设置软限位
extern s32 HZ_ExAxSetLimit_S(u8 ExAxNum, s32 pos_Up, u8 en_Up, s32 pos_Dw, u8 en_Dw);
//设置动作
extern s32 HZ_ExAxSetAct(u8 ExAxNum, s32 pos, ExAxActDef Cmd, u8 Spd);

//获取位置
extern s32 HZ_ExAxGetCurPos(u8 ExAxNum);
//获取轴（忙）状态
extern s32 HZ_ExAxGetExe(u8 ExAxNum);
//相对位置模式
extern void HZ_ExAxMoveRel(u8 ExAxNum, s32 pos);
//绝对位置模式
extern void HZ_ExAxMoveAbs(u8 ExAxNum, s32 pos);
//速度模式
extern void HZ_ExAxMoveVelocity(u8 ExAxNum, s32 pos);
;
extern void HZ_ExAxStopDec(u8 ExAxNum);
extern void HZ_ExAxStop(u8 ExAxNum);
extern void HZ_ExAxHome(u8 ExAxNum);
//获取错误状态
//return bit8: 硬上限bit9:软上限bit12：硬下限bit13：软下限
extern u32 HZ_ExAxGetErr(u8 ExAxNum);

//设置输出
//bid:板卡号（从0开始）
//num:第几个（从0开始）
//val:所要写入的值
//return:所要获取的值
extern s32 HZ_ExOutPutSet(u8 bid, u8 num, u8 val);
extern s32 HZ_ExOutPutGet(u8 bid, u8 num);
extern s32 HZ_ExEnSet(u8 bid, u8 num, u8 val);
extern s32 HZ_ExEnGet(u8 bid, u8 num);
extern s32 HZ_ExInPutGet(u8 bid, u8 num);
extern s32 HZ_ExAlmGet(u8 bid, u8 num);
extern s32 HZ_ExLimGet(u8 bid, u8 num);

//获取32位输入值
//bid:板卡号（从0开始）
//num:第几个（从0开始）
//val:所要写入的值
//return:所要获取的值
extern u32 HZ_ExD32IGet(u32 bid, u32 num);
//获取32位输出值
//bid:板卡号（从0开始）
//num:第几个（从0开始）
//val:所要写入的值
//return:所要获取的值
extern u32 HZ_ExD32OGet(u32 bid, u32 num);
//设置32位输出值
//bid:板卡号（从0开始）
//num:第几个（从0开始）
//val:所要写入的值
//return:所要获取的值
extern s32 HZ_ExD32OSet(u32 bid, u32 num, u32 val);


/****************************************************************************************************************************************/
#endif

