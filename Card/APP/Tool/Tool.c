#include "TOOL.H"
/********************************************************/

/**
* @author 19/02/19  yang
* @Description: ÿСʱ����
* @param -- ����
* @param -- ʱ�� �����룩
* @return -- units per hour(ͨ��ʹ��ʱ��Ҫ��1000���ѵ�λ���k)
*/
float UPH(u32 time,u32 products)
{
    return (float)products * 1000 * 3600 / (float)time;
}
/**
* @author 19/02/19  yang
* @Description: ��λ���� ����ת����
* @param -- num ���
* @param -- MM ����
* @return -- ������
*/
u32 ConvertToMachineUnit(u32 num,float MM)
{
	return   MM *GSS.Conversion[num].PPR / GSS.Conversion[num].MPR;
}
/**
* @author 19/02/19  yang
* @Description: ��λת�� ����ת����
* @param --
* @param --
* @return --
*/
float ConvertToUserUnit(u32 num,u32 pulse)
{
    return (float)GSS.Conversion[num].MPR * pulse / (float)GSS.Conversion[num].PPR;
}
/**
* @author  yang 2019/3/8
* @Description: �ٶȻ���
* @param -- �����ٶ�ת��ʵ��ת��
* @param --
* @return --
*/

/*****************************************************************/



