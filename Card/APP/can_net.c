#include "can_net.h"
#include "string.h"

//#define	MYTEST	0

static u8 m_bid = 0;
static u16 m_uid = 0;
static u8 m_fun = 0;
static u8 m_sn = 1;	//��һ������Ϊ1�����򷢲���ȥ


extern void myfree(u8 memx, void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
extern void *mymalloc(u8 memx, u32 size);			//�ڴ����(�ⲿ����)
//��ǰ���ڵ��ᣬ
//static 	AxisType * pAxis = 0;
//��ǰ���ڵĵ�Ԫ
static UnitObj *m_Unit = 0;

static u8 rx_buf[8] = {0};
static u8 tx_buf[8] = {0};
static s8 rx_len = 0;
static s8 tx_len = 0;
static u32 rx_id = 0;
static u32 tx_id = 0;

static CanNetDataDef * CanNetData = 0;
//static CanNetDataDef *CanNetData __attribute__((section(".ARM.__at_0x10000000")))= {0};
static UnitObj* LoopData = 0;//(UnitObj*)&CanNetData->CommData;


//Cannet_StatusType Cannet_Status = {0};
//static
//CommDataDef CommData =
//{
//	0
//};
//static
//AxListDef AxList[MAX_AXIS_NUM] =
//{
//	0
//};
//static
//IOListDef IOList[MAX_BOARD_NUM] =
//{
//	0
//};


//static BoardListDef	BoardList[MAX_BOARD_NUM] = {0};	//���10����
//static UnitObj *BoardList[10] = {0};	//���10����
//static TxListDef PDOList = {0};
//static TxListDef SDOList = {0};
//static TxListDef ODOList = {0};

//���һ���ڵ�
static s32 ListAdd(TxListDef * List, UnitObj* Unit)
{
    if(List->Len < 5)
    {
        if(0 == List->Data[List->Head])
        {
            List->Data[List->Head] = Unit;
            List->Head ++;
            List->Len ++;
            if(5 == List->Head)
                List->Head = 0;
            return 0;
        }
        else
        {
//			while(1){}	//������
            return -1;
        }
    }
    return -1;
}
//��ȡһ���ڵ�
static UnitObj* ListGet(TxListDef * List)
{
    UnitObj* u = 0;
    if(List->Len > 0)
    {
        u = List->Data[List->Tail];
        return u;
    }
    return 0;
}
//ɾ��һ���ڵ�
static s32 ListDel(TxListDef * List)
{
    if(List->Len > 0)
    {
        List->Data[List->Tail] = 0;	//�ڵ����
        List->Tail ++;
        List->Len --;
        if(5 == List->Tail)
            List->Tail = 0;
        return 0;
    }
    return -1;
}

static void CAN_send(void)
{
    //���·�������Ϊ0
    if(RO == m_Unit->Attr)
    {
        tx_len = 0;
    }
    else	//д��Ҫ�·�����
    {
//		if(m_Unit->Flg)	//SDO,�·����ݣ�PDO,ODO���ݸı�Ż��·������ݸı��־��
//		{
////			m_Unit->Flg = 0;	//���ͳɹ��ˣ�����0
//			tx_len = m_Unit->Size;
//			memcpy(tx_buf,m_Unit->Data,tx_len);
//		}
//		else
//		{
//			tx_len = 0;
//		}
        tx_len = m_Unit->Size;
        memcpy(tx_buf, m_Unit->Data, tx_len);


    }
    m_bid = m_Unit->bid;
    m_uid = m_Unit->uid;
    m_sn  = m_sn;
    m_fun = m_Unit->Attr;
    tx_id = FRAME_ID;
    Can_SendData(tx_id, tx_len, tx_buf);
}

static s32 CAN_recv(u32 id, u8 len, u8* buf)
{
    u8 bid =	FRAME_BID(id);
    u16 uid =	FRAME_UID(id);
//	u8 fun =	FRAME_FUN(id);
    u8 sn  =	FRAME_SN(id);


    rx_len = len;
    if(((0 == rx_len) || m_Unit->Size == rx_len )\
            && bid == m_bid \
            && uid == m_uid \
            && sn == m_sn)
    {
        //������Ҫ��ȡ���ݣ�
        if(RO == m_Unit->Attr)
            memcpy(m_Unit->Data, rx_buf, rx_len);
        //������Ҫ��ȡ���ݣ�
        else
            return 0;
        return 0;
    }
    else
    {   //���մ���
        return -1;
    }
}
//u32 times[10];

//CAN����һ֡�ȴ��ظ�
//static
StepParaDef CAN_send_onePara = {0};
static void CAN_send_one()
{
    static unsigned int time_old = 0;
//	static unsigned int resend_cnt = 0;

    if(CAN_send_onePara.exec && 0 == CAN_send_onePara.step)
    {
        CAN_send_onePara.step = 1;
    }
    switch(CAN_send_onePara.step)
    {
    case 1:	//����(CAN_exec����������)
        if(0 == Can_GetSendSta())
        {
            CAN_send();
            time_old = SysTim_GetSys();
            CAN_send_onePara.step ++;
//				times[0] = 0;
        }
        break;
    case 2:	//�ȴ�����
        rx_len = Can_GetRecvData(&rx_id, rx_buf);
        if(rx_len >= 0)
        {
//				times[1] = times[0];
            //������ȷ
            if(0 == CAN_recv(rx_id, rx_len, rx_buf))
            {
                if(m_Unit->err)
                    m_Unit->err --;
                CAN_send_onePara.exec = 0;
                CAN_send_onePara.step = 0;
            }
            //���մ���(���ݴ���)
            else
            {
                m_Unit->err ++;
                if(SDO == m_Unit->Type || SDO2 == m_Unit->Type)
                {   //�ط�
                    CAN_send_onePara.step = 1;
                }
                if(m_Unit->err >= CAN_RESEND_TIMES)
                {   //ͨѶʧ��
                    CAN_send_onePara.exec = 0;
                    CAN_send_onePara.step = 0;
                    CanNetData->Cannet_Status.CommErr |= (1 << m_Unit->bid);
                }
            }
        }
//			else
//				times[0] ++;
        if(SysTim_GetSys() - time_old > CAN_REC_TIME_OUT)//�ط�
        {
            m_Unit->err ++;
            if(SDO == m_Unit->Type || SDO2 == m_Unit->Type)
            {   //SDO�ط�
                CAN_send_onePara.step = 0;
            }
            if(m_Unit->err >= CAN_RESEND_TIMES)
            {   //PDO,ODOͨѶʧ��
                CAN_send_onePara.exec = 0;
                CAN_send_onePara.step = 0;
                CanNetData->Cannet_Status.CommErr |= (1 << m_Unit->bid);
            }
        }
        break;
    default:
        break;
    }
}
static void PDO_Loop(void)
{
    const static u16 PDO_Loop_Max = 20;
    u16 PDO_Loop_cnt = 0;
    static  u32 Idx = 0;

    while(CanNetData->PDOList.Len < 5 && PDO_Loop_cnt < PDO_Loop_Max)
    {
        if(PDO == LoopData[Idx].Type)
        {
            ListAdd(&CanNetData->PDOList, (UnitObj*)&LoopData[Idx]);
        }
        PDO_Loop_cnt ++;
        Idx ++;
        if(CanNetData->Cannet_Status.Unit_Num <= Idx)
            Idx = 0;
    }
//	if(PDO_Loop_Max == PDO_Loop_cnt)
//		PDO_Loop_cnt = PDO_Loop_Max;
}
static void SDO_Loop(void)
{
    const static u16 SDO_Loop_Max = 100;
    u16 SDO_Loop_cnt = 0;
    static  u32 Idx = 0;

    while(CanNetData->SDOList.Len < 5 && SDO_Loop_cnt < SDO_Loop_Max)
    {
        if((1 == LoopData[Idx].Flg) && (SDO == LoopData[Idx].Type || SDO2 == LoopData[Idx].Type))
        {
            LoopData[Idx].Flg = 2;		//��ӽ�����������δ����
            ListAdd(&CanNetData->SDOList, (UnitObj*)&LoopData[Idx]);
        }
        SDO_Loop_cnt ++;
        Idx ++;
        if(CanNetData->Cannet_Status.Unit_Num <= Idx)
            Idx = 0;
    }
//	if(SDO_Loop_Max == SDO_Loop_cnt)
//		SDO_Loop_cnt = SDO_Loop_Max;
}
static void ODO_Loop(void)
{
    const static u16 ODO_Loop_Max = 10;
    u16 ODO_Loop_cnt = 0;
    static  u32 Idx = 0;

    while(CanNetData->ODOList.Len < 5 && ODO_Loop_cnt < ODO_Loop_Max)
    {
        if(ODO == LoopData[Idx].Type)
        {
            ListAdd(&CanNetData->ODOList, (UnitObj*)&LoopData[Idx]);
        }
        ODO_Loop_cnt ++;
        Idx ++;
        if(CanNetData->Cannet_Status.Unit_Num <= Idx)
            Idx = 0;
    }
}
void SetCannetErrCode(u8 errcode)
{
    CanNetData->Cannet_Status.err_code = 1;
    CanNetData->Cannet_Status.state = Cannet_State_Err;
}

////��ȡ�忨������
//static u8 GetBoardAxisNum(u8 bid)
//{
//	if(bid < Cannet_Status.Slave_Num)
//		return BoardList[bid].start->Data[1];
//	else
//		return 0;
//}

static void GenerateAHead(u8 bid, u16 usn, UnitObj *p)
{
    memset(p, 0, sizeof(BoardHeadType));
    ((BoardHeadType*)(p))->BoardType.Attr = RO;
    ((BoardHeadType*)(p))->BoardType.bid = bid;
//	((BoardHeadType*)(p))->BoardType.Data[0] = Bt_NONR;	//Type
//	((BoardHeadType*)(p))->BoardType.Data[1] = 0;	//Anum
//	((BoardHeadType*)(p))->BoardType.Data[2] = 0;	//Inum
//	((BoardHeadType*)(p))->BoardType.Data[3] = 0;	//Onum
//	((BoardHeadType*)(p))->BoardType.Data[4] = 0;	//Data32In
//	((BoardHeadType*)(p))->BoardType.Data[5] = 0;	//Data32Out
    ((BoardHeadType*)(p))->BoardType.err = 0;
    ((BoardHeadType*)(p))->BoardType.Flg = 1;
    ((BoardHeadType*)(p))->BoardType.Size = 8;
    ((BoardHeadType*)(p))->BoardType.Type = SDO;
    ((BoardHeadType*)(p))->BoardType.uid = usn ++;
    ((BoardHeadType*)(p))->reserved[0].uid = usn ++;
    ((BoardHeadType*)(p))->reserved[1].uid = usn ++;
    ((BoardHeadType*)(p))->reserved[2].uid = usn ++;
    ((BoardHeadType*)(p))->reserved[0].bid = bid;
    ((BoardHeadType*)(p))->reserved[1].bid = bid;
    ((BoardHeadType*)(p))->reserved[2].bid = bid;
}
static void AxisDataAdd(u8 bid, u16 usn, UnitObj *p, u8 Anum)
{
    u8 i = 0;
    AxisType t_axis = AxisTypeInitializer(bid);
    memcpy(p, &t_axis, sizeof(AxisType));
    for(i = 0; i < sizeof(AxisType) / sizeof(UnitObj); i ++)
    {
        if(SDO == p->Type || SDO2 == p->Type)
        {
            p->Flg = 1;
        }
        p++ ->uid = usn ++;
    }
}
//static
void BoardDataAdd(u8 bid)//,u16 usn,UnitObj *p,u8 * Para)
{
    u16 usn = 4;	//ͷ�Ѿ�����
    u8 i = 0;
    u8 Anum = CanNetData->BoardList[bid].Anum;
    u8 Inum = CanNetData->BoardList[bid].Inum;
    u8 Onum = CanNetData->BoardList[bid].Onum;
    u8 D32Inum = CanNetData->BoardList[bid].D32Inum;
    u8 D32Onum = CanNetData->BoardList[bid].D32Onum;
    UnitObj *p = CanNetData->BoardList[bid].start;


//				t_unit += (sizeof(BoardHeadType))/sizeof(UnitObj);	//����ͷ
    CmdSnType cmd 			= CmdSnTypeTypeInitializer(bid, Anum);
    InPutType input 		= InPutTypeInitializer(bid, Inum);			//��ͨ����
    InPutType inputAxis 	= InPutTypeInitializer(bid, 32);				//������(��������λ)
    OutPutType output 		= OutPutTypeInitializer(bid, Onum);			//��ͨ���
    OutPutType outputAxis 	= OutPutTypeInitializer(bid, Anum);			//�������ʹ�ܣ�
    D32InPutType D32InPut	= D32InPutTypeInitializer(bid);
    D32OutPutType D32OutPut	= D32OutPutTypeInitializer(bid);

    p += (sizeof(BoardHeadType)) / sizeof(UnitObj);	//����ͷ



    if(bid < MAX_BOARD_NUM)
    {
        for(i = 0; i < Anum; i ++)
        {
            AxisDataAdd(bid, usn, p, i);
            CanNetData->AxList[CanNetData->Cannet_Status.Axis_Num + i].Data = (AxisType*)p;				//addlist�������
            CanNetData->AxList[CanNetData->Cannet_Status.Axis_Num + i].Axisn = i;
            CanNetData->AxList[CanNetData->Cannet_Status.Axis_Num + i].Cmdsn = 0;
            CanNetData->AxList[CanNetData->Cannet_Status.Axis_Num + i].Bid = bid;
            usn += sizeof(AxisType) / sizeof(UnitObj);
            p += sizeof(AxisType) / sizeof(UnitObj);
        }
        if(Anum) {
            memcpy(p, &cmd, sizeof(CmdSnType));
            for(i = 0; i < Anum; i ++)
            {
                //		AxList[Cannet_Status.Axis_Num].Axisn = i;
                CanNetData->AxList[CanNetData->Cannet_Status.Axis_Num].CmdSnData = (CmdSnType*)p;		//addlist
                CanNetData->Cannet_Status.Axis_Num ++;
            }
            p++ ->uid = usn ++;
        }
        if(Inum) {
            memcpy(p, &input, sizeof(InPutType));
            CanNetData->IOList[bid].Gpi = (InPutType*)p;
            p++ ->uid = usn ++;
        }
        if(Anum) {
            memcpy(p, &inputAxis, sizeof(InPutType));
            CanNetData->IOList[bid].Axi = (InPutType*)p;
            p++ ->uid = usn ++;
        }
        if(Onum) {
            memcpy(p, &output, sizeof(OutPutType));
            CanNetData->IOList[bid].Gpo = (OutPutType*)p;
            p->Flg = 1;								//������ϵ��ʼ��
            p++ ->uid = usn ++;
        }
        if(Anum) {
            memcpy(p, &outputAxis, sizeof(OutPutType));
            CanNetData->IOList[bid].Axo = (OutPutType*)p;
            p->Flg = 1;
            p++ ->uid = usn ++;
        }
        if(D32Inum) {
            for(i = 0; i < D32Inum; i ++)
            {
                memcpy(p, &D32InPut, sizeof(D32InPutType));
                //ֻ��¼ͷ�ͺ�
                if(0 == CanNetData->BoardList[bid].D32IHead)
                {
                    CanNetData->BoardList[bid].D32IHead = (D32InPutType*)p;
                }
                p++ ->uid = usn ++;
            }
//			memcpy(p,&input,sizeof(InPutType));
//			CanNetData->IOList[bid].Gpi = (InPutType*)p;
//			p++ ->uid = usn ++;
        }
        if(D32Onum) {
            for(i = 0; i < D32Onum; i ++)
            {
                memcpy(p, &D32OutPut, sizeof(D32OutPutType));
                //ֻ��¼ͷ�ͺ�
                if(0 == CanNetData->BoardList[bid].D32OHead)
                {
                    CanNetData->BoardList[bid].D32OHead = (D32OutPutType*)p;
                }
                p->Flg = 1;
                p++ ->uid = usn ++;
            }
        }
    }
}

static void ScanSlave(void)
{
    static u16 step = 0;
    static u16 Idx = 0;	//����ƫ��
    static u32 oldtime = 0;
    UnitObj * t_unit = 0;
    u8 i = 0;
//	u16 usn = 0; //uid sn
    switch(step)
    {
    case 0://��ʼ
        CanNetData->Cannet_Status.state = Cannet_State_Init;
        step ++;
        break;
    case 1:	//���ð�ͷ
        if(MAX_UNIT_NUM - Idx - 1 > (sizeof(BoardHeadType)) / sizeof(UnitObj))
        {
            t_unit = &CanNetData->CommData.data[Idx];
            GenerateAHead(CanNetData->Cannet_Status.Slave_Num, 0, t_unit);
            oldtime = SysTim_GetSys();
            CanNetData->Cannet_Status.Unit_Num += (sizeof(BoardHeadType)) / sizeof(UnitObj);
            step ++;
        }
        else
        {
            SetCannetErrCode(1);	//�ռ䲻��
            step = 7;
        }
        break;
    case 2:	//�������忨
        t_unit = &CanNetData->CommData.data[Idx];
        if(Bt_NONR != t_unit->Data[0])	//�������忨
        {
            if(t_unit->Data[0] < Bt_ERR)	//�忨���ͺϷ�
            {
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].start = t_unit;

                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].BoardType = t_unit->Data[0];
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].Anum = t_unit->Data[1];
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].Inum = t_unit->Data[2];
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].Onum = t_unit->Data[3];
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].D32Inum = t_unit->Data[4];
                CanNetData->BoardList[CanNetData->Cannet_Status.Slave_Num].D32Onum = t_unit->Data[5];

                Idx += sizeof(BoardHeadType) / sizeof(UnitObj);						/*ͷ*/\
                Idx += t_unit->Data[1] * (sizeof(AxisType) / sizeof(UnitObj));			/*��*/\
                Idx += t_unit->Data[1] ? 3 : 0;											/*��IO�����*/\
                Idx += t_unit->Data[2] ? 1 : 0;											/*GPI*/\
                Idx += t_unit->Data[3] ? 1 : 0;											/*GPO*/
                Idx += t_unit->Data[4];
                Idx += t_unit->Data[5];
                CanNetData->Cannet_Status.Slave_Num ++;
                CanNetData->Cannet_Status.Unit_Num = Idx;
                step --;
            }
            else
            {
                SetCannetErrCode(3);	//�忨���ʹ���
                step = 7;
            }
        }
        else if(SysTim_GetSys() - oldtime > 3000)	//300msδ������
        {
            if(CanNetData->Cannet_Status.Slave_Num)	//�������
            {
                step ++;
            }
            else
            {
                SetCannetErrCode(2);	//δ������
                step = 7;
            }
        }
        break;
    case 3:	//���ð忨����
        for(i = 0; i < CanNetData->Cannet_Status.Slave_Num; i ++)
        {
//				t_unit = CanNetData->BoardList[i].start;
//				t_unit += (sizeof(BoardHeadType))/sizeof(UnitObj);	//����ͷ
            BoardDataAdd(i);//,4,t_unit,&CanNetData->BoardList[i].start->Data[1]);
        }
        step = 7;
        break;
    case 4:	//��������
        break;
    case 5:	//�������
        break;
    case 6:	//��ʼ���忨
        break;
    case 7:	//����
        CanNetData->Cannet_Status.state = Cannet_State_Ready;
        //cannet
        break;
    }
}

