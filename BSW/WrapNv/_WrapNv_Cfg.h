/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        OEM-specific wrapper for FEE access
 *  -------------------------------------------------------------------------------------------------------------------
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \par Copyright
 *  \verbatim
 *  Copyright (c) 2016 by Vector Informatik GmbH.                                                  All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 */
/*********************************************************************************************************************/

/**********************************************************************************************************************
 *  AUTHOR IDENTITY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Name                          Initials      Company
 *  -------------------------------------------------------------------------------------------------------------------
 *  Marco Riedl                   Rie           Vector Informatik GmbH
 *  -------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  -------------------------------------------------------------------------------------------------------------------
 *  02.00.00   2016-02-05  Rie     -                Initial version
 *  02.01.00   2016-03-11  Rie     -                Fixed macros/defines for EepM use-case
 *                                 -                Update to new interface
 *  02.02.00   2016-07-05  Rie     ESCAN00090830    Added support for no NV driver use-case
 *  02.02.01   2016-09-06  Rie     ESCAN00091757    Fixed MISRA findings
 *  02.02.02   2016-10-17  Rie     ESCAN00092351    Configure used FEE functions
 *********************************************************************************************************************/

#ifndef __WRAPNV_CFG_H__
#define __WRAPNV_CFG_H__

/***********************************************************************************************************************
 *  CONFIGURATION SWITCHES
 **********************************************************************************************************************/

#if defined( WRAPNV_USECASE_DRVEEP ) 
#else
#  define WRAPNV_USECASE_DRVEEP
#endif

/***********************************************************************************************************************
 *  COMPATIBILITY DEFINES
 **********************************************************************************************************************/

#if defined( WRAPNV_USECASE_DRVEEP ) 

/* TODO: add configuration for DrvEep/EepM */
/* Below is an example. The configuration can also be copied from an existing configuration */
/*
*                                 ADDRESS        HANDLE         DESCRIPTION
*
*   Memory Layout
*   |
*   +-- ResetFlags                                              Reset Flags
*       |
*       +-- ProgReqFlag           0x40000800     1              Reprogramming request flag
*       |
*       +-- ResetResponseFlag     0x40000801     2              Reset response flag
*/

/* Size defines ************************************************************** */
#define kEepSizeProgReqFlag                  0x01u
#define kEepSizeResetResponseFlag            0x01u
#define kEepSizeResetFlags                   (kEepSizeProgReqFlag + kEepSizeResetResponseFlag)

/* Address defines *********************************************************** */
#define kEepFblBaseAddress                   ((tFblAddress)eepData)
#define kEepStartAddress                     kEepFblBaseAddress
#define kEepAddressResetFlags               kEepStartAddress
#define kEepAddressProgReqFlag               kEepAddressResetFlags
#define kEepAddressResetResponseFlag         (kEepAddressProgReqFlag + kEepSizeProgReqFlag)
#define kEepSizeOfEeprom                     (kEepEndAddress - kEepStartAddress + 1)
/* Initialize NvStructSize-Array */
#define kNvNoOfStructs                       0x01u
#define kNvSizeStructs                       {kEepSizeResetFlags}

/* Handle defines ************************************************************ */
#define kEepMgrFirstHandle                   1
#define kNvHandleStruct0                     0x00u
#define kEepMgrHandle_ResetFlags             NV_MK_STRUCT_ID(kNvHandleStruct0, kEepMgrFirstHandle)
#define kEepMgrHandle_ProgReqFlag            0
#define kEepMgrHandle_ResetResponseFlag      (kEepMgrHandle_ProgReqFlag + 1)
#define kEepMgrNrOfResetFlagsHdls            (kEepMgrHandle_ResetResponseFlag + 1)
#define kEepMgrLastHandle                    (NV_GET_STRUCT_ID(kEepMgrHandle_ResetFlags) + kEepMgrNrOfResetFlagsHdls - 1)
#define kEepMgrNumberOfHandles               (kEepMgrLastHandle - kEepMgrFirstHandle + 1)

/* Access macros ************************************************************* */
#define ApplFblNvReadProgReqFlag(idx, buf)   ((WrapNv_ReturnType)((EepromDriver_RReadSync(buf, kEepSizeProgReqFlag, kEepAddressProgReqFlag) == IO_E_OK) ? WRAPNV_E_OK : WRAPNV_E_NOT_OK))
#define ApplFblNvWriteProgReqFlag(idx, buf)  ((WrapNv_ReturnType)((EepromDriver_RWriteSync(buf, kEepSizeProgReqFlag, kEepAddressProgReqFlag) == IO_E_OK) ? WRAPNV_E_OK : WRAPNV_E_NOT_OK))
#define ApplFblNvReadAsyncProgReqFlag(idx, buf, op)      ApplFblNvReadProgReqFlag(idx, buf)
#define ApplFblNvWriteAsyncProgReqFlag(idx, buf, op)     ApplFblNvWriteProgReqFlag(idx, buf)

#define ApplFblNvReadResetResponseFlag(idx, buf) ((WrapNv_ReturnType)((EepromDriver_RReadSync(buf, kEepSizeResetResponseFlag, kEepAddressResetResponseFlag) == IO_E_OK) ? WRAPNV_E_OK : WRAPNV_E_NOT_OK))
#define ApplFblNvWriteResetResponseFlag(idx, buf) ((WrapNv_ReturnType)((EepromDriver_RWriteSync(buf, kEepSizeResetResponseFlag, kEepAddressResetResponseFlag) == IO_E_OK) ? WRAPNV_E_OK : WRAPNV_E_NOT_OK))
#define ApplFblNvReadAsyncResetResponseFlag(idx, buf, op)      ApplFblNvReadResetResponseFlag(idx, buf)
#define ApplFblNvWriteAsyncResetResponseFlag(idx, buf, op)     ApplFblNvWriteResetResponseFlag(idx, buf)

#define ApplFblIsValidEepAddress(i)          ((i >= kEepStartAddress) && (i <= kEepEndAddress))

#define ApplFblIsValidEepMgrHandle(i)        ((i >= kEepMgrFirstHandle) && (i <= kEepMgrLastHandle))

#endif /* WRAPNV_USECASE_NONE */

#endif /* __WRAPNV_CFG_H__ */

/***********************************************************************************************************************
 *  END OF FILE: WRAPNV_CFG.H
 **********************************************************************************************************************/
