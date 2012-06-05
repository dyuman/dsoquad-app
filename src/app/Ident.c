/********************* (C) COPYRIGHT 2010 e-Design Co.,Ltd. ********************
 File Name : Authenticate.c  
 Version   : DS203_APP Ver 2.3x                                  Author : bure
*******************************************************************************/
#include "Interrupt.h"
#include "Function.h"
#include "Ident.h"
#include "BIOS.h"

/************************** Licence Block defination ***************************
offset 0x00~0x03: ProjectID 4Bytes (example:  0x12345678)  
offset 0x04~0x07: LicenceNo 4Bytes (example:  0x0A1B2C3D)
offset 0x08~0x0B: DemoCnt   4Bytes (example:  0~0xFFFFFFFF Sec.)
offset 0x0C~0x1F: Developer Str 20Bytes (example: "e-Design 2011.3")
*******************************************************************************/

/*******************************************************************************
 Chk_DemoCnt: Check demo counter limit                       RET: 1 = Time out
*******************************************************************************/
u8 Chk_DemoCnt(u16 Offset, u32 Sec_Cnt)
{
  if((*(u32*)(REC_BASE + Offset + 8))>= Sec_Cnt) return 1;
  else                                           return 0;
}
/*******************************************************************************
 Get_Cnt: Get demo counter value from recoed                    RET: Cnt value
*******************************************************************************/
u8 Get_Cnt(u16 Offset)
{
  u8 i, Result = 0;
  
  for(i=0; i<32; i++){
    if(((*(u32*)(REC_BASE + Offset +  8))&(1<<i))!=0)  Result++;
    if(((*(u32*)(REC_BASE + Offset + 12))&(1<<i))!=0)  Result++;
  }  
  return Result; 
}
/*******************************************************************************
 Get_Lic: Get licence number from recoed                   RET: licence number
*******************************************************************************/
u32 Get_Lic(u16 Offset)
{
      return *(u32*)(REC_BASE + Offset + 4); 
}
/*******************************************************************************
 Seek_Proj: Seek Project ID from recoeds         RET: offset, 2048 = Not found 
*******************************************************************************/
u16 Seek_Proj(u32 Proj_ID)
{
  u16 i;
  for(i=32; i<2048; i+=32){
    if(Proj_ID ==(*(u32*)(REC_BASE+i)))  return i;
  }
  return 2048;
}
/*******************************************************************************
 Seek_Blank: Seek Blank from recoeds             RET: offset, 2048 = Not found 
*******************************************************************************/
u16 Seek_Blank(void)
{
  u16 i, j;
  u32 Tmp;
  
  for(i=32; i<2048; i+=32){
    Tmp = 0xFFFFFFFF;
    for(j=0; j<32; j+=4)  Tmp &= *(u32*)(REC_BASE+i+j);
    if(Tmp == 0xFFFFFFFF)  return i;
  }
  return 2048;
}
/*******************************************************************************
 Add_Proj: Add a Project ID into recoed                  RET: 1 = Ok, 0 = Fail
*******************************************************************************/
u8 Add_Proj(u32 ProjectID, u16 Offset) 
{
  u16 Result;
  
  __FLASH_Unlock();
  Result  = __FLASH_Prog(REC_BASE + Offset + 0, ProjectID & 0xFFFF);
  Result &= __FLASH_Prog(REC_BASE + Offset + 2, ProjectID >> 16);
  __FLASH_Lock();
  return Result;
}
/*******************************************************************************
 Add_Str: Add a Project string into recoed               RET: 1 = Ok, 0 = Fail
*******************************************************************************/
u8 Add_Str(u32 ProjStrAddr, u16 Offset) 
{
  u8 i, Result = FLASH_COMPLETE;
  u16 Data;
  __FLASH_Unlock();
  for(i=0; i<16; i+=2){
    Data  = *(u8*)ProjStrAddr;
    ProjStrAddr++;
    Data |= (*(u8*)ProjStrAddr)<<8;
    ProjStrAddr++;
    Result &= __FLASH_Prog(REC_BASE + Offset + i + 16, Data);
  }
  __FLASH_Lock();
  return Result;
}
/*******************************************************************************
 Add_Cnt: Add demo counter value into recoed             RET: 1 = Ok, 0 = Fail
*******************************************************************************/
u8 Add_Cnt(u8 DemoCnt, u16 Offset)
{
  u32 TmpL, TmpH;
  u8  i, Result;
  
  __FLASH_Unlock();
  TmpH = 0;
  TmpL = 0;
  if(DemoCnt != 0){
    if(DemoCnt <= 32)  for(i=0; i<DemoCnt; i++)  TmpL |= 1<<i; 
    else {
      TmpL = 0xFFFFFFFF;
      for(i=0; i<(DemoCnt-32); i++)  TmpH |= 1<<i;
    }
  }
  Result  = __FLASH_Prog(REC_BASE + Offset +  8, TmpL & 0xFFFF);
  Result &= __FLASH_Prog(REC_BASE + Offset + 10, TmpL >> 16);
  Result &= __FLASH_Prog(REC_BASE + Offset + 12, TmpH & 0xFFFF);
  Result &= __FLASH_Prog(REC_BASE + Offset + 14, TmpH >> 16);
  __FLASH_Lock();
  return Result;
}
/*******************************************************************************
 Add_Lic: Add Licence number into recoed                 RET: 1 = Ok, 0 = Fail
*******************************************************************************/
u8 Add_Lic(u32 LicenceNo, u32 Offsaet) 
{
  u8 Result;
  
  __FLASH_Unlock();
  Result  = __FLASH_Prog(REC_BASE+Offsaet+4, LicenceNo & 0xFFFF);
  Result &= __FLASH_Prog(REC_BASE+Offsaet+6, LicenceNo >> 16);
  __FLASH_Lock();
  return Result;
}
/*******************************************************************************
 Input_Lic: Input 32 Bytes Licence                         RET: licence number
*******************************************************************************/
u32 Input_Lic(u16 x0, u8 y0) // Return: 32Bits Lic 
{
  u8  i=0, j;
  char n[2]={"0"};
  u32 Lic=0;
  u8  Type = 0;
  char NumStr[9];
  
  Word2Hex(NumStr, Lic);
  __Display_Str(x0, y0, 0xFFFF, 0, NumStr);
  
  while (i<8){
    Word2Hex(NumStr, Lic);
    if( Type != Twink ){  // Blink current number each 0.5 Sec.
      Type = Twink;
      for(j=0; j<8; ++j){ // ˢ����ʾ8λ���֣���ǰλ��˸
        n[0] = NumStr[j];
        if(i==j) __Display_Str(j*8+ x0, y0, 0xFFFF, Type, n);
        else     __Display_Str(j*8+ x0, y0, 0xFFFF, 0,  n);
      }
    }
    if(Key_Buffer != 0){
      switch (Key_Buffer){  
      case K_INDEX_DEC:
        if(((Lic >>((7-i)*4))& 0x0F )> 0) Lic -= 0x01<<((7-i)*4);
        else                              Lic += 0x0F<<((7-i)*4);
        break;
      case K_INDEX_INC:
        if(((Lic >>((7-i)*4))& 0x0F )<15) Lic += 0x01<<((7-i)*4);
        else                              Lic -= 0x0F<<((7-i)*4);
        break;
      case K_ITEM_INC:
        if(i<7) i++;
        else    i=0;
        break;
      case K_ITEM_DEC:
        if(i>0) i--;
        else    i=7;
        break;
      case KEY2:
        i = 8;        // Exit 
        break;
      }
      Key_Buffer = NO_KEY;
    }
  }
  return Lic;
}
/********************************* END OF FILE ********************************/