//static void Data_Init(void)
//{
//	static u8 flg = 1;
//	u16 i = 0;
//	if(flg)
//	{
//		flg = 0;
//		for(i = 0;i < UNIT_NUM;i ++)
//		{
//			LoopData[i].uid = i;
//		}
//	}
//}

//void can_netInit(void)
//{
//	static u8 flg = 1;
//	char i;
//	if(flg)
//	{
//		flg = 0;
//		m_sn = 1;	//�ӻ���ʼ��Ϊ0,������ʼ��Ϊ1�����й�����û�����������У�������ȥ����ֹ��һ��ָ�ִ��
//		for(i = 0;i < PULS_NUM;i ++)
//		{
//			AxList[i].Cmdsn = 1;
//			HZ_ExAxStop(i);
//		}
////		CommData.Board16Axis.OutPut[0].Data.Flg = 1;
////		CommData.Board16Axis.OutPut[1].Data.Flg = 1;
//	}
//}

#define SENDCYCLE	9	//����ÿ5֡��һ������
TxListDef * ListSelect(void)
{

    static u8 cnt = 0;

    if(0 == CAN_send_onePara.exec)
    {
        cnt ++;
        cnt %= SENDCYCLE;

        if(0 == cnt)
        {
            //ODO֡
            if(CanNetData->ODOList.Len)
            {
                return  &CanNetData->ODOList;
            }
        }
        else
        {
            if(0 == cnt % 3)	//PDO
            {
                if(CanNetData->PDOList.Len)
                {
                    return  &CanNetData->PDOList;
                }
            }
            else			//SDO
            {
                if(CanNetData->SDOList.Len)
                {
                    return &CanNetData->SDOList;
                }
                else if(CanNetData->PDOList.Len)
                {
                    return  &CanNetData->PDOList;
                }
            }
        }
    }
    return 0;
}

