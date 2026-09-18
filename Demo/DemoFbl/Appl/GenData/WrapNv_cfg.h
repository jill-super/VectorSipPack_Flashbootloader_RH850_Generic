/* -----------------------------------------------------------------------------
  Filename:    WrapNv_cfg.h
  Description: Toolversion: 07.03.01.01.70.10.35.00.00.00
               
               Serial Number: CBD1701035
               Customer Info: Nexteer Automotive (Suzhou) Co.
                              Package: FBL Vector SLP3 - CANfbl license for the project EPS for OEMs without manufacturer specific requirements
                              Micro: R7F701313EAFP 
                              Compiler: GreenHills 2015.1.7
               
               
               Generator Fwk   : GENy 
               Generator Module: SysService_WrapperNv
               
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

#if !defined(__WRAPNV_CFG_H__)
#define __WRAPNV_CFG_H__

#ifdef FBL_ENABLE_EEPMGR
#else
#define NV_ENABLE_ADDRESS_LOOKUP
#endif


/*
*                                 ADDRESS        HANDLE         DESCRIPTION
*
*   Memory Layout
*   |
*   +-- ProgReqFlag               0xfede3000     1              Programming request flag
*   |
*   +-- ResetResponseFlag         0xfede3001     2              Reset response flag
*   |
*   +-- ApplValidity              0xfede3002     3              Application validity flag
*   |
*   +-- ValidityFlags             0xfede3003     4              Logical block validity flags
*   |
*   +-- SecAccessDelayFlag        0xfede3004     5              Security Access Delay flag
*   |
*   +-- SecAccessInvalidCount     0xfede3005     6              Security Access Invalid count
*   |
*   +-- 2 x Metadata                                            Internal meta data (for each logical block)
*       |
*       +-- Fingerprint           0xfede3006     7              Download fingerprint
*       |
*       +-- ProgCounter           0xfede300f     8              Successful reprogramming attempts
*       |
*       +-- ProgAttempts          0xfede3011     9              Reprogramming attempts
*       |
*       +-- CRCValue              0xfede3013     10             CRC total of logical block
*       |
*       +-- CRCStart              0xfede3017     11             Start address of CRC total
*       |
*       +-- CRCLength             0xfede301b     12             Length of CRC total
*/

/* Size defines ************************************************************** */
#define kEepSizeProgReqFlag                  0x01u
#define kEepSizeResetResponseFlag            0x01u
#define kEepSizeApplValidity                 0x01u
#define kEepSizeValidityFlags                0x01u
#define kEepSizeSecAccessDelayFlag           0x01u
#define kEepSizeSecAccessInvalidCount        0x01u
#define kEepSizeFingerprint                  0x09u
#define kEepSizeProgCounter                  0x02u
#define kEepSizeProgAttempts                 0x02u
#define kEepSizeCRCValue                     0x04u
#define kEepSizeCRCStart                     0x04u
#define kEepSizeCRCLength                    0x04u
#define kEepSizeMetadata                     (kEepSizeFingerprint + kEepSizeProgCounter + kEepSizeProgAttempts + kEepSizeCRCValue + kEepSizeCRCStart + kEepSizeCRCLength)
/* Address defines *********************************************************** */
#ifdef FBL_ENABLE_EEPMGR
#else
#define kEepFblBaseAddress                   0xFEDE3000u
#define kEepStartAddress                     kEepFblBaseAddress
#define kEepAddressProgReqFlag               kEepStartAddress
#define kEepAddressResetResponseFlag         (kEepAddressProgReqFlag + kEepSizeProgReqFlag)
#define kEepAddressApplValidity              (kEepAddressResetResponseFlag + kEepSizeResetResponseFlag)
#define kEepAddressValidityFlags             (kEepAddressApplValidity + kEepSizeApplValidity)
#define kEepAddressSecAccessDelayFlag        (kEepAddressValidityFlags + kEepSizeValidityFlags)
#define kEepAddressSecAccessInvalidCount     (kEepAddressSecAccessDelayFlag + kEepSizeSecAccessDelayFlag)
#define kEepAddressMetadata                  (kEepAddressSecAccessInvalidCount + kEepSizeSecAccessInvalidCount)
#define kEepAddressFingerprint               kEepAddressMetadata
#define kEepAddressProgCounter               (kEepAddressFingerprint + kEepSizeFingerprint)
#define kEepAddressProgAttempts              (kEepAddressProgCounter + kEepSizeProgCounter)
#define kEepAddressCRCValue                  (kEepAddressProgAttempts + kEepSizeProgAttempts)
#define kEepAddressCRCStart                  (kEepAddressCRCValue + kEepSizeCRCValue)
#define kEepAddressCRCLength                 (kEepAddressCRCStart + kEepSizeCRCStart)
#define kEepEndAddress                       (kEepAddressMetadata + kEepSizeMetadata * 2 - 1)
#define kEepSizeOfEeprom                     (kEepEndAddress - kEepStartAddress + 1)
/* Initialize NvStructSize-Array */
#define kNvNoOfStructs                       0x01u
#define kNvSizeStructs                       {kEepSizeMetadata}
#endif

