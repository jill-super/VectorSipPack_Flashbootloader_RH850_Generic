/***********************************************************************************************************************
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        OEM dependent configuration for FblLib_Mem
 *
 *  --------------------------------------------------------------------------------------------------------------------
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \par Copyright
 *  \verbatim
 *  Copyright (c) 2017 by Vector Informatik GmbH.                                                  All rights reserved.
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
 *  Joern Herwig                  JHg           Vector Informatik GmbH
 *  Achim Strobelt                Ach           Vector Informatik GmbH
 *  Marco Riedl                   Rie           Vector Informatik GmbH
 *  --------------------------------------------------------------------------------------------------------------------
 *  TEMPLATE REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  01.00.00   2012-03-23  JHg     -                Initial release
 *  01.00.01   2012-03-26  JHg     -                Additional comments
 *  01.01.00   2012-04-05  JHg     -                Changes after code review
 *  01.02.00   2012-04-27  JHg     -                Updated type casts in macros using ternary operator
 *  01.04.00   2013-02-01  Ach     ESCAN00062919    Adapted comments to use Doxygen
 *                         Ach     -                Correct macro __ApplFblMemDriverReady
 *                         JHg     ESCAN00064292    Added pre and post handlers for erase routine
 *                         JHg     ESCAN00064296    Renamed default preamble length define
 *                         JHg     ESCAN00064301    Split verify functions for RAM and ROM verification
 *                         JHg     ESCAN00064333    Differentiate watchdog trigger w/ and w/o status
 *                         JHg     ESCAN00064334    Added interface to remap error codes to OEM dependent value
 *                         JHg     -                Added additional error codes for new functionality
 *                         JHg     ESCAN00064720    Replaced __ApplFblMemCheckDfi by __ApplFblMemIsDataProcessingRequired
 *  01.05.00   2013-04-10  JHg     ESCAN00066379    Added interface version compatibility check
 *                         JHg     ESCAN00066377    Added additional error codes for new functionality
 *  01.06.00   2013-07-22  JHg     ESCAN00067433    Updated interface version (added FblMemDeinit)
 *                         JHg     ESCAN00069161    Updated interface version (added pipelined verification)
 *                                                   Changed verification error codes
 *  01.07.00   2013-08-16  JHg     ESCAN00069803    Updated interface version (enable/disable verification at run-time)
 *                         JHg     -                Corrected name of parameter type overwrite
 *  02.00.00   2013-12-12  JHg     ESCAN00072568    Updated interface version (initialize verification in segment start)
 *                                                   Error / status for BlockStartVerifyInit replaced by SegmentStartVerifyInit
 *                         JHg     ESCAN00072569    Raised major version to match API version
 *  02.01.00   2014-03-12  JHg     ESCAN00073504    Corrected overwrite macro names for verification functions
 *  03.00.00   2015-03-03  JHg     ESCAN00076591    Added support for external stream output
 *                                                   Added __ApplFblMemIsStreamOutputRequired
 *                                                   Added additional error codes
 *                                                   Updated interface version
 *                         JHg     ESCAN00081493    Added __ApplFblMemIsPipelinedProgrammingDisabled
 *  03.01.00   2015-04-23  JHg     ESCAN00082606    Added __ApplFblMemReportProgress
 *  03.01.03   2015-06-15  JHg     ESCAN00083392    Added FBL_MEM_ENABLE_VERIFY_OUTPUT_FULL_BLOCK_LENGTH
 *                                                  Added SEC_ENABLE_VERIFICATION_ADDRESS_LENGTH check
 *                                                  Updated interface version
 *  03.02.00   2015-07-21  JHg     ESCAN00084102    Updated interface version (parameter type structure for stream processing)
 *  04.00.00   2015-09-17  JHg     ESCAN00085249    Updated interface version (explicit parameters for processed verification)
 *                         JHg     ESCAN00085250    Run-time decision whether address and length info is included in verification
 *                                                  Added __ApplFblMemVerifyInputIsAddressAndLengthIncluded,
 *                                                   __ApplFblMemVerifyProcessedIsAddressAndLengthIncluded and
 *                                                   __ApplFblMemVerifyPipelinedIsAddressAndLengthIncluded
 *                         JHg     ESCAN00085251    Pass external segment info to __ApplFblMemIsPipelinedProgrammingDisabled
 *  --------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  07.00.00   2017-08-15  Ach     ESCAN00095332    Update to FblDiag_14229_Core 3.x
 *  07.00.01   2017-09-01  Ach     ESCAN00096493    No changes
 *  07.00.02   2017-10-24  Rie     ESCAN00097182    No changes
 *  07.01.00   2017-11-22  Ach     ESCAN00097502    No changes
 **********************************************************************************************************************/