static StepParaDef	can_loopStepPara = {0};
void can_netLoop(void)
{

    static TxListDef* TxList = 0;
    if(!CanNetData)
    {
        CanNetData = mymalloc(1, sizeof(CanNetDataDef));
        memset(CanNetData, 0, sizeof(CanNetDataDef));
        LoopData = (UnitObj*)&CanNetData->CommData;
    }
    ScanSlave();
//	Data_Init();
//	can_netInit();


    SDO_Loop();
    PDO_Loop();
    ODO_Loop();

    CAN_send_one();

    if(0 == can_loopStepPara.exec && 0 == can_loopStepPara.step)
    {
        can_loopStepPara.exec = 1;
        can_loopStepPara.step = 1;
    }
    switch(can_loopStepPara.step)
    {
    case 1://��һ֡������ɣ�������һ֡���ݲ����ͣ�
        if(0 == CAN_send_onePara.exec)
        {
            TxList = ListSelect();
            if(0 != TxList)
            {
                m_Unit = ListGet(TxList);
                can_loopStepPara.step ++;
                CAN_send_onePara.exec = 1;
            }
        }
        break;
    case 2://�ȴ��������
        if(0 == CAN_send_onePara.exec)
        {
            m_sn ++;	//���к�++
            if(0 == m_sn)
                m_sn = 2;	//����0��1
            if((SDO == m_Unit->Type || SDO2 == m_Unit->Type) && 1 != m_Unit->Flg )
            {
                if( CAN_RESEND_TIMES > m_Unit->err)
                {   //�������
                    m_Unit->Flg = 0;
                }
                else
                    while(0);
            }
            ListDel(TxList);
            can_loopStepPara.step = 1;
        }
        break;
    case 3:
        break;
    case 4:
        break;
    }
}