/* Handle defines ************************************************************ */
#define kEepMgrFirstHandle                   1
#define kEepMgrHandle_ProgReqFlag            kEepMgrFirstHandle
#define kEepMgrHandle_ResetResponseFlag      (kEepMgrHandle_ProgReqFlag + 1)
#define kEepMgrHandle_ApplValidity           (kEepMgrHandle_ResetResponseFlag + 1)
#define kEepMgrHandle_ValidityFlags          (kEepMgrHandle_ApplValidity + 1)
#define kEepMgrHandle_SecAccessDelayFlag     (kEepMgrHandle_ValidityFlags + 1)
#define kEepMgrHandle_SecAccessInvalidCount  (kEepMgrHandle_SecAccessDelayFlag + 1)
#define kNvHandleStruct0                     0x00u
#define kEepMgrHandle_Metadata               (NV_MK_STRUCT_ID(kNvHandleStruct0, kEepMgrHandle_SecAccessInvalidCount + 1))
#define kEepMgrHandle_Fingerprint            0
#define kEepMgrHandle_ProgCounter            (kEepMgrHandle_Fingerprint + 1)
#define kEepMgrHandle_ProgAttempts           (kEepMgrHandle_ProgCounter + 1)
#define kEepMgrHandle_CRCValue               (kEepMgrHandle_ProgAttempts + 1)
#define kEepMgrHandle_CRCStart               (kEepMgrHandle_CRCValue + 1)
#define kEepMgrHandle_CRCLength              (kEepMgrHandle_CRCStart + 1)
#define kEepMgrNrOfMetadataHdls              (kEepMgrHandle_CRCLength + 1)
#define kEepMgrLastHandle                    (NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrNrOfMetadataHdls * 2 - 1)
#define kEepMgrNumberOfHandles               (kEepMgrLastHandle - kEepMgrFirstHandle + 1)
#ifdef FBL_ENABLE_EEPMGR
/* Initialize NvStructSize-Array */
#define kNvNoOfStructs                       0x01u
#define kNvSizeStructs                       {kEepMgrNrOfMetadataHdls}
#endif

