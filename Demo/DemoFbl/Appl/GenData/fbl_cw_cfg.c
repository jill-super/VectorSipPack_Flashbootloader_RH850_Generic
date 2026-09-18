/* -----------------------------------------------------------------------------
  Filename:    fbl_cw_cfg.c
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: FblWrapperCom_Can
               
               Configuration   : D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Config\DemoFbl_CBD1701035_multi_device.gny
               
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

#define FBL_CW_CFG_SOURCE

#include "fbl_inc.h"

#define FBL_CW_CFG_START_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

V_MEMROM0 V_MEMROM1 vuint32 V_MEMROM2 diagPhysRxIdList[1] = { MK_STDID(0x5A0u) };
V_MEMROM0 V_MEMROM1 vuint32 V_MEMROM2 diagFuncRxIdList[1] = { MK_STDID(0x777u) };
V_MEMROM0 V_MEMROM1 vuint32 V_MEMROM2 diagPhysTxIdList[1] = { MK_STDID(0x5B0u) };
/* RX ID configuration */
V_MEMROM0 V_MEMROM1 tFblCanRxIdList V_MEMROM2 fblCanIdRxConfiguration[FBL_NUMBER_OF_RX_ID] = 
{
  
  {
    1u /* Nodes / nrOfNodes */, 
    1u /* Channels / nrOfChannels */, 
    0u /* MessageObject / fblCanIdTableIndex */, 
    FblRxCanMsg0Hdl /* ReceiveHandle / rxObject */, 
    diagPhysRxIdList /* GenName / idList */, 
    FblCwProcessPhysicalRequest /* IndicationConfirmationFunction / fblCwIndicationHandler */
  }, 
  
  {
    1u /* Nodes / nrOfNodes */, 
    1u /* Channels / nrOfChannels */, 
    1u /* MessageObject / fblCanIdTableIndex */, 
    FblRxCanMsg1Hdl /* ReceiveHandle / rxObject */, 
    diagFuncRxIdList /* GenName / idList */, 
    FblCwProcessFunctionalRequest /* IndicationConfirmationFunction / fblCwIndicationHandler */
  }
};
/* TX ID configuration */
V_MEMROM0 V_MEMROM1 tFblCanTxIdList V_MEMROM2 fblCanIdTxConfiguration[FBL_NUMBER_OF_TX_ID] = 
{
  
  {
    1u /* Nodes / nrOfNodes */, 
    1u /* Channels / nrOfChannels */, 
    0u /* MessageObject / txObject */, 
    diagPhysTxIdList /* GenName / idList */, 
    FblTpConfirmation /* IndicationConfirmationFunction / fblCwConfirmationHandler */
  }
};

#define FBL_CW_CFG_STOP_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281688174
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_cw_cfg.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_cw_cfg.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

