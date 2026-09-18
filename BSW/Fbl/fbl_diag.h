/***********************************************************************************************************************
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief         Diagnostic module external interface
 *                 Declaration of functions, variables, and constants
 *
 *  --------------------------------------------------------------------------------------------------------------------
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \par Copyright
 *  \verbatim
 *  Copyright (c) 2018 by Vector Informatik GmbH.                                                  All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 */
/**********************************************************************************************************************/

/***********************************************************************************************************************
 *  AUTHOR IDENTITY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Name                          Initials      Company
 *  --------------------------------------------------------------------------------------------------------------------
 *  Achim Strobelt                Ach           Vector Informatik GmbH
 *  Marco Riedl                   Rie           Vector Informatik GmbH
 *  Christian Baeuerle            CB            Vector Informatik GmbH
 *  --------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  03.00.00   2017-06-28  Ach     ESCAN00095351    General rework
 *  03.00.01   2017-07-03  Rie     ESCAN00095702    No changes
 *  03.01.00   2017-07-17  Ach     ESCAN00095908    No changes
 *  03.01.01   2017-10-13  Ach     ESCAN00097038    No changes
 *                         CB      ESCAN00097044    Added compatibility define GetMemDriverInitialized()
 *  03.02.00   2017-11-22  Ach     ESCAN00097279    No changes
 *                                 ESCAN00097501    No changes
 *  03.02.01   2018-01-26  Ach     ESCAN00098105    No changes
 **********************************************************************************************************************/

#ifndef __FBL_DIAG_H__
#define __FBL_DIAG_H__

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

/* OEM specific interface additions */
#include "fbl_diag_oem.h"

/***********************************************************************************************************************
 *  VERSION
 **********************************************************************************************************************/

#define FBLDIAG_14229_CORE_VERSION           0x0302u
#define FBLDIAG_14229_CORE_RELEASE_VERSION   0x01u

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* Diagnostic base timing */
#if !defined( kDiagCallCycle )
# define kDiagCallCycle    1u
#endif

/* Maximum segment size of all memory devices */
#define FBL_DIAG_SEGMENT_SIZE                   FBL_MAX_SEGMENT_SIZE

/* Generic helper macros */
/* PRQA S 3453 2 */ /* MD_MSR_19.7 */
#define GET_ID_HIGH(id)                         ((vuint8)(((id) >> 8u) & 0xFFu))
#define GET_ID_LOW(id)                          ((vuint8)((id) & 0xFFu))

/***********************************************************************************************************************
*  Core state definitions
**********************************************************************************************************************/

/* Bootloader states */
#define kFblDiagStateSessionDefault             0u
#define kFblDiagStateSessionExtended            1u
#define kFblDiagStateSessionProgramming         2u
#define kFblDiagStateFunctionalRequest          3u
#define kFblDiagStateDiagBufferLocked           4u
#define kFblDiagStateExecutePostHandler         5u
#define kFblDiagStateServiceInProgress          6u
#define kFblDiagStateRcrRpAllowed               7u
#define kFblDiagStateRcrRpInProgress            8u
#define kFblDiagStateSuppressPosRspMsg          9u
#define kFblDiagStateServiceHasSubfunction      10u
#define kFblDiagStateResponseProcessing         11u
#define kFblDiagStateTransferTypeFlash          12u
/* Reset handling */
#define kFblDiagStateWaitEcuReset               13u
#define kFblDiagStateEcuResetFctFinished        14u
#define kFblDiagStateResetMsgConfirmed          15u
/* Generic download handling */
#define kFblDiagStatePreconditionsChecked       16u
#define kFblDiagStateMemDriverInitialized       17u

#define kFblDiagLastCoreStateIdx                kFblDiagStateMemDriverInitialized

/* Number of states (Core and OEM) */
#define kFblDiagNumberOfStates                  ( kFblDiagLastOemStateIdx + 1u )
#define kFblDiagNumberOfDispatchStates          kFblDiagNumberOfStates

