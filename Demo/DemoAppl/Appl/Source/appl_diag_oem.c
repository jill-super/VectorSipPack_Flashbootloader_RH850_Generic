/***********************************************************************************************************************
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief         Diagnostic services supported in boot mode
 *                 Declaration of functions, variables, and constants
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
 *  Achim Strobelt                Ach           Vector Informatik GmbH
 *  Marco Riedl                   Rie           Vector Informatik GmbH
 *  --------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  07.00.00   2017-08-15  Ach     ESCAN00095332    Update to FblDiag_14229_Core 3.x
 *  07.00.01   2017-09-01  Ach     ESCAN00096493    Erase error is responded with NRC 31 instead of positive response
 *  07.00.02   2017-10-24  Rie     ESCAN00097182    Fixed overlapping states
 *  07.01.00   2017-11-22  Ach     ESCAN00097502    Support configurable stay in boot message
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

/* Common Bootloader includes */
#include "fbl_inc.h"
/* Core Diagnostic interface and global symbols */
#include "fbl_diag_core.h"

/***********************************************************************************************************************
 *  VERSION
 **********************************************************************************************************************/

/* Diagnostic OEM module version check */
#if ((FBLDIAG_14229_UDS2_VERSION != 0x0701u) || \
     (FBLDIAG_14229_UDS2_RELEASE_VERSION != 0x00u))
# error "Error in fbl_diag_oem.c: Source and header file are inconsistent!"
#endif
#if(( FBLDIAG_14229_UDS2_VERSION != _FBLDIAG_OEM_VERSION ) || \
    ( FBLDIAG_14229_UDS2_RELEASE_VERSION != _FBLDIAG_OEM_RELEASE_VERSION ))
# error "Error in fbl_diag_oem.c: Source and v_ver.h are inconsistent!"
#endif

/***********************************************************************************************************************
 *  TYPE DEFINITIONS
 **********************************************************************************************************************/

#if defined( FBL_DIAG_ENABLE_FLASHDRV_ROM )
/** Structure to initialize flash drivers from ROM */
typedef struct tagFlashDriverInitData
{
   V_MEMROM1 vuint8 V_MEMROM2 V_MEMROM3 * sourceBuffer;  /**< Flash driver image source buffer */
   V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * destBuffer;    /**< Flash driver destination buffer */
   vuint32 copySize;                                     /**< Size of data to be copied */
   vuint8 decryptValue;                                  /**< Value to decrypt XOR-encrypted driver */
} tFlashDriverInitData;
#endif /* FBL_DIAG_ENABLE_FLASHDRV_ROM */

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

#if defined( FBL_DIAG_FLASH_CODE_BASE_ADDR )
/* Allow to override the flashCode base address for certain platforms.
 * Example: For word-addressed platforms the download address of the flash driver is going to be virtual, i.e. twice
 *          the physical address. For those platforms the base address can be overwritten in a user configuration file. */
#else
# define FBL_DIAG_FLASH_CODE_BASE_ADDR    ((tFblAddress)flashCode)
#endif

#if defined( FBL_ENABLE_UNALIGNED_DATA_TRANSFER )
   /* Report full diagnostic buffer size */
# define FBL_DIAG_RESPONSE_BUFFER_SIZE FBL_DIAG_BUFFER_LENGTH
#else
   /* Calculate buffer size so that a multiple of the diagnostic bufffer size + 2 is reported */
# define FBL_DIAG_RESPONSE_BUFFER_SIZE ((((FBL_DIAG_SEGMENT_SIZE - 1u) ^ 0xFFFFFFFFu) & (FBL_DIAG_BUFFER_LENGTH - 2u)) + 2u)
#endif /* FBL_ENABLE_UNALIGNED_DATA_TRANSFER */

#if( FBL_DIAG_RESPONSE_BUFFER_SIZE >= 0x1000000u )
# define FBL_DIAG_RESPONSE_BUFFER_LFI 4u
#elif( FBL_DIAG_RESPONSE_BUFFER_SIZE >= 0x10000u )
# define FBL_DIAG_RESPONSE_BUFFER_LFI 3u
#else
# define FBL_DIAG_RESPONSE_BUFFER_LFI 2u
#endif

/***********************************************************************************************************************
 *  State handling
 **********************************************************************************************************************/
/* Download sequence states */
#define FblDiagSetSecurityKeyAllowed()    SetFblDiagState( kFblDiagStateSecurityKeyAllowed )
#define FblDiagClrSecurityKeyAllowed()    ClrFblDiagState( kFblDiagStateSecurityKeyAllowed )
#define FblDiagSetSecurityUnlock()        SetFblDiagState( kFblDiagStateSecurityUnlock )
#define FblDiagClrSecurityUnlock()        ClrFblDiagState( kFblDiagStateSecurityUnlock )
#define FblDiagSetFingerprintValid()      SetFblDiagState( kFblDiagStateFingerprintValid )
#define FblDiagClrFingerprintValid()      ClrFblDiagState( kFblDiagStateFingerprintValid )
#define FblDiagSetEraseSucceeded()        SetFblDiagState( kFblDiagStateEraseSucceeded )
#define FblDiagClrEraseSucceeded()        ClrFblDiagState( kFblDiagStateEraseSucceeded )
#define FblDiagSetTransferDataAllowed()   SetFblDiagState( kFblDiagStateTransferDataAllowed )
#define FblDiagClrTransferDataAllowed()   ClrFblDiagState( kFblDiagStateTransferDataAllowed )
#define FblDiagSetTransferDataSucceeded() SetFblDiagState( kFblDiagStateTransferDataSucceeded )
#define FblDiagClrTransferDataSucceeded() ClrFblDiagState( kFblDiagStateTransferDataSucceeded )
#define FblDiagSetChecksumAllowed()       SetFblDiagState( kFblDiagStateChecksumAllowed )
#define FblDiagClrChecksumAllowed()       ClrFblDiagState( kFblDiagStateChecksumAllowed )
#define FblDiagSetFlashDriverPresent()    SetFblDiagState( kFblDiagStateFlashDriverPresent )
#define FblDiagClrFlashDriverPresent()    ClrFblDiagState( kFblDiagStateFlashDriverPresent )

/***********************************************************************************************************************
 *  Local constants
 **********************************************************************************************************************/
#define kDiagInitSequenceNum     ((vuint8) 0x01u)

#if defined( FBL_DIAG_ENABLE_FLASHDRV_ROM )
# if !defined( FLASH_DRIVER_INIT_DATA )
/** Initialization data for default flash driver. If additional drivers should be initialized, please overwrite macro. */
#  define FLASH_DRIVER_INIT_DATA {{flashDrvBlk0, flashCode, FLASHDRV_BLOCK0_LENGTH, FLASHDRV_DECRYPTVALUE}}
# endif
#endif /* FBL_DIAG_ENABLE_FLASHDRV_ROM */

/***********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

/* Diagnostic service help functions */
static void FblDiagSessionControlParamInit(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static vuint8 FblDiagDownloadCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Erase help functions */
static tFblResult FblDiagEraseBlock( V_MEMRAM1 tBlockDescriptor V_MEMRAM2 V_MEMRAM3 *pBlockDescriptor );
static tFblResult FblDiagCheckErasePreconditions(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pbDiagData,
                                                 V_MEMRAM1 tFblAddress V_MEMRAM2 V_MEMRAM3 * pEraseAddress,
                                                 V_MEMRAM1 tFblLength V_MEMRAM2 V_MEMRAM3 * pEraseLength);
static tFblResult FblDiagCheckProgAttemptCounter(V_MEMRAM1 tBlockDescriptor V_MEMRAM2 V_MEMRAM3 *pBlockDescriptor);
static tFblResult FblDiagPrepareFlashDriver( void );
/* Request download help functions */
static tFblResult FblDiagCheckRequestDownloadPreconditions(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pbDiagData,
                                                           V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo);
#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
static tFblResult FblDiagCheckForFlashDriverDownload(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pDownloadBlockNr,
                                                     V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo);
static tFblResult FblDiagCheckFlashDriverDownload(V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo);
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */
static tFblResult FblDiagCheckFlashMemoryDownload(V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo);
static tFblResult FblDiagPrepareFirstDownloadSegment(V_MEMRAM1 tFblMemBlockInfo V_MEMRAM2 V_MEMRAM3 * pBlockInfo,
                                                     V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo,
                                                     vuint8 tempBlockNr);

/* NRC handler functions */
static tFblResult FblDiagProcessSubfunctionNrc(vuint32 serviceErrorMask);
static tFblResult FblDiagProcessServiceNrc(vuint32 serviceErrorMask);
static tFblResult FblDiagProcessRoutineNrc(vuint32 serviceErrorMask);

/* Session handling */
static tFblResult FblDiagDefaultSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagExtendedSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagProgrammingSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagTesterPresentMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Reset */
static tFblResult FblDiagEcuResetMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* DID handling */
static tFblResult FblDiagDataByIdLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagReadDataByIdMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagWriteDataByIdMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Security access */
static tFblResult FblDiagSecAccessSeedMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagSecAccessKeyMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Communication control / DTC handling */
static tFblResult FblDiagCommCtrlMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagControlDTCMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Routine control */
static tFblResult FblDiagRCStartCsumMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
#if defined( FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS )
static tFblResult FblDiagRCStartCheckProgPreCondMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
#endif /* FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS */
#if defined( FBL_ENABLE_STAY_IN_BOOT )
static tFblResult FblDiagRCStartForceBootModeMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
#endif /* FBL_ENABLE_STAY_IN_BOOT */
static tFblResult FblDiagRCStartEraseLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagRCStartEraseMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagRCStartCheckProgDepMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Data transfer */
static tFblResult FblDiagRequestDownloadLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagRequestDownloadMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagTransferDataLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagTransferDataMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);
static tFblResult FblDiagReqTransferExitMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/* Service pre-handler functions */
static tFblResult FblDiagDefaultPreHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

/***********************************************************************************************************************
 *  LOCAL CONSTANTS
 **********************************************************************************************************************/

#if defined( FBL_DIAG_ENABLE_FLASHDRV_ROM )
/** List of flash driver source and destination buffers to be initialized. */ /* PRQA S 3218 1 */ /* MD_FblDiag_3218 */
V_MEMROM0 static V_MEMROM1 tFlashDriverInitData V_MEMROM2 kFlashDriverInitData[] = FLASH_DRIVER_INIT_DATA;
#endif /* FBL_DIAG_ENABLE_FLASHDRV_ROM */

/***********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/

/** Current block sequence counter */
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      currentSequenceCnt;
#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
/** Invalid key counter */
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      secSendKeyInvalid;   /* PRQA S 3218 */ /* MD_FblDiag_3218 */
#endif

/* Segment handling variables */
/** Verification information to be transmitted to security module */
V_MEMRAM0 static V_MEMRAM1 SecM_VerifyParamType       V_MEMRAM2 verifyParam;
/** Download segment information. Used for gap fill and data verification. */
V_MEMRAM0 static V_MEMRAM1 FL_SegmentInfoType         V_MEMRAM2 downloadSegments[SWM_DATA_MAX_NOAR];  /* PRQA S 3218 */ /* MD_FblDiag_3218 */
#if defined( FBL_MEM_ENABLE_VERIFY_PIPELINED )
/** Verification information to be transmitted to security module - pipelined verification */
V_MEMRAM0 static V_MEMRAM1 SecM_SignatureParamType    V_MEMRAM2 pipeSigParam; /* PRQA S 3218 */ /* MD_FblDiag_3218 */
/** Length of already verified data */
V_MEMRAM0 static V_MEMRAM1 vuint32                    V_MEMRAM2 pipeSigDataLength;  /* PRQA S 3218 */ /* MD_FblDiag_3218 */
#endif /* FBL_MEM_ENABLE_VERIFY_PIPELINED */

