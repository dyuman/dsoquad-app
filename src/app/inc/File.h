/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : file.h  
 Version   : DS203_APP Ver 2.5x                                  Author : bure
*******************************************************************************/
#ifndef __FILE_H
#define __FILE_H

#include "stm32f10x_lib.h"

#define FLASH_WAIT_TIMEOUT      100000 
#define PAGE_0                  0 
#define PAGE_1                  1 
#define PAGE_2                  2 
#define PAGE_3                  3 

#define OK           0            // �������
#define SEC_ERR      1            // ������д����
#define FAT_ERR      2            // FAT���д����
#define OVER         3            // �������
#define NEW          4            // �հ�/��Ŀ¼��
#define EMPT         4            // �ļ�������
#define VER_ERR      5            // �汾����
#define SUM_ERR      6            // У��ʹ���
#define RD_ERR       8            // ����������
#define WR_ERR       9            // ����д����
#define DISK_ERR     10           // ���̴���

extern u8  FileBuff[1200];

u8 Save_Bmp(u8 FileNum);
u8 Color_Num(u16 Color);
u8 Load_Param(void);
u8 Save_Param(void);
u8 Save_Dat(u8 FileNum);
u8 Load_Dat(u8 FileNum);
u8 Save_Buf(u8 FileNum);
u8 Load_Buf(u8 FileNum);
u8 Save_Csv(u8 FileNum);
u8 Make_Filename(u8 FileNum,char* FileName);
void reset_parameter(void);
#endif
/********************************* END OF FILE ********************************/