/* Diagnostic handling states */
#define FblDiagGetDefaultSession()              GetFblDiagState( kFblDiagStateSessionDefault )
#define FblDiagGetProgrammingSession()          GetFblDiagState( kFblDiagStateSessionProgramming )
#define FblDiagGetExtendedSession()             GetFblDiagState( kFblDiagStateSessionExtended )
#define FblDiagGetBufferLocked()                GetFblDiagState( kFblDiagStateDiagBufferLocked )
#define FblDiagGetExecutePostHandler()          GetFblDiagState( kFblDiagStateExecutePostHandler )
#define FblDiagGetRcrRpAllowed()                GetFblDiagState( kFblDiagStateRcrRpAllowed )
#define FblDiagGetRcrRpInProgress()             GetFblDiagState( kFblDiagStateRcrRpInProgress )
#define FblDiagGetResponseProcessing()          GetFblDiagState( kFblDiagStateResponseProcessing )
#define FblDiagGetSuppressPosRspMsg()           GetFblDiagState( kFblDiagStateSuppressPosRspMsg )
#define FblDiagGetServiceHasSubfunction()       GetFblDiagState( kFblDiagStateServiceHasSubfunction )
#define FblDiagGetServiceInProgress()           GetFblDiagState( kFblDiagStateServiceInProgress )
#define FblDiagGetFunctionalRequest()           GetFblDiagState( kFblDiagStateFunctionalRequest )
#define FblDiagGetTransferTypeFlash()           GetFblDiagState( kFblDiagStateTransferTypeFlash )
/* Reset handling states */
#define FblDiagGetWaitEcuReset()                GetFblDiagState( kFblDiagStateWaitEcuReset )
#define FblDiagGetEcuResetFctFinished()         GetFblDiagState( kFblDiagStateEcuResetFctFinished )
#define FblDiagGetResetMsgConfirmed()           GetFblDiagState( kFblDiagStateResetMsgConfirmed )
/* Generic download states */
#define FblDiagGetPreconditionsChecked()        GetFblDiagState( kFblDiagStatePreconditionsChecked )
#define FblDiagGetMemDriverInitialized()        GetFblDiagState( kFblDiagStateMemDriverInitialized )

/* Compatibility defines */
#define GetDiagDefaultSession()                 FblDiagGetDefaultSession()
#define GetDiagProgrammingSession()             FblDiagGetProgrammingSession()
#define GetDiagExtendedSession()                FblDiagGetExtendedSession()
#define GetDiagBufferLocked()                   FblDiagGetBufferLocked()
#define GetRcrRpInProgress()                    FblDiagGetRcrRpInProgress()
#define GetMemDriverInitialized()               FblDiagGetMemDriverInitialized()

/***********************************************************************************************************************
 *  State handling framework
 **********************************************************************************************************************/

#define FBL_DIAG_STATECHECK_ARRAYSIZE        (((kFblDiagNumberOfDispatchStates - 1u) / FBL_STATE_BITS) + 1u)

/* State access macros */
#define GetFblDiagState(state)               FBL_STATE_TEST(fblDiagStates, (state)) /* PRQA S 3453 */ /* MD_MSR_19.7 */

/* Diagnostic state handling */
/* Control macros */
#define FBL_DIAG_STATE_CHECK_COUNT  0u
#define FBL_DIAG_STATE_ADDCLR_COUNT 0u
#define FBL_DIAG_STATE_COUNT        (FBL_DIAG_STATE_CHECK_COUNT + FBL_DIAG_STATE_ADDCLR_COUNT)

/* State array build macros */
#define FBL_DIAG_STATE_ARRAYENTRY(mask, idx)    ((tFblStateBitmap)(((mask) >> ((idx) * FBL_STATE_BITS)) & kFblDiagStateMaskAll))  /* PRQA S 3453 */ /* MD_MSR_19.7 */
#if (FBL_DIAG_STATECHECK_ARRAYSIZE == 1)
# define FBL_STATE_BUILDARRAYENTRIES(mask)      FBL_STATE_ARRAYENTRY(mask, 0) /* PRQA S 3453 */ /* MD_MSR_19.7 */
#endif
#if (FBL_DIAG_STATECHECK_ARRAYSIZE == 2)
# define FBL_STATE_BUILDARRAYENTRIES(mask)      FBL_STATE_ARRAYENTRY(mask, 0), FBL_STATE_ARRAYENTRY(mask, 1)   /* PRQA S 3453 */ /* MD_MSR_19.7 */
#endif
#if (FBL_DIAG_STATECHECK_ARRAYSIZE == 3)
# define FBL_STATE_BUILDARRAYENTRIES(mask)      FBL_STATE_ARRAYENTRY(mask, 0), FBL_STATE_ARRAYENTRY(mask, 1), FBL_STATE_ARRAYENTRY(mask, 2)  /* PRQA S 3453 */ /* MD_MSR_19.7 */
#endif
#if (FBL_DIAG_STATECHECK_ARRAYSIZE == 4)
# define STATE_BUILDARRAYENTRIES(mask)          FBL_STATE_ARRAYENTRY(mask, 0), FBL_STATE_ARRAYENTRY(mask, 1), FBL_STATE_ARRAYENTRY(mask, 2), FBL_STATE_ARRAYENTRY(mask, 3) /* PRQA S 3453 */ /* MD_MSR_19.7 */
#endif
#if (FBL_DIAG_STATECHECK_ARRAYSIZE > 4)
# error "Invalid array size for state check!"
#endif

