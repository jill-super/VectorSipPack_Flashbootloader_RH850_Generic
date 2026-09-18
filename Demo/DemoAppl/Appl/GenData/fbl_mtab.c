/* -----------------------------------------------------------------------------
  Filename:    fbl_mtab.c
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

#define FBL_LBT_SOURCE

/* Includes ****************************************************************** */
#include "fbl_inc.h"

/* Global data *************************************************************** */
#define FBL_LBT_START_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */
V_MEMROM0 V_MEMROM1 tLogicalBlockTable V_MEMROM2 FblLogicalBlockTable = 
{
  FBL_MTAB_MAGIC_FLAG, 
  
  {
    0x3
  } /* Disposability bit field (mandatory/optional) */, 
  FBL_MTAB_NO_OF_BLOCKS, 
  
  {
    
    {
      0, 
      0x018000u /* Block Start Address */, 
      0x028000u /* Block Length (End Address = 0x3FFFF) */, 
      0x00 /* Max Prog Attempts */, 
      (tExportFct)SecM_Verification /* Output Verification */
    } /* Name 'DemoAppl', Mandatory */, 
    
    {
      1, 
      0x040000u /* Block Start Address */, 
      0x060000u /* Block Length (End Address = 0x9FFFF) */, 
      0x00 /* Max Prog Attempts */, 
      (tExportFct)SecM_Verification /* Output Verification */
    } /* Name 'Cal1', Mandatory */, 
    
    {
      2, 
      0x0A0000u /* Block Start Address */, 
      0x060000u /* Block Length (End Address = 0xFFFFF) */, 
      0x00 /* Max Prog Attempts */, 
      (tExportFct)SecM_Verification /* Output Verification */
    } /* Name 'Cal2', Optional */
  }
};
#define FBL_LBT_STOP_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */


/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281181119
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_mtab.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_mtab.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