static u64 ReadUnitVal(UnitObj* u)
{
    return *((u64*)&u->Data[0]);
}
static void WriteUnitVal(UnitObj* u, u64 val)
{
    if(SDO == u->Type)
    {
        //if(1)	//�ٶ��࣬�仯����
        if(val != *((u64*)&u->Data[0]))
        {
            u->Flg = 1;
        }
    }
    else if(SDO2 == u->Type)
    {
        if(1)	//�ᶯ�࣬û�仯ҲҪ����
            //if(val != *((u64*)&u->Data[0]))
        {
            u->Flg = 1;
        }
    }
    *((u64*)&u->Data[0]) =  val;
}

u32 HZ_ExD32IGet(u32 bid, u32 num) //bid:��չ�����
{
    u32 val;
    UnitObj* u;
    if(num < CanNetData->BoardList[bid].D32Inum)
    {
        u = (UnitObj*)CanNetData->BoardList[bid].D32IHead;
        val = ReadUnitVal(&u[num]);
    }
    return val;
}
u32 HZ_ExD32OGet(u32 bid, u32 num)
{
    u32 val;
    UnitObj* u;
    if(num < CanNetData->BoardList[bid].D32Onum)
    {
        u = (UnitObj*)CanNetData->BoardList[bid].D32OHead;
        val = ReadUnitVal(&u[num]);
    }
    return val;
}
s32 HZ_ExD32OSet(u32 bid, u32 num, u32 val)
{
    UnitObj* u;
    if(num < CanNetData->BoardList[bid].D32Onum)
    {
        u = (UnitObj*)CanNetData->BoardList[bid].D32OHead;
        WriteUnitVal(&u[num], val);
        return 0;
    }
    return -1;
}