#define DIAG_STATE_BUILDARRAY(mask)             { DIAG_STATE_BUILDARRAYENTRIES(mask) }

/* State masks and access macros */
#define kFblDiagStateMaskAllLong                FBL_STATE_MULTIMASKLONG(0, ((FBL_DIAG_STATECHECK_ARRAYSIZE * FBL_STATE_BITS) - 1))

#define kFblDiagStateMask(state)                FBL_STATE_MASKLONG(state)   /* PRQA S 3453 */ /* MD_MSR_19.7 */

#define SetFblDiagState(state)                  FBL_STATE_SET(fblDiagStates, state)  /* PRQA S 3453 */ /* MD_MSR_19.7 */
#define ClrFblDiagState(state)                  FBL_STATE_CLR(fblDiagStates, state)  /* PRQA S 3453 */ /* MD_MSR_19.7 */

/***********************************************************************************************************************
 *  Service handling
 **********************************************************************************************************************/

/* Diagnostic services */
#define kDiagSidDiagnosticSessionControl                 0x10u    /**< Service ID - Diagnostic session control */
#define kDiagSidEcuReset                                 0x11u    /**< Service ID - ECU reset */
#define kDiagSidClearDiagInfo                            0x14u    /**< Service ID - Clear Diagnostic Information */
#define kDiagSidReadDTCInformation                       0x19u    /**< Service ID - Read DTC information */
#define kDiagSidReadDataByIdentifier                     0x22u    /**< Service ID - Read data by Identifier */
#define kDiagSidSecurityAccess                           0x27u    /**< Service ID - Security Access */
#define kDiagSidCommunicationControl                     0x28u    /**< Service ID - Communication Control */
#define kDiagSidWriteDataByIdentifier                    0x2Eu    /**< Service ID - Write data by Identifier */
#define kDiagSidRoutineControl                           0x31u    /**< Service ID - Routine Control */
#define kDiagSidRequestDownload                          0x34u    /**< Service ID - Request Download */
#define kDiagSidTransferData                             0x36u    /**< Service ID - Transfer Data */
#define kDiagSidRequestTransferExit                      0x37u    /**< Service ID - Request Transfer Exit */
#define kDiagSidTesterPresent                            0x3Eu    /**< Service ID - Tester Present */
#define kDiagSidControlDTCSetting                        0x85u    /**< Service ID - Control DTC */
#define kDiagSidLinkControl                              0x87u    /**< Service ID - Link Control */

/* Response identifier service definition */
#define kDiagRidNegativeResponse                         0x7Fu    /**< Service ID - Negative response */

/* Diagnostic sub-services */
#define kDiagSuppressPosRspMsgIndicationBit              0x80u

/* DiagnosticSessionControl */
#define kDiagSubDefaultSession                           0x01u    /**< Subservice ID - Default session */
#define kDiagSubProgrammingSession                       0x02u    /**< Subservice ID - Programming session */
#define kDiagSubExtendedDiagSession                      0x03u    /**< Subservice ID - Extended session */

/* TesterPresent */
#define kDiagSubTesterPresent                            0x00u    /**< Subservice ID - Zero subfunction */

/* ECUReset */
#define kDiagSubHardReset                                0x01u    /**< Subservice ID - Hard reset */
#define kDiagSubKeyOffOnReset                            0x02u    /**< Subservice ID - Key Off/On reset */
#define kDiagSubSoftReset                                0x03u    /**< Subservice ID - Soft reset */

/* ReadDTCInformation */
#define kDiagSubReportDTCByStatusMask                    0x02u    /**< Subservice ID - Report DTC by status mask */

