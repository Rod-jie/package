//#include "bsp.h"
#include "interlayer.h"
#include "bsp_config.h"
#include "appinit.h"
#include "lock.h"
#include "axismove.h"
const unsigned char *HEAD_VER = (unsigned char *)0X803f000;
const unsigned char *BOOT_VER = (unsigned char *)0X803f040;
const unsigned char UserDev_Inf[3][64] =
{
    "F305_Ver.01.00.00_Beta",
    "A305_Ver.01.00.10_Beta",
    "__Debug__"
    "_Release_"
};

int main()
{
    u8 i;
//	sys_init();
    sys_init_IAP();
    bsp_init();
    bsp_exec();
    AppInit();
    for(i = 0; i < 40; i++)
    {
        GSR.HardWare_Ver.Code[i] = UserDev_Inf[0][i + 5];
        GSR.SoftWare_Ver.Code[i] = UserDev_Inf[1][i + 5];
    }

    while(1)
    {
        InterLayer();
        bsp_exec();
    }
}


int pwr_Task(void)
{
    HZ_Data_Write();
    return 0;
}



