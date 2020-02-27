#ifndef __IODEFINE_H
#define __IODEFINE_H

typedef enum	//轴号定义
{
    Axis_FeedFilm,							//	AXIS1   走纸轴
    Axis_Cutter,							//	AXIS2	辊刀轴
    Axis_FeedProduct,						//	AXIS3	送料轴
    Axis4,
    Axis5,
} AxisNum;

typedef enum	//轴号定义
{
    CutterEncode,							//	AXIS1	辊刀轴编码器
    FeedFilmEncode,							//	AXIS2	走纸轴编码器
    Encode2,
    FeedProductEncode,						//	AXIS3	送料轴编码器
} EncodeNum;

typedef enum	//输入口定义
{
    X1,  X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25,	X26, X27, X28, X29, X30, X31, X32,
} InPutNum;

typedef enum	//输入口定义
{
    Y1,  Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9, Y10, Y11, Y12, Y13, Y14, Y15, Y16, Y17, Y18, Y19, Y20, Y21, Y22, Y23, Y24, Y25,	Y26, Y27, Y28, Y29, Y30, Y31, Y32,
} OutNum;


#define LOCAL 0
#define CARD1 1
#define CARD2 2

//输出定义
#define Q_FeedMotorEn								LOCAL,Y1 	//送料变频启动
#define Q_TemperatureStart 					LOCAL,Y2		//端封温度控制
#define Q_MiddleClose	 							LOCAL,Y3		//中封电磁铁
#define Q_ServoAlmReset 						LOCAL,Y4 	//伺服报警清除
#define Q_Red												LOCAL,Y5	// 红灯
#define Q_Green											LOCAL,Y6	// 绿灯
#define Q_Yellow										LOCAL,Y7	// 黄灯
#define Q_SprayWater								LOCAL,Y8	// 喷酒精
#define Q_AirInflation1							LOCAL,Y9	// 充气1
#define Q_Code											LOCAL,Y10	// 打码
#define Q_FeedFilmMotorPos					LOCAL,Y11 	//走纸变频启动
#define Q_FeedFilmMotorNeg					LOCAL,Y12 	//走纸变频启动
#define Q_CutSign										LOCAL,Y13	// 切刀信号输出
#define Q_MarkSign									LOCAL,Y14	// 色标信号输出



//#define Q_AirInflation2							LOCAL,Y13	// 充气1
//#define Q_AirInflation3							LOCAL,Y14	// 充气1
//#define Q_AirInflation4							LOCAL,Y15	// 充气1

//输入定义
#define I_CutOrgin 									LOCAL,X1 	//切刀接近开关
#define I_FeedProductOrgin 							LOCAL,X2 	//送料接近开关
#define I_ColorMark									LOCAL,X3 	//走纸色标电眼
#define I_EmptyCheck								LOCAL,X4 	//防空包传感器
#define I_PreventCut								LOCAL,X5 	//防切料感应
#define I_TorqueAlm									LOCAL,X6 	//力矩到达

#define I_NoFilm										LOCAL,X8 	//无膜停机

#define I_SafeDoor									LOCAL,X10 	//安全门
#define I_StartBt										LOCAL,X11 	//开始按钮
#define I_StopBt	 									LOCAL,X12		//停止按钮
#define I_JogBt 										LOCAL,X13 	//点动按钮
#define I_EmergencyBt 							LOCAL,X14 	//急停按钮
#define I_ConnectSigle              LOCAL,X15 	//联机信号

//#define I_FeedProductOrgin					LOCAL,X15 	//理料位置检测传感器

#endif