/* CommunicationControl */
#define kDiagSubEnableRxAndTx                            0x00u    /**< ControlType - Enable Rx and Disable Tx */
#define kDiagSubEnableRxAndDisableTx                     0x01u    /**< ControlType - Enable Rx and Disable Tx */
#define kDiagSubDisableRxAndTx                           0x03u    /**< ControlType - Disable Rx and Tx */
#define kDiagSubNormalCommunication                      0x01u    /**< Communication type - normal communication */
#define kDiagSubNmCommunication                          0x02u    /**< Communication type - network management communication */
#define kDiagSubNmAndNormalCommunication                 0x03u    /**< Communication type - network management and normal communication */

/* RoutineControl, routineControlType */
#define kDiagSubStartRoutine                             0x01u
#define kDiagSubStopRoutine                              0x02u
#define kDiagSubRequestRoutineResults                    0x03u

/***********************************************************************************************************************
 *  Error handling
 **********************************************************************************************************************/

/* Negative Response Codes */
#define kDiagErrorNone                                   0x00u
#define kDiagNrcGeneralReject                            0x10u
#define kDiagNrcServiceNotSupported                      0x11u
#define kDiagNrcSubFunctionNotSupported                  0x12u
#define kDiagNrcIncorrectMessageLengthOrInvalidFormat    0x13u
#define kDiagNrcResponseTooLong                          0x14u
#define kDiagNrcBusyRepeatRequest                        0x21u
#define kDiagNrcConditionsNotCorrect                     0x22u
#define kDiagNrcRequestSequenceError                     0x24u
#define kDiagNrcRequestOutOfRange                        0x31u
#define kDiagNrcSecurityAccessDenied                     0x33u
#define kDiagNrcInvalidKey                               0x35u
#define kDiagNrcExceedNumberOfAttempts                   0x36u
#define kDiagNrcRequiredTimeDelayNotExpired              0x37u
#define kDiagNrcUploadDownloadNotAccepted                0x70u
#define kDiagNrcTransferDataSuspended                    0x71u
#define kDiagNrcGeneralProgrammingFailure                0x72u
#define kDiagNrcWrongBlockSequenceCounter                0x73u
#define kDiagNrcIllegalByteCountInBlockTransfer          0x75u
#define kDiagNrcRcrResponsePending                       0x78u
#define kDiagNrcSubfunctionNotSupportedInActiveSession   0x7Eu
#define kDiagNrcServiceNotSupportedInActiveSession       0x7Fu
#define kDiagNrcVoltageTooHigh                           0x92u
#define kDiagNrcVoltageTooLow                            0x93u

/* Diagnostic exception setting macros */
#define DiagNRCGeneralReject()                           FblDiagSetError(kDiagNrcGeneralReject)
#define DiagNRCServiceNotSupported()                     FblDiagSetError(kDiagNrcServiceNotSupported)
#define DiagNRCSubFunctionNotSupported()                 FblDiagSetError(kDiagNrcSubFunctionNotSupported)
#define DiagNRCIncorrectMessageLengthOrInvalidFormat()   FblDiagSetError(kDiagNrcIncorrectMessageLengthOrInvalidFormat)
#define DiagNRCResponseTooLong()                         FblDiagSetError(kDiagNrcResponseTooLong)
#define DiagNRCBusyRepeatRequest()                       FblDiagSetError(kDiagNrcBusyRepeatRequest)
#define DiagNRCConditionsNotCorrect()                    FblDiagSetError(kDiagNrcConditionsNotCorrect)
#define DiagNRCRequestSequenceError()                    FblDiagSetError(kDiagNrcRequestSequenceError)
#define DiagNRCRequestOutOfRange()                       FblDiagSetError(kDiagNrcRequestOutOfRange)
#define DiagNRCSecurityAccessDenied()                    FblDiagSetError(kDiagNrcSecurityAccessDenied)
#define DiagNRCInvalidKey()                              FblDiagSetError(kDiagNrcInvalidKey)
#define DiagNRCExceedNumberOfAttempts()                  FblDiagSetError(kDiagNrcExceedNumberOfAttempts)
#define DiagNRCRequiredTimeDelayNotExpired()             FblDiagSetError(kDiagNrcRequiredTimeDelayNotExpired)
#define DiagNRCUploadDownloadNotAccepted()               FblDiagSetError(kDiagNrcUploadDownloadNotAccepted)
#define DiagNRCTransferDataSuspended()                   FblDiagSetError(kDiagNrcTransferDataSuspended)
#define DiagNRCGeneralProgrammingFailure()               FblDiagSetError(kDiagNrcGeneralProgrammingFailure)
#define DiagNRCWrongBlockSequenceCounter()               FblDiagSetError(kDiagNrcWrongBlockSequenceCounter)
#define DiagNRCIllegalByteCountInBlockTransfer()         FblDiagSetError(kDiagNrcIllegalByteCountInBlockTransfer)
#define DiagNRCRcrResponsePending()                      FblDiagSetError(kDiagNrcRcrResponsePending)
#define DiagNRCSubfunctionNotSupportedInActiveSession()  FblDiagSetError(kDiagNrcSubfunctionNotSupportedInActiveSession)
#define DiagNRCServiceNotSupportedInActiveSession()      FblDiagSetError(kDiagNrcServiceNotSupportedInActiveSession)
#define DiagNRCVoltageTooHigh()                          FblDiagSetError(kDiagNrcVoltageTooHigh)
#define DiagNRCVoltageTooLow()                           FblDiagSetError(kDiagNrcVoltageTooLow)