#ifndef __FBL_MEM_OEM_H__
#define __FBL_MEM_OEM_H__

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* PRQA S 3458 TAG_FblMem_3458_1 */ /* MD_MSR_19.4 */
/* PRQA S 3453 TAG_FblMem_3453_1 */ /* MD_MSR_19.7 */

/*-- Version information ----------------------------------------------------*/

/* --- Version --- */
#define FBLLIB_MEM_VECTOR_VERSION                0x0701u
#define FBLLIB_MEM_VECTOR_RELEASE_VERSION        0x00u

/* Reference interface version */
/** Major interface version identifies incompatible changes */
#define FBL_MEM_API_REFERENCE_VERSION_MAJOR     0x04u
/** Minor interface version identifies backward compatible changes */
#define FBL_MEM_API_REFERENCE_VERSION_MINOR     0x00u
/** Release interface version identifies cosmetic changes */
#define FBL_MEM_API_REFERENCE_VERSION_RELEASE   0x00u

/*-- Buffer configuration ---------------------------------------------------*/

/** Input buffer size */
#define FBL_MEM_BUFFER_SIZE               FBL_DIAG_BUFFER_LENGTH
/** Length of default preamble */
#define FBL_MEM_DEFAULT_PREAMBLE_LENGTH   2u
/** Maximum preamble length */
#define FBL_MEM_MAX_PREAMBLE_LENGTH       2u

#if defined( FBL_MEM_ENABLE_STREAM_OUTPUT )
/* Allow override in configuration */
# if defined( __ApplFblMemIsDataProcessingRequired )
# else
/** Check whether specific data format identifier implies the need for data processing */
#  define __ApplFblMemIsDataProcessingRequired(dataFormat) \
   ((tFblResult)(((kDiagSubNoDataProcessing == (dataFormat)) || ApplFblGetOemStreamModeSupported(dataFormat)) ? kFblFailed : kFblOk))
# endif /* __ApplFblMemIsDataProcessingRequired */
/* Allow override in configuration */
# if defined( __ApplFblMemIsStreamOutputRequired )
# else
/** Check whether specific data format identifier implies the need for stream output */
#  define __ApplFblMemIsStreamOutputRequired(dataFormat) \
   ((tFblResult)((ApplFblGetOemStreamModeSupported(dataFormat)) ? kFblOk : kFblFailed))
# endif /* __ApplFblMemIsStreamOutputRequired */
#else
/* Allow override in configuration */
# if defined( __ApplFblMemIsDataProcessingRequired )
# else
/** Check whether specific data format identifier implies the need for data processing */
#  define __ApplFblMemIsDataProcessingRequired(dataFormat) \
   ((tFblResult)((kDiagSubNoDataProcessing == (dataFormat)) ? kFblFailed : kFblOk))
# endif /* __ApplFblMemIsDataProcessingRequired */
#endif /* FBL_MEM_ENABLE_STREAM_OUTPUT */

/*-- Verification -----------------------------------------------------------*/
#if defined( FBL_MEM_ENABLE_VERIFY_PIPELINED )
# define FBL_MEM_ENABLE_SEGMENT_HANDLING
#endif /* FBL_MEM_ENABLE_VERIFY_PIPELINED */

#if defined( SEC_ENABLE_VERIFICATION_ADDRESS_LENGTH )
/* Address and length are included in signature checks, but are not used in case a simple
   checksum is calculated.
*/
# if (SEC_SECURITY_CLASS != SEC_CLASS_DDD )
/** Include address and length info in on-the-fly verification on input data */
#  define FBL_MEM_ENABLE_VERIFY_INPUT_ADDRESS_LENGTH
/** Include address and length info in on-the-fly verification on processed data */
#  define FBL_MEM_ENABLE_VERIFY_PROCESSED_ADDRESS_LENGTH
/** Include address and length info in pipelined verification */
#  define FBL_MEM_ENABLE_VERIFY_PIPELINED_ADDRESS_LENGTH
# endif
#endif /* SEC_ENABLE_VERIFICATION_ADDRESS_LENGTH */

/** Size of temporary verification result buffer */
#define FBL_MEM_SIG_RESULT_BUFFER_SIZE                   kDiagSigBufLength