/* Download handling variables */
/** Transfer block information */
V_MEMRAM0 static V_MEMRAM1 tBlockDescriptor     V_MEMRAM2      downloadBlockDescriptor;
/** Number of remaining transfer bytes */
V_MEMRAM0 static V_MEMRAM1 tFblLength           V_MEMRAM2      transferRemainder;
/** Block sequence counter */
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      expectedSequenceCnt;

/***********************************************************************************************************************
 *  Diagnostic handler function call table
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Subfunction table configuration
 **********************************************************************************************************************/

/** Sub-function defintion for Default Session (01) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableDsc_DefaultSession[] = { kDiagSubDefaultSession };
/** Sub-function defintion for Programming Session (02) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableDsc_ProgSession[] = { kDiagSubProgrammingSession };
/** Sub-function defintion for Extended Session (03) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableDsc_ExtendedSession[] = { kDiagSubExtendedDiagSession };

/** Sub-function definition for Diagnostic Session Control service (10) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableSessionControl[] =
{
   /* Default Session (01) */
   {
      kFblDiagSubtableDsc_DefaultSession,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionFunctionalRequest),
      kDiagRqlDiagnosticSessionControl,
      (tFblDiagLengthCheck)0u,
      FblDiagDefaultSessionMainHandler
   },
   /* Programming Session (02) */
   {
      kFblDiagSubtableDsc_ProgSession,
      (kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionFunctionalRequest),
      kDiagRqlDiagnosticSessionControl,
      (tFblDiagLengthCheck)0u,
      FblDiagProgrammingSessionMainHandler
   },
   /* Extended Session (03) */
   {
      kFblDiagSubtableDsc_ExtendedSession,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlDiagnosticSessionControl,
      (tFblDiagLengthCheck)0u,
      FblDiagExtendedSessionMainHandler
   }
};

/** Sub-function definition for Hard Reset sub-function (01) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableEcuReset_HardReset[] = { kDiagSubHardReset };

/** Sub-function definition for ECU Reset service (11) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableEcuReset[] =
{
   /* Hard Reset (01) */
   {
      kFblDiagSubtableEcuReset_HardReset,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionFunctionalRequest),
      kDiagRqlEcuReset,
      (tFblDiagLengthCheck)0u,
      FblDiagEcuResetMainHandler
   }
};

/** Sub-function definition for Request Seed sub-function (configurable) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableSecAccess_Seed[] = { kFblDiagSubRequestSeed };
/** Sub-function definition for Send Key sub-function (configurable) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableSecAccess_Key[] = { kFblDiagSubSendKey };

/** Sub-function definition for Security Access service (27) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableSecAccess[] =
{
   /* Request Seed (configurable) */
   {
      kFblDiagSubtableSecAccess_Seed,
      (kFblDiagOptionSessionProgramming),
      kDiagRqlSecurityAccessSeed,
      (tFblDiagLengthCheck)0u,
      FblDiagSecAccessSeedMainHandler
   },
   /* Send Key (configurable) */
   {
      kFblDiagSubtableSecAccess_Key,
      (kFblDiagOptionSessionProgramming),
      kDiagRqlSecurityAccessKey,
      (tFblDiagLengthCheck)0u,
      FblDiagSecAccessKeyMainHandler
   }
};

/** Sub-function definition for EnableRxAndEnableTx sub-function (00) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableCommControl_EnRxEnTx[] = { kDiagSubEnableRxAndTx };
/** Sub-function definition for Disable sub-function (configurable) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableCommControl_Disable[] = { FBL_DIAG_COMMUNICATION_CONTROL_TYPE };

/** Sub-function defintion for Communication Control service (28) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableCommControl[] =
{
   /* Enable Rx and Tx (00) */
   {
      kFblDiagSubtableCommControl_EnRxEnTx,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlCommunicationControl,
      (tFblDiagLengthCheck)0u,
      FblDiagCommCtrlMainHandler
   },
   /* Disable communication (configurable) */
   {
      kFblDiagSubtableCommControl_Disable,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlCommunicationControl,
      (tFblDiagLengthCheck)0u,
      FblDiagCommCtrlMainHandler
   }
};

/** Sub-function / RID definition for checksum verification (010202) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableRC_StartChecksum[] = { kDiagSubStartRoutine, kDiagRoutineIdChecksumHigh, kDiagRoutineIdChecksumLow };
#if defined( FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS )
/** Sub-function / RID definition for programming pre-condition check (010203) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableRC_StartProgPreCond[] = { kDiagSubStartRoutine, kDiagRoutineIdCheckProgPreCondHigh, kDiagRoutineIdCheckProgPreCondLow };
#endif /* FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS */
#if defined( FBL_ENABLE_STAY_IN_BOOT )
/** Sub-function / RID definition for force boot mode request (01F518) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableRC_StartForceBoot[] = { kDiagSubStartRoutine, kDiagRoutineIdStayInBootHigh, kDiagRoutineIdStayInBootLow };
#endif /* FBL_ENABLE_STAY_IN_BOOT */
/** Sub-function / RID definition for erase memory request (01FF00) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableRC_StartErase[] = { kDiagSubStartRoutine, kDiagRoutineIdEraseMemoryHigh, kDiagRoutineIdEraseMemoryLow };
/** Sub-function / RID definition for check programming dependencies request (01FF01) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableRC_StartCheckProgDep[] = { kDiagSubStartRoutine, kDiagRoutineIdCheckProgDepHigh, kDiagRoutineIdCheckProgDepLow };

/** Sub-function / RID defintion for Routine Control service (31) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableRoutine[] =
{
   /* Checksum verification (010202) */
   {
      kFblDiagSubtableRC_StartChecksum,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlRoutineControlCheckRoutine,
      (tFblDiagLengthCheck)0u,
      FblDiagRCStartCsumMainHandler
   },
#if defined( FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS )
   /* Check programming pre-conditions (010203) */
   {
      kFblDiagSubtableRC_StartProgPreCond,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlRoutineControlProgPreCond,
      (tFblDiagLengthCheck)0u,
      FblDiagRCStartCheckProgPreCondMainHandler
   },
#endif /* FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS */
#if defined( FBL_ENABLE_STAY_IN_BOOT )
   /* Force boot mode request (01F518) */
   {
      kFblDiagSubtableRC_StartForceBoot,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming),
      kDiagRqlRoutineControlForceBoot,
      (tFblDiagLengthCheck)0u,
      FblDiagRCStartForceBootModeMainHandler
   },
#endif /* FBL_ENABLE_STAY_IN_BOOT */
   /* Erase memory request (01FF00) */
   {
      kFblDiagSubtableRC_StartErase,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlRoutineControlEraseRoutine,
      FblDiagRCStartEraseLengthCheck,
      FblDiagRCStartEraseMainHandler
   },
   /* Check programming dependencies (01FF01) */
   {
      kFblDiagSubtableRC_StartCheckProgDep,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlRoutineControlCheckProgDep,
      (tFblDiagLengthCheck)0u,
      FblDiagRCStartCheckProgDepMainHandler
   }
};

/** Sub-function definition for Zero sub-function (00) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableTesterPresent_ZeroSubfunction[] = { kDiagSubTesterPresent };

/** Sub-function definition for Tester Present service (3E) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableTesterPresent[] =
{
   /* Zero sub-function (00) */
   {
      kFblDiagSubtableTesterPresent_ZeroSubfunction,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionFunctionalRequest),
      kDiagRqlTesterPresent,
      (tFblDiagLengthCheck)0u,
      FblDiagTesterPresentMainHandler
   }
};

/** Sub-function definition for Control DTC - On sub-function (01) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableControlDtc_OnSubfunction[] = { kDiagSubDtcOn };
/** Sub-function definition for Control DTC - Off sub-function (02) */
V_MEMROM0 static V_MEMROM1 vuint8 V_MEMROM2 kFblDiagSubtableControlDtc_OffSubfunction[] = { kDiagSubDtcOff };

/** Sub-function definition for Control DTC service (85) */
V_MEMROM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 kFblDiagSubtableControlDtc[] =
{
   /* On sub-function (01) */
   {
      kFblDiagSubtableControlDtc_OnSubfunction,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlControlDTCSetting,
      (tFblDiagLengthCheck)0u,
      FblDiagControlDTCMainHandler
   },
   /* Off sub-function (02) */
   {
      kFblDiagSubtableControlDtc_OffSubfunction,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest),
      kDiagRqlControlDTCSetting,
      (tFblDiagLengthCheck)0u,
      FblDiagControlDTCMainHandler
   }
};

/***********************************************************************************************************************
 *  Main service table configuration
 **********************************************************************************************************************/