/* Access macros for user diagnostic call-backs */
/* PRQA S 3453 2 */ /* MD_MSR_19.7 */
#define DiagPutResponseSId(a)    (DiagBuffer[0] = (a))
#define DiagGetRequestSId()      (DiagBuffer[0])

/***********************************************************************************************************************
 *  Data processing
 **********************************************************************************************************************/

#define kDiagSubNoEncryption              0x00u
#define kDiagSubNoCompression             0x00u
#define kDiagSubNoDataProcessing          ((kDiagSubNoCompression << 4u) | kDiagSubNoEncryption)

#define kDiagFmtEncryptionMask            0x0Fu
#define kDiagFmtCompressionMask           0xF0u

/* PRQA S 3453 2 */ /* MD_MSR_19.7 */
#define FblDiagGetCompressionMode(m)      (((m) & kDiagFmtCompressionMask) >> 4u)
#define FblDiagGetEncryptionMode(m)       ((m) & kDiagFmtEncryptionMask)

/***********************************************************************************************************************
 *  Response processing
 **********************************************************************************************************************/

/* Parameter to the function FblDiagEcuReset() */
#define kDiagResetPutResponse                0x00u
#define kDiagResetNoResponse                 0x01u

/* Parameters for busy response handling function */
#define kNotForceSendResponsePending         0x00u
#define kForceSendResponsePending            0x01u
#define kForceSendRpIfNotInProgress          0x02u

/* Sequence counter retry init number */
#if !defined( kDiagInitDataRetries )
# define kDiagInitDataRetries                0x03u
#endif

/* Read memory function for flash driver validation */
#define FblReadRam                           FblReadBlock

/***********************************************************************************************************************
 *  Timeout handling
 **********************************************************************************************************************/

/* Tester present timeout handling */
#if !defined( TESTER_PRESENT_TIMEOUT )
/** S3 timeout */
# define TESTER_PRESENT_TIMEOUT                 5000u
#endif

#define TimeoutTesterValue()                    testerPresentTimeout
#define DecTimeoutTesterValue()                 ( testerPresentTimeout-- )
#define ResetTesterTimeout()                    ( testerPresentTimeout = (vuint16)( TESTER_PRESENT_TIMEOUT / kDiagCallCycle ))
#define StopTesterTimeout()                     ( testerPresentTimeout = 0u )

#if defined( FBL_ENABLE_SLEEPMODE )
/* Sleep mode handling */
# define kFblSleepDelayTime                     ((vuint32)( FBL_SLEEP_TIME / FBL_REPEAT_CALL_CYCLE ))
# define FblSleepCounterClear()                 ( sleepCounter = 0x00u )
# define FblSleepCounterReload()                ( sleepCounter = kFblSleepDelayTime )
# define FblDecSleepCounter()                   ( sleepCounter-- )
# define FblGetSleepCounter()                   ((vuint32) sleepCounter )
#endif

/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/

typedef tFblResult (*tFblDiagLengthCheck )( vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen );
typedef tFblResult (*tFblDiagPreHandler )( vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen );
typedef tFblResult (*tFblDiagMainHandler)( vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen );
typedef tFblResult (*tFblDiagNrcHandler)( vuint32 serviceErrorMask );
typedef void       (*tFblDiagPostHandler)( vuint8 postParam );

