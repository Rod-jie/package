#ifndef _DATADEFINE_H_
#define _DATADEFINE_H_
#include "interlayer_config.h"
#include "logicdata.h"
#include "lock.h"
//#include "axismove.h"

#define DATACHAECK 0x01237654
#define SYSREADLEN 1000
#define USERREADLEN 1000
#define READLEN (SYSREADLEN + USERREADLEN)

#define SYSWRITELEN 1000
#define USERWRITELEN 1000
#define WRITELEN (SYSWRITELEN + USERWRITELEN)

#define SYSSAVELEN 4000
#define USERSAVELEN 4000
#define SAVELEN (SYSSAVELEN + USERSAVELEN + 4)

#define GLOBALDATALEN (READLEN + WRITELEN + SAVELEN)

#define MAXAXISNUM 50
#define LOCALAXISNUM 12
#define INPUTSTARTADDR 310
#define INPUTENDADDR 389
#define OUTPUTSTARTADDR 1020
#define OUTPUTENDADDR 1099
/************************************************************************/
/* 用户自定义的存储数据结构区，用户存储数据按照功能分成类存放 */
/************************************************************************/





/************************************************************************/
/* 此处之后的系统区不允许更改，用户只能更改用户区数据		           */
/************************************************************************/
typedef struct
{
    u8 hour;		//范围0-23
    u8 min;			//0-59
    u8 sec;			//0-59
    u8 ampm;		//这个值没用，禁止使用
} Time;

typedef struct
{
    s32 StartSpeed;	//初速度
    s32 Acctime;	   //加速时间
    s32 RunSpeed;	  //点动速度
    s32 Dectime;	   //减速时间
    s32 EndSpeed;	  //末速度
    s32 HomeSpeedFast; //回原点快速速度
    s32 HomeSpeedSlow; //回原点慢速速度
    s32 HomeOffset;	//回原点偏移坐标
    s32 SoftMinLimit;  //软负限位
    s32 SoftMaxLimit;  //软正限位
} AxisDef;
typedef struct
{
    u32 PPR;	//每转脉冲数
    float MPR;	//每转毫米__导程
} AxisConversion;
typedef struct
{
    u16 limitMode;		//限位模式：0 没限位 1 软件限位 2 硬件限位 3 软硬都限
    u16 Poslimit;    	//限位信号
    u16 Poslimitlev;    //限位信号电平
    u16 Neglimit;       //限位信号
    u16 Neglimitlev;    //限位信号电平
    u16 OrgNum;         //原点信号
    u16 Orglev;			//原点信号电平
    u16 HomeMode;		//回零模式
    u16 alarmmode;		//轴报警电平:0：低电平有效 1：高电平有效 其他：轴报警无效
    u16 res;			//
} AxisIOConfig;

