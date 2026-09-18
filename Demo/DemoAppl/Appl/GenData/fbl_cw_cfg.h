/* -----------------------------------------------------------------------------
  Filename:    fbl_cw_cfg.h
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: FblWrapperCom_Can
               
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

#if !defined(__FBL_CW_CFG_H__)
#define __FBL_CW_CFG_H__

/* Configuration types */
typedef void (*tFblCwIndicationHandler)( vuint8, volatile vuint8* );
typedef void (*tFblCwConfirmationHandler)( vuint8);
/* Receive identifier structure */
typedef struct tFblCanRxIdListTag
{
  vuintx nrOfNodes; /* Number of nodes, e.g. used by multiple nodes setups */
  vuintx nrOfChannels; /* Number of channels, e.g. used by multiple platforms setups */
  vuintx fblCanIdTableIndex; /* Index into FblCanIdTable */
  vuintx rxObject; /* Message object parameter in FblHandleRxMsg */
  V_MEMROM1 vuint32 V_MEMROM2 V_MEMROM3 * idList; /* Flattened ID list */
  tFblCwIndicationHandler fblCwIndicationHandler; /* Message handler called by FblHandleRxMsg */
} tFblCanRxIdList;
/* Transmit identifier structure */
typedef struct tFblCanTxIdListTag
{
  vuintx nrOfNodes; /* Number of nodes, e.g. used by multiple nodes setups */
  vuintx nrOfChannels; /* Number of channels, e.g. used by multiple platforms setups */
  vuintx txObject; /* Message object parameter in FblHandleRxMsg */
  V_MEMROM1 vuint32 V_MEMROM2 V_MEMROM3 * idList; /* Flattened ID list */
  tFblCwConfirmationHandler fblCwConfirmationHandler; /* Confirmation function called after message has been transmitted */
} tFblCanTxIdList;
/* Basic CAN defines */
/* Receive identifiers */
#define FBL_CAN_NUMBER_OF_RX_ID              2u
#define FBL_CAN_NUMBER_OF_RANGES             0u
#define FBL_NUMBER_OF_RX_ID                  (FBL_CAN_NUMBER_OF_RX_ID + FBL_CAN_NUMBER_OF_RANGES)
/* Special configuration for NormalFixed Addressing */
#define FBL_CW_DISABLE_NORMAL_FIXED_ADDRESSING
#define FBL_DISABLE_CAN_RX_RANGE
/* Special configuration for Extended Addressing */
#define FBL_CW_DISABLE_EXTENDED_ADDRESSING
/* Special configuration for Mixed Addressing */
#define FBL_CW_DISABLE_MIXED_ADDRESSING
/* Response identifiers */
#define FBL_NUMBER_OF_TX_ID                  1u
#define kFblCwDiagTxObject                   0u

#define FBL_CW_CFG_START_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/* Configuration */
V_MEMROM0 extern  V_MEMROM1 vuint32 V_MEMROM2 diagPhysRxIdList[1];
V_MEMROM0 extern  V_MEMROM1 vuint32 V_MEMROM2 diagFuncRxIdList[1];
V_MEMROM0 extern  V_MEMROM1 vuint32 V_MEMROM2 diagPhysTxIdList[1];
V_MEMROM0 extern  V_MEMROM1 tFblCanRxIdList V_MEMROM2 fblCanIdRxConfiguration[FBL_NUMBER_OF_RX_ID];
V_MEMROM0 extern  V_MEMROM1 tFblCanTxIdList V_MEMROM2 fblCanIdTxConfiguration[FBL_NUMBER_OF_TX_ID];
#define FBL_CW_NUMBER_OF_CHANNELS            1
#define FBL_CW_DISABLE_MULTIPLE_CHANNELS
#define FBL_CW_NUMBER_OF_NODES               1
#define FBL_CW_DISABLE_MULTIPLE_NODES

#define FBL_CW_CFG_STOP_SEC_CONST
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */

/* User Config File ********************************************************** */
/* User Section ************************************************************** */
/* FblWrapperCom_Can feature selection */
#define FBL_CW_ENABLE_PHYSICAL_REQUEST_HANDLER
#define FBL_CW_ENABLE_FUNCTIONAL_REQUEST_HANDLER

#if defined( FBL_ENABLE_STAY_IN_BOOT )
# define CAN_RX_SLOT_STARTMSG CAN_RX_SLOT_0
#endif /* FBL_ENABLE_STAY_IN_BOOT */
/* *************************************************************************** */

/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281181119
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_cw_cfg.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 281181119
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __FBL_CW_CFG_H__ */