typedef tFblAddress  tFblDiagAddr;
typedef vuint8       tFblDiagNrc;

/***********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

/* Initialization functions */
void        FblDiagInitPowerOn( void );
void        FblDiagInit( void );
void        FblDiagPostInit( void );
/* Error handling interface */
void        FblDiagSetError( vuint8 errorCode );
void        DiagProcessingDone( tCwDataLengthType dataLength );
/* Response pending handling functions */
vuint8      FblRealTimeSupport( void );
void        DiagExRCRResponsePending( vuint8 forceSend );
/* Diagnostic task functions */
void        FblDiagTimerTask( void );
void        FblDiagStateTask( void );
/* Diagnostic response after reset handling */
void        FblDiagInitStartFromAppl( void );
#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
void        FblDiagInitStartFromReset( vuint8 resetRequestType );
#endif /* FBL_ENABLE_RESPONSE_AFTER_RESET */

/* Memory handling functions */
vsint16     FblMemSegmentNrGet( tFblAddress address );
tFblLength  FblReadProm( tFblAddress address, vuint8 *buffer, tFblLength length );
V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * FblDiagMemGetActiveBuffer(void);

/* Callback functions from FblLib_Mem */
tFblResult  FblDiagMemPreWrite( void );
tFblResult  FblDiagMemPostWrite( void );

/* Start section to execute code from RAM */
#define FBLDIAG_RAMCODE_START_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */

/* Communication wrapper API */
#if defined( FBL_ENABLE_STAY_IN_BOOT )
vuint8      FblDiagCheckStartMsg( const V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 *pData, vuintx length );
#endif  /* FBL_ENABLE_STAY_IN_BOOT */
void        FblDiagRxStartIndication( void );
void        FblDiagRxIndication( vuint8* pbDiagBuffer, tCwDataLengthType rxDataLength );
vuint8*     FblDiagRxGetPhysBuffer( tCwDataLengthType rxDataLength );
vuint8*     FblDiagRxGetFuncBuffer( tCwDataLengthType rxDataLength );
void        FblDiagTxConfirmation( vuint8 cwMsgType );
void        FblDiagTxErrorIndication( vuint8 cwMsgType );
void        FblDiagRxErrorIndication( void );

/* End section to execute code from RAM */
#define FBLDIAG_RAMCODE_STOP_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */

/***********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/

V_MEMRAM0 extern V_MEMRAM1       vuint8            V_MEMRAM2 V_MEMRAM3 * V_MEMRAM1 V_MEMRAM2 DiagBuffer;
V_MEMRAM0 extern V_MEMRAM1       vsint16           V_MEMRAM2      memSegment;
V_MEMRAM0 extern V_MEMRAM1_NEAR  tCwDataLengthType V_MEMRAM2_NEAR DiagDataLength;
V_MEMRAM0 extern V_MEMRAM1_NEAR  vuint16           V_MEMRAM2_NEAR testerPresentTimeout;
V_MEMRAM0 extern V_MEMRAM1_NEAR  vuint8            V_MEMRAM2_NEAR diagServiceCurrent;
V_MEMRAM0 extern V_MEMRAM1_NEAR  tFblStateBitmap   V_MEMRAM2_NEAR fblDiagStates[FBL_STATE_INDEX(kFblDiagNumberOfStates - 1) + 1];

#if defined( FBL_ENABLE_SLEEPMODE )
V_MEMRAM0 extern V_MEMRAM1       vuint32           V_MEMRAM2      sleepCounter;
#endif

#if defined( FBL_ENABLE_STAY_IN_BOOT )
V_MEMROM0 extern V_MEMROM1 vuint8 V_MEMROM2 fblDiagStayInBootMsg[];
V_MEMROM0 extern V_MEMROM1 vuintx V_MEMROM2 kSizeOfStayInBootMsg;
#endif  /* FBL_ENABLE_STAY_IN_BOOT */

/***********************************************************************************************************************
 *  CONFIGURATION CHECKS
 **********************************************************************************************************************/

#if !(defined(FBL_ENABLE_SYSTEM_CHECK) || defined(FBL_DISABLE_SYSTEM_CHECK))
# error "Error in fbl_cfg.h: Usage of system check not defined"
#endif

