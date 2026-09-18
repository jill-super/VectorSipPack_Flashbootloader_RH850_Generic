/* -----------------------------------------------------------------------------
  Filename:    fbl_apfb.c
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: GenTool_GenyFblCanBase
               
               Configuration   : D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Config\DemoFbl_CBD1701035.gny
               
               ECU: 
                       TargetSystem: Hw_Rh850Cpu
                       Compiler:     GreenHills
                       Derivates:    P1M
               
               Channel "Channel0":
                       Databasefile: D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Config\DemoFBL_Vector_SLP3.dbc
                       Bussystem:    CAN
                       Manufacturer: Vector
                       Node:         Demo_0_CAN11

 ----------------------------------------------------------------------------- */
/* -----------------------------------------------------------------------------
  C O P Y R I G H T
 -------------------------------------------------------------------------------
  Copyright (c) 2001-2015 by Vector Informatik GmbH. All rights reserved.
 
  This software is copyright protected and proprietary to Vector Informatik 
  GmbH.
  
  Vector Informatik GmbH grants to you only those rights as set out in the 
  license conditions.
  
  All other rights remain with Vector Informatik GmbH.
 -------------------------------------------------------------------------------
 ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
    &&&~ 
 ----------------------------------------------------------------------------- */

#define FBL_FBT_SOURCE

/* Includes ****************************************************************** */
#include "fbl_inc.h"

/* Global data *************************************************************** */
#define FBL_FBT_START_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */
V_MEMROM0 V_MEMROM1_FAR tFlashBlock V_MEMROM2_FAR FlashBlock[18] = 
{
  
  {
    0x018000u /* 32KB Block5 */, 
    0x01FFFFu
  }, 
  
  {
    0x020000u /* 32KB Block6 */, 
    0x027FFFu
  }, 
  
  {
    0x028000u /* 32KB Block7 */, 
    0x02FFFFu
  }, 
  
  {
    0x030000u /* 32KB Block8 */, 
    0x037FFFu
  }, 
  
  {
    0x038000u /* 32KB Block9 */, 
    0x03FFFFu
  }, 
  
  {
    0x040000u /* 64KB Block10 */, 
    0x04FFFFu
  }, 
  
  {
    0x050000u /* 64KB Block11 */, 
    0x05FFFFu
  }, 
  
  {
    0x060000u /* 64KB Block12 */, 
    0x06FFFFu
  }, 
  
  {
    0x070000u /* 64KB Block13 */, 
    0x07FFFFu
  }, 
  
  {
    0x080000u /* 64KB Block14 */, 
    0x08FFFFu
  }, 
  
  {
    0x090000u /* 64KB Block15 */, 
    0x09FFFFu
  }, 
  
  {
    0x0A0000u /* 64KB Block16 */, 
    0x0AFFFFu
  }, 
  
  {
    0x0B0000u /* 64KB Block17 */, 
    0x0BFFFFu
  }, 
  
  {
    0x0C0000u /* 64KB Block18 */, 
    0x0CFFFFu
  }, 
  
  {
    0x0D0000u /* 64KB Block19 */, 
    0x0DFFFFu
  }, 
  
  {
    0x0E0000u /* 64KB Block20 */, 
    0x0EFFFFu
  }, 
  
  {
    0x0F0000u /* 64KB Block21 */, 
    0x0FFFFFu
  }, 
  
  {
    0x01000000u /* Extended User Area (32KB) */, 
    0x01007FFFu
  }
};
V_MEMROM0 V_MEMROM1_FAR vuint8 V_MEMROM2_FAR kNrOfFlashBlock = sizeof(FlashBlock) / sizeof(FlashBlock[0]);
#define FBL_FBT_STOP_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/* Try to check consistency between memory device driver */
/* header files (e.g. flashdrv.h) and GENy configuration */
#ifdef FLASH_SEGMENT_SIZE
#if (FLASH_SEGMENT_SIZE != 256)
# error "Consistency check failed! Adjust GENy configuration!"
#endif

#endif

#ifdef FBL_FLASH_DELETED
#if (FBL_FLASH_DELETED != 0xFFu)
# error "Consistency check failed! Adjust GENy configuration!"
#endif

#endif



/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281181119
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_apfb.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_apfb.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

