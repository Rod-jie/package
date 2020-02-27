SET PATH=D:\KEIL\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Microsoft SQL Server\110\Tools\Binn\;;C:\Users\Administrator\AppData\Local\Programs\Microsoft VS Code\bin
SET ARMCC5_ASMOPT=--diag_suppress=9931
SET ARMCC5_CCOPT=--diag_suppress=9931
SET ARMCC5_LINKOPT=--diag_suppress=9931
SET CPU_TYPE=STM32F407ZG
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=12Axis
SET CPU_CLOCK=0x00B71B00
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\conmode .__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\lengthdifmode.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\underfeedflimmode.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\flycut_pou.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\appinit.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\fsm.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\reset.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\logicterm.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\logic.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\logicnotused.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\can_net.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\axismove.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\tool.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\usertool.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\axisconfig.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\bsp_config.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\exiq.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\interlayer.__i"
"D:\KEIL\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\startup_stm32f40_41xxx._ia"
"D:\KEIL\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\App.lnp"
"D:\KEIL\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\App.axf" --i32combined --output ".\Objects\App.hex"