//���ò���
s32 HZ_ExAxSetPara(u8 ExAxNum, \
                   u32 Start, u32 TAcc, u32 Run, u32 TDec, u32 End, \
                   u32 HomFast, u32 HomSlow, u32 HomOffset, \
                   u32 TorS, u32 HomTimeOut)
{
    if(ExAxNum >= CanNetData->Cannet_Status.Axis_Num)
        return -1;

    if(Start >= Run)
        Start = Run - 1;
    if(End >= Run)
        End = Run - 1;
    if(TAcc < 10)
        TAcc = 10;
    if(TDec < 10)
        TDec = 10;
    if(TAcc > 10 * 1000)
        TAcc = 10 * 1000;
    if(TDec > 10 * 1000)
        TDec = 10 * 1000;

    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->Start, Start);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->TAcc, TAcc);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->Run, Run);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->TDec, TDec);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->End, End);

    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->TorS, TorS);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->HomFast, HomFast);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->HomSlow, HomSlow);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->HomOffSet, HomOffset);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->HomTimeOut, HomTimeOut);

    return 0;
}
//���û�ԭ��ģʽ
//AxNum :���
//en	:ʹ��
//Sig	:ԭ���ź�
//Mod	:ģʽ
//		0��������ԭ��
//		1��������������λ���ٷ�����ԭ��
//		2���ȷ���������λ����������ԭ��
s32 HZ_ExAxSetHomMod(u8 ExAxNum, u8 En, u8 Sig, u8 Lev, u8 Mod)
{
    u64 temp = 0;
    if(ExAxNum >= CanNetData->Cannet_Status.Axis_Num)
        return -1;
    temp = En | (Sig << 8) | (Lev << 16) | (Mod << 24);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->HomAttr, temp);
    return 0;
}
//����Ӳ��λ
s32 HZ_ExAxSetLimit_H(u8 ExAxNum, u8 en_Up, u8 sig_Up, u8 lev_Up, u8 en_Dw, u8 sig_Dw, u8 lev_Dw)
{
    u64 temp = 0;
    if(ExAxNum >= CanNetData->Cannet_Status.Axis_Num)
        return -1;
    temp = en_Up | (sig_Up << 8) | (lev_Up << 16);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->LimUp_H, temp);
    temp = 0;
    temp = en_Dw | (sig_Dw << 8) | (lev_Dw << 16);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->LimDw_H, temp);
    CanNetData->AxList[ExAxNum].LimHUp = sig_Up;
    CanNetData->AxList[ExAxNum].LimHUpEn = en_Up;
    CanNetData->AxList[ExAxNum].LimHUpLev = lev_Up;
    CanNetData->AxList[ExAxNum].LimHDw = sig_Dw;
    CanNetData->AxList[ExAxNum].LimHDwEn = en_Dw;
    CanNetData->AxList[ExAxNum].LimHDwLev = lev_Dw;

    return 0;
}
//��������λ
s32 HZ_ExAxSetLimit_S(u8 ExAxNum, s32 pos_Up, u8 en_Up, s32 pos_Dw, u8 en_Dw)
{
    u64 temp = 0;
    if(ExAxNum >= CanNetData->Cannet_Status.Axis_Num)
        return -1;
//	(s32)temp = pos_Up;
    temp |= pos_Up;
    temp |= ((u64)en_Up << 32);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->LimUp_S, temp);
    temp = 0;
