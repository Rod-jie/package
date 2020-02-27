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
/* �û��Զ���Ĵ洢���ݽṹ�����û��洢���ݰ��չ��ֳܷ����� */
/************************************************************************/





/************************************************************************/
/* �˴�֮���ϵͳ����������ģ��û�ֻ�ܸ����û�������		           */
/************************************************************************/
typedef struct
{
    u8 hour;		//��Χ0-23
    u8 min;			//0-59
    u8 sec;			//0-59
    u8 ampm;		//���ֵû�ã���ֹʹ��
} Time;

typedef struct
{
    s32 StartSpeed;	//���ٶ�
    s32 Acctime;	   //����ʱ��
    s32 RunSpeed;	  //�㶯�ٶ�
    s32 Dectime;	   //����ʱ��
    s32 EndSpeed;	  //ĩ�ٶ�
    s32 HomeSpeedFast; //��ԭ������ٶ�
    s32 HomeSpeedSlow; //��ԭ�������ٶ�
    s32 HomeOffset;	//��ԭ��ƫ������
    s32 SoftMinLimit;  //����λ
    s32 SoftMaxLimit;  //������λ
} AxisDef;
typedef struct
{
    u32 PPR;	//ÿת������
    float MPR;	//ÿת����__����
} AxisConversion;
typedef struct
{
    u16 limitMode;		//��λģʽ��0 û��λ 1 �����λ 2 Ӳ����λ 3 ��Ӳ����
    u16 Poslimit;    	//��λ�ź�
    u16 Poslimitlev;    //��λ�źŵ�ƽ
    u16 Neglimit;       //��λ�ź�
    u16 Neglimitlev;    //��λ�źŵ�ƽ
    u16 OrgNum;         //ԭ���ź�
    u16 Orglev;			//ԭ���źŵ�ƽ
    u16 HomeMode;		//����ģʽ
    u16 alarmmode;		//�ᱨ����ƽ:0���͵�ƽ��Ч 1���ߵ�ƽ��Ч �������ᱨ����Ч
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
//                    u32 HardWare_Ver[10];		  //0-19	Ӳ���汾
//                    u32 SoftWare_Ver[10];		  //20-39	����汾
                    union
                    {
                        u32 HardWare_Ver[10];		  //0-19	Ӳ���汾
                        u8 Code[40];
                    } HardWare_Ver;

                    union
                    {
                        u32 HardWare_Ver[10];		  //0-19	Ӳ���汾
                        u8 Code[40];
                    } SoftWare_Ver;

                    u32 res[5];                   //40-49
                    u16 AXSTA[MAXAXISNUM];		  //50-99	��״̬
                    float AxisUnitPos[MAXAXISNUM];  //100-199	�ᵱǰ�û���λλ��
                    s32 AxisPosition[MAXAXISNUM]; //200-299	�ᵱǰλ��
                    u32 AxisEncoder[5];			  //300-309	�ᵱǰ״̬
                    u32 InputStatus[40];		  //310-389	�����״̬
                    u32 ErrorCode[20];			  //390-	������
                    u32 ErrorLevel;				  //430-	����ȼ�
                    Time time;	//432	rtc ʱ��
                    Date date;	//434	rtc ����
                    u32 CID[2];	//436 438 �������ɵĿͻ������
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
                    s32 CurrPosPulse[5];   //506-515 ��ǰ����λ��
                    float CurrPosUU[5];		//516-525 ��ǰλ�� �û���λ
                    s32 MainAxPosPP;      //526 ��������λ��
                    s32 SlaveAxPosPP;			//528 ��������λ��
                    s32 SlaveEncoderPP;   //530 �����������ǰ����λ��
                    s32 CamCurrPos[2];		//532-535 ͹�ֵ�ǰ����λ��
                    float MainAxPosUU;    //536 ����λ��
                    float SlaveAxPosUU;   //538 ����λ��
                    float SlaveEncoderUU;   //540 ���������λ��
                    float CamCurrPosUU[2];	//542-545 ͹�ֵ�ǰλ��

                    float CutPosDif;         //546 �е�λ��ƫ��
                    float LengthArr[2]; 			//548 - 550
                    int MarkCurrPos; 				//552 ɫ�굱ǰλ��
                    int ProductCurrPos;  		//554 ���ϵ�ǰλ��
                    int ProductLength;  		//556 ���ϵ�ǰλ��
                    u32 CurrPackageSpd;     //558 ʵ�ʰ�װ�ٶ�  MM/Min
                    int FeedAxPosPP;        //560 ���ϵ�ǰλ��
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
                    u32 res[10];		  	//1000-1019 ����
                    u32 OutputStatus[40]; 	//1020-1099	�����״̬
                    u32 FlashOperate;	 	//1100-	Flash����
                    u32 FilesOperate;	 	//1102-	�ļ�����
                    u32 FramOperate;	  	//1104-	�������
                    u32 JogMode;		  	//1106-	�㶯ģʽ
                    u32 JogPositionLevel; 	//1108-	�㶯�г̼���
                    u32 JogPosition;	  	//1110-	�㶯�趨�г�
                    u32 JogForward[2];		//1112-1115	����㶯
                    u32 JogBackward[2];   	//1116-1119	����㶯
                    u32 JogGohome[2];	 	//1120-1123	�㶯����
                    u32 JogStop[2];		  	//1124-1127	�㶯����ֹͣ
                    u32 jogSpd;			  	//1128  ��㶯�ٷֱ��ٶ�
                    u32 ClearAlarm;		  	//1130-	�������
                    struct
                    {
                        u32 refreshId;	//	1132 �����ˢ��
                        s32 decode[4];	//1134 1136 1138 1140  4�������� *6���� һ��24����
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
                    float CutterDif;  //1526 �е�����ƫ��
                    u16 CamCurveDraw;  //1528 ��͹������
                    u16 CamPointNum;   //1529 ͹�����ߵ���
                    u16 CamCurveDraw1;  //1530 ��͹������
                    u16 CamPointNum1;   //1531 ͹�����ߵ���
                    u32 HMI_pageChange;   //1532 ��λ�������л�
                    u32 HMI_page;         //1534 ��λ���л������
                    u16 TermCtl;          //1536 �¶ȿ��� 0:��  1���ر�
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
                    AxisDef AxisData[MAXAXISNUM]; //2000-2999	50���ᣬÿ��ռ20���Ĵ���
                    AxisConversion Conversion[MAXAXISNUM]; //3000 -3199 ÿת������
                    AxisIOConfig AxIOconfig[MAXAXISNUM];   //3200 - 3699
                    lockdataflashsave LockDate;	//3700 �������õ���������
                } Data;
            } Sys;

            union//USER SAVE ADDR 4000-5999
            {
                u16	Data_16[USERSAVELEN / 2];
                struct
                {
                    u32 PackageSpeed;			//4000 ��װ�ٶ� ��/��
                    s32 colorMarkPos; 		//4002 ɫ��λ��
                    s32 PackgeLength; 	//4004 ��װ����

                    s32 ProductPos; 				//4006 ����λ��   ���ϼ��
                    u32 JogSpeed; 				//4008 �㶯�ٶ�
                    s32 CutterStopAngle;  //4010 ͣ���Ƕ�

                    u16 ColorMarkEn;                 //4012 ɫ�����   0������ 1������
                    u16 CutProductEN;										//4013 ������ 0���ر�  1����
                    u16 ProductPosCheckEN;							//4014 ���ϴ�λ��� 0���ر�  1����
                    u16 ScaldFilmEn;										//4015 ����Ĥ 0���ر�  1����
                    u16 EmptyCheckEn;									//4016 ���հ� 0���ر�  1����
                    u16 FastStopEn;     							//4017 ������ͣ 0:�ر� 1����
                    u32 term1[6];       	//4018 - 4029

                    u32 ProductLength; 		 //4030 ���ϳ���

                    u32 AlarmDelay;   //4032 ����ʱ��

                    s32 CutPosAdjust;      //4034 �ж�λ��΢��
                    float RealPackgeLength;  //4036 ʵ�ʴ���
                    u32 MiddleOpenDelay;    //4038 �з����ʱ
                    s32 ProductDistance;     //4040 ���ϼ��
                    u32 PackageSpeedMPS;   //4042 ��װ�ٶ�  M/Min
                    u32 NoProductDelay;    //4044 ����ͣ����ʱ
                    u32 MiddleCloseDelay;    //4046 �з�պ���ʱ
                    u32 term[9];           //4046 - 4065

                    struct
                    {
                        u32 SetNum;					//4066
                        u32 CurrNum;				//4068
                        u16 StopEn;					//4070 ������������ 0������  1������
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
                        //ϵͳ����1
                        u32 CutterNum; 									//4100 �е�����
                        u32 MinPackageSpeed; 						//4102 �ٶ�����
                        u32 MaxPackageSpeed; 						//4104 �ٶ�����
                        u32 MinPackageLength;	 					//4106 ��������
                        u32 MaxPackageLength; 					//4108 ��������
                        u32 MinDifPackageLength; 				//4110 ��������������
                        u32 MaxDifPackageLength; 				//4112 ��������������
                        float CutterSpeedCorrect; 			//4114 ��������
                        float CutterCloseAngle; 					//4116 �պϽǶ�
                        u32 CutterEncoderResolution;  	//4118 �������ֱ���
                        u32 FirstFeedObjectCirc;				//4120 �������ܳ�
                        float UnfeedSpeedMultiple ;   		//4122 ���ϱ�������Ӧ0`10Vģ������������Թ�ϵ
                        //ϵͳ����2
                        u16 JogMiddlePackageSwitch;			//4124 �㶯�з�򿪹ر�ѡ��
                        u16 MiddlePackageCtlMode;       //4125 �з���ƣ�0�������  1���������
                        u16 SpeedSetMode;  							//4126 �ٶȷ�ʽ 0������������ 1����λ������
                        u16 FeedMotorSel; 	 						//4127 ���ϵ��ѡ�� 0���ŷ����  1���첽���
                        u16 FeedFilmAxisSel; 						//4128 ��ֽ���ѡ�� 0���ŷ����  1���첽���
                        u16 CutterStuckMode; 						//4129 �е���ת���� 0:���ؿ���  1��λ�ÿ���
                        float EmptySensorToCutterDis;		//4130 ���հ����������е��ľ��룬MM
                        s32 CutterReversalAngle;				//4132 ��ת�ǶȲ���
                        float CutterStuckSen;						//4134 �е���ת������
                        u32 CutterReversalTimes;        //4136 �е�������ת����ͣ��
                        u32 CutterEmptyTimes;						//4138 �е��Ը�λ��ֽ���߸���
                        u32 LoseColorMarkTimes;					//4140 ���걨�������趨
                        //ϵͳ����3
                        u16 CuterrMode;                  //4142 �豸����ѡ�� 0����תʽ  1������ʽ
                        u16 ColorMarkEnTerm;                 //4143 ɫ�����   0������ 1������
                        float FirstBellSpeedCoe;				 //4144 ��һƤ���ٶ�ϵ��
                        float SecondBellSpeedCoe;				 //4146 �ڶ�Ƥ���ٶ�ϵ��
                        u16 FirstBellMotorSel;					 //4148 ��һ�����ϵ��ѡ��
                        u16 term4;											 //
                        u32 NumOfElseStation;						 //4150 ���ϵ�λ��������һ������Ƥ��ÿһ����λ�����ϸ���
                        u32 CutterTorqueCheckTime;				//4152 �е�Ť�ؼ��ʱ��
                        u32 SurplusPackage;               //4154 ���ϸ����趨
                        u16 CutterRunMode;								//4156 �е�����  0����������  1������͹������
                        //ϵͳ����4
                        u16 NoMaterialStop;								//4157 ������������ֹͣ�� 0���ر�  1����
                        u16 EmptyPackageReClose;					//4158 ���հ��з���ǰ�պϿ���
                        u16 CutterTwoCamSwitch;      			//4159 �е�˫͹�ֿ���
                        u32 TwoCamSpeed;    							//4160 ˫͹����С�ٶ�
                        u32 TwoCamAngleRange;							//4162 ˫͹�ֽǶȷ�Χ
                        u16 DeviceMode; 									//4164 ����ѡ��
                        u16 term2; 												//4165
                        //ϵͳ����5
                        s32 CylStopButton;										//4166	ѭͣ��ť
                        s32 CutEncoderCyclePlusNum;  			//4168 �ü�ʽ�е�������������
                        s32 CutSpeedRate;									//4170 �������е��ٶȱ���
                        s32 FeedPackagingFilmHoldLength; 	//4172 �ü�ʽ��ֽ��ͣ����
                        s32 ObjectPosSen;									//4174 ����ʽ����λ�ò�������
                        s32 UnfeedMotorHoldLength;  			//4176 ����ʽ�ǳ��ϵ����ͣ����
                        s32 SensorDistance; 							//4178 ����ʽ����������
                        s32 AirInflationDelay;  					//4180 ����ʽ����ʱ��
                        s32 CutterCloseDelay;   					//4182 �ü�ʽ�е��պ�ʱ��
                        u32 CutterOnDelay;								//4184 �ü�ʽ�е����������ʱ1
                        u32 CutterOffDelay;								//4186 �ü�ʽ�е����������ʱ2
                        u16 CloseMotorSel;     						//4188 �ü�ʽ�˷���ѡ�� 0���ŷ���� 1���������
                        u16 FeedAlmIsStop;     						//4189 ���ϱ���ѡ��0������
                        u32 CutCylinderAlmDelay;       		//4190 �ü�ʽ�е����ױ���ʱ��
                        u32 SensorCheckDistance;   				//4192 �����������ų���
                        u32 LableLength;    							//4194 ��λ��ǩ����
                        u32 CamPNum;											//4196 ͹�ֵ���
                        u16 CodePrintEn; 									//4198 ��������  0���ر�  1����
                        u16 BlowEn;      									//4199 ��������  0���ر�  1����
                        s32 CodePrintPos;       					//4200 ����λ��
                        s32 BlowAngle[4];										//4202-4208  �����Ƕ�
                        s32 BlowLength[4];									//4210-4216 ��������
                        u32 ScaldFilmDelay;									//4218 ����Ĥ��ʱ
                        u16 CutProductENTerm;										//4220 ������ 0���ر�  1����
                        u16 ProductPosCheckENTerm;							//4221 ���ϴ�λ��� 0���ر�  1����
                        float ProductLength;        				// 4222 ���ϳ���
                        float EmptyLength;          				// 4224 ���пհ׳���
                        u16 ScaldFilmEnTerm;										//4226 ����Ĥ 0���ر�  1����
                        u16 Term;														//4227
                        float ScaldFilmAngle;								//4228 ����Ĥ�Ƕȷ�Χ
                        u16 CutterAxisEn;									//4230 ��ֽ������� 0���ر� 1����
                        u16 FeedAxisEn;											//4231 ���ϵ������ 0���ر� 1����
                        u16 CutterOrgAlmEn;									//4232 �е��ӽ����ر��� 0���ر� 1����
                        u16 FeedFilmOrgAlmEn;								//4233 ��ֽ��翪�ر��� 0���ر� 1����
                        u16 FeedOrgAlmEn;										//4234 ���Ͻӽ����ر��� 0���ر� 1����
                        u16 ConnectMode;										//4235 ������־    1������
                        s32 MarkReactionTime; 							//4236 ɫ�귴Ӧʱ�� MS
                        u32 MaxDifPackageSpeed; 						//4238 �������ٶ�����
                        s32 EmptyCount;                     //4240 ���ϸ���
                        s32 TorqueCheckAngleMin;			//4242 ���ؼ��Ƕȷ�ΧMin
                        s32 TorqueCheckAngleMax;			//4244 ���ؼ��Ƕȷ�ΧMax

                        u16 SprayWaterEn; 									//4246 ��ƾ�����  0���ر�  1����
                        u16 termU16_1;      									//4247
                        s32 SprayWaterPos;       					//4248 ��ƾ�λ��
                        u32 SprayWaterDelay;       					//4250 ��ƾ���ʱ
                        u32 PassPackageNum;        					//4252 ���ط��й�������
                        s32 CutterHomeOffset;       				//4254 �е���ԭ��ƫ��,��λ�� 1��
                    } SysPara;
                    u32 term3[16];											//4256 - 4286
                    struct
                    {
                        u32 Packagespeed1;							//4288 ��װ�ٶ�1
                        u32 Packagespeed2;							//4290 ��װ�ٶ�2
                        float MarkDis;                  //4292 ɫ��ƫ������ֲ�ͬ
                        float MarkReactionTime;         //4294 ɫ����Ӧʱ��
                        u32 PackageLTest;               //4296 ���Դ���
                        u32 TestSpeed;                     //4298 �����ٶ�
                        s32 CutterCyclePlusNum;   				//4300 �е�����������
                        s32 CutterEncoderCyclePlusNum;   	//4302 �е�����������������
                        s32 FeedPackagingFilmPlusNum;    	//4304 ��ֽ������
                        s32 FeedProductPlusNum; 						//4306 ����������
                        float FeedPackagingFilmSpeedStd;    //4308 ��ֽ�ٶȻ�׼
                        float FeedSpeedStd;    				//4310 �����ٶȻ�׼
                        float FirstFeedSpeedStd;  		//4312 ��һ�����ٶȻ�׼
                    } TestPara;
                    u32 CamTablePointNum;  								//4314
                    u16 CamCurve[500];										//4316 - 4815
                    u16 Ymin;															//4816
                    u16 Ymax;															//4817
                    u16 YTable[500];										//4819 - 5317
                    u16 FeedStartPos;					//5318
                    u16 FeedEndPos;						//5320

                    s32 FeedAxPosTerm;          				//4254 ����λ���ݴ棬�ϵ粻��ԭ��
                    s32 FeedFilmAxPosTerm;          				//4256 ��Ĥλ���ݴ棬�ϵ粻��ԭ��
                    s32 CutterAxPosTerm;          				//4258 	�е�λ���ݴ棬�ϵ粻��ԭ��
                    s32 CutterEncoderPosTerm;          				//4260 �е�������λ���ݴ棬�ϵ粻��ԭ��
                    s32 CamExcuteStatusTerm;						//�ݴ�͹��״̬
                    s32 IsReset;    //ϵͳ�ϵ��Ƿ���Ҫ��λ
                    s32 GearSCurrPosPP;  //����ͬ��λ�ã��ϵ��ݴ�
                    s32 GearMCurrPosPP;  //����ͬ��λ�ã��ϵ��ݴ�
                    u32 GearStatus ;   //ͬ����־���ϵ��ݴ�
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
