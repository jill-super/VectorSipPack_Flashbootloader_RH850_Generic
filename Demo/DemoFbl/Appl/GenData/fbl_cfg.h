/* -----------------------------------------------------------------------------
  Filename:    fbl_cfg.h
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: GenTool_GenyFblCanBase
               
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

#if !defined(__FBL_CFG_H__)
#define __FBL_CFG_H__

/* -----------------------------------------------------------------------------
    &&&~ 
 ----------------------------------------------------------------------------- */

#define FBL_DISABLE_STAY_IN_BOOT
#define FBL_USE_OWN_MEMCPY
#define FBL_WATCHDOG_ON
#define FBL_WATCHDOG_TIME                    (1 / FBL_REPEAT_CALL_CYCLE)
#define FBL_HEADER_ADDRESS                   0x0200u
#define FBL_ENABLE_APPL_TASK
#define FBL_ENABLE_MULTIPLE_MODULES
#define SWM_DATA_MAX_NOAR                    16
#define FBL_DIAG_BUFFER_LENGTH               2050
#define FBL_DIAG_TIME_P2MAX                  (25 / FBL_REPEAT_CALL_CYCLE)
#define FBL_DIAG_TIME_P3MAX                  (5000 / FBL_REPEAT_CALL_CYCLE)
#define FBL_DISABLE_SLEEPMODE
#define FBL_SLEEP_TIME                       300000
#define FBL_ENABLE_GAP_FILL
#define kFillChar                            0x55u
#define FBL_ENABLE_MULTIPLE_MEM_DEVICES
#define FBL_MEMDRV_SEGMENT_SIZE              1
#define FBL_ENABLE_PRESENCE_PATTERN
#define FBL_ENABLE_FBL_START
#define FBL_DISABLE_RESPONSE_AFTER_RESET
#define FBL_DISABLE_USERSUBFUNCTION
#define FBL_DISABLE_USERSERVICE
#define FBL_DISABLE_USERROUTINE
#define FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD
#define FBL_DIAG_DISABLE_FLASHDRV_ROM
#define FBL_MTAB_NO_OF_BLOCKS                4
/* Data Processing */
#define FBL_ENABLE_DATA_PROCESSING
#define FBL_ENABLE_ENCRYPTION_MODE
#define FBL_DISABLE_COMPRESSION_MODE
#define FBL_MEM_PROC_BUFFER_SIZE             0x0100
/* Project State */
#define FBL_INTEGRATION                      2
#define FBL_PRODUCTION                       1
#define FBL_PROJECT_STATE                    FBL_INTEGRATION
#define FBL_ENABLE_SYSTEM_CHECK
#define FBL_ENABLE_DEBUG_STATUS
#define FBL_ENABLE_ASSERTION
/* FblLib_Mem */
#define FBL_MEM_DISABLE_VERIFY_PIPELINED
#define FBL_MEM_ENABLE_VERIFY_OUTPUT
#define FBL_MEM_VERIFY_SEGMENTATION          0x40
#define FBL_DISABLE_ADAPTIVE_DATA_TRANSFER_RCRRP
#define FBL_DISABLE_PIPELINED_PROGRAMMING
#define FBL_MEM_WRITE_SEGMENTATION           0x0100
#define FBL_ENABLE_UNALIGNED_DATA_TRANSFER
/* CAN Identifier */

/* RH850 specific ************************************************************ */
#define FLASH_SIZE                           8192
#define FBL_TIMER_PRESCALER_VALUE            0x01
#define FBL_TIMER_RELOAD_VALUE               0x9C3F
#define FBL_SYSTEM_FREQUENCY                 160
#define FLASH_ENABLE_MACHINE_CHECK_ECC_DETECTION

#define CAN_BCFG                             0x140009
#define kFblCanBaseAdr                       0xFFD20000u
#define kFblCanMaxPhysChannels               6u
#define FBL_HW_DISABLE_ALTERNATIVE_CLOCK_SOURCE
#define FBL_CAN_0
#define kFblCanChannel                       0x00

/* Manufacturer specific part ************************************************ */
/* FBL multiple nodes support: */
#define FBL_DISABLE_MULTIPLE_NODES

#define FBL_ENABLE_SEC_ACCESS_DELAY
#define FBL_SEC_ACCESS_DELAY_TIME            10000u
#define FBL_DIAG_COMMUNICATION_CONTROL_TYPE  kDiagSubEnableRxAndDisableTx
#define FBL_DIAG_ENABLE_CONTROLDTC_OPTIONRECORD
#define FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS
#define FBL_APPL_DISABLE_STARTUP_DEPENDENCY_CHECK


/* -----------------------------------------------------------------------------
    &&&~ 
 ----------------------------------------------------------------------------- */

/* User Config File ********************************************************** */
#define FBL_ENABLE_VECTOR_HW
/* User Section ************************************************************** */
#define FBL_ENABLE_CAN_CONFIRMATION
#define FBL_ENABLE_SECMOD_VECTOR
#if !defined( FBL_DISABLE_WRAPPER_NV )
# define FBL_ENABLE_WRAPPER_NV
#endif

/* Task handling in RAM */

/* User task handling */
#if defined( FBL_ENABLE_APPL_TASK )
# if !defined( FBL_DISABLE_APPL_STATE_TASK )
#  define FBL_ENABLE_APPL_STATE_TASK
# endif
# if !defined( FBL_DISABLE_APPL_TIMER_TASK )
#  define FBL_ENABLE_APPL_TIMER_TASK
# endif
#endif

#if defined( FBL_ENABLE_MULTIPLE_NODES )
# undef FBL_ENABLE_MULTIPLE_NODES
#endif
/* *************************************************************************** */


/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281688174
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\fbl_cfg.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 281688174
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __FBL_CFG_H__ */