//	(s32)temp = pos_Dw;
    temp |= pos_Dw;
    temp |= ((u64)en_Dw << 32);
    WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->LimDw_S, temp);
    CanNetData->AxList[ExAxNum].LimSUp = pos_Up;
    CanNetData->AxList[ExAxNum].LimSDw = pos_Dw;
    CanNetData->AxList[ExAxNum].LimSUpEn = en_Up;
    CanNetData->AxList[ExAxNum].LimSDwEn = en_Dw;

    return 0;
}
//���ö���
//Cmd:
//	0x00:�޶���
//	0x01��ABSACT		//�����ȼ�
//	0x02��RELACT
//	0x03��SPDACT
//	0x04��HOMACT
//	0x05��
//	0x06��
//	0x07��
//	0x08��
//	0x09��
//	...
//	0x80��STOPDECACT	//�θ����ȼ�
//	0x81��
//	0x90��STOPACT		//������ȼ�
s32 HZ_ExAxSetAct(u8 ExAxNum, s32 pos, ExAxActDef Cmd, u8 Spd)
{
    u64 temp = 0;
    u8  CmdLast = 0;	//��ȡ��һ�ε���������
    if(ExAxNum >= CanNetData->Cannet_Status.Axis_Num)
        return -1;
    CmdLast = CanNetData->AxList[ExAxNum].Data->Cmd.Data[4];
    temp |= (u32)pos;
    //(s32)temp = pos;
    temp |= ((u64)Cmd << 32);
    temp |= ((u64)Spd << 40);
    if(0 == HZ_ExAxGetExe(ExAxNum))
    {
        if(Cmd < 0x10)	//���п���д����
        {
            CanNetData->AxList[ExAxNum].Cmdsn ++;	//�Ƚϵ�ʱ��ֻȡ����λ
            if(0 == (CanNetData->AxList[ExAxNum].Cmdsn % 8))
                CanNetData->AxList[ExAxNum].Cmdsn = 2;
            temp |= ((u64)CanNetData->AxList[ExAxNum].Cmdsn << 48);
            WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->Cmd, temp);
            return 1;
        }
        else			//����ͣ�ͼ���ͣ���ԣ�����ִ��
        {
            return 0;
        }
    }
    else	//����æ�������ȼ���ִ��
    {
        if(Cmd > CmdLast)	//��ָ�����ȼ���
        {
            CanNetData->AxList[ExAxNum].Cmdsn ++;
            if(0 == (CanNetData->AxList[ExAxNum].Cmdsn % 8))
                CanNetData->AxList[ExAxNum].Cmdsn = 2;
            temp |= ((u64)CanNetData->AxList[ExAxNum].Cmdsn << 48);
            WriteUnitVal(&CanNetData->AxList[ExAxNum].Data->Cmd, temp);
            return 1;
        }
        else				//��ִ�У����ش���
        {
            return -1;
        }
    }
}
s32 HZ_ExAxGetCurPos(u8 ExAxNum)
{
    return *(s32*)(CanNetData->AxList[ExAxNum].Data->CurPos.Data);
}
//��ȡ����״̬
//return bit 8: Ӳ����9:������12��Ӳ����13��������
u32 HZ_ExAxGetErr(u8 ExAxNum)
{
    return *(u16*)(&CanNetData->AxList[ExAxNum].Data->CurPos.Data[4]);
}