/*-- Memory driver ----------------------------------------------------------*/
/*
   Perform actions directly before and after memory driver write
   Remove macro if not applicable
*/
#define __ApplFblMemPreWrite()                     FblDiagMemPreWrite()
#define __ApplFblMemPostWrite()                    FblDiagMemPostWrite()
/*
   Perform actions directly before and after memory driver erase
   Remove macro if not applicable
*/
#define __ApplFblMemPreErase()                     FblDiagMemPreWrite()
#define __ApplFblMemPostErase()                    FblDiagMemPostWrite()
/*
   Perform actions directly after segment end indication
   Remove macro if not applicable
*/
/* #define __ApplFblMemPostSegmentEnd() */

/*-- Watchdog ---------------------------------------------------------------*/
/* Default return code */
#define FBL_MEM_WD_TRIGGER_DEFAULT                 (FBL_NO_TRIGGER)
/*
   Overwrite status type returned by watchdog trigger function
   Remove macro if not applicable
*/
#define FBL_MEM_TRIGGER_STATUS_OVERWRITE           vuint8
/*
   Default watchdog trigger
   Remove macro if not applicable
*/
#define __ApplFblMemWdTrigger()                    FblLookForWatchdog()
/*
   Watchdog trigger including adaptive generation of RCR-RP
   Remove macro if not applicable
*/
#define __ApplFblMemAdaptiveRcrRp()                FblRealTimeSupport()
/*
   Check whether RCR-RP is already active
   Remove macro if not applicable or required (e.g. if kForceSendRpIfNotInProgress is available)
*/
#define __ApplFblMemIsRcrRpActive()                ((tFblResult)(FblDiagGetRcrRpInProgress() ? kFblOk : kFblFailed))
/*
   Forced generation of RCR-RP (hint: use kForceSendRpIfNotInProgress if available)
   Remove macro if not applicable
*/
#define __ApplFblMemForcedRcrRp()                  DiagExRCRResponsePending(kForceSendResponsePending)

/* Pass watchdog trigger with extended status result to data processing */
#define FBL_MEM_ENABLE_EXT_TRIGGER_DATA_PROC
/* Pass watchdog trigger with extended status result to signature verification over input data */
#define FBL_MEM_ENABLE_EXT_TRIGGER_INPUT_VERIFY
/* Pass watchdog trigger with extended status result to signature verification over output data */
#define FBL_MEM_ENABLE_EXT_TRIGGER_OUTPUT_VERIFY

#if defined( FBL_MEM_ENABLE_PROGRESS_INFO )
/** Progress information reporting */
# define __ApplFblMemReportProgress(progressInfo)
#endif /* FBL_MEM_ENABLE_PROGRESS_INFO */

/*-- Error handling ---------------------------------------------------------*/
/** Overwrite error codes by OEM dependent value */
#define FBL_MEM_ENABLE_STATUS_OVERWRITE
/* Enable interface to remap error codes to OEM dependent value */
#define FBL_MEM_DISABLE_STATUS_REMAPPING
/* OEM dependent status type */
#define FBL_MEM_STATUS_TYPE                           vuint8

#if defined( FBL_MEM_ENABLE_STATUS_OVERWRITE ) || \
    defined( FBL_MEM_ENABLE_STATUS_REMAPPING )
