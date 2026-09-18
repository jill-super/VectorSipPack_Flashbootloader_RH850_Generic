/* -----------------------------------------------------------------------------
  Filename:    WrapNv_cfg.c
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: SysService_WrapperNv
               
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

#include "WrapNv_inc.h"
#ifdef FBL_ENABLE_EEPMGR
#else
#ifdef NV_ENABLE_ADDRESS_LOOKUP
V_MEMROM0 V_MEMROM1_FAR tNvRecordTbl V_MEMROM2_FAR kNvRecordTbl[] = 
{
  
  {
    kEepMgrHandle_ProgReqFlag, 
    kNvAccessSingle, 
    kEepAddressProgReqFlag, 
    kEepSizeProgReqFlag
  }, 
  
  {
    kEepMgrHandle_ResetResponseFlag, 
    kNvAccessSingle, 
    kEepAddressResetResponseFlag, 
    kEepSizeResetResponseFlag
  }, 
  
  {
    kEepMgrHandle_ApplValidity, 
    kNvAccessSingle, 
    kEepAddressApplValidity, 
    kEepSizeApplValidity
  }, 
  
  {
    kEepMgrHandle_ValidityFlags, 
    kNvAccessSingle, 
    kEepAddressValidityFlags, 
    kEepSizeValidityFlags
  }, 
  
  {
    kEepMgrHandle_SecAccessDelayFlag, 
    kNvAccessSingle, 
    kEepAddressSecAccessDelayFlag, 
    kEepSizeSecAccessDelayFlag
  }, 
  
  {
    kEepMgrHandle_SecAccessInvalidCount, 
    kNvAccessSingle, 
    kEepAddressSecAccessInvalidCount, 
    kEepSizeSecAccessInvalidCount
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_Fingerprint, 
    kNvAccessStructured, 
    kEepAddressFingerprint, 
    kEepSizeFingerprint
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_ProgCounter, 
    kNvAccessStructured, 
    kEepAddressProgCounter, 
    kEepSizeProgCounter
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_ProgAttempts, 
    kNvAccessStructured, 
    kEepAddressProgAttempts, 
    kEepSizeProgAttempts
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_CRCValue, 
    kNvAccessStructured, 
    kEepAddressCRCValue, 
    kEepSizeCRCValue
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_CRCStart, 
    kNvAccessStructured, 
    kEepAddressCRCStart, 
    kEepSizeCRCStart
  }, 
  
  {
    kEepMgrHandle_Metadata + kEepMgrHandle_CRCLength, 
    kNvAccessStructured, 
    kEepAddressCRCLength, 
    kEepSizeCRCLength
  }
};
V_MEMROM0 V_MEMROM1_FAR vuint8 V_MEMROM2_FAR kNvNoOfNvRecords = sizeof(kNvRecordTbl) / sizeof(kNvRecordTbl[0]);
#endif

#endif


/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281181119
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\WrapNv_cfg.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\WrapNv_cfg.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