s32 HZ_ExAxGetExe(u8 ExAxNum)
{
    u8 sn = *((u64*)(CanNetData->AxList[ExAxNum].CmdSnData->Data.Data)) >> (3 * CanNetData->AxList[ExAxNum].Axisn);
    if((CanNetData->AxList[ExAxNum].Cmdsn & 0x7) == (sn & 0x7))
    {
        return  0;
    }
    else
    {
        return 1;
    }
}
void HZ_ExAxMoveRel(u8 ExAxNum, s32 pos)
{
    HZ_ExAxSetAct(ExAxNum, pos, RELCMD, 100);
}
void HZ_ExAxMoveAbs(u8 ExAxNum, s32 pos)
{
    HZ_ExAxSetAct(ExAxNum, pos, ABSCMD, 100);
}
void HZ_ExAxMoveVelocity(u8 ExAxNum, s32 pos)
{
    HZ_ExAxSetAct(ExAxNum, pos, SPDCMD, 100);
}
s32 HZ_ExAxGetStatus(u8 ExAxNum)
{
    return HZ_ExAxGetExe(ExAxNum);
}
void HZ_ExAxStopDec(u8 ExAxNum)
{
    HZ_ExAxSetAct(ExAxNum, 0, STOPCMD, 100);
}
void HZ_ExAxStop(u8 ExAxNum)
{
    HZ_ExAxSetAct(ExAxNum, 0, STOPCMD, 100);
}
void HZ_ExAxHome(u8 ExAxNum)
{
    HZ_ExAxSetAct(ExAxNum, 0, HOMCMD, 100);
}
//bid:��ţ���0��ʼ
//type,0:��ͨ�����1��ʹ��
//num: ��ţ��ڼ�����
//val:	Ҫд���ֵ��0��1
//return: -1����������0������
s32 OutputSet1(u8 bid, u8 type, u8 num, u8 val)
{

    u64 temp = 0;
    if(bid < MAX_BOARD_NUM)
    {
        //��ͨ���
        if(0 == type)
        {
            temp = *(u64*)(CanNetData->IOList[bid].Gpo->Data.Data);
            temp &= ~(1l << num);
            temp |= ((u64)val << num);
            WriteUnitVal(&CanNetData->IOList[bid].Gpo->Data, temp);
            return 0;
        }
        //ʹ��
        else
        {
            temp = *(u64*)(CanNetData->IOList[bid].Axo->Data.Data);
            temp &= ~(1l << num);
            temp |= ((u64)val << num);
            WriteUnitVal(&CanNetData->IOList[bid].Axo->Data, temp);
        }
        return 0;
    }
    return -1;
}