#if !(defined(FBL_ENABLE_DEBUG_STATUS) || defined(FBL_DISABLE_DEBUG_STATUS))
# error "Error in fbl_cfg.h: Usage of debug support not defined"
#endif

#if !(defined(FBL_ENABLE_APPL_TASK) || defined(FBL_DISABLE_APPL_TASK))
# error "Error in fbl_cfg.h: Usage of application task function not defined"
#endif

#if !(defined(FBL_ENABLE_USERSUBFUNCTION) || defined(FBL_DISABLE_USERSUBFUNCTION))
# error "Error in fbl_cfg.h: Usage of diag user-subfunction call not defined"
#endif

#if !(defined(FBL_ENABLE_DATA_PROCESSING) || defined(FBL_DISABLE_DATA_PROCESSING))
# error "Error in fbl_cfg.h: Usage of data processing interface not defined"
#endif

#if !(defined(FBL_WATCHDOG_ON) || defined(FBL_WATCHDOG_OFF))
# error "Error in fbl_cfg.h: Usage of watchdog support not defined"
#endif

#if defined( FBL_WATCHDOG_ON) && !defined(FBL_WATCHDOG_TIME )
# error "Error in fbl_cfg.h: Watchdog trigger period not defined"
#endif

#if !(defined(FBL_ENABLE_FBL_START) || defined(FBL_DISABLE_FBL_START))
# error "Error in fbl_cfg.h: Usage of FblStart function not defined"
#endif

#if defined ( FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING )
# if !defined(SWM_DATA_MAX_NOAR)
#  error "Error in fbl_cfg.h: Number of supported download segments (SWM_DATA_MAX_NOAR) not defined"
# endif
#endif

#if !defined(FBL_HEADER_ADDRESS)
# error "Error in fbl_cfg.h: Start address of bootblock header not defined"
#endif

#if !defined(FBL_DIAG_BUFFER_LENGTH)
# error "Error in fbl_cfg.h: FBL_DIAG_BUFFER_LENGTH not defined"
#endif

#if ( FBL_DIAG_BUFFER_LENGTH < (FBL_MAX_SEGMENT_SIZE+2) )
# error "Error in fbl_cfg.: DiagBuffer size too small - modify FBL_DIAG_BUFFER_LENGTH"
#endif

#if defined( SEC_SECURITY_CLASS )
# if ( SEC_SECURITY_CLASS == SEC_CLASS_C ) || ( SEC_SECURITY_CLASS == SEC_CLASS_CCC )
#  if ( FBL_DIAG_BUFFER_LENGTH < (kSecSigLength+4) )
#   error "Error in fbl_cfg.h: DiagBuffer size too small - modify FBL_DIAG_BUFFER_LENGTH"
#  endif
# endif
#endif

#if defined( FBL_DIAG_ENABLE_OEM_SEGMENTNRGET) || \
    defined( FBL_DIAG_ENABLE_OEM_READPROM )
   /* Currently, these switches must be used together */
# if defined( FBL_DIAG_ENABLE_OEM_SEGMENTNRGET) && \
     defined( FBL_DIAG_ENABLE_OEM_READPROM )
# else
#  error "FBL_DIAG_ENABLE_OEM_SEGMENTNRGET and FBL_DIAG_ENABLE_OEM_READPROM must be used together."
# endif
#endif

/* Make sure OEM layer configures this module */
#if !defined( FBL_DIAG_ENABLE_CORE_READ_SECACCESSDELAY_FLAG ) && \
    !defined( FBL_DIAG_DISABLE_CORE_READ_SECACCESSDELAY_FLAG )
# error "OEM diagnostic module has to configure this setting"
#endif
#if !defined( FBL_DIAG_ENABLE_CORE_WRITE_SECACCESSDELAY_FLAG ) && \
    !defined( FBL_DIAG_DISABLE_CORE_WRITE_SECACCESSDELAY_FLAG )
# error "OEM diagnostic module has to configure this setting"
#endif

#if !defined(FBL_DIAG_TIME_P3MAX)
# error "Error in fbl_cfg.h: P2Star time not defined"
#endif

#if !defined(FLASH_SIZE)
# error "Error in fbl_cfg.h: Size of flash code buffer not defined"
#endif

#endif /* __FBL_DIAG_H__ */

/***********************************************************************************************************************
 *  END OF FILE: FBL_DIAG.H
 **********************************************************************************************************************/