/* Default */
# define FBL_MEM_STATUS_Ok                            kDiagErrorNone
# define FBL_MEM_STATUS_Failed                        kDiagNrcConditionsNotCorrect
/* BlockErase indication */
# define FBL_MEM_STATUS_BlockEraseSequence            kDiagNrcRequestSequenceError
/* BlockStart indication */
# define FBL_MEM_STATUS_BlockStartSequence            kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_BlockStartParam               kDiagNrcConditionsNotCorrect
/* BlockEnd indication */
# define FBL_MEM_STATUS_BlockEndSequence              kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_BlockEndVerifyFinalize        kDiagNrcGeneralProgrammingFailure
/* BlockVerify */
# define FBL_MEM_STATUS_BlockVerifySequence           kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_BlockVerifyInputVerify        kDiagNrcGeneralReject
# define FBL_MEM_STATUS_BlockVerifyProcessedVerify    kDiagNrcGeneralReject
# define FBL_MEM_STATUS_BlockVerifyPipeVerify         kDiagNrcGeneralReject
# define FBL_MEM_STATUS_BlockVerifyOutputVerify       kDiagNrcGeneralReject
/* SegmentStart indication */
# define FBL_MEM_STATUS_SegmentStartSequence          kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_SegmentStartDataProcInit      kDiagNrcUploadDownloadNotAccepted
# define FBL_MEM_STATUS_SegmentStartStreamOutInit     kDiagNrcUploadDownloadNotAccepted
# define FBL_MEM_STATUS_SegmentStartVerifyInit        kDiagNrcConditionsNotCorrect
# define FBL_MEM_STATUS_SegmentStartVerifyCompute     kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_SegmentStartSegmentCount      kDiagNrcRequestOutOfRange
/* SegmentEnd indication */
# define FBL_MEM_STATUS_SegmentEndSequence            kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_SegmentEndInsufficientData    kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_SegmentEndPost                kDiagNrcConditionsNotCorrect
/* Data indication */
# define FBL_MEM_STATUS_DataIndSequence               kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_DataIndParam                  kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DataIndOverflow               kDiagNrcTransferDataSuspended
/* Data processing */
# define FBL_MEM_STATUS_DataProc                      kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DataProcConsume               kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DataProcDeinit                kDiagNrcConditionsNotCorrect
/* Stream output */
# define FBL_MEM_STATUS_StreamOutput                  kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_StreamOutputConsume           kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_StreamOutputOverflow          kDiagNrcTransferDataSuspended
# define FBL_MEM_STATUS_StreamOutputDeinit            kDiagNrcConditionsNotCorrect
/* Memory driver */
# define FBL_MEM_STATUS_DriverResumeWrite             kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DriverWrite                   kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DriverErase                   kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DriverRemainder               kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_DriverSuspendWrite            kDiagNrcGeneralProgrammingFailure
/* Programming operation */
# define FBL_MEM_STATUS_ProgramOverflow               kDiagNrcTransferDataSuspended
# define FBL_MEM_STATUS_ProgramOutsideFbt             kDiagNrcTransferDataSuspended
# define FBL_MEM_STATUS_ProgramUnalignedAddress       kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_ProgramDriverNotReady         kDiagNrcRequestSequenceError
# define FBL_MEM_STATUS_ProgramPreWrite               kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_ProgramPostWrite              kDiagNrcGeneralProgrammingFailure
/* Erase operation */
# define FBL_MEM_STATUS_EraseOutsideFbt               kDiagNrcRequestOutOfRange
# define FBL_MEM_STATUS_EraseDriverNotReady           kDiagNrcConditionsNotCorrect
# define FBL_MEM_STATUS_ErasePreErase                 kDiagNrcConditionsNotCorrect
# define FBL_MEM_STATUS_ErasePostErase                kDiagNrcConditionsNotCorrect
/* On-the-fly verification */
# define FBL_MEM_STATUS_VerifyCompute                 kDiagNrcGeneralProgrammingFailure
# define FBL_MEM_STATUS_VerifyFinalize                kDiagNrcGeneralProgrammingFailure
/* Pass-through */
# define FBL_MEM_STATUS_PassThroughLocal              kDiagNrcConditionsNotCorrect
# define FBL_MEM_STATUS_PassThroughRemote             kDiagNrcConditionsNotCorrect
#endif /* FBL_MEM_ENABLE_STATUS_OVERWRITE || FBL_MEM_ENABLE_STATUS_REMAPPING */

#if defined( FBL_ENABLE_DEBUG_STATUS )
/** Enable extended error status handling */
# define FBL_MEM_ENABLE_EXT_STATUS

