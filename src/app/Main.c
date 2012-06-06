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
�汾�޸�˵��
APP V2.30: �Ӹð汾���ټ���PCB_V2.6���°汾������
           �Ӹð汾���ټ���SYS_V1.31���°汾
           ���߶�ʱ�޸�Ϊ600��(Main.c)
           �޸ļ�������µ�SYS�⺯��(BIOS.s)
           �޸��˿�����Ϣ��ʾ����(Main.c)
APP V2.31: �����˿���ʶ��FPGA���������������б�(Main.c)
           ������LicenceȨ�޹����ܵ�Demo������(Ident.c,Main.c)
           �޸���ģ��ͨ��У�����ܵĽ�����˳���ز���(calibrat.c)
           ������144MHz�������ģʽ�µ���ع���(Process.c)
APP V2.32  �Ӹð汾��ɲ���ʹ��IAR 4.42��5.0�汾
           Դ����û�Ķ����������ļ��С�IAR_V5_Prpject
APP V2.33  �޸���ɨ��ʱ��<1uSʱ����ʾˢ�µ�BUG(Process.c)
           �޸�����У׼״̬�£�������ʾ��Ϣ��BUG(Calibrat.c)
APP V2.34  ��Ϊ��ͨ������У׼(Calibrat.c & Main.c)
           �޸���У׼��ѡ��Ĳ�����ʽ(Calibrat.c)
APP V2.35  �޸���У׼�����е�BUG(Calibrat.c)
           �޸���ɨ��ʱ��<5uSʱ����ͣ���˵�BUG(Process.c)
           �Ż�����ʾ���ݴ������(Process.c)
           ������ģ��ͨ���Զ����ƽ�⹦��(Main.c,Process.c,Calibrat.c)
APP V2.36  ����У׼������Ϊ�Զ�ģʽ(Calibrat.c,Process.c,Function.c)
           �޸��˿������ع��������ķ�ʽ(Main.c)
APP V2.37  ��һ�����ƺ��Ż�����ʾ���ݴ������(Process.c)
           �޸���32λ�з��ż��޷�������ת���������������BUG(Function.c)
           ������ʱ��Ƶ������ռ�ձȲ�������(Process.c, Menu.c)
APP V2.40  ������дU�̴����ļ�������(Main.c, Flies.c, dosfs.c)
           �޸Ĵ���ʱ��ʾ�ļ����BUG(Menu.c) 
APP V2.41  �������ļ���ʽΪ.BUF�Ķ�/д���������������ļ�(Main.c,Flies.c,Menu.c)
           �������ļ���ʽΪ.CSV�ĵ������������������ļ�(Main.c,Flies.c,Menu.c)
APP V2.42  Ϊ��ʡ�ռ佫�ļ�ϵͳת�Ƶ�SYS_V1.40ģ����(ASM.s, Flies.c, dosfs.c)
           ��Ϊ��"SerialNo.WPT"���ļ���ʽ���湤��������(Flies.c)
           ע��APP V2.42���ϰ汾������SYS V1.40���ϰ汾һ�����ʹ��
APP V2.43  �޸���ģ��ͨ����λ����ʱ��BUG(Main.c)
APP V2.44  �޸���У׼����ʱ���������BUG(Calibrat.c)
           �����˿������ز����ɹ������ʾ(Main.c)
APP V2.45  �޸��˶�дBUF�ļ�ʱ�ָ���ʾ�˵��и�����Ӧ��ʱ��BUG(Files.c)
           ɾ���˶�BUF�ļ�ʱ�Ĳ�����Ϣ������ʾ(Main.c)
APP V2.50  ��д�˻�����FAT12�ļ�ϵͳ���ļ���д����(Files.c, ASM.s)
           �޸���TH,TL������ʾ��BUG(Menu.c)
           �Ż��˴�������ֵ��ʾ��غ���(Menu.c,Function.c,Calibrat.c)
           �޸��������������BUG(Process.c)
APP V2.51  �޸���Vmin,Vmax,Vpp������BUG(Process.c)
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
  //  Load_Attr();                                 // ��ֵY_Attr��

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