/* Access macros ************************************************************* */
#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadProgReqFlag(buf)        ((EepMgrRead(kEepMgrHandle_ProgReqFlag, buf, kEepSizeProgReqFlag) == kEepSizeProgReqFlag) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgReqFlag(buf)       ((EepMgrWrite(kEepMgrHandle_ProgReqFlag, buf, kEepSizeProgReqFlag) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadProgReqFlag(buf)        ((EepromDriver_RReadSync(buf, kEepSizeProgReqFlag, kEepAddressProgReqFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgReqFlag(buf)       ((EepromDriver_RWriteSync(buf, kEepSizeProgReqFlag, kEepAddressProgReqFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadResetResponseFlag(buf)  ((EepMgrRead(kEepMgrHandle_ResetResponseFlag, buf, kEepSizeResetResponseFlag) == kEepSizeResetResponseFlag) ? kFblOk : kFblFailed)
#define ApplFblNvWriteResetResponseFlag(buf) ((EepMgrWrite(kEepMgrHandle_ResetResponseFlag, buf, kEepSizeResetResponseFlag) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadResetResponseFlag(buf)  ((EepromDriver_RReadSync(buf, kEepSizeResetResponseFlag, kEepAddressResetResponseFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteResetResponseFlag(buf) ((EepromDriver_RWriteSync(buf, kEepSizeResetResponseFlag, kEepAddressResetResponseFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadApplValidity(buf)       ((EepMgrRead(kEepMgrHandle_ApplValidity, buf, kEepSizeApplValidity) == kEepSizeApplValidity) ? kFblOk : kFblFailed)
#define ApplFblNvWriteApplValidity(buf)      ((EepMgrWrite(kEepMgrHandle_ApplValidity, buf, kEepSizeApplValidity) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadApplValidity(buf)       ((EepromDriver_RReadSync(buf, kEepSizeApplValidity, kEepAddressApplValidity) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteApplValidity(buf)      ((EepromDriver_RWriteSync(buf, kEepSizeApplValidity, kEepAddressApplValidity) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadValidityFlags(buf)      ((EepMgrRead(kEepMgrHandle_ValidityFlags, buf, kEepSizeValidityFlags) == kEepSizeValidityFlags) ? kFblOk : kFblFailed)
#define ApplFblNvWriteValidityFlags(buf)     ((EepMgrWrite(kEepMgrHandle_ValidityFlags, buf, kEepSizeValidityFlags) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadValidityFlags(buf)      ((EepromDriver_RReadSync(buf, kEepSizeValidityFlags, kEepAddressValidityFlags) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteValidityFlags(buf)     ((EepromDriver_RWriteSync(buf, kEepSizeValidityFlags, kEepAddressValidityFlags) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadSecAccessDelayFlag(buf) ((EepMgrRead(kEepMgrHandle_SecAccessDelayFlag, buf, kEepSizeSecAccessDelayFlag) == kEepSizeSecAccessDelayFlag) ? kFblOk : kFblFailed)
#define ApplFblNvWriteSecAccessDelayFlag(buf) ((EepMgrWrite(kEepMgrHandle_SecAccessDelayFlag, buf, kEepSizeSecAccessDelayFlag) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadSecAccessDelayFlag(buf) ((EepromDriver_RReadSync(buf, kEepSizeSecAccessDelayFlag, kEepAddressSecAccessDelayFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteSecAccessDelayFlag(buf) ((EepromDriver_RWriteSync(buf, kEepSizeSecAccessDelayFlag, kEepAddressSecAccessDelayFlag) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadSecAccessInvalidCount(buf) ((EepMgrRead(kEepMgrHandle_SecAccessInvalidCount, buf, kEepSizeSecAccessInvalidCount) == kEepSizeSecAccessInvalidCount) ? kFblOk : kFblFailed)
#define ApplFblNvWriteSecAccessInvalidCount(buf) ((EepMgrWrite(kEepMgrHandle_SecAccessInvalidCount, buf, kEepSizeSecAccessInvalidCount) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadSecAccessInvalidCount(buf) ((EepromDriver_RReadSync(buf, kEepSizeSecAccessInvalidCount, kEepAddressSecAccessInvalidCount) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteSecAccessInvalidCount(buf) ((EepromDriver_RWriteSync(buf, kEepSizeSecAccessInvalidCount, kEepAddressSecAccessInvalidCount) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadFingerprint(idx, buf)   ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_Fingerprint + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeFingerprint) == kEepSizeFingerprint) ? kFblOk : kFblFailed)
#define ApplFblNvWriteFingerprint(idx, buf)  ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_Fingerprint + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeFingerprint) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadFingerprint(idx, buf)   ((EepromDriver_RReadSync(buf, kEepSizeFingerprint, kEepAddressFingerprint + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteFingerprint(idx, buf)  ((EepromDriver_RWriteSync(buf, kEepSizeFingerprint, kEepAddressFingerprint + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadProgCounter(idx, buf)   ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_ProgCounter + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeProgCounter) == kEepSizeProgCounter) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgCounter(idx, buf)  ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_ProgCounter + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeProgCounter) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadProgCounter(idx, buf)   ((EepromDriver_RReadSync(buf, kEepSizeProgCounter, kEepAddressProgCounter + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgCounter(idx, buf)  ((EepromDriver_RWriteSync(buf, kEepSizeProgCounter, kEepAddressProgCounter + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadProgAttempts(idx, buf)  ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_ProgAttempts + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeProgAttempts) == kEepSizeProgAttempts) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgAttempts(idx, buf) ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_ProgAttempts + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeProgAttempts) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadProgAttempts(idx, buf)  ((EepromDriver_RReadSync(buf, kEepSizeProgAttempts, kEepAddressProgAttempts + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteProgAttempts(idx, buf) ((EepromDriver_RWriteSync(buf, kEepSizeProgAttempts, kEepAddressProgAttempts + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadCRCValue(idx, buf)      ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCValue + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCValue) == kEepSizeCRCValue) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCValue(idx, buf)     ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCValue + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCValue) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadCRCValue(idx, buf)      ((EepromDriver_RReadSync(buf, kEepSizeCRCValue, kEepAddressCRCValue + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCValue(idx, buf)     ((EepromDriver_RWriteSync(buf, kEepSizeCRCValue, kEepAddressCRCValue + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadCRCStart(idx, buf)      ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCStart + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCStart) == kEepSizeCRCStart) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCStart(idx, buf)     ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCStart + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCStart) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadCRCStart(idx, buf)      ((EepromDriver_RReadSync(buf, kEepSizeCRCStart, kEepAddressCRCStart + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCStart(idx, buf)     ((EepromDriver_RWriteSync(buf, kEepSizeCRCStart, kEepAddressCRCStart + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif

