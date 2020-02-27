#ifndef __IODEFINE_H
#define __IODEFINE_H

typedef enum	//��Ŷ���
{
    Axis_FeedFilm,							//	AXIS1   ��ֽ��
    Axis_Cutter,							//	AXIS2	������
    Axis_FeedProduct,						//	AXIS3	������
    Axis4,
    Axis5,
} AxisNum;

typedef enum	//��Ŷ���
{
    CutterEncode,							//	AXIS1	�����������
    FeedFilmEncode,							//	AXIS2	��ֽ�������
    Encode2,
    FeedProductEncode,						//	AXIS3	�����������
} EncodeNum;

typedef enum	//����ڶ���
{
    X1,  X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15, X16, X17, X18, X19, X20, X21, X22, X23, X24, X25,	X26, X27, X28, X29, X30, X31, X32,
} InPutNum;

typedef enum	//����ڶ���
{
    Y1,  Y2, Y3, Y4, Y5, Y6, Y7, Y8, Y9, Y10, Y11, Y12, Y13, Y14, Y15, Y16, Y17, Y18, Y19, Y20, Y21, Y22, Y23, Y24, Y25,	Y26, Y27, Y28, Y29, Y30, Y31, Y32,
} OutNum;


#define LOCAL 0
#define CARD1 1
#define CARD2 2

//�������
#define Q_FeedMotorEn								LOCAL,Y1 	//���ϱ�Ƶ����
#define Q_TemperatureStart 					LOCAL,Y2		//�˷��¶ȿ���
#define Q_MiddleClose	 							LOCAL,Y3		//�з�����
#define Q_ServoAlmReset 						LOCAL,Y4 	//�ŷ��������
#define Q_Red												LOCAL,Y5	// ���
#define Q_Green											LOCAL,Y6	// �̵�
#define Q_Yellow										LOCAL,Y7	// �Ƶ�
#define Q_SprayWater								LOCAL,Y8	// ��ƾ�
#define Q_AirInflation1							LOCAL,Y9	// ����1
#define Q_Code											LOCAL,Y10	// ����
#define Q_FeedFilmMotorPos					LOCAL,Y11 	//��ֽ��Ƶ����
#define Q_FeedFilmMotorNeg					LOCAL,Y12 	//��ֽ��Ƶ����
#define Q_CutSign										LOCAL,Y13	// �е��ź����
#define Q_MarkSign									LOCAL,Y14	// ɫ���ź����



//#define Q_AirInflation2							LOCAL,Y13	// ����1
//#define Q_AirInflation3							LOCAL,Y14	// ����1
//#define Q_AirInflation4							LOCAL,Y15	// ����1

//���붨��
#define I_CutOrgin 									LOCAL,X1 	//�е��ӽ�����
#define I_FeedProductOrgin 							LOCAL,X2 	//���Ͻӽ�����
#define I_ColorMark									LOCAL,X3 	//��ֽɫ�����
#define I_EmptyCheck								LOCAL,X4 	//���հ�������
#define I_PreventCut								LOCAL,X5 	//�����ϸ�Ӧ
#define I_TorqueAlm									LOCAL,X6 	//���ص���

#define I_NoFilm										LOCAL,X8 	//��Ĥͣ��

#define I_SafeDoor									LOCAL,X10 	//��ȫ��
#define I_StartBt										LOCAL,X11 	//��ʼ��ť
#define I_StopBt	 									LOCAL,X12		//ֹͣ��ť
#define I_JogBt 										LOCAL,X13 	//�㶯��ť
#define I_EmergencyBt 							LOCAL,X14 	//��ͣ��ť
#define I_ConnectSigle              LOCAL,X15 	//�����ź�

//#define I_FeedProductOrgin					LOCAL,X15 	//����λ�ü�⴫����

#endif

