/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Main.c  
 Version   : DS203_APP Ver 2.5x                                  Author : bure
*******************************************************************************/
#include "Interrupt.h"
#include "Function.h"
#include "Calibrat.h"
#include "Process.h"
#include "Ident.h"
#include "Draw.h"
#include "BIOS.h"
#include "Menu.h"
#include "File.h"

/*******************************************************************************
版本修改说明
APP V2.30: 从该版本起不再兼容PCB_V2.6以下版本的主板
           从该版本起不再兼容SYS_V1.31以下版本
           休眠定时修改为600秒(Main.c)
           修改及添加了新的SYS库函数(BIOS.s)
           修改了开机信息显示程序(Main.c)
APP V2.31: 增加了开机识别FPGA加载配置完成与否判别(Main.c)
           增加了Licence权限管理功能的Demo程序范例(Ident.c,Main.c)
           修改了模拟通道校正功能的进入和退出相关操作(calibrat.c)
           增加了144MHz交替采样模式下的相关功能(Process.c)
APP V2.32  从该版本起可并行使用IAR 4.42与5.0版本
           源程序没改动，增加了文件夹　IAR_V5_Prpject
APP V2.33  修改了扫描时基<1uS时，显示刷新的BUG(Process.c)
           修改了在校准状态下，操作提示信息的BUG(Calibrat.c)
APP V2.34  改为按通道单独校准(Calibrat.c & Main.c)
           修改了校准项选择的操作方式(Calibrat.c)
APP V2.35  修改了校准过程中的BUG(Calibrat.c)
           修改了扫描时基<5uS时，暂停不了的BUG(Process.c)
           优化了显示数据处理程序(Process.c)
           增加了模拟通道自动零点平衡功能(Main.c,Process.c,Calibrat.c)
APP V2.36  部分校准操作改为自动模式(Calibrat.c,Process.c,Function.c)
           修改了开机加载工作参数的方式(Main.c)
APP V2.37  进一步完善和优化了显示数据处理程序(Process.c)
           修改了32位有符号及无符号整数转换程序四舍五入的BUG(Function.c)
           增加了时间频率脉宽占空比测量功能(Process.c, Menu.c)
APP V2.40  增加了写U盘创建文件名功能(Main.c, Flies.c, dosfs.c)
           修改存盘时显示文件序号BUG(Menu.c) 
APP V2.41  增加了文件格式为.BUF的读/写采样缓冲区数据文件(Main.c,Flies.c,Menu.c)
           增加了文件格式为.CSV的导出采样缓冲区数据文件(Main.c,Flies.c,Menu.c)
APP V2.42  为节省空间将文件系统转移到SYS_V1.40模块上(ASM.s, Flies.c, dosfs.c)
           改为用"SerialNo.WPT"的文件形式保存工作参数表(Flies.c)
           注：APP V2.42以上版本必须与SYS V1.40以上版本一起配合使用
APP V2.43  修改了模拟通道档位调整时的BUG(Main.c)
APP V2.44  修改了校准操作时保存参数的BUG(Calibrat.c)
           增加了开机加载参数成功与否提示(Main.c)
APP V2.45  修改了读写BUF文件时恢复显示菜单中各个对应项时的BUG(Files.c)
           删除了读BUF文件时的测试信息反馈显示(Main.c)
APP V2.50  重写了基于新FAT12文件系统的文件读写程序(Files.c, ASM.s)
           修改了TH,TL测量显示的BUG(Menu.c)
           优化了带量纲数值显示相关函数(Menu.c,Function.c,Calibrat.c)
           修改了脉宽触发程序的BUG(Process.c)
APP V2.51  修改了Vmin,Vmax,Vpp计量的BUG(Process.c)
*******************************************************************************/

#define APP_VERSION       "     DS203 Mini DSO APP Ver 2.51 (gcc)"

uc8 PROJECT_STR[20] = "Demo PROG. Ver 1.00";

extern u8 _vectors[];

/*******************************************************************************
  main : Main routine.
*******************************************************************************/
int main(void)
{ 
  u16 i;
  u32 Batt_Volt;
  char Batt_Volt_Str[6];
  u32 FIFO_readout;
  u32 FIFO_readout_avg;
  char FIFO_readout_avg_Str[6];

  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (u32)&_vectors);
  Y_Attr = (Y_attr*)__Get(VERTICAL);     
  X_Attr = (X_attr*)__Get(HORIZONTAL);
  G_Attr = (G_attr*)__Get(GLOBAL);
  T_Attr = (T_attr*)__Get(TRIGGER);
  //  Load_Attr();                                 // 赋值Y_Attr等

  Balance();
  App_init();
  
  while(1) {
    Batt_Volt = __Get(V_BATTERY); // Get the battery voltage
    u16ToDec5(Batt_Volt_Str, Batt_Volt);// convert it to a String type
    __Display_Str(30, 30, PURPL, PRN, Batt_Volt_Str);// Display it
    __Set(FIFO_CLR, W_PTR);
    Delayms(20);
    FIFO_readout_avg = 0;
    for(i = 0; i < 4096; i++) {
      FIFO_readout = __Read_FIFO();
      FIFO_readout = (FIFO_readout & 0xFF); // Ch A
      FIFO_readout_avg += FIFO_readout;
    }
    FIFO_readout_avg = FIFO_readout_avg / 4096;
    u16ToDec5(FIFO_readout_avg_Str, FIFO_readout_avg);
    __Display_Str(60, 60, WHT, PRN, FIFO_readout_avg_Str);
    Delayms(100);
   //    __Set(BEEP_VOLUME, 50); // Reload volume
    //Beep_mS = 500;                                        // Beep
    if(Key_Buffer != 0) {
      if(Key_Buffer == KEY_P) { // Check if there is any inut
	__Display_Str(0,0,GRN,PRN,"1");
	Delayms(1000);
	__Display_Str(0,0,BLACK,PRN,"1");
      } 
      if(Key_Buffer == KEY2) {
	__Display_Str(0,0,GRN,PRN,"2");
	Delayms(1000);
	__Display_Str(0,0,BLACK,PRN,"2");
      }
      if(Key_Buffer == KEY3) {
	__Display_Str(0,0,GRN,PRN,"3");
	Delayms(1000);
	__Display_Str(0,0,BLACK,PRN,"3");
      } 
      if(Key_Buffer == KEY4) {
	__Display_Str(0,0,GRN,PRN,"4");
	Delayms(1000);
	__Display_Str(0,0,BLACK,PRN,"4");
      } 
      Key_Buffer = 0;
    }
    __Point_SCR(50,50);
    __LCD_SetPixl(GRN);
  }
}