#ifdef FBL_ENABLE_EEPMGR
#define ApplFblNvReadCRCLength(idx, buf)     ((EepMgrRead(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCLength + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCLength) == kEepSizeCRCLength) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCLength(idx, buf)    ((EepMgrWrite(NV_GET_STRUCT_ID(kEepMgrHandle_Metadata) + kEepMgrHandle_CRCLength + (idx * kEepMgrNrOfMetadataHdls), buf, kEepSizeCRCLength) == kEepMgrOk) ? kFblOk : kFblFailed)
#else
#define ApplFblNvReadCRCLength(idx, buf)     ((EepromDriver_RReadSync(buf, kEepSizeCRCLength, kEepAddressCRCLength + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#define ApplFblNvWriteCRCLength(idx, buf)    ((EepromDriver_RWriteSync(buf, kEepSizeCRCLength, kEepAddressCRCLength + (idx * kEepSizeMetadata)) == IO_E_OK) ? kFblOk : kFblFailed)
#endif


#ifdef FBL_ENABLE_EEPMGR
#else
#define ApplFblIsValidEepAddress(i)          ((i >= kEepStartAddress) && (i <= kEepEndAddress))
#endif

#define ApplFblIsValidEepMgrHandle(i)        ((i >= kEepMgrFirstHandle) && (i <= kEepMgrLastHandle))

/* User Config File ********************************************************** */

/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 281688174
      #error "The magic number of the generated file <D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\Demo\DemoFbl\Appl\GenData\WrapNv_cfg.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 281688174
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __WRAPNV_CFG_H__ */