typedef union
{
    u8 Data_8[GLOBALDATALEN];
    u16 Data_16[GLOBALDATALEN / 2];
    struct
    {
        struct //READ ADDR 0-999
        {
            union //SYS READ ADDR 0-499
            {
                u16 Data_16[SYSREADLEN / 2];
                struct
                {
//                    u32 HardWare_Ver[10];		  //0-19	硬件版本
//                    u32 SoftWare_Ver[10];		  //20-39	软件版本
                    union
                    {
                        u32 HardWare_Ver[10];		  //0-19	硬件版本
                        u8 Code[40];
                    } HardWare_Ver;

                    union
                    {
                        u32 HardWare_Ver[10];		  //0-19	硬件版本
                        u8 Code[40];
                    } SoftWare_Ver;

                    u32 res[5];                   //40-49
                    u16 AXSTA[MAXAXISNUM];		  //50-99	轴状态
                    float AxisUnitPos[MAXAXISNUM];  //100-199	轴当前用户单位位置
                    s32 AxisPosition[MAXAXISNUM]; //200-299	轴当前位置
                    u32 AxisEncoder[5];			  //300-309	轴当前状态
                    u32 InputStatus[40];		  //310-389	输入口状态
                    u32 ErrorCode[20];			  //390-	错误码
                    u32 ErrorLevel;				  //430-	错误等级
                    Time time;	//432	rtc 时间
                    Date date;	//434	rtc 日期
                    u32 CID[2];	//436 438 本机生成的客户随机码
                } Data;
            } Sys;

            union //USER READ ADDR 500-999
            {
                u16 Data_16[USERREADLEN / 2];
                struct
                {
                    u32 RunStatus;			//500
                    u32 HaveToReset;		//502
                    u32 HaveToRepower;		//504
                    s32 CurrPosPulse[5];   //506-515 当前脉冲位置
                    float CurrPosUU[5];		//516-525 当前位置 用户单位
                    s32 MainAxPosPP;      //526 主轴脉冲位置
                    s32 SlaveAxPosPP;			//528 从轴脉冲位置
                    s32 SlaveEncoderPP;   //530 从轴编码器当前脉冲位置
                    s32 CamCurrPos[2];		//532-535 凸轮当前脉冲位置
                    float MainAxPosUU;    //536 主轴位置
                    float SlaveAxPosUU;   //538 从轴位置
                    float SlaveEncoderUU;   //540 从轴编码器位置
                    float CamCurrPosUU[2];	//542-545 凸轮当前位置

                    float CutPosDif;         //546 切刀位置偏差
                    float LengthArr[2]; 			//548 - 550
                    int MarkCurrPos; 				//552 色标当前位置
                    int ProductCurrPos;  		//554 物料当前位置
                    int ProductLength;  		//556 物料当前位置
                    u32 CurrPackageSpd;     //558 实际包装速度  MM/Min
                    int FeedAxPosPP;        //560 送料当前位置
                } Data;
            } User;
        } Read;

        struct //WRITE ADDR 1000-1999
        {
            union //SYS WRITE ADDR 1000-1499
            {
                u16 Data_16[SYSWRITELEN / 2];
                struct
                {
                    u32 res[10];		  	//1000-1019 保留
                    u32 OutputStatus[40]; 	//1020-1099	输出口状态
                    u32 FlashOperate;	 	//1100-	Flash操作
                    u32 FilesOperate;	 	//1102-	文件操作
                    u32 FramOperate;	  	//1104-	铁电操作
                    u32 JogMode;		  	//1106-	点动模式
                    u32 JogPositionLevel; 	//1108-	点动行程级别
                    u32 JogPosition;	  	//1110-	点动设定行程
                    u32 JogForward[2];		//1112-1115	正向点动
                    u32 JogBackward[2];   	//1116-1119	反向点动
                    u32 JogGohome[2];	 	//1120-1123	点动回零
                    u32 JogStop[2];		  	//1124-1127	点动立即停止
                    u32 jogSpd;			  	//1128  轴点动百分比速度
                    u32 ClearAlarm;		  	//1130-	清除错误
                    struct
                    {
                        u32 refreshId;	//	1132 随机数刷新
                        s32 decode[4];	//1134 1136 1138 1140  4个解锁码 *6个数 一共24个数
                    } LockPara;
                    //end
                } Data;
            } Sys;

            union//USER WRITE ADDR 1500-1999
            {
                u16	Data_16[USERWRITELEN / 2];
                struct
                {
                    struct
                    {
                        u32 RunCommand;     //1500
                        u32 DeviceMode;		  //1502
                        u32 StepRun;			  //1504
                        u32 BitControlWord;	//1506
                        u32 term[6];        //
                    } button;

                    u32 TechWord;   //1520
                    u32 RunStatus; 	//1522
                    u32 Runing; 	//1524
                    float CutterDif;  //1526 切刀周期偏差
                    u16 CamCurveDraw;  //1528 画凸轮曲线
                    u16 CamPointNum;   //1529 凸轮曲线点数
                    u16 CamCurveDraw1;  //1530 画凸轮曲线
                    u16 CamPointNum1;   //1531 凸轮曲线点数
                    u32 HMI_pageChange;   //1532 上位机界面切换
                    u32 HMI_page;         //1534 上位机切换界面号
                    u16 TermCtl;          //1536 温度控制 0:打开  1：关闭
                    u16 term1;
                    GoPosPara AxisMove[PULS_NUM]; //1538
                } Data;
            } User;
        } Write;

        struct //SAVE ADDR 2000-
        {
            union //SYS SAVE ADDR 2000-3999
            {
                u16 Data_16[SYSSAVELEN / 2];
                struct
                {
                    AxisDef AxisData[MAXAXISNUM]; //2000-2999	50个轴，每个占20个寄存器
                    AxisConversion Conversion[MAXAXISNUM]; //3000 -3199 每转脉冲数
                    AxisIOConfig AxIOconfig[MAXAXISNUM];   //3200 - 3699
                    lockdataflashsave LockDate;	//3700 保存设置的锁机日期
                } Data;
            } Sys;

            union//USER SAVE ADDR 4000-5999
            {
                u16	Data_16[USERSAVELEN / 2];
                struct
                {
                    u32 PackageSpeed;			//4000 包装速度 包/分
                    s32 colorMarkPos; 		//4002 色标位置
                    s32 PackgeLength; 	//4004 包装袋长

                    s32 ProductPos; 				//4006 物料位置   物料间距
                    u32 JogSpeed; 				//4008 点动速度
                    s32 CutterStopAngle;  //4010 停刀角度

                    u16 ColorMarkEn;                 //4012 色标跟踪   0：启用 1：禁用
                    u16 CutProductEN;										//4013 防切料 0：关闭  1：打开
                    u16 ProductPosCheckEN;							//4014 物料错位检测 0：关闭  1：打开
                    u16 ScaldFilmEn;										//4015 防烫膜 0：关闭  1：打开
                    u16 EmptyCheckEn;									//4016 防空包 0：关闭  1：打开
                    u16 FastStopEn;     							//4017 快速启停 0:关闭 1：打开
                    u32 term1[6];       	//4018 - 4029

                    u32 ProductLength; 		 //4030 物料长度

                    u32 AlarmDelay;   //4032 报警时间

                    s32 CutPosAdjust;      //4034 切断位置微调
                    float RealPackgeLength;  //4036 实际袋长
                    u32 MiddleOpenDelay;    //4038 中封打开延时
                    s32 ProductDistance;     //4040 物料间距
                    u32 PackageSpeedMPS;   //4042 包装速度  M/Min
                    u32 NoProductDelay;    //4044 物料停机延时
                    u32 MiddleCloseDelay;    //4046 中封闭合延时
                    u32 term[9];           //4046 - 4065

                    struct
                    {
                        u32 SetNum;					//4066
                        u32 CurrNum;				//4068
                        u16 StopEn;					//4070 定量报警开关 0：启用  1：禁用
                        u16 term2;					//4071
                        u32 term[7];       	//4072 - 4085
                    } Product;

                    struct
                    {
                        AxisConversion FeedFilmAxisGearRate;   //4086 - 4089
                        AxisConversion FeedAxisGearRate;       // 4090-4093
                        u32 CutttorAxisEncodePPR;			   //4094
                        AxisConversion CutterAxisGearRate;       // 4096 4098
                    } AxisPara;


                    struct
                    {
                        //系统参数1
                        u32 CutterNum; 									//4100 切刀数量
                        u32 MinPackageSpeed; 						//4102 速度下限
                        u32 MaxPackageSpeed; 						//4104 速度上限
                        u32 MinPackageLength;	 					//4106 长度下限
                        u32 MaxPackageLength; 					//4108 长度上限
                        u32 MinDifPackageLength; 				//4110 不定长长度下限
                        u32 MaxDifPackageLength; 				//4112 不定长长度上限
                        float CutterSpeedCorrect; 			//4114 刀速修正
                        float CutterCloseAngle; 					//4116 闭合角度
                        u32 CutterEncoderResolution;  	//4118 编码器分辨率
                        u32 FirstFeedObjectCirc;				//4120 理料轴周长
                        float UnfeedSpeedMultiple ;   		//4122 出料倍数，对应0`10V模拟量输出的线性关系
                        //系统参数2
                        u16 JogMiddlePackageSwitch;			//4124 点动中封打开关闭选择
                        u16 MiddlePackageCtlMode;       //4125 中封控制，0：电磁铁  1：电机滚轮
                        u16 SpeedSetMode;  							//4126 速度方式 0：触摸屏给定 1：电位器给定
                        u16 FeedMotorSel; 	 						//4127 送料电机选择： 0：伺服电机  1：异步电机
                        u16 FeedFilmAxisSel; 						//4128 走纸电机选择 0：伺服电机  1：异步电机
                        u16 CutterStuckMode; 						//4129 切刀反转控制 0:力矩控制  1：位置控制
                        float EmptySensorToCutterDis;		//4130 防空包传感器至切刀的距离，MM
                        s32 CutterReversalAngle;				//4132 反转角度补偿
                        float CutterStuckSen;						//4134 切刀堵转灵敏度
                        u32 CutterReversalTimes;        //4136 切刀连续反转次数停机
                        u32 CutterEmptyTimes;						//4138 切刀自复位走纸空走个数
                        u32 LoseColorMarkTimes;					//4140 丢标报警次数设定
                        //系统参数3
                        u16 CuterrMode;                  //4142 设备类型选择 0：回转式  1：往返式
                        u16 ColorMarkEnTerm;                 //4143 色标跟踪   0：启用 1：禁用
                        float FirstBellSpeedCoe;				 //4144 第一皮带速度系数
                        float SecondBellSpeedCoe;				 //4146 第二皮带速度系数
                        u16 FirstBellMotorSel;					 //4148 第一段理料电机选择
                        u16 term4;											 //
                        u32 NumOfElseStation;						 //4150 理料单位个数，第一段理料皮带每一个工位的物料个数
                        u32 CutterTorqueCheckTime;				//4152 切刀扭矩检测时间
                        u32 SurplusPackage;               //4154 余料个数设定
                        u16 CutterRunMode;								//4156 切刀特性  0：匀速运行  1：电子凸轮运行
                        //系统参数4
                        u16 NoMaterialStop;								//4157 有料运行无料停止， 0：关闭  1：打开
                        u16 EmptyPackageReClose;					//4158 防空包中封提前闭合开关
                        u16 CutterTwoCamSwitch;      			//4159 切刀双凸轮开关
                        u32 TwoCamSpeed;    							//4160 双凸轮最小速度
                        u32 TwoCamAngleRange;							//4162 双凸轮角度范围
                        u16 DeviceMode; 									//4164 机型选择
                        u16 term2; 												//4165
                        //系统参数5
                        s32 CylStopButton;										//4166	循停按钮
                        s32 CutEncoderCyclePlusNum;  			//4168 裁剪式切刀编码器脉冲数
                        s32 CutSpeedRate;									//4170 裁切是切刀速度倍率
                        s32 FeedPackagingFilmHoldLength; 	//4172 裁剪式走纸暂停长度
                        s32 ObjectPosSen;									//4174 裁切式物料位置补偿长度
                        s32 UnfeedMotorHoldLength;  			//4176 裁切式是出料电机暂停长度
                        s32 SensorDistance; 							//4178 裁切式传感器距离
                        s32 AirInflationDelay;  					//4180 裁切式充气时间
                        s32 CutterCloseDelay;   					//4182 裁剪式切刀闭合时间
                        u32 CutterOnDelay;								//4184 裁剪式切刀气缸输出延时1
                        u32 CutterOffDelay;								//4186 裁剪式切刀气缸输出延时2
                        u16 CloseMotorSel;     						//4188 裁剪式端封电机选择 0：伺服电机 1：交流电机
                        u16 FeedAlmIsStop;     						//4189 进料报警选择：0：运行
                        u32 CutCylinderAlmDelay;       		//4190 裁剪式切刀气缸报警时间
                        u32 SensorCheckDistance;   				//4192 传感器抗干扰长度
                        u32 LableLength;    							//4194 单位标签长度
                        u32 CamPNum;											//4196 凸轮点数
                        u16 CodePrintEn; 									//4198 打码启用  0：关闭  1：打开
                        u16 BlowEn;      									//4199 吹气启用  0：关闭  1：打开
                        s32 CodePrintPos;       					//4200 打码位置
                        s32 BlowAngle[4];										//4202-4208  吹气角度
                        s32 BlowLength[4];									//4210-4216 吹气长度
                        u32 ScaldFilmDelay;									//4218 防烫膜延时
                        u16 CutProductENTerm;										//4220 防切料 0：关闭  1：打开
                        u16 ProductPosCheckENTerm;							//4221 物料错位检测 0：关闭  1：打开
                        float ProductLength;        				// 4222 物料长度
                        float EmptyLength;          				// 4224 防切空白长度
                        u16 ScaldFilmEnTerm;										//4226 防烫膜 0：关闭  1：打开
                        u16 Term;														//4227
                        float ScaldFilmAngle;								//4228 防烫膜角度范围
                        u16 CutterAxisEn;									//4230 走纸电机禁用 0：关闭 1：打开
                        u16 FeedAxisEn;											//4231 送料电机禁用 0：关闭 1：打开
                        u16 CutterOrgAlmEn;									//4232 切刀接近开关报警 0：关闭 1：打开
                        u16 FeedFilmOrgAlmEn;								//4233 走纸光电开关报警 0：关闭 1：打开
                        u16 FeedOrgAlmEn;										//4234 送料接近开关报警 0：关闭 1：打开
                        u16 ConnectMode;										//4235 联机标志    1：联机
                        s32 MarkReactionTime; 							//4236 色标反应时间 MS
                        u32 MaxDifPackageSpeed; 						//4238 不定长速度上限
                        s32 EmptyCount;                     //4240 空料个数
                        s32 TorqueCheckAngleMin;			//4242 力矩检测角度范围Min
                        s32 TorqueCheckAngleMax;			//4244 力矩检测角度范围Max

                        u16 SprayWaterEn; 									//4246 喷酒精启用  0：关闭  1：打开
                        u16 termU16_1;      									//4247
                        s32 SprayWaterPos;       					//4248 喷酒精位置
                        u32 SprayWaterDelay;       					//4250 喷酒精延时
                        u32 PassPackageNum;        					//4252 力矩防切过料数量
                        s32 CutterHomeOffset;       				//4254 切刀回原点偏移,单位： 1度
                    } SysPara;
                    u32 term3[16];											//4256 - 4286
                    struct
                    {
                        u32 Packagespeed1;							//4288 包装速度1
                        u32 Packagespeed2;							//4290 包装速度2
                        float MarkDis;                  //4292 色标偏差，在两种不同
                        float MarkReactionTime;         //4294 色标相应时间
                        u32 PackageLTest;               //4296 测试袋长
                        u32 TestSpeed;                     //4298 测试速度
                        s32 CutterCyclePlusNum;   				//4300 切刀周期脉冲量
                        s32 CutterEncoderCyclePlusNum;   	//4302 切刀编码器周期脉冲量
                        s32 FeedPackagingFilmPlusNum;    	//4304 走纸脉冲量
                        s32 FeedProductPlusNum; 						//4306 送料脉冲量
                        float FeedPackagingFilmSpeedStd;    //4308 走纸速度基准
                        float FeedSpeedStd;    				//4310 送料速度基准
                        float FirstFeedSpeedStd;  		//4312 第一理料速度基准
                    } TestPara;
                    u32 CamTablePointNum;  								//4314
                    u16 CamCurve[500];										//4316 - 4815
                    u16 Ymin;															//4816
                    u16 Ymax;															//4817
                    u16 YTable[500];										//4819 - 5317
                    u16 FeedStartPos;					//5318
                    u16 FeedEndPos;						//5320

                    s32 FeedAxPosTerm;          				//4254 送料位置暂存，上电不会原点
                    s32 FeedFilmAxPosTerm;          				//4256 送膜位置暂存，上电不会原点
                    s32 CutterAxPosTerm;          				//4258 	切刀位置暂存，上电不会原点
                    s32 CutterEncoderPosTerm;          				//4260 切刀编码器位置暂存，上电不会原点
                    s32 CamExcuteStatusTerm;						//暂存凸轮状态
                    s32 IsReset;    //系统上电是否需要复位
                    s32 GearSCurrPosPP;  //从轴同步位置，断电暂存
                    s32 GearMCurrPosPP;  //主轴同步位置，断电暂存
                    u32 GearStatus ;   //同步标志，断电暂存
                } Data;
            } User;
        } Save;
        u32 Check;
    } SpaceType;
} GlobalDataDef;
extern GlobalDataDef GlobalData;

#define GSR GlobalData.SpaceType.Read.Sys.Data
#define GSW GlobalData.SpaceType.Write.Sys.Data
#define GSS GlobalData.SpaceType.Save.Sys.Data
#define GUR GlobalData.SpaceType.Read.User.Data
#define GUW GlobalData.SpaceType.Write.User.Data
#define GUS GlobalData.SpaceType.Save.User.Data

#endif