/* Default */
# define FBL_MEM_EXT_STATUS_Ok(status)
# define FBL_MEM_EXT_STATUS_Failed(status)
/* BlockErase indication */
# define FBL_MEM_EXT_STATUS_BlockEraseSequence(status)
/* BlockStart indication */
# define FBL_MEM_EXT_STATUS_BlockStartSequence(status)
# define FBL_MEM_EXT_STATUS_BlockStartParam(status)
/* BlockEnd indication */
# define FBL_MEM_EXT_STATUS_BlockEndSequence(status)
# define FBL_MEM_EXT_STATUS_BlockEndVerifyFinalize(status)
/* BlockVerify */
# define FBL_MEM_EXT_STATUS_BlockVerifySequence(status)
# define FBL_MEM_EXT_STATUS_BlockVerifySigVerify(status)
# define FBL_MEM_EXT_STATUS_BlockVerifyInputVerify(status)
# define FBL_MEM_EXT_STATUS_BlockVerifyProcessedVerify(status)
# define FBL_MEM_EXT_STATUS_BlockVerifyPipeVerify(status)
# define FBL_MEM_EXT_STATUS_BlockVerifyOutputVerify(status)
/* SegmentStart indication */
# define FBL_MEM_EXT_STATUS_SegmentStartSequence(status)
# define FBL_MEM_EXT_STATUS_SegmentStartDataProcInit(status)
# define FBL_MEM_EXT_STATUS_SegmentStartStreamOutInit(status)
# define FBL_MEM_EXT_STATUS_SegmentStartVerifyInit(status)
# define FBL_MEM_EXT_STATUS_SegmentStartVerifyCompute(status)
# define FBL_MEM_EXT_STATUS_SegmentStartSegmentCount(status)   FblErrStatSetError(FBL_ERR_TOO_MANY_SEGMENTS_IN_MODULE)
/* SegmentEnd indication */
# define FBL_MEM_EXT_STATUS_SegmentEndSequence(status)
# define FBL_MEM_EXT_STATUS_SegmentEndInsufficientData(status)
# define FBL_MEM_EXT_STATUS_SegmentEndPost(status)
/* Data indication */
# define FBL_MEM_EXT_STATUS_DataIndSequence(status)
# define FBL_MEM_EXT_STATUS_DataIndParam(status)
# define FBL_MEM_EXT_STATUS_DataIndOverflow(status)            FblErrStatSetState(FBL_ERR_DATA_OVERFLOW)
/* Data processing */
# define FBL_MEM_EXT_STATUS_DataProc(status)
# define FBL_MEM_EXT_STATUS_DataProcConsume(status)
# define FBL_MEM_EXT_STATUS_DataProcDeinit(status)
/* Stream output */
# define FBL_MEM_EXT_STATUS_StreamOutput(status)
# define FBL_MEM_EXT_STATUS_StreamOutputConsume(status)
# define FBL_MEM_EXT_STATUS_StreamOutputOverflow(status)
# define FBL_MEM_EXT_STATUS_StreamOutputDeinit(status)
/* Memory driver */
# define FBL_MEM_EXT_STATUS_DriverResumeWrite(status)
/* Status contains flash driver error code */
# define FBL_MEM_EXT_STATUS_DriverWrite(status)                { FblErrStatSetFlashDrvError(status); FblErrStatSetError(FBL_ERR_FLASH_PROG_ERROR); }
# define FBL_MEM_EXT_STATUS_DriverErase(status)                { FblErrStatSetFlashDrvError(status); FblErrStatSetError(FBL_ERR_FLASH_ERASE_ERROR); }
# define FBL_MEM_EXT_STATUS_DriverRemainder(status)
# define FBL_MEM_EXT_STATUS_DriverSuspendWrite(status)
/* Programming operation */
# define FBL_MEM_EXT_STATUS_ProgramOverflow(status)            FblErrStatSetState(FBL_ERR_DATA_OVERFLOW)
# define FBL_MEM_EXT_STATUS_ProgramOutsideFbt(status)          FblErrStatSetError(FBL_ERR_NO_MEMORY_REGION_FOUND)
# define FBL_MEM_EXT_STATUS_ProgramUnalignedAddress(status)    FblErrStatSetError(FBL_ERR_FLASH_PROG_ERROR)
# define FBL_MEM_EXT_STATUS_ProgramDriverNotReady(status)      FblErrStatSetError(FBL_ERR_FLASHCODE_INIT_FAILED)
# define FBL_MEM_EXT_STATUS_ProgramPreWrite(status)
# define FBL_MEM_EXT_STATUS_ProgramPostWrite(status)
/* Erase operation */
# define FBL_MEM_EXT_STATUS_EraseOutsideFbt(status)            FblErrStatSetError(FBL_ERR_NO_MEMORY_REGION_FOUND)
# define FBL_MEM_EXT_STATUS_EraseDriverNotReady(status)        FblErrStatSetError(FBL_ERR_FLASHCODE_INIT_FAILED)
# define FBL_MEM_EXT_STATUS_ErasePreErase(status)
# define FBL_MEM_EXT_STATUS_ErasePostErase(status)
/* On-the-fly verification */
# define FBL_MEM_EXT_STATUS_VerifyCompute(status)
# define FBL_MEM_EXT_STATUS_VerifyFinalize(status)
/* Pass-through */
# define FBL_MEM_EXT_STATUS_PassThroughLocal(status)
# define FBL_MEM_EXT_STATUS_PassThroughRemote(status)

/* Extended info: status contains current address */
# define FBL_MEM_EXT_STATUS_ProgramAddress(status)             FblErrStatSetAddress(status)
# define FBL_MEM_EXT_STATUS_EraseAddress(status)               FblErrStatSetAddress(status)
#endif /* FBL_ENABLE_DEBUG_STATUS */

/* PRQA L:TAG_FblMem_3458_1 */
/* PRQA L:TAG_FblMem_3453_1 */

#endif /* __FBL_MEM_OEM_H__ */

/***********************************************************************************************************************
 *  END OF FILE: FBL_MEM_OEM.H
 **********************************************************************************************************************/