/** Table of supported diagnostic servcies */
V_MEMROM0 V_MEMROM1 tFblDiagServiceTable V_MEMROM2 kFblDiagServiceTable[] =
{
   /* Diagnostic Session Control (10) */
   {
      kDiagSidDiagnosticSessionControl,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionServiceIsSubfunction | kFblDiagOptionFunctionalRequest),
      kDiagRqlDiagnosticSessionControl,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableSessionControl),
      ARRAY_SIZE(kFblDiagSubtableDsc_DefaultSession),
      kFblDiagSubtableSessionControl,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   },
   /* ECU Reset (11) */
   {
      kDiagSidEcuReset,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionServiceIsSubfunction | kFblDiagOptionFunctionalRequest),
      kDiagRqlEcuReset,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableEcuReset),
      ARRAY_SIZE(kFblDiagSubtableEcuReset_HardReset),
      kFblDiagSubtableEcuReset,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   },
   /* Read Data by Identifier (22) */
   {
      kDiagSidReadDataByIdentifier,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionFunctionalRequest),
      kDiagRqlReadDataByIdentifier,
      FblDiagDataByIdLengthCheck,
      0u,
      0u,
      (tFblDiagServiceSubTable*)V_NULL,
      FblDiagDefaultPreHandler,
      FblDiagReadDataByIdMainHandler,
      FblDiagProcessServiceNrc,
      FblDiagDefaultPostHandler
   },
   /* Security Access (27) */
   {
      kDiagSidSecurityAccess,
      (kFblDiagSecurityAccessSessionMask | kFblDiagOptionServiceIsSubfunction),
      kDiagRqlServiceWithSubfunction,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableSecAccess),
      ARRAY_SIZE(kFblDiagSubtableSecAccess_Seed),
      kFblDiagSubtableSecAccess,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   },
   /* Communication Control (28) */
   {
      kDiagSidCommunicationControl,
      (kFblDiagOptionSessionExtended | kFblDiagOptionServiceIsSubfunction | kFblDiagOptionFunctionalRequest),
      kDiagRqlServiceWithSubfunction,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableCommControl),
      ARRAY_SIZE(kFblDiagSubtableCommControl_EnRxEnTx),
      kFblDiagSubtableCommControl,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   },
   /* Write Data by Identifier (2E) */
   {
      kDiagSidWriteDataByIdentifier,
      (kFblDiagOptionSessionProgramming),
      kDiagRqlWriteDataByIdentifier,
      FblDiagDataByIdLengthCheck,
      0u,
      0u,
      (tFblDiagServiceSubTable*)V_NULL,
      FblDiagDefaultPreHandler,
      FblDiagWriteDataByIdMainHandler,
      FblDiagProcessServiceNrc,
      FblDiagDefaultPostHandler
   },
   /* Routine Control (31) */
   {
      kDiagSidRoutineControl,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionServiceIsSubfunction | kFblDiagOptionFunctionalRequest),
      kDiagRqlRoutineControl,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableRoutine),
      ARRAY_SIZE(kFblDiagSubtableRC_StartChecksum),
      kFblDiagSubtableRoutine,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessRoutineNrc,
      FblDiagDefaultPostHandler
   },
   /* Request Download (34) */
   {
      kDiagSidRequestDownload,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlRequestDownload,
      FblDiagRequestDownloadLengthCheck,
      0u,
      0u,
      (tFblDiagServiceSubTable*)V_NULL,
      FblDiagDefaultPreHandler,
      FblDiagRequestDownloadMainHandler,
      FblDiagProcessServiceNrc,
      FblDiagDefaultPostHandler
   },
   /* Transfer Data (36) */
   {
      kDiagSidTransferData,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlTransferData,
      FblDiagTransferDataLengthCheck,
      0u,
      0u,
      (tFblDiagServiceSubTable*)V_NULL,
      FblDiagDefaultPreHandler,
      FblDiagTransferDataMainHandler,
      FblDiagProcessServiceNrc,
      FblDiagDefaultPostHandler
   },
   /* Request Transfer Exit (37) */
   {
      kDiagSidRequestTransferExit,
      (kFblDiagOptionSessionProgramming | kFblDiagOptionSecuredService),
      kDiagRqlRequestTransferExit,
      (tFblDiagLengthCheck)0u,
      0u,
      0u,
      (tFblDiagServiceSubTable*)V_NULL,
      FblDiagDefaultPreHandler,
      FblDiagReqTransferExitMainHandler,
      FblDiagProcessServiceNrc,
      FblDiagDefaultPostHandler
   },
   /* Tester Present (3E) */
   {
      kDiagSidTesterPresent,
      (kFblDiagOptionSessionDefault | kFblDiagOptionSessionExtended | kFblDiagOptionSessionProgramming | kFblDiagOptionServiceIsSubfunction | kFblDiagOptionFunctionalRequest),
      kDiagRqlTesterPresent,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableTesterPresent),
      ARRAY_SIZE(kFblDiagSubtableTesterPresent_ZeroSubfunction),
      kFblDiagSubtableTesterPresent,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   },
   /* Control DTC (85) */
   {
      kDiagSidControlDTCSetting,
      (kFblDiagOptionSessionExtended | kFblDiagOptionFunctionalRequest | kFblDiagOptionServiceIsSubfunction),
      kDiagRqlServiceWithSubfunction,
      (tFblDiagLengthCheck)0u,
      ARRAY_SIZE(kFblDiagSubtableControlDtc),
      ARRAY_SIZE(kFblDiagSubtableControlDtc_OnSubfunction),
      kFblDiagSubtableControlDtc,
      FblDiagDefaultPreHandler,
      (tFblDiagMainHandler)0u,
      FblDiagProcessSubfunctionNrc,
      FblDiagDefaultPostHandler
   }
};

/** Size of service table */
V_MEMROM0 V_MEMROM1 vuintx V_MEMROM2 kSizeOfServiceTable = ARRAY_SIZE(kFblDiagServiceTable);

/***********************************************************************************************************************
 *  Service support functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagDownloadCheck
 **********************************************************************************************************************/
/*! \brief       Verify the downloaded data.
 *  \details     Uses the configured security module to verify if the download has been done correctly and
 *               the data is allowed to be downloaded. Also calculates CRC total if enabled.
 *  \pre         Logical block or flash driver has to be downloaded completely.
 *  \param[in]   pbDiagData Pointer to data in the diagBuffer (without SID).
 *  \param[in]   diagReqDataLen Length of data (without SID).
 *  \return      Verification result supplied by the security module.
 **********************************************************************************************************************/
static vuint8 FblDiagDownloadCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblMemBlockVerifyData verifyData;
   SecM_StatusType verifyErrorCode;
   vuint8 result;

   /* Length is not explicitly submitted - use (request length - sub-parameter and routine ID length) */

#if defined( FBL_MEM_ENABLE_VERIFY_PIPELINED )
   verifyData.verifyDataPipe.length = diagReqDataLen - 3u;
   verifyData.verifyDataPipe.data = (V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 *)&pbDiagData[kDiagLocFmtRoutineStatus];
#endif /* FBL_MEM_ENABLE_VERIFY_PIPELINED */

#if defined( FBL_MEM_ENABLE_VERIFY_OUTPUT )
   verifyData.verifyDataOutput.length = diagReqDataLen - 3u;
   verifyData.verifyDataOutput.data = (V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 *)&pbDiagData[kDiagLocFmtRoutineStatus];