//bid:��ţ���0��ʼ
//type,0:��ͨ�����1��ʹ��
//num: ��ţ��ڼ�����
//return: -1������������������ȡ��ֵ
s32 OutputGet1(u8 bid, u8 type, u8 num)
{
    u64 temp = 0;
    if(bid < MAX_BOARD_NUM)
    {
        //��ͨ���
        if(0 == type)
        {
            temp = *(u64*)(CanNetData->IOList[bid].Gpo->Data.Data);
            return (temp >> num) & 1;
        }
        //ʹ��
        else
        {
            temp = *(u64*)(CanNetData->IOList[bid].Axo->Data.Data);
            return (temp >> num) & 1;
        }
    }
    return -1;
}
//bid:��ţ���0��ʼ
//type,0:��ͨ���롣1��������2����λ
//num: ��ţ��ڼ�����
//return: -1������������������ȡ��ֵ
s32 InputGet1(u8 bid, u8 type, u8 num)
{
    u64 temp = 0;
    if(bid < MAX_BOARD_NUM)
    {
        //��ͨ����
        if(0 == type)
        {
            temp = *(u64*)(CanNetData->IOList[bid].Gpi->Data.Data);
            return (temp >> num) & 1;
        }
        //����
        else if(1 == type)
        {
            temp = *(u64*)(CanNetData->IOList[bid].Axi->Data.Data);
            return (temp >> num) & 1;
        }
        //��λ
        else
        {
            temp = *(u64*)(CanNetData->IOList[bid].Axi->Data.Data) >> 16;
            return (temp >> num) & 1;
        }
    }
    return -1;
}
s32 HZ_ExOutPutSet(u8 bid, u8 num, u8 val)
{
    return OutputSet1(bid, 0, num, val);
}
s32 HZ_ExOutPutGet(u8 bid, u8 num)
{
    return OutputGet1(bid, 0, num);
}
s32 HZ_ExEnSet(u8 bid, u8 num, u8 val)
{
    return OutputSet1(bid, 1, num, val);
}
s32 HZ_ExEnGet(u8 bid, u8 num)
{
    return OutputGet1(bid, 1, num);
}
s32 HZ_ExInPutGet(u8 bid, u8 num)
{
    return InputGet1(bid, 0, num);
}
s32 HZ_ExAlmGet(u8 bid, u8 num)
{
    return InputGet1(bid, 1, num);
}
s32 HZ_ExLimGet(u8 bid, u8 num)
{
    return InputGet1(bid, 2, num);
}
s32 HZ_ExCanNetStateGet(void)
{
    return CanNetData->Cannet_Status.state;
}
//�忨ͨѶ����ÿ��λ����һ���忨����
u32 HZ_ExCanComErrGet(void)
{
    return CanNetData->Cannet_Status.CommErr;
}