#endif /* FBL_MEM_ENABLE_VERIFY_OUTPUT */

   /* Call verification routine */
   if (FblMemRemapStatus(FblMemBlockVerify(&verifyData, &verifyErrorCode)) != kFblMemStatus_Ok)
   {
      result = kDiagCheckVerificationFailed;
   }
   else
   {
      result = kDiagCheckVerificationOk;
   }

   if (result == kDiagCheckVerificationOk)
   {
      if (FblDiagGetTransferTypeFlash())
      {
         (void)FblRealTimeSupport();
#if defined( SEC_ENABLE_CRC_TOTAL )
         (void)ApplFblWriteCRCTotal(&downloadBlockDescriptor,
                                    (vuint32)verifyParam.blockStartAddress,
                                    (vuint32)verifyParam.blockLength,
                                    (vuint32)verifyParam.crcTotal
                                   );
#endif /* SEC_ENABLE_CRC_TOTAL */

         /* Call validation function to inform application */
         if (ApplFblValidateBlock(downloadBlockDescriptor) != kFblOk)
         {
            result = kDiagCheckVerificationFailed;
         }
         else
         {
            /* Increment programming success counter */
            (void)ApplFblIncProgCounts(downloadBlockDescriptor);
         }
      }
#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
      else /* Download of flash driver */
      {
         /* Send response pending for flash driver initialization */
         DiagExRCRResponsePending(kForceSendRpIfNotInProgress);

         /* Mark flash driver as present */
         FblDiagSetFlashDriverPresent();

         if (FblDiagPrepareFlashDriver() == kFblOk)
         {
            /* Expect download of application next */
            FblDiagSetTransferTypeFlash();
         }
         else
         {
            /* Flash driver initialization failed */
            result = kDiagCheckVerificationFailed;
         }
      }
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagEraseBlock
 **********************************************************************************************************************/
/*! \brief       Erase selected block
 *  \details     Prepare data structure for FblLib_Mem and erase block
 *  \pre         Flash driver initialized, erase pre-conditions have been checked
 *  \param[in]   pBlockDescriptor Pointer to block descriptor structure
 *  \return      Erase result
 **********************************************************************************************************************/
static tFblResult FblDiagEraseBlock( V_MEMRAM1 tBlockDescriptor V_MEMRAM2 V_MEMRAM3 *pBlockDescriptor )
{
   tFblResult result;
   tFblMemBlockInfo blockInfo;

   /* Initialize download block descriptor continued */
   *pBlockDescriptor = FblLogicalBlockTable.logicalBlock[pBlockDescriptor->blockNr];

   /* Initialize block structure for FblLib_Mem */
   blockInfo.targetAddress = pBlockDescriptor->blockStartAddress;
   blockInfo.targetLength = pBlockDescriptor->blockLength;
   blockInfo.logicalAddress = pBlockDescriptor->blockStartAddress;
   blockInfo.logicalLength = pBlockDescriptor->blockLength;

   /* Start erase by FblLib_Mem */
   if (FblMemRemapStatus(FblMemBlockEraseIndication(&blockInfo)) == kFblMemStatus_Ok)
   {
      /* Erase succeeded */
      FblDiagSetEraseSucceeded();
      /* After erase, the first segment of a logical block will be downloaded */
# if defined( FBL_ENABLE_PRESENCE_PATTERN )
      /* Adjust the size of the logical block according to presence pattern size. */
      (void)ApplFblAdjustLbtBlockData(&downloadBlockDescriptor);
# endif /* FBL_ENABLE_PRESENCE_PATTERN */

      /* Initialize segment counter */
      FblDiagSegmentInit();

      result = kFblOk;
   }
   else
   {
      /* Erase failed */
      FblDiagClrEraseSucceeded();
      result = kFblFailed;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagCheckErasePreconditions
 **********************************************************************************************************************/
/*! \brief         Verify that all erase preconditions are fulfilled
 *  \pre           Fingerprint is available, memory driver initialized.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[out]    pEraseAddress Start address of erase area
 *  \param[out]    pEraseLength Length of erase area
 *  \return        kFblOk: Preconditions fulfilled; kFblFailed: Preconditions not fulfilled
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagCheckErasePreconditions(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pbDiagData,
                                                 V_MEMRAM1 tFblAddress V_MEMRAM2 V_MEMRAM3 * pEraseAddress,
                                                 V_MEMRAM1 tFblLength V_MEMRAM2 V_MEMRAM3 * pEraseLength)
{
   tFblResult result;

   vuint8 addrFormat;
   vuint8 lengthFormat;

   /* Initialize variables */
   *pEraseAddress = 0u;
   *pEraseLength = 0u;

   /* Get address and length format from request */
   addrFormat = (vuint8)(pbDiagData[kDiagLocFmtRoutineAlfi] & 0x0Fu);
   lengthFormat = (vuint8)((pbDiagData[kDiagLocFmtRoutineAlfi] & 0xF0u) >> 4u);

   /* Check address and length format */
   if ((addrFormat == 0u) || (lengthFormat == 0u) || (addrFormat > 4u) || (lengthFormat > 4u))
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
   /* Check status flags */
   else if (!FblDiagGetFingerprintValid())
   {
      DiagNRCConditionsNotCorrect();
      result = kFblFailed;
   }
   else if (FblDiagPrepareFlashDriver() != kFblOk)
   {
      DiagNRCConditionsNotCorrect();
      result = kFblFailed;
   }
   else
   {
      /* Get memory address and length */
      *pEraseAddress = FblMemGetInteger(addrFormat, &pbDiagData[kDiagLocFmtStartAddress]);
      *pEraseLength = FblMemGetInteger(lengthFormat, &pbDiagData[kDiagLocFmtStartAddress + addrFormat]);

      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagPrepareFlashDriver
 **********************************************************************************************************************/
/*! \brief         Prepare flash driver for usage
 *  \return        Preparation result
 **********************************************************************************************************************/
static tFblResult FblDiagPrepareFlashDriver( void )
{
   tFblResult result;
   tFlashErrorCode flashErrorCode;
#if defined( FBL_DIAG_ENABLE_FLASHDRV_ROM )
   vuintx flashDriverIndex;
   vuint32 byteIndex;
   V_MEMROM1 tFlashDriverInitData V_MEMROM2 V_MEMROM3 * flashDriverInitData;
#endif /* FBL_DIAG_ENABLE_FLASHDRV_ROM */

   /* Initialize variable */
   result = kFblFailed;

   if (!FblDiagGetMemDriverInitialized())
   {
#if defined( FBL_DIAG_ENABLE_FLASHDRV_ROM )
      /* Use flash driver from image in case no driver has been downloaded */
      if (!FblDiagGetFlashDriverPresent())
      {
         /* Copy data of all configured flash drivers */
         for (flashDriverIndex = 0u; flashDriverIndex < ARRAY_SIZE(kFlashDriverInitData); flashDriverIndex++)
         {
            flashDriverInitData = &kFlashDriverInitData[flashDriverIndex];
            for (byteIndex = 0u; byteIndex < flashDriverInitData->copySize; byteIndex++)
            {
               flashDriverInitData->destBuffer[byteIndex] = (flashDriverInitData->decryptValue ^ flashDriverInitData->sourceBuffer[byteIndex]);
               if ((byteIndex & 0xFFu) == 0x00u)
               {
                  FblLookForWatchdogVoid();
               }
            }
         }
         /* Data copied */
         FblDiagSetFlashDriverPresent();
      }
#endif /* FBL_DIAG_ENABLE_FLASHDRV_ROM */

      if (FblDiagGetFlashDriverPresent())
      {
         /* Initialize version */
         FblErrStatSetFlashDrvVersion();  /* PRQA S 3109 */ /* MD_FblDiag_3109 */

         /* Turn on programming voltage (if necessary) */
         ApplFblSetVfp();

         /* Initialze memory drivers */
         FblCwSetOfflineMode();
         /* Check version and initialize flash driver */
         flashErrorCode = MemDriver_InitSync(V_NULL);
         FblCwSetOnlineMode();

         if (flashErrorCode == IO_E_OK)
         {
            FblDiagSetMemDriverInitialized();
            result = kFblOk;
         }
         else
         {
            FblErrStatSetError(FBL_ERR_FLASHCODE_INIT_FAILED);
            FblErrStatSetFlashDrvError(flashErrorCode);
         }
      }
   }
   else
   {
      /* Flash driver already initialized */
      result = kFblOk;
   }

   return result;
}  /* PRQA S 6080 */ /* MD_MSR_STMIF */

/***********************************************************************************************************************
 *  FblDiagCheckProgAttemptCounter
 **********************************************************************************************************************/
/*! \brief         Check if programming attempt counter allows another programming cycle
 *  \pre           Response pending processing has been started, block has been selected
 *  \param[in]     pBlockDescriptor Pointer to current download block descriptor
 *  \return        kFblOk: Programming allowed; kFblFailed: Programming not allowed
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagCheckProgAttemptCounter(V_MEMRAM1 tBlockDescriptor V_MEMRAM2 V_MEMRAM3 *pBlockDescriptor)
{
   tFblResult result;
   vuint16 progAttempts;

   /* Initialize variables */
   result = kFblOk;

   /* Initialize variables */
   if (ApplFblGetProgAttempts(*pBlockDescriptor, &progAttempts) != kFblOk)
   {
      /* Programming attempt counter couldn't be read. Use 0 as default value. */
      progAttempts = 0u;
   }

   /* Check if logical block does not restrict the number of programming attempts */
   if (ApplFblGetPromMaxProgAttempts(pBlockDescriptor->blockNr) != 0u)
   {
      /* Check logical block programming attempts */
      if (progAttempts >= ApplFblGetPromMaxProgAttempts(pBlockDescriptor->blockNr))
      {
         /* Logical block cannot be programmed anymore */
         FblErrStatSetError(FBL_ERR_FLASH_PROG_CONDITIONS);
         DiagNRCConditionsNotCorrect();
         result = kFblFailed;
      }
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagCheckRequestDownloadPreconditions
 **********************************************************************************************************************/
/*! \brief         Verify that all request download preconditions are fulfilled
 *  \pre           Fingerprint is available
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[out]    pSegmentInfo FblLib_Mem segment information for this download
 *  \return        kFblOk: Preconditions fulfilled; kFblFailed: Preconditions not fulfilled
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagCheckRequestDownloadPreconditions(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pbDiagData,
                                                           V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo)
{
   tFblResult result;

   vuint8  lengthFormat;
   vuint8  addrFormat;

   /* Initialize variables */
   pSegmentInfo->logicalAddress = 0u;
   pSegmentInfo->logicalLength = 0u;
   pSegmentInfo->targetAddress = 0u;
   pSegmentInfo->targetLength = 0u;

   /* Get length and address format from message */
   lengthFormat = (vuint8)((pbDiagData[kDiagLocFmtFormatOffset] & 0xF0u) >> 4u);
   addrFormat = (vuint8) (pbDiagData[kDiagLocFmtFormatOffset] & 0x0Fu);
   /* Read compression and encryption method */
   pSegmentInfo->dataFormat = (tFblMemDfi)pbDiagData[kDiagLocFmtSubparam];

   /* Check address and length format */
   if ((addrFormat == 0u) || (lengthFormat == 0u) || (addrFormat > 4u) || (lengthFormat > 4u))
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
   else if ((FblDiagGetTransferDataAllowed()) || (!FblDiagGetFingerprintValid()))
   {
      DiagNRCConditionsNotCorrect();
      result = kFblFailed;
   }
#if defined( FBL_ENABLE_DATA_PROCESSING )
# if defined( FBL_ENABLE_ENCRYPTION_MODE )
# else
   /* No encrypted data supported */
   else if (FblDiagGetEncryptionMode(pSegmentInfo->dataFormat) != kDiagSubNoEncryption)
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
# endif /* FBL_ENABLE_ENCRYPTION_MODE */
# if defined( FBL_ENABLE_COMPRESSION_MODE )
# else
   /* No compressed data supported */
   else if (FblDiagGetCompressionMode(pSegmentInfo->dataFormat) != kDiagSubNoCompression)
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
# endif /* FBL_ENABLE_COMPRESSION_MODE */
#else
   else if (pSegmentInfo->dataFormat != kDiagSubNoDataProcessing)
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
#endif /* FBL_ENABLE_DATA_PROCESSING */
   else
   {
      /* Get memoryAddress */
      pSegmentInfo->logicalAddress = FblMemGetInteger(addrFormat, &(pbDiagData[kDiagLocFmtAddrOffset]));
      /* Get memorySize */
      pSegmentInfo->logicalLength = FblMemGetInteger(lengthFormat, &(pbDiagData[kDiagLocFmtAddrOffset + addrFormat]));

      result = kFblOk;
   }

   return result;
}

#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
/***********************************************************************************************************************
 *  FblDiagCheckForFlashDriverDownload
 **********************************************************************************************************************/
/*! \brief         Check if a flash driver or application should be downloaded
 *  \pre           Download precondition check succeeded
 *  \param[in,out] pDownloadBlockNr Logical index of verification routine for this download
 *  \param[out]    pSegmentInfo FblLib_Mem segment information for this download
 *  \return        kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagCheckForFlashDriverDownload(V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * pDownloadBlockNr,
                                                     V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo)
{
   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

# if defined( FLASHCODE_RELOCATABLE ) || \
     defined( FLASH_DRIVER_RELOCATABLE )
   /* For relocatable drivers: If erase succeeded state is set, assume a data download
      is performed. Otherwise, a driver download is assumed. This enables downloads to
      logical blocks starting with address 0x00 in combination with relocatable flash
      drivers.
   */
   if ((pSegmentInfo->logicalAddress == 0u) && (!FblDiagGetEraseSucceeded()))
# else
   if (pSegmentInfo->logicalAddress == FBL_DIAG_FLASH_CODE_BASE_ADDR) /* PRQA S 0306 */ /* MD_FblDiag_0306 */
# endif /* FLASHCODE_RELOCATABLE || FLASH_DRIVER_RELOCATABLE */
   {
      /* Download of flash driver requested */
      FblDiagClrFlashDriverPresent();
      /* Deinit flash driver in case it is already initialized */
      if (FblDiagGetMemDriverInitialized())
      {
         DiagExRCRResponsePending(kForceSendRpIfNotInProgress);
         FblDiagClrMemDriverInitialized();
         (void)MemDriver_DeinitSync(V_NULL);
      }
      /* Download of flash driver requested */
      FblDiagClrTransferTypeFlash();
      pSegmentInfo->type = kFblMemType_RAM;

      /* Verification routine of first logical block used for flash driver */
      *pDownloadBlockNr = 0u;
   }
   else
   {
      /* Check if erased logical block and requested address match */
      if (    (FblGetBlockNrFromAddress(pSegmentInfo->logicalAddress, pSegmentInfo->logicalLength, pDownloadBlockNr) != kFblOk)
            || (*pDownloadBlockNr != downloadBlockDescriptor.blockNr))
      {
            /* Requested download block does not match erased block */
            DiagNRCRequestOutOfRange();
            result = kFblFailed;
      }
      else
      {
         /* Download of data requested */
         FblDiagSetTransferTypeFlash();

         /* Set target memory type for download */
         pSegmentInfo->type = kFblMemType_ROM;
      }
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagCheckFlashDriverDownload
 **********************************************************************************************************************/
/*! \brief         Check if the download request matches the flash driver buffer
 *  \param[in,out] pSegmentInfo FblLib_Mem segment information for this download
 *  \return        kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagCheckFlashDriverDownload(V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo)
{
   tFblResult result;

   /* Initialize target address. This address is used to store the downloaded data */
#if defined( FLASHCODE_RELOCATABLE ) || \
    defined( FLASH_DRIVER_RELOCATABLE )
   pSegmentInfo->targetAddress = FBL_DIAG_FLASH_CODE_BASE_ADDR; /* PRQA S 0306 */ /* MD_FblDiag_0306 */
#else
   pSegmentInfo->targetAddress = pSegmentInfo->logicalAddress;
#endif /* FLASHCODE_RELOCATABLE || FLASH_DRIVER_RELOCATABLE */

   /* Store download length */
   pSegmentInfo->targetLength = pSegmentInfo->logicalLength;

   /* Check if flash driver fits into flash driver buffer */ /* PRQA S 0306 1 */ /* MD_FblDiag_0306 */
   if (FblCheckRangeContained(pSegmentInfo->targetAddress, pSegmentInfo->targetLength, FBL_DIAG_FLASH_CODE_BASE_ADDR, FLASH_SIZE) == kFblOk)
   {
      /* Initialize segment counter */
      FblDiagSegmentInit();
      /* Copy data and address of first segment to initialize downloadHeader */
      (void)FblDiagSegmentNext();

      result = kFblOk;
   }
   else
   {
      FblErrStatSetError(FBL_ERR_FLASHCODE_EXCEEDS_MEMORY);
      DiagNRCRequestOutOfRange();

      result = kFblFailed;
   }

   return result;
}
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */

/***********************************************************************************************************************
 *  FblDiagCheckFlashMemoryDownload
 **********************************************************************************************************************/
/*! \brief         Check if the download request matches the erased logical block
 *  \param[in,out] pSegmentInfo FblLib_Mem segment information for this download
 *  \return        kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagCheckFlashMemoryDownload(V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo)
{
   FL_SegmentInfoType * segment;

   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

   /* Memory driver has to be available */
   if (!FblDiagGetMemDriverInitialized())
   {
      DiagNRCUploadDownloadNotAccepted();
      result = kFblFailed;
   }
   /* Memory has to be erased in case of first segment in logical block */
   else if ((segmentCount == 0u) && (!FblDiagGetEraseSucceeded()))
   {
      DiagNRCUploadDownloadNotAccepted();
      result = kFblFailed;
   }
   /* Verify addresses (address must be in the area described by downloadBlockDescriptor)
      * this check requires EraseRoutine, because this initializes downloadBlockDescriptor */
   else if (   (FblCheckRangeContained(pSegmentInfo->logicalAddress,
                                       pSegmentInfo->logicalLength,
                                       downloadBlockDescriptor.blockStartAddress,
                                       downloadBlockDescriptor.blockLength) != kFblOk)
            || ( transferRemainder == 0u)
            )
   {
      FblErrStatSetError(FBL_ERR_LBT_ADDR_MISMATCH);
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
   else
   {
      if (FblDiagSegmentNext() == kSwmOutOfRange)
      {
         FblErrStatSetError(FBL_ERR_TOO_MANY_SEGMENTS_IN_MODULE);
         DiagNRCRequestOutOfRange();
         result = kFblFailed;
      }
      else
      {
         /* Store address/length information of RequestDownload for flash operation */
         pSegmentInfo->targetAddress = pSegmentInfo->logicalAddress;
         pSegmentInfo->targetLength = pSegmentInfo->logicalLength;

         /* Check if requested segment overlaps with previously downloaded data */
         if (segmentCount > 1u)
         {
            /* Download second segment */
            segment = &verifyParam.segmentList.segmentInfo[segmentCount - 2u];

            if (   (pSegmentInfo->targetAddress <= segment->transferredAddress)
                || ((pSegmentInfo->targetAddress - segment->transferredAddress) < segment->length)
               )
            {
               /* New segment is not in order or overlaps previous one */
               DiagNRCRequestOutOfRange();
               result = kFblFailed;
            }
         }
      }
   }

   return result;
}  /* PRQA S 6080 */ /* MD_MSR_STMIF */

/***********************************************************************************************************************
 *  FblDiagPrepareFirstDownloadSegment
 **********************************************************************************************************************/
/*! \brief         Add block structure to FblLib_Mem data
 *  \param[out]    pBlockInfo Logical block information data provided to FblLib_Mem
 *  \param[in]     pSegmentInfo Segment information of requested download
 *  \param[in]     tempBlockNr Logical index of verification routine for this download
 *  \return        kFblOk/kFblFailed
 **********************************************************************************************************************/
/* PRQA S 3673 3 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagPrepareFirstDownloadSegment(V_MEMRAM1 tFblMemBlockInfo V_MEMRAM2 V_MEMRAM3 * pBlockInfo,
                                                     V_MEMRAM1 tFblMemSegmentInfo V_MEMRAM2 V_MEMRAM3 * pSegmentInfo,
                                                     vuint8 tempBlockNr)
{
   tFblResult result;
   tFblDiagNrc libMemResult;

   /* Initialize variables */
   result = kFblOk;

   /* Info: Flash driver uses verification routines of first logical block */
#if defined( FBL_MEM_ENABLE_VERIFY_OUTPUT )
   pBlockInfo->verifyRoutineOutput.function = (tFblMemVerifyFctOutput)(FblLogicalBlockTable.logicalBlock[tempBlockNr].verifyOutput); /* PRQA S 0313 */ /* MD_FblDiag_0313 */
   pBlockInfo->verifyRoutineOutput.param = (V_MEMRAM1 tFblMemVerifyParamOutput V_MEMRAM2 V_MEMRAM3 *)&verifyParam;
#endif /* FBL_MEM_ENABLE_VERIFY_OUTPUT */

#if defined( FBL_MEM_ENABLE_VERIFY_PIPELINED )
   /* Configure pipelined verification */
   pBlockInfo->verifyRoutinePipe.function = (tFblMemVerifyFctInput)(FblLogicalBlockTable.logicalBlock[tempBlockNr].verifyPipelined); /* PRQA S 0313 */ /* MD_FblDiag_0313 */
   pBlockInfo->verifyRoutinePipe.param = (V_MEMRAM1 tFblMemVerifyParamInput V_MEMRAM2 V_MEMRAM3 *)&pipeSigParam;
   /* Force use of internal workspace */
   pipeSigParam.currentHash.sigResultBuffer = (SecM_ResultBufferType)V_NULL;  /* PRQA S 0306 */ /* MD_FblDiag_0306 */
   pipeSigParam.currentHash.length = 0u;
   pipeSigParam.currentDataLength = &pipeSigDataLength;
#endif /* FBL_MEM_ENABLE_VERIFY_PIPELINED */

   pBlockInfo->segmentList = &verifyParam.segmentList;
   pBlockInfo->maxSegments = SWM_DATA_MAX_NOAR;
   verifyParam.segmentList.segmentInfo = downloadSegments;

   /* Prepare block data structure */
   if( FblDiagGetTransferTypeFlash())
   {
      pBlockInfo->targetAddress = downloadBlockDescriptor.blockStartAddress;
      pBlockInfo->targetLength = downloadBlockDescriptor.blockLength;
      pBlockInfo->logicalAddress = downloadBlockDescriptor.blockStartAddress;
      pBlockInfo->logicalLength = downloadBlockDescriptor.blockLength;

      pBlockInfo->readFct = (tFblMemVerifyReadFct)FblReadProm;
   }
#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
   else
   {
      pBlockInfo->targetAddress = pSegmentInfo->targetAddress;
      pBlockInfo->targetLength = pSegmentInfo->targetLength;
      pBlockInfo->logicalAddress = pSegmentInfo->logicalAddress;
      pBlockInfo->logicalLength = pSegmentInfo->logicalLength;

      pBlockInfo->readFct = (tFblMemVerifyReadFct)FblReadRam;
   }
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */

   /* Add block to FblLib_Mem state machine */
   libMemResult = FblMemRemapStatus(FblMemBlockStartIndication(pBlockInfo));
   /* Check if FblLib_Mem reported an error */
   if (libMemResult != kDiagErrorNone)
   {
      FblDiagSetError(libMemResult);
      result = kFblFailed;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagSessionControlParamInit
 **********************************************************************************************************************/
/*! \brief         Initialization of session control request response.
 *  \param[in,out] pbDiagData Pointer to data in the diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Data length (without SID).
 **********************************************************************************************************************/
static void FblDiagSessionControlParamInit(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   /* Reset internal states */
   FblDiagClrPreconditionsChecked();
   FblDiagClrSecurityKeyAllowed();
   FblDiagClrSecurityUnlock();
   FblDiagClrFingerprintValid();
   FblDiagClrEraseSucceeded();
   FblDiagClrTransferDataAllowed();
   FblDiagClrTransferDataSucceeded();
   FblDiagClrChecksumAllowed();

   /* Reset variables */
   transferRemainder = 0u;
   /* Initialize flash driver download handling */
   FblDiagClrTransferTypeFlash();

   /* Initialize segment counter */
   FblDiagSegmentInit();

   /* Initialize security module */
   (void)ApplFblSecurityInit();

   /* Prepare diagnostic response */
   pbDiagData[kDiagLocFmtSubparam + 1u] = (vuint8)((kDiagSessionTimingP2 >> 8u) & 0xFFu);
   pbDiagData[kDiagLocFmtSubparam + 2u] = (vuint8)(kDiagSessionTimingP2 & 0xFFu);
   pbDiagData[kDiagLocFmtSubparam + 3u] = (vuint8)((kDiagSessionTimingP2Star >> 8u) & 0xFFu);
   pbDiagData[kDiagLocFmtSubparam + 4u] = (vuint8)(kDiagSessionTimingP2Star & 0xFFu);
}

/***********************************************************************************************************************
 *  FblDiagOemInitPowerOn
 **********************************************************************************************************************/
/*! \brief       Initialize module variables
 **********************************************************************************************************************/
void FblDiagOemInitPowerOn(void)
{
   expectedSequenceCnt = 0;
}

/***********************************************************************************************************************
 *  FblDiagProcessServiceNrc
 **********************************************************************************************************************/
/*! \brief       This routine maps service check errors to NRCs
 *  \details     The checks done against the service table can be mapped to NRCs here. This function can be
 *               used as default NRC handler for services without sub-table entry.
 *  \param[in]   serviceErrorMask Error bit mask of table check
 *  \return      kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagProcessServiceNrc(vuint32 serviceErrorMask)
{
   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

   /* Check if service should be supported functionally and drop support if not */
   if ((serviceErrorMask & kFblDiagServiceNoFuncSupport) != 0u)
   {
      DiagSetNoResponse();
   }
   else
   {
      /* Service supported in active session? */
      if ((serviceErrorMask & kFblDiagServiceSessionFailure) == kFblDiagServiceSessionFailure)
      {
         DiagNRCServiceNotSupportedInActiveSession();
      }
      /* Minimum length check on service level */
      else if ((serviceErrorMask & kFblDiagServiceMinLenFailure) == kFblDiagServiceMinLenFailure)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      /* Check for security access */
      else if ((serviceErrorMask & kFblDiagServiceSecAccessFailure) == kFblDiagServiceSecAccessFailure)
      {
         DiagNRCSecurityAccessDenied();
      }
      /* Pending length checks */
      else if ((serviceErrorMask & kFblDiagServiceLenFailure) == kFblDiagServiceLenFailure)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      else if ((serviceErrorMask & kFblDiagServicePrehandlerFailure) == kFblDiagServicePrehandlerFailure)
      {
         /* Set default NRC if no NRC has been set in callout function. */
         if (DiagGetError() == kDiagErrorNone)
         {
            DiagNRCConditionsNotCorrect();
         }
      }
      /* Remaining, uncovered errors */
      else
      {
         DiagNRCGeneralReject();
      }
   }

   return result;
}  /* PRQA S 6080 */ /* MD_MSR_STMIF */

/***********************************************************************************************************************
 *  FblDiagProcessSubfunctionNrc
 **********************************************************************************************************************/
/*! \brief       This routine maps service check errors to NRCs
 *  \details     The checks done against the service table can be mapped to NRCs here. This function can be
 *               used as default NRC handler for services with sub-functions.
 *  \param[in]   serviceErrorMask Error bit mask of table check
 *  \return      kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagProcessSubfunctionNrc(vuint32 serviceErrorMask)
{
   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

   /* Check if service should be supported functionally and drop support if not */
   if ((serviceErrorMask & (kFblDiagServiceNoFuncSupport | kFblDiagSubNoFuncSupport)) != 0u)
   {
      DiagSetNoResponse();
   }
   else
   {
      /* Service supported in active session? */
      if ((serviceErrorMask & kFblDiagServiceSessionFailure) == kFblDiagServiceSessionFailure)
      {
         DiagNRCServiceNotSupportedInActiveSession();
      }
      /* Minimum length check on service level */
      else if ((serviceErrorMask & kFblDiagServiceMinLenFailure) == kFblDiagServiceMinLenFailure)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      /* Check if sub-function is supported */
      else if ((serviceErrorMask & kFblDiagSubfunctionNotFound) == kFblDiagSubfunctionNotFound)
      {
#if defined( FBL_ENABLE_USERSUBFUNCTION )
         /* Subfunction not found in check
          * Call user handler to manage its own subfunction */
         FblDiagSetRcrRpAllowed();
         ApplDiagUserSubFunction(&DiagBuffer[kDiagFmtSubparam], DiagDataLength);
#else
         DiagNRCSubFunctionNotSupported();
#endif /* FBL_ENABLE_USERSUBFUNCTION */
      }
      /* Check if sub-parameter is supported */
      else if ((serviceErrorMask & kFblDiagSubparamNotFound) == kFblDiagSubparamNotFound)
      {
         DiagNRCRequestOutOfRange();
      }
      /* Check for security access (service level and sub-function level) */
      else if ((serviceErrorMask & (kFblDiagServiceSecAccessFailure | kFblDiagSubfuncSecAccessFailure | kFblDiagSubparamSecAccessFailure)) != 0u)
      {
         DiagNRCSecurityAccessDenied();
      }
      /* Pending length checks */
      else if ((serviceErrorMask & (kFblDiagServiceLenFailure | kFblDiagSubMinLenFailure | kFblDiagSubLenFailure)) != 0u)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      /* Subfunction session check */
      else if ((serviceErrorMask & (kFblDiagSubfuncSessionFailure | kFblDiagSubparamSessionFailure)) != 0u)
      {
         DiagNRCSubfunctionNotSupportedInActiveSession();
      }
      else if ((serviceErrorMask & kFblDiagServicePrehandlerFailure) == kFblDiagServicePrehandlerFailure)
      {
         /* Set default NRC if no NRC has been set in callout function. */
         if (DiagGetError() == kDiagErrorNone)
         {
            DiagNRCConditionsNotCorrect();
         }
      }
      /* Remaining, uncovered errors */
      else
      {
         DiagNRCGeneralReject();
      }
   }

   return result;
}  /* PRQA S 6030, 6080 */ /* MD_MSR_STCYC, MD_MSR_STMIF */

/***********************************************************************************************************************
 *  FblDiagProcessRoutineNrc
 **********************************************************************************************************************/
/*! \brief       This routine maps service check errors to NRCs
 *  \details     The checks done against the service table can be mapped to NRCs here. This function can
 *               be used as a NRC hanler for the routine control service.
 *  \param[in]   serviceErrorMask Error bit mask of table check
 *  \return      kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagProcessRoutineNrc(vuint32 serviceErrorMask)
{
   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

   /* Check if service should be supported functionally and drop support if not */
   if ((serviceErrorMask & (kFblDiagServiceNoFuncSupport | kFblDiagSubNoFuncSupport)) != 0u)
   {
      DiagSetNoResponse();
   }
   else
   {
      /* Service supported in active session? */
      if ((serviceErrorMask & kFblDiagServiceSessionFailure) == kFblDiagServiceSessionFailure)
      {
         DiagNRCServiceNotSupportedInActiveSession();
      }
      /* Minimum length check on service level */
      else if ((serviceErrorMask & kFblDiagServiceMinLenFailure) == kFblDiagServiceMinLenFailure)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      /* Check if sub-parameter is supported */
      else if ((serviceErrorMask & kFblDiagSubparamNotFound) == kFblDiagSubparamNotFound)
      {
#if defined( FBL_ENABLE_USERROUTINE )
         /* Subfunction not found in check
          * Call user handler to manage its own subfunction */
         FblDiagSetRcrRpAllowed();
         ApplDiagUserRoutine(&DiagBuffer[kDiagFmtSubparam], DiagDataLength);
#else
         DiagNRCRequestOutOfRange();
#endif /* FBL_ENABLE_USERSUBFUNCTION */
      }
      /* Check if sub-function is supported */
      else if ((serviceErrorMask & kFblDiagSubfunctionNotFound) == kFblDiagSubfunctionNotFound)
      {
#if defined( FBL_ENABLE_USERSUBFUNCTION )
         /* Subfunction not found in check
          * Call user handler to manage its own subfunction */
         FblDiagSetRcrRpAllowed();
         ApplDiagUserSubFunction(&DiagBuffer[kDiagFmtSubparam], DiagDataLength);
#else
         DiagNRCSubFunctionNotSupported();
#endif /* FBL_ENABLE_USERSUBFUNCTION */
      }
      /* Check for security access (service level and sub-function level) */
      else if ((serviceErrorMask & (kFblDiagServiceSecAccessFailure | kFblDiagSubfuncSecAccessFailure | kFblDiagSubparamSecAccessFailure)) != 0u)
      {
         DiagNRCSecurityAccessDenied();
      }
      /* Pending length checks */
      else if ((serviceErrorMask & (kFblDiagServiceLenFailure | kFblDiagSubMinLenFailure | kFblDiagSubLenFailure)) != 0u)
      {
         DiagNRCIncorrectMessageLengthOrInvalidFormat();
      }
      /* Subfunction session check */
      else if ((serviceErrorMask & (kFblDiagSubfuncSessionFailure | kFblDiagSubparamSessionFailure)) != 0u)
      {
         DiagNRCSubfunctionNotSupportedInActiveSession();
      }
      else if ((serviceErrorMask & kFblDiagServicePrehandlerFailure) == kFblDiagServicePrehandlerFailure)
      {
         /* Set default NRC if no NRC has been set in callout function. */
         if (DiagGetError() == kDiagErrorNone)
         {
            DiagNRCConditionsNotCorrect();
         }
      }
      /* Remaining, uncovered errors */
      else
      {
         DiagNRCGeneralReject();
      }
   }

   return result;
}  /* PRQA S 6030, 6080 */ /* MD_MSR_STCYC, MD_MSR_STMIF */

/***********************************************************************************************************************
 *  Diagnostic main handler service functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagDefaultSessionMainHandler
 **********************************************************************************************************************/
/*! \brief         Default session request service handler.
 *  \param[in,out] pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Data length (without SID).
 *  \return        Result: Always kFblOk.
 **********************************************************************************************************************/
static tFblResult FblDiagDefaultSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
   vuint8 responseFlag;
#endif /* FBL_ENABLE_REPONSE_AFTER_RESET */

   if (FblDiagGetProgrammingSession())
   {
      /* Request bootloader reset */
      FblDiagSetWaitEcuReset();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
      /* Send response pending before reset */
      DiagExRCRResponsePending(kForceSendResponsePending);

      /* Set flag for response after reset */
      responseFlag = RESET_RESPONSE_SDS_REQUIRED;
      (void)ApplFblWriteResetResponseFlag(&responseFlag);   /* PRQA S 3425 */ /* MD_FblDiag_3425 */
      /* Save target address */
      if (FblCwSaveResponseAddress() != kFblOk) /* PRQA S 3325 */ /* MD_MSR_14.1 */
      {  /* PRQA S 3201 */ /* MD_MSR_14.1 */
         /* If clause is necessary here to process return value or macro. However,
          * there is currently no strategy to handle this error. */
      }
#else
      /* Prepare response and reset states */
      FblDiagSessionControlParamInit(pbDiagData, diagReqDataLen);
      /* Send response */
      DiagProcessingDone(kDiagRslDiagnosticSessionControl);
#endif /* FBL_ENABLE_RESPONSE_AFTER_RESET */
      FblDiagSetEcuResetFctFinished();
   }
   else
   {
      /* Change to default session without issuing a reset */
      FblDiagSetDefaultSession();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

      /* Prepare response and reset states */
      FblDiagSessionControlParamInit(pbDiagData, diagReqDataLen);
      /* Send response */
      DiagProcessingDone(kDiagRslDiagnosticSessionControl);
   }

   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagExtendedSessionMainHandler
 **********************************************************************************************************************/
/*! \brief         Extended session request service handler.
 *  \param[in,out] pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Data length (without SID).
 *  \return        Result: Always kFblOk.
 **********************************************************************************************************************/
static tFblResult FblDiagExtendedSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   /* Change to extended session */
   FblDiagSetExtendedSession();  /* PRQA S 3109 */ /* MD_FblDiag_3109 */

   /* Prepare response and reset states */
   FblDiagSessionControlParamInit(pbDiagData, diagReqDataLen);
   /* Send response */
   DiagProcessingDone(kDiagRslDiagnosticSessionControl);

   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagProgrammingSessionMainHandler
 **********************************************************************************************************************/
/*! \brief         Programming session request service handler.
 *  \param[in,out] pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Data length (without SID).
 *  \return        Result: kFblOk if reprogramming conditions are fulfilled, kFblFailed if not.
 **********************************************************************************************************************/
static tFblResult FblDiagProgrammingSessionMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

   /* Check of programming preconditions done? */
#if defined( FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS )
   if ((!FblDiagGetPreconditionsChecked()) && (!FblMainGetStartFromAppl()))
#else
   if (ApplFblCheckProgConditions() != kFblOk)
#endif /* FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS */
   {
      /* Check of programming preconditions failed */
      FblErrStatSetError(FBL_ERR_FLASH_PROG_CONDITIONS);
      DiagNRCConditionsNotCorrect();
      result = kFblFailed;
   }
   else
   {
      /* Check of programming preconditions succeeded - switch to programming session */
      /*###########################################################################/
/# Modification to use Bootloader as an application #########################/
/# Jump from application back to bootloader on prog session request #########/
/# Set EEPROM Flags and restart the ECU #####################################/
/###########################################################################*/
#if defined( FBL_ENABLE_FBL_START )
      extern vuint8 fblStartReset;
#else
      vuint8 progReqFlag;
#endif /* FBL_ENABLE_FBL_START */                         /*vcshwel*/
      /* Request for reset / FBL shutdown in the confirmation of RCR-RP message */
      FblDiagSetWaitEcuReset();                            /*vcshwel*/
      /* Send response pending */
      DiagExRCRResponsePending(kForceSendResponsePending);      /*vcshwel*/
#if defined( FBL_ENABLE_FBL_START )
      fblStartReset = 1;
#else
      /* Set reprogramming flag */
      progReqFlag = kEepFblReprogram;
      (void) ApplFblNvWriteProgReqFlag(&progReqFlag);
#endif /* FBL_ENABLE_FBL_START */                               /*vcshwel*/
      FblDiagSetEcuResetFctFinished();
/*###########################################################################/
/# Modification to use Bootloader as an application END #####################/
/###########################################################################*/
      result = kFblOk;

      
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagEcuResetMainHandler
 **********************************************************************************************************************/
/*! \brief         Function manages the EcuReset service request
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        Result: Always kFblOk.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagEcuResetMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
   vuint8 responseFlag;
#endif /* FBL_ENABLE_REPONSE_AFTER_RESET */

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)diagReqDataLen;
   (void)pbDiagData;
#endif

   /* Request bootloader reset */
   FblDiagSetWaitEcuReset();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
   /* Send response pending before reset */
   DiagExRCRResponsePending(kForceSendResponsePending);

   /* Set flag for response after reset */
   responseFlag = RESET_RESPONSE_ECURESET_REQUIRED;
   (void)ApplFblWriteResetResponseFlag(&responseFlag);   /* PRQA S 3425 */ /* MD_FblDiag_3425 */
   /* Save target address */
   if (FblCwSaveResponseAddress() != kFblOk) /* PRQA S 3325 */ /* MD_MSR_14.1 */
   {  /* PRQA S 3201 */ /* MD_MSR_14.1 */
      /* If clause is necessary here to process return value or macro. However,
       * there is currently no strategy to handle this error. */
   }
#else
   /* Send response */
   DiagProcessingDone(kDiagRslEcuReset);
#endif /* FBL_ENABLE_RESPONSE_AFTER_RESET */
   FblDiagSetEcuResetFctFinished();

   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagSecAccessSeedMainHandler
 **********************************************************************************************************************/
/*! \brief         Request Seed request main handler.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagSecAccessSeedMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   /* Initialize variables */
   result = kFblOk;

#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
   /* Check if security level is locked (delay counter is active) */
   if (GetSecurityAccessDelay() > 0u)
   {
      DiagNRCRequiredTimeDelayNotExpired();
      result = kFblFailed;
   }
#endif /* FBL_ENABLE_SEC_ACCESS_DELAY */

   if (result == kFblOk)
   {
      /* Check if security level is already active */
      if (FblDiagGetSecurityUnlock())
      {
         vuintx i;

         /* ECU is already unlocked, send zero seed */
         for (i = 0; i < kSecSeedLength; i++)
         {
            pbDiagData[kDiagLocFmtSeedKeyStart + i] = 0x00u;
         }
      }
      else
      {
         /* Generate seed by application function */
         if (ApplFblSecuritySeed() == kFblOk)
         {
            /* Accept security key next */
            FblDiagSetSecurityKeyAllowed();
         }
         else
         {
            /* Error occurred during seed computation */
            FblErrStatSetError(FBL_ERR_SEED_GENERATION_FAILED);
            DiagNRCConditionsNotCorrect();
            result = kFblFailed;
         }
      }
   }

   /* Send positive response if seed was computed correctly */
   if (result == kFblOk)
   {
      DiagProcessingDone(kDiagRslSecurityAccessSeed);
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagSecAccessKeyMainHandler
 **********************************************************************************************************************/
/*! \brief         Send Key request main handler.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagSecAccessKeyMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)diagReqDataLen;
   (void)pbDiagData;
#endif

   /* Check preconditions for key processing */
   if (    (!FblDiagGetSecurityKeyAllowed())
#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
        || (GetSecurityAccessDelay() > 0u)
#endif /* FBL_EANBLE_SECURITY_ACCESS_DELAY */
      )
   {
      DiagNRCRequestSequenceError();
      result = kFblFailed;
   }
   else
   {
      /* Send response pending */
      DiagExRCRResponsePending(kForceSendResponsePending);

      /* No second key allowed for this seed */
      FblDiagClrSecurityKeyAllowed();

      /* Check security access key in application function */
      if (ApplFblSecurityKey() == kFblOk)
      {
         /* Security access successful */
         FblDiagSetSecurityUnlock();
         result = kFblOk;
#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
         /* Reset invalid counter */
         secSendKeyInvalid = 0u;
         /* Reset flags in non-volatile memory */
         (void)ApplFblClrSecAccessDelayFlag();
         (void)ApplFblWriteSecAccessInvalidCount(&secSendKeyInvalid);
#endif /* FBL_ENABLE_SEC_ACCESS_DELAY */
      }
      else
      {
         result = kFblFailed;
#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
         /* Read invalid access counter */
         if (ApplFblReadSecAccessInvalidCount(&secSendKeyInvalid) != kFblOk)
         {
            /* If read failed set default value */
            secSendKeyInvalid = (kSecMaxInvalidKeys - 1u);
         }

         secSendKeyInvalid++;

         if (secSendKeyInvalid >= kSecMaxInvalidKeys)
         {
            /* Too many invalid security attempts, enable security access delay */
            SetSecurityAccessDelay();
            (void)ApplFblSetSecAccessDelayFlag();

            DiagNRCExceedNumberOfAttempts();
         }
         else
         {
            /* Write counter to non-volatile memory */
            (void)ApplFblWriteSecAccessInvalidCount(&secSendKeyInvalid);

            DiagNRCInvalidKey();
         }
#else
         DiagNRCInvalidKey();
#endif /* FBL_ENABLE_SEC_ACCESS_DELAY */
      }
   }

   if (result == kFblOk)
   {
      DiagProcessingDone(kDiagRslSecurityAccessKey);
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagCommCtrlMainHandler
 **********************************************************************************************************************/
/*! \brief         Communication Control request main handler.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagCommCtrlMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   /* Check communication type (Bits 1-7 must be 0) */
   if (pbDiagData[kDiagLocFmtSubparam + 0x01u] != kDiagSubNormalCommunication)
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
   else
   {
      /* Simply transmit a positive response message with subfunction parameter */
      DiagProcessingDone(kDiagRslCommunicationControl);
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagDataByIdLengthCheck
 **********************************************************************************************************************/
/*! \brief         Read/WriteDataByIdentifier service dynamic length check.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Length of data (without SID).
 *  \return        kFblOk: Length check OK; kFblFailed: Length check failed
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagDataByIdLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)pbDiagData;
   (void)diagReqDataLen;
#endif

   /* Nothing to do here. Length check should be done in corresponding user callback function. */
   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagWriteDataByIdMainHandler
 **********************************************************************************************************************/
/*! \brief         WriteDataByIdentifier service, handling of fingerprint and identification data.
 *  \pre           Request only accepted after successful security access procedure.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID).
 *  \param[in]     diagReqDataLen Length of data (without SID).
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagWriteDataByIdMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

   /* Initialize variables */
   result = kFblOk;

   /* Callback function for WDBI */
   if (ApplFblWriteDataByIdentifier(pbDiagData, diagReqDataLen) == kDiagReturnValidationOk)
   {
      /* if fingerprint is completely written, set valid */
      FblDiagSetFingerprintValid();
   }

   if (DiagGetError() != kDiagErrorNone)
   {
      /* NRC already set - do nothing */
      result = kFblFailed;
   }
   else
   {
      /* Send positive response */
      DiagProcessingDone(kDiagRslWriteDataByIdentifier);
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRCStartCsumMainHandler
 **********************************************************************************************************************/
/*! \brief         Routine Control - Check Memory.
 *  \pre           Data to be checked has been downloaded to the corresponding memory.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagRCStartCsumMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblDiagNrc serviceNrc;
   vuint8 checkResult;
   tFblResult result;

   /* Initialize variables */
   serviceNrc = kDiagErrorNone;
   checkResult = kDiagCheckVerificationFailed;

   /* Check state flags */
   if (    (!FblDiagGetFingerprintValid())
        || (!FblDiagGetTransferDataSucceeded())
        || (!FblDiagGetChecksumAllowed())
      )
   {
      serviceNrc = kDiagNrcConditionsNotCorrect;
   }

   if (serviceNrc == kDiagErrorNone)
   {

      /* Invoke LibMem block end indication */
      serviceNrc = FblMemRemapStatus(FblMemBlockEndIndication());
   }

   if (serviceNrc == kDiagErrorNone)
   {
      /* Watchdog and response pending handling */
      (void)FblRealTimeSupport();

      /* Do verification */
#if defined( FBL_MEM_ENABLE_VERIFY_OUTPUT )
      (void)SecM_InitVerification(V_NULL);
#endif /* FBL_MEM_ENABLE_VERIFY_OUTPUT */
      checkResult = FblDiagDownloadCheck(pbDiagData, diagReqDataLen);
#if defined( FBL_MEM_ENABLE_VERIFY_OUTPUT )
      (void)SecM_DeinitVerification(V_NULL);
#endif /* FBL_MEM_ENABLE_VERIFY_OUTPUT */
   }
   else
   {
      /* Prepare NRC possibly set before */
      FblDiagSetError(serviceNrc);
   }

   if (DiagGetError() != kDiagErrorNone)
   {
      /* NRC already set - do nothing */
      result = kFblFailed;
   }
   else
   {
      /* No NRC set - submit positive response */
      pbDiagData[kDiagLocFmtRoutineStatus] = checkResult;
      DiagProcessingDone(kDiagRslRoutineControlCheckRoutine);
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRCStartEraseLengthCheck
 **********************************************************************************************************************/
/*! \brief         Routine Control - Erase Memory length check
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: Length of erase request is OK; kFblFailed: Length check failed
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagRCStartEraseLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

   vuint8 addrFormat;
   vuint8 lengthFormat;

   /* Get length and address format from message */
   lengthFormat = (vuint8)((pbDiagData[kDiagLocFmtRoutineAlfi] & 0xF0u ) >> 4u);
   addrFormat   = (vuint8)(pbDiagData[kDiagLocFmtRoutineAlfi] & 0x0Fu);

   /* Check length of request against calculated lengt */
   if( diagReqDataLen != (kDiagRqlRoutineControlEraseRoutine + lengthFormat + addrFormat))
   {
      result = kFblFailed;
   }
   else
   {
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRCStartEraseMainHandler
 **********************************************************************************************************************/
/*! \brief         Routine Control - Erase Memory.
 *  \pre           Fingerprint is available, memory driver initialized.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagRCStartEraseMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{


    tFblResult result;


    DiagNRCConditionsNotCorrect();
    result = kFblFailed;


   return result;
}  /* PRQA S 6010, 6030, 6050 */ /* MD_MSR_STPTH, MD_MSR_STCYC, MD_MSR_STCAL */

#if defined( FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS )
/***********************************************************************************************************************
 *  FblDiagRCStartCheckProgPreCondMainHandler
 **********************************************************************************************************************/
/*! \brief         Check Programming Preconditions service function
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagRCStartCheckProgPreCondMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
# if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)pbDiagData;
   (void)diagReqDataLen;
# endif

   /* Clear status variables */
   FblDiagClrFingerprintValid();

   /* Check for preconditions - response should be prepared in callback function */
   if (ApplFblCheckProgConditions() == kFblOk)
   {
      FblDiagSetPreconditionsChecked();
   }
   else
   {
      FblDiagClrPreconditionsChecked();
   }

   /* DiagProcessingDone is called from callback function */
   return kFblOk;
}
#endif /* FBL_DIAG_ENABLE_CHECK_PROGRAMMING_PRECONDITIONS */

#if defined( FBL_ENABLE_STAY_IN_BOOT )
/***********************************************************************************************************************
 *  FblDiagRCStartForceBootModeMainHandler
 **********************************************************************************************************************/
/*! \brief         Check for Force Boot Mode message
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagRCStartForceBootModeMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)pbDiagData;
   (void)diagReqDataLen;
#endif

   /* Nothing to do here - just send positive response */
   DiagProcessingDone(kDiagRslRoutineControlStayInBoot);

   return kFblOk;
}
#endif /* FBL_EANBLE_STAY_IN_BOOT */

/***********************************************************************************************************************
 *  FblDiagRCStartCheckProgDepMainHandler
 **********************************************************************************************************************/
/*! \brief         Check program dependencies
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagRCStartCheckProgDepMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   if ((!FblDiagGetFingerprintValid()) || (!FblDiagGetMemDriverInitialized()))
   {
      DiagNRCConditionsNotCorrect();
      result = kFblFailed;
   }
   else
   {
      /* Call function to check programming dependencies */
      pbDiagData[kDiagLocFmtRoutineStatus] = ApplFblCheckProgDependencies();
      DiagProcessingDone(kDiagRslRoutineControlCheckRoutine);
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRequestDownloadLengthCheck
 **********************************************************************************************************************/
/*! \brief         Request download length check
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: Length of check request is OK; kFblFailed: Length check failed
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagRequestDownloadLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

   vuint8 addrFormat;
   vuint8 lengthFormat;

   /* Get length and address format from message */
   lengthFormat = (vuint8)((pbDiagData[kDiagLocFmtFormatOffset] & 0xF0u) >> 4u);
   addrFormat   = (vuint8)(pbDiagData[kDiagLocFmtFormatOffset] & 0x0Fu);

   /* Check length of request against calculated length */
   if(diagReqDataLen != (kDiagRqlRequestDownload + lengthFormat + addrFormat))
   {
      result = kFblFailed;
   }
   else
   {
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRequestDownloadMainHandler
 **********************************************************************************************************************/
/*! \brief         Request download service function
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
static tFblResult FblDiagRequestDownloadMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblMemBlockInfo blockInfo;
   tFblMemSegmentInfo segmentInfoLocal;
   tFblResult result;
   tFblDiagNrc libMemResult;
   vuint8 verifyBlockNr;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   /* Initialize variables */
   segmentInfoLocal.logicalAddress = 0u;
   segmentInfoLocal.logicalLength = 0u;
   verifyBlockNr = 0u;

   /* Verify all programming preconditions are fulfilled. Get address and length (logical) from request */
   result = FblDiagCheckRequestDownloadPreconditions(pbDiagData, &segmentInfoLocal);

#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
   /* Flash Driver download? */
   if (result == kFblOk)
   {
      result = FblDiagCheckForFlashDriverDownload(&verifyBlockNr, &segmentInfoLocal);
   }
#else
   FblDiagSetTransferTypeFlash();
   segmentInfoLocal.type = kFblMemType_ROM;
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */

   if (result == kFblOk)
   {
      /* Get length from request */
      transferRemainder = segmentInfoLocal.logicalLength;

      /* Initialize error status address */
      FblErrStatSetAddress(segmentInfoLocal.logicalAddress);

      /* Init expected sequence counter for TransferData */
      expectedSequenceCnt = kDiagInitSequenceNum;
      /* Init current sequence counter for TransferData */
      currentSequenceCnt = kDiagInitSequenceNum;

      /* Prepare download of application or flash driver. Target addresses are valid after these calls */
      if (FblDiagGetTransferTypeFlash())
      {
         result = FblDiagCheckFlashMemoryDownload(&segmentInfoLocal);
      }
#if defined( FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD )
      else
      {
         result = FblDiagCheckFlashDriverDownload(&segmentInfoLocal);
      }
#endif /* FBL_DIAG_ENABLE_FLASHDRV_DOWNLOAD */
   }

   /* First download segment */
   if ((result == kFblOk) && (segmentCount <= 1u))
   {
      result = FblDiagPrepareFirstDownloadSegment(&blockInfo, &segmentInfoLocal, verifyBlockNr);
      pbDiagData = FblDiagMemGetActiveBuffer(); /* PRQA S 3198 */ /* MD_FblDiag_319x */
   }

   /* Add segment data to FblLib_Mem */
   if (result == kFblOk)
   {
      /* Indicate start of new segment */
      libMemResult = FblMemRemapStatus(FblMemSegmentStartIndication(&segmentInfoLocal));
      pbDiagData = FblDiagMemGetActiveBuffer();
      if (libMemResult == kDiagErrorNone)
      {
         /* Init expected sequence counter for TransferData */
         expectedSequenceCnt = kDiagInitSequenceNum;
         /* Init current sequence counter for TransferData */
         currentSequenceCnt = kDiagInitSequenceNum;

         /* Now allow reception of TransferData */
         FblDiagSetTransferDataAllowed();
         FblDiagClrTransferDataSucceeded();
         FblDiagClrChecksumAllowed();

         /* Prepare response */
         pbDiagData[kDiagLocFmtSubparam] = (FBL_DIAG_RESPONSE_BUFFER_LFI << 4u);
         FblMemSetInteger(FBL_DIAG_RESPONSE_BUFFER_LFI, FBL_DIAG_RESPONSE_BUFFER_SIZE, &pbDiagData[kDiagLocFmtSubparam + 1u]);

         DiagProcessingDone(kDiagRslRequestDownload + FBL_DIAG_RESPONSE_BUFFER_LFI);
      }
      else
      {
         /* Set NRC provided by FblLib_Mem */
         FblDiagSetError(libMemResult);
         result = kFblFailed;
      }
   }

   return result;
}  /* PRQA S 6050 */ /* MD_MSR_STCAL */

/***********************************************************************************************************************
 *  FblDiagTransferDataLengthCheck
 **********************************************************************************************************************/
/*! \brief         Transfer data length check
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: Length of transfer data service is OK; kFblFailed: Length check failed
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagTransferDataLengthCheck(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)pbDiagData;       /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   if ((diagReqDataLen <= (kDiagRqlTransferData)) || (diagReqDataLen > (tTpDataType)(FBL_DIAG_RESPONSE_BUFFER_SIZE - 1u)))
   {
      /* Requested transfer length is larger than indicated data length */
      FblDiagClrTransferDataAllowed();
      result = kFblFailed;
   }
   else
   {
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagTransferDataMainHandler
 **********************************************************************************************************************/
/*! \brief         TransferData service function;
 *  \pre           TransferData must be enabled by RequestDownload service
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagTransferDataMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;
   tFblDiagNrc libMemResult;
   tFblLength transferDataLength;

   if (!FblDiagGetTransferDataAllowed())
   {
      DiagNRCRequestSequenceError();
      result = kFblFailed;
   }
   /* Check if the requested sequence number is expected */
   else if (pbDiagData[kDiagLocFmtSubparam] != expectedSequenceCnt)
   {
      /* Check if sequence number corresponds to a retransmission of the last message */
      if (pbDiagData[kDiagLocFmtSubparam] == currentSequenceCnt)
      {
         /* Repetition of last transferData request
          * Simply send a positive response without loading data to memory */
         DiagProcessingDone(kDiagRslTransferData);
         result = kFblOk;
      }
      else /* Sequence number is not for a retry */
      {
         /* Send a WrongSequenceError */
         DiagNRCWrongBlockSequenceCounter();
         result = kFblFailed;
      }
   }
   else
   {
      /* Length without sequence counter byte */
      transferDataLength = diagReqDataLen - 1u;

      /* Indicate data to FblLib_Mem */
      FblDiagClrEraseSucceeded();
      libMemResult = FblMemRemapStatus(FblMemDataIndication(DiagBuffer, kDiagFmtDataOffset, transferDataLength));

      /* Caution: Depending on configuration, DiagBuffer pointer may change. */
      pbDiagData = FblDiagMemGetActiveBuffer(); /* PRQA S 3199 */ /* MD_FblDiag_319x */
      if (libMemResult == kDiagErrorNone)
      {
         /* Memorize current counter */
         currentSequenceCnt = expectedSequenceCnt;
         /* Sequence counter value of next transferData request
          * Note: We do not rely on an implicit 8-bit caused overflow at 256, which does not happen on certain platforms */
         expectedSequenceCnt = ((expectedSequenceCnt + 1u) & 0xFFu);

         DiagProcessingDone(kDiagRslTransferData);
         result = kFblOk;
      }
      else
      {
         FblDiagSetError(libMemResult);
         FblDiagClrTransferDataAllowed();
         result = kFblFailed;
      }
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagReqTransferExitMainHandler
 **********************************************************************************************************************/
/*! \brief         RequestTransferExit service function
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagReqTransferExitMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;
   tFblDiagNrc libMemResult;
   tFblLength  totalProgramLength;  /* Total number of programmed bytes */

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)pbDiagData;
   (void)diagReqDataLen;
#endif

   /* Do sequence and parameter checks */
   if (!FblDiagGetTransferDataAllowed())
   {
      DiagNRCRequestSequenceError();
      result = kFblFailed;
   }
   else
   {
      FblDiagClrTransferDataAllowed();

      /* Finalize programming of current segment */
      libMemResult = FblMemRemapStatus((FblMemSegmentEndIndication(&totalProgramLength)));
      pbDiagData = FblDiagMemGetActiveBuffer(); /* PRQA S 3199 */ /* MD_FblDiag_319x */

      if (libMemResult == kDiagErrorNone)
      {
         /* RequestTransferExit was successful */
         FblDiagSetTransferDataSucceeded();
         FblDiagSetChecksumAllowed();
         DiagProcessingDone(kDiagRslRequestTransferExit);
         result =  kFblOk;
      }
      else
      {
         /* Set return value of FblLib_Mem as NRC */
         FblDiagSetError(libMemResult);
         result = kFblFailed;
      }
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagTesterPresentMainHandler
 **********************************************************************************************************************/
/*! \brief         FblTesterPresent service function.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagTesterPresentMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */ /* PRQA S 3112 2 */ /* MD_MSR_14.2 */
   (void)pbDiagData;
   (void)diagReqDataLen;
#endif

   DiagProcessingDone(kDiagRslTesterPresent);

   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagControlDTCMainHandler
 **********************************************************************************************************************/
/*! \brief         ControlDTCSetting service function.
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 **********************************************************************************************************************/
/* PRQA S 3673 1 */ /* MD_FblDiag_3673 */
static tFblResult FblDiagControlDTCMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
# if !defined( FBL_DIAG_ENABLE_CONTROLDTC_OPTIONRECORD )
   (void)pbDiagData;       /* PRQA S 3112 */ /* MD_MSR_14.2 */
# endif
   (void)diagReqDataLen;   /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

#if defined( FBL_DIAG_ENABLE_CONTROLDTC_OPTIONRECORD )
   /* Check DTCSettingControlOptionRecord */
   if (   ( pbDiagData[kDiagLocFmtRoutineIdHigh] != 0xFFu) \
       || ( pbDiagData[kDiagLocFmtRoutineIdLow]  != 0xFFu) \
       || ( pbDiagData[kDiagLocFmtRoutineStatus] != 0xFFu)
      )
   {
      DiagNRCRequestOutOfRange();
      result = kFblFailed;
   }
   else
#endif /* FBL_DIAG_ENABLE_CONTROLDTC_OPTIONRECORD */
   {
      /* Simply transmit a positive response message with subfunction parameter */
      DiagProcessingDone(kDiagRslControlDTCSetting);

      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagReadDataByIdMainHandler
 **********************************************************************************************************************/
/*! \brief         ReadDataByIdentifier service function
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 *********************************************************************************************************************/
static tFblResult FblDiagReadDataByIdMainHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   ApplFblReadDataByIdentifier(pbDiagData, diagReqDataLen);

   return kFblOk;
}

/***********************************************************************************************************************
 * Diagnostic pre handler service functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagDefaultPreHandler
 **********************************************************************************************************************/
/*! \brief         Pre-handler function to call ApplFblCheckConditions()
 *  \param[in,out] pbDiagData Pointer to the data in the diagBuffer (without SID)
 *  \param[in]     diagReqDataLen Length of data (without SID)
 *  \return        kFblOk: service processed successfully (goto next state), kFblFailed: Service processing failed.
 *********************************************************************************************************************/
static tFblResult FblDiagDefaultPreHandler(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   return ApplFblCheckConditions(pbDiagData, diagReqDataLen);
}

/***********************************************************************************************************************
 * Diagnostic post handler service functions
 **********************************************************************************************************************/

/* Start section to execute code from RAM */
#define FBLDIAG_RAMCODE_START_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */
/***********************************************************************************************************************
 *  FblDiagDefaultPostHandler
 **********************************************************************************************************************/
/*! \brief       Can be used to switch to next state AFTER successful transmission of the service, e.g. EcuReset.
 *  \param[in]   postParam Parameter indicating service response.
 **********************************************************************************************************************/
void FblDiagDefaultPostHandler( vuint8 postParam )
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   /* Parameters not used: avoid compiler warning */
   (void)postParam;  /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif
   if(FblDiagGetDefaultSession())
   {
      FblCwResetResponseAddress();
   }
}
/* Stop section to execute code from RAM */
#define FBLDIAG_RAMCODE_STOP_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */

