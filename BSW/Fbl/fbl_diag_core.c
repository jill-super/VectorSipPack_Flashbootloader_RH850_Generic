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
 *  03.00.01   2017-07-03  Rie     ESCAN00095702    Encapsulate FblMemResumeIndication
 *  03.01.00   2017-07-17  Ach     ESCAN00095908    Missing sub-table entry is not reported
 *  03.01.01   2017-10-13  Ach     ESCAN00097038    Bootloader is locked to a specific tester after a reset
 *                         CB      ESCAN00097044    No changes
 *  03.02.00   2017-11-22  Ach     ESCAN00097279    Tester present timeout is not started after session change
 *                                 ESCAN00097501    No changes
 *  03.02.01   2018-01-26  Ach     ESCAN00098105    Sub-table is evaluated even if request is too short
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* Common Bootloader includes */
#include "fbl_inc.h"
#include "fbl_diag_core.h"

/***********************************************************************************************************************
 *  VERSION
 **********************************************************************************************************************/
/* Diagnostic core module version check */
#if ((FBLDIAG_14229_COREINT_VERSION != 0x0302u) || \
     (FBLDIAG_14229_COREINT_RELEASE_VERSION != 0x01u))
# error "Error in fbl_diag_core.c: Source and header file are inconsistent!"
#endif
#if (( FBLDIAG_14229_COREINT_VERSION != FBLDIAG_14229_CORE_VERSION ) || \
     ( FBLDIAG_14229_COREINT_RELEASE_VERSION != FBLDIAG_14229_CORE_RELEASE_VERSION ))
# error "Error in fbl_diag_core.h: Header versions are inconsistent."
#endif
#if ((FBLDIAG_14229_CORE_VERSION != _FBLDIAG_14229_CORE_VERSION) || \
     (FBLDIAG_14229_CORE_RELEASE_VERSION != _FBLDIAG_14229_CORE_RELEASE_VERSION))
# error "Error in fbl_diag_core.c: Source and v_ver.h file are inconsistent!"
#endif

/* Interface version compatibility check */
#if defined( FBL_DIAGCORE_API_REFERENCE_VERSION_MAJOR ) && \
    defined( FBL_DIAGCORE_API_REFERENCE_VERSION_MINOR )
#else
#  error "Error in fbl_diag_core.c: Interface version requirements not defined!"
#endif

#if ( FBL_DIAGCORE_API_REFERENCE_VERSION_MAJOR != FBL_DIAGCORE_API_VERSION_MAJOR) || \
    ( FBL_DIAGCORE_API_REFERENCE_VERSION_MINOR > FBL_DIAGCORE_API_VERSION_MINOR)
# error "Error in fbl_diag_core.c: Interface version compatibility check failed!"
#endif

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* Diagnostic states configuration checks */
#if( kFblDiagStateSessionDefault != 0u )  || \
   ( kFblDiagStateSessionExtended != 1u ) || \
   ( kFblDiagStateSessionProgramming != 2u )
# error "Session states must not be changed to different values."
#endif

/** Bit mask to check if response is already configured */
#define kDiagResponseActive               ( kDiagPutNoResponse  | kDiagPutPosResponse | kDiagPutNegResponse )

#if defined( FBL_DIAG_ENABLE_OEM_SESSIONCHECK_NRC )
#else
# define DiagNRCServiceCheckNotSucceeded() DiagNRCServiceNotSupportedInActiveSession() /**< Define configurable session check NRC */
#endif

#if !defined( FBL_DIAG_ENABLE_SPS_EXTENDED_SESSION ) && \
    !defined( FBL_DIAG_DISABLE_SPS_EXTENDED_SESSION )
# define FBL_DIAG_ENABLE_SPS_EXTENDED_SESSION
#endif
#if !defined( kDiagRqlSpsFromAppl )
# define kDiagRqlSpsFromAppl kDiagRqlDiagnosticSessionControl
#endif

/* Specific service responses */
/* RoutineControl status for eraseMemory */
#if !defined( kDiagEraseSucceeded )
# define kDiagEraseSucceeded                                0x00u                /**< Erase memory was started and succeeded */
#endif /* kDiagEraseSucceeded */
#if !defined( kDiagEraseFailed )
# define kDiagEraseFailed                                   0x01u                /**< Failure occurred after routine was started */
#endif /* kDiagEraseFailed */

/* Signature handling */
#if !defined(kSecSigSize)
# define kSecSigSize 0
#endif

/* Gap fill */
/* Buffer size for gap fill function */
#if ( FBL_MAX_SEGMENT_SIZE > 64u )
# define FILL_BUFFER_SIZE FBL_MAX_SEGMENT_SIZE
#else
# define FILL_BUFFER_SIZE 64u
#endif

#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
/* Flags indicate which task is currently running */
# define kFblDiagStateTaskRunning      FBL_BIT0    /**< State task active */
# define kFblDiagTimerTaskRunning      FBL_BIT1    /**< Timer task active */

# define SetDiagStateTaskRunning()     (diagTaskState |= kFblDiagStateTaskRunning)                                /**< Set running state of state task */
# define ClrFblDiagStateTaskRunning()  (diagTaskState &= FblInvert8Bit(kFblDiagStateTaskRunning))                 /**< Clear running state of state task */
# define GetDiagStateTaskRunning()     ((diagTaskState & kFblDiagStateTaskRunning) == kFblDiagStateTaskRunning)   /**< Return running state of state task */

# define SetDiagTimerTaskRunning()     (diagTaskState |= kFblDiagTimerTaskRunning)                                /**< Set running state of timer task */
# define ClrDiagTimerTaskRunning()     (diagTaskState &= FblInvert8Bit(kFblDiagTimerTaskRunning))                 /**< Clear running state of timer task */
# define GetDiagTimerTaskRunning()     ((diagTaskState & kFblDiagTimerTaskRunning) == kFblDiagTimerTaskRunning)   /**< Return running state of timer task */
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */

/***********************************************************************************************************************
 *  TYPEDEFS AND STRUCTURES FOR INTERNAL USE
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/
/** Pointer to the data transfer buffer */
V_MEMRAM0 V_MEMRAM1      vuint8              V_MEMRAM2 V_MEMRAM3 * V_MEMRAM1 V_MEMRAM2 DiagBuffer;
/** Memory segment handle */
V_MEMRAM0 V_MEMRAM1      vsint16             V_MEMRAM2      memSegment;
/** Stores number of received data */
V_MEMRAM0 V_MEMRAM1_NEAR tCwDataLengthType   V_MEMRAM2_NEAR DiagDataLength;
/** Timer value for TesterPresent timeout */
V_MEMRAM0 V_MEMRAM1_NEAR vuint16             V_MEMRAM2_NEAR testerPresentTimeout;
/** Contains the currently processed diag service ID */
V_MEMRAM0 V_MEMRAM1_NEAR vuint8              V_MEMRAM2_NEAR diagServiceCurrent;
/** State bitmap array */
V_MEMRAM0 V_MEMRAM1_NEAR tFblStateBitmap     V_MEMRAM2_NEAR fblDiagStates[FBL_STATE_INDEX(kFblDiagNumberOfStates - 1u) + 1u];

#if defined( FBL_ENABLE_DEBUG_STATUS )
/* Variables for error status reporting */
V_MEMRAM0 V_MEMRAM1      vuint16          V_MEMRAM2      errStatErrorCode;          /**< Error status */
V_MEMRAM0 V_MEMRAM1      vuint16          V_MEMRAM2      errStatFblStates;          /**< FBL state flag */
V_MEMRAM0 V_MEMRAM1      vuint8           V_MEMRAM2      errStatLastServiceId;      /**< Last received service identifier */
V_MEMRAM0 V_MEMRAM1      vuint8           V_MEMRAM2      errStatFlashDrvVersion[3]; /**< Flash driver version information */
V_MEMRAM0 V_MEMRAM1      vuint16          V_MEMRAM2      errStatFlashDrvErrorCode;  /**< Flash driver error code */
V_MEMRAM0 V_MEMRAM1      tBlockDescriptor V_MEMRAM2      errStatDescriptor;         /**< Error status block descriptor */
#endif

#if defined( FBL_ENABLE_STAY_IN_BOOT )
# if defined( FBL_DIAG_ENABLE_CORE_STAYINBOOT )
/** Stay in boot message definition */
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 fblDiagStayInBootMsg[] = FBL_DIAG_STAY_IN_BOOT_ARRAY;
/** Size of stay in boot message */
V_MEMROM0 V_MEMROM1 vuintx V_MEMROM2 kSizeOfStayInBootMsg = ARRAY_SIZE(fblDiagStayInBootMsg);
# endif /* FBL_DIAG_ENABLE_CORE_FBLSTART */
#endif  /* FBL_ENABLE_STAY_IN_BOOT */

/***********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      diagPostParam;
/** Buffer for response pending message */
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      rcrrpBuffer[3];   /* PRQA S 3218 */ /* MD_FblDiag_3218 */

/** Diagnostic response length */
V_MEMRAM0 static V_MEMRAM1 tCwDataLengthType    V_MEMRAM2      diagResLen;

#if defined( FBL_DIAG_ENABLE_OEM_SEGMENTNRGET )
#else
/* FblMemSegmentNrGet caching */
V_MEMRAM0 static V_MEMRAM1 vsint16              V_MEMRAM2      cachedSegment;
V_MEMRAM0 static V_MEMRAM1 tFlashBlock          V_MEMRAM2      cachedBlock;
V_MEMRAM0 static V_MEMRAM1 vsint16              V_MEMRAM2      nextValidSegment;
#endif /* FBL_DIAG_ENABLE_OEM_SEGMENTNRGET || FBL_DIAG_ENABLE_OEM_READPROM */

#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
V_MEMRAM0 static V_MEMRAM1 vuint8               V_MEMRAM2      diagTaskState;
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */

/** Pointer to current service's information table */
V_MEMRAM0 static V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 * V_MEMRAM1 V_MEMRAM2 serviceInfo;
/** Pointer to current service's sub table */
V_MEMRAM0 static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3 * V_MEMRAM1 V_MEMRAM2 subServiceInfo;

#if defined( FBL_DIAG_ENABLE_SERVICE_PREHANDLER )
/** Pre handler of current request */
V_MEMRAM0 static V_MEMRAM1 tFblDiagPreHandler V_MEMRAM2 fblDiagServicePreHandler;
#endif /* FBL_DIAG_ENABLE_SERVICE_PREHANDLER */
/** Main handler of current request */
V_MEMRAM0 static V_MEMRAM1 tFblDiagMainHandler V_MEMRAM2 fblDiagServiceMainHandler;
/** NRC handler of current request */
V_MEMRAM0 static V_MEMRAM1 tFblDiagNrcHandler V_MEMRAM2 fblDiagServiceNrcHandler;
#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
/** Post handler of current request */
V_MEMRAM0 static V_MEMRAM1 tFblDiagPostHandler V_MEMRAM2 fblDiagServicePostHandler;
#endif /* FBL_DIAG_ENABLE_SERVICE_POSTHANDLER */

/***********************************************************************************************************************
 *  LOCAL DATA - visible in core and OEM layer
 **********************************************************************************************************************/

/** Flag indicating the state of positive response */
V_MEMRAM0 V_MEMRAM1_NEAR vuint8          V_MEMRAM2_NEAR diagResponseFlag;
/** Diagnostic error code */
V_MEMRAM0 V_MEMRAM1 vuint8               V_MEMRAM2      diagErrorCode;

#if defined( FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING )
/* Temporary data used during download in TransferData */
/** Actual number of segments */
V_MEMRAM0 V_MEMRAM1 vuint8               V_MEMRAM2      segmentCount;
#endif /* FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING */

/** Reset timeout observation [ms] */
V_MEMRAM0 V_MEMRAM1 vuint16              V_MEMRAM2      ecuResetTimeout;
/* Sleep mode handling */
#if defined( FBL_ENABLE_SLEEPMODE )
/** Counter, determines when to sleep */
V_MEMRAM0 V_MEMRAM1        vuint32              V_MEMRAM2      sleepCounter;
#endif
#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
/** Security access delay [ms] */
V_MEMRAM0 V_MEMRAM1 vuint32              V_MEMRAM2      secSecurityAccessDelay;
#endif

/***********************************************************************************************************************
 *  EXTERNAL DATA
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

static void       ChkSuppressPosRspMsgIndication(vuint8 *subparam);
static void       FblDiagDiscardReception(void);
static void       FblDiagResponseProcessor(void);

static V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 * FblDiagGetServiceInfo(vuint8 receivedServiceId);

static vuint32 FblDiagCheckServiceTableEntry(V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3* pServiceInfo,
                                             vuint8 *pbDiagData,
                                             tCwDataLengthType diagReqDataLen);
static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3* FblDiagFindSubTableEntry(V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3* pServiceInfo,
                                        V_MEMRAM1 vuint32 V_MEMRAM2 V_MEMRAM3* pServiceErrorMask,
                                        vuint8 *pbDiagData,
                                        tCwDataLengthType diagReqDataLen);
static vuint32 FblDiagCheckSubTableEntry(V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3* pSubtableInfo,
                                         vuint8 *pbDiagData,
                                         tCwDataLengthType diagReqDataLen);

static tFblResult FblDiagServiceProcessingTask(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen);

static tFblResult FblDiagDefaultNrcHandler(vuint32 serviceErrorMask);

static void FblDiagDeinit(void);

/* Start codeseg to be executed in RAM */
#define FBLDIAG_RAMCODE_START_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */
static void FblDiagResetServiceFlags(void);
static void FblDiagConfirmation(void);
/* End section to execute code from RAM */
#define FBLDIAG_RAMCODE_STOP_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */

/***********************************************************************************************************************
 *  ROUTINES TO MANAGE DIAGNOSTIC SERVICES
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Service support functions
 **********************************************************************************************************************/

#if defined( FBL_DIAG_ENABLE_OEM_READPROM )
#else
/***********************************************************************************************************************
 *  FblReadProm
 **********************************************************************************************************************/
/*! \brief       Read from ROM memory.
 *  \details     Reads data from ROM memory using the read function supplied with the flash driver.
 *  \param[in]   address Memory address to read out (logical address).
 *  \param[out]  buffer Target buffer.
 *  \param[in]   length Number of bytes to be read.
 *  \return      Number of actually copied bytes.
 **********************************************************************************************************************/
tFblLength FblReadProm(tFblAddress address, vuint8 *buffer, tFblLength length)
{
   tFblLength actualReadCount;    /* Actual read bytes */
   tFblLength currentReadCount;
   vsint16    nextMemSegment;

   /* Initialize variables */
   actualReadCount = 0u;

   (void)FblRealTimeSupport();

   /* Find start segment */
   memSegment = FblMemSegmentNrGet(address);

   if (memSegment < 0)
   {
      /* Start address does not lie within a valid memory segment
       * Search first segment which overlaps with requested range
       * This allows segmented reading of memory areas which include gaps
       * (e.g. CRC calculation or signature verification) */

      /* nextValidSegment contains first segment behind gap */
      if (nextValidSegment < kNrOfFlashBlock)
      {
         /* Gap detection */
         if (
               (address < FlashBlock[nextValidSegment].begin) &&
               (length > (FlashBlock[nextValidSegment].begin - address))
            )
         {
            /* Found first segment which overlaps requested range */
            memSegment = nextValidSegment;

            /* Remove gap from read length */
            length -= (FlashBlock[memSegment].begin - address);
            /* Adapt start address to segment start */
            address = FlashBlock[memSegment].begin;
         }
      }
   }

   /* Initialize next memory segment index */
   nextMemSegment = memSegment;

   /* Start segment found, read segment */
   if (memSegment >= 0)
   {
      while (length > 0u)
      {
         /* Check if read range crosses Flash block boundary */
         if ((address + (length - 1u)) > FlashBlock[memSegment].end)
         {
            currentReadCount = (FlashBlock[memSegment].end - address) + 1u;

            nextMemSegment++;

            if (nextMemSegment < kNrOfFlashBlock)
            {
               length = ((FlashBlock[nextMemSegment].begin - address) < length)
                  ? (length - (FlashBlock[nextMemSegment].begin - address)) : 0u;
            }
            else
            /* Flash block index overflow, exit loop */
            {
               length = 0u;
            }
         }
         else
         {
            currentReadCount = length;
            length = 0u;
         }

         if (MemDriver_RReadSync(buffer, currentReadCount, address) == IO_E_OK)
         {
            memSegment = nextMemSegment;
            buffer = (vuint8 *)&buffer[currentReadCount];
            address = FlashBlock[memSegment].begin;   /* PRQA S 3689 */ /* MD_FblDiag_3689 */
            actualReadCount += currentReadCount;
         }
         else
         {
            /* In case of error quit loop */
            length = 0u;
         }

         (void)FblRealTimeSupport();
      }
   }
   else
   {
      actualReadCount = 0u;
   }

   return actualReadCount;
}
#endif /* FBL_DIAG_ENABLE_OEM_READPROM */

/***********************************************************************************************************************
 *  FblEcuReset
 **********************************************************************************************************************/
/*! \brief       Start ECU reset.
 **********************************************************************************************************************/
void FblDiagEcuReset(void)
{
   /* Disable tester present timeout monitoring */
   StopTesterTimeout();

   /* Do not transmit response message */
   DiagSetNoResponse();
   FblDiagDeinit();
}

/***********************************************************************************************************************
 *  END OF DIAGNOSTIC SERVICE MANAGEMENT HANDLERS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  DIAGNOSTIC SUPPORT FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Global diagnostic support functions (OEM independent)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagSetError
 **********************************************************************************************************************/
/*! \brief       Sets the corresponding error code for the currently active service
 *  \param[in]   errorCode Negative response code
 **********************************************************************************************************************/
void FblDiagSetError( vuint8 errorCode )
{
   diagErrorCode = errorCode;

   /* No negative response for functional requests with these error codes */
   if (    (FblDiagGetFunctionalRequest())
        && (!FblDiagGetRcrRpInProgress())
        && (   (errorCode == kDiagNrcServiceNotSupported )
            || (errorCode == kDiagNrcSubFunctionNotSupported )
            || (errorCode == kDiagNrcRequestOutOfRange )
#if defined( FBL_DIAG_ENABLE_14229_2013 )
            || (errorCode == kDiagNrcSubfunctionNotSupportedInActiveSession)
            || (errorCode == kDiagNrcServiceNotSupportedInActiveSession)
#endif /* FBL_DIAG_ENABLE_14229_2013 */
           )
      )
   {
     DiagSetNoResponse();
   }
   else
   {
      /* Send negative response for all other cases */
      diagResponseFlag |= kDiagPutNegResponse;
   }
}

/***********************************************************************************************************************
 *  FblRealTimeSupport
 **********************************************************************************************************************/
/*! \brief       Transmit busy messages and triggers the watchdog according to system timer.
 *  \pre         Diagnosis and communication stack must be initialized, diagnostic request has to be received.
 **********************************************************************************************************************/
vuint8 FblRealTimeSupport( void )
{
   vuint8 returnCode;

   returnCode = (vuint8)FblLookForWatchdog();
   DiagExRCRResponsePending(kNotForceSendResponsePending);
   return returnCode;
}

/***********************************************************************************************************************
 *  DiagExRCRResponsePending
 **********************************************************************************************************************/
/*! \brief       Transmit a busy message if timer expires
 *  \details     (forceSend == kNotForceSendResponsePending) or if kForceSendResponsePending is passed.
 *  \param[in]   forceSend Determines if a message is sent independently from timer state
 **********************************************************************************************************************/
void DiagExRCRResponsePending( vuint8 forceSend )
{
   /* Only send response pending if service is in progress */
   if (FblDiagGetRcrRpAllowed())
   {
      /* Conditions to send an RCR-RP if P2-timer expired
       * or parameter contains kForceSendResponsePending */
      if ((forceSend == kForceSendResponsePending)
            || ((forceSend == kForceSendRpIfNotInProgress) && (!FblDiagGetRcrRpInProgress()))
            || (GetP2Timer() < (FblDiagGetRcrRpInProgress() ? kFblDiagP2StarMinThreshold : kFblDiagP2MinThreshold)) )
      {
         /* Prepare parameter for the Diag-confirmation function */
         diagPostParam = kDiagPostRcrRp;

         /* Prepare and transmit RCRRP buffer */
         rcrrpBuffer[0] = kDiagRidNegativeResponse;
         rcrrpBuffer[1] = diagServiceCurrent;
         rcrrpBuffer[2] = kDiagNrcRcrResponsePending;
         FblCwTransmitRP(rcrrpBuffer);

         /* Restart P2-Timer to P2* */
         SetP2Timer(kFblDiagTimeP2Star);

         /* If response pending is transmitted and no response
          * is set, a positive response has to be transmitted anyway. */
         diagResponseFlag &= FblInvert8Bit(kDiagPutNoResponse);

         FblDiagSetRcrRpInProgress();
      }
   }
}

#if defined( FBL_DIAG_ENABLE_OEM_SEGMENTNRGET )
#else
/***********************************************************************************************************************
 *  FblMemSegmentNrGet
 **********************************************************************************************************************/
/*! \brief       Get the number of the corresponding flash block for the given address
 *  \return      Index of flash block; -1, if not found
 *  \param[in]   address
 **********************************************************************************************************************/
vsint16 FblMemSegmentNrGet( tFblAddress address )
{
   vsint16 segment;

   /* Check address against cached range */
   if (   ( address >= cachedBlock.begin )
       && ( address <= cachedBlock.end )
      )
   {
      /* Range matches, finish processing and return cached segment */
      segment = cachedSegment;
   }
   else
   {
      /* Search matching flash block */
      for (nextValidSegment = 0; nextValidSegment < (vsint16)kNrOfFlashBlock; nextValidSegment++)
      {
         /* Reduced number of watchdog trigger calls */
         if (((vuint8)nextValidSegment & (vuint8)0x0Fu) == (vuint8)0x00u)
         {
            (void)FblRealTimeSupport();
         }

         /* Found first potential match */
         if (FlashBlock[nextValidSegment].end >= address)
         {
            break;
         }
      }

      /* Valid match found */
      if (nextValidSegment < (vsint16)kNrOfFlashBlock)
      {
         if (address >= FlashBlock[nextValidSegment].begin)
         {
            /* Address lies within block => valid segment */
            segment = nextValidSegment;

            /* Cache current match */
            cachedBlock.begin = FlashBlock[segment].begin;
            cachedBlock.end = FlashBlock[segment].end;
         }
         else
         {
            /* Address lies in front of block => gap detected */
            segment = -1;

            /* Cache current gap */
            if (nextValidSegment <= 0)
            {
               /* First block, gap starts at beginning of address space */
               cachedBlock.begin = 0x00u;
            }
            else
            {
               /* Gap starts after end of previous block */
               cachedBlock.begin = FlashBlock[nextValidSegment - 1].end + 1;
            }
            /* Gap ends in front of current segment */
            cachedBlock.end = FlashBlock[nextValidSegment].begin - 1;
         }
      }
      else
      {
         /* Address lies behind last block => gap detected */
         segment = -1;

         /* Cache current gap
          * Gap starts after end of previous block */
         cachedBlock.begin = FlashBlock[kNrOfFlashBlock - 1u].end + 1u;
         /* Gap ends at end of address space */
         /* PRQA S 0277 1 */ /* MD_FblDiag_0277 */
         cachedBlock.end = (tFblAddress) - 1;   /* Note: The correct conversion is guaranteed by the C standard */
      }

      /* Cache current segment for next request */
      cachedSegment = segment;
   }

   return segment;
}
#endif /* FBL_DIAG_ENABLE_OEM_SEGMENTNRGET */

/***********************************************************************************************************************
 *  FblDiagMemGetActiveBuffer
 **********************************************************************************************************************/
/*! \brief       Function updates core Diagnostic buffer (DiagBuffer) and returns updated pbDiagData to handler
 *  \return      updated pbDiagData buffer used by diagnostic handler functions *
 **********************************************************************************************************************/
V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 * FblDiagMemGetActiveBuffer(void)
{
   DiagBuffer = FblMemGetActiveBuffer();
   return &DiagBuffer[kDiagFmtpbDiagData];
}

#if defined( FBL_DIAG_ENABLE_CORE_RANGECHECKS )
/***********************************************************************************************************************
 *  FblCheckAddressRange
 **********************************************************************************************************************/
/*! \brief       Check whether an address lies within a specified address range
 *  \param[in]   address Address to be checked
 *  \param[in]   rangeStart Start of range
 *  \param[in]   rangeLength Length of range
 *  \return      kFblOk if address lies within range, kFblFailed otherwise
 **********************************************************************************************************************/
tFblResult FblCheckAddressRange( tFblAddress address, tFblAddress rangeStart, tFblLength rangeLength )
{
   tFblResult result;

   result = kFblFailed;

   if ((address >= rangeStart)
        && ((address - rangeStart) < rangeLength))
   {
      result = kFblOk;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblCheckRangeContained
 **********************************************************************************************************************/
/*! \brief       Check whether an address range lies within another specified address range
 *  \details     This function checks if the range specified with address and length lies within the
 *               enclosing range specified by rangeStart and rangeLength.
 *  \param[in]   address Start address of range to be checked
 *  \param[in]   length Length of range to be checked
 *  \param[in]   rangeStart Start of range
 *  \param[in]   rangeLength Length of range
 *  \return      kFblOk if range lies within range, kFblFailed otherwise
 **********************************************************************************************************************/
tFblResult FblCheckRangeContained( tFblAddress address, tFblLength length, tFblAddress rangeStart, tFblLength rangeLength )
{
   tFblResult result;

   result = kFblFailed;

   if ((FblCheckAddressRange(address, rangeStart, rangeLength) == kFblOk)
       && (length <= rangeLength)
       && ((address - rangeStart) <= (rangeLength - length)))
   {
      result = kFblOk;
   }

   return result;
}
#endif /* FBL_DIAG_ENABLE_CORE_RANGECHECKS */

/***********************************************************************************************************************
 *  Local diagnostic support functions (OEM independent)
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  DiagProcessingDone
 **********************************************************************************************************************/
/*! \brief       Prepare positive response.
 *  \details     Prepares the positive response for the currently processed service.
 *  \pre         Service must be in progress
 *  \param[in]   dataLength Data length to be transmitted (excluding the service-id itself).
 **********************************************************************************************************************/
void DiagProcessingDone( tCwDataLengthType dataLength )
{
   diagResponseFlag |= kDiagPutPosResponse;
   diagResLen        = dataLength;
}

/***********************************************************************************************************************
 *  FblDiagDiscardReception
 **********************************************************************************************************************/
/*! \brief       Received diagnostic messages are discarded
 *  \details     Discards all messages which have been received during a service is processed.
 **********************************************************************************************************************/
static void FblDiagDiscardReception( void )
{
   FblDiagSetResponseProcessing();   /* Set flag for running response procedure */

   FblCwDiscardReception();
   FblDiagClrResponseProcessing();
}

/* Start codeseg to be executed in RAM */
#define FBLDIAG_RAMCODE_START_SEC_CODE
#include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */
/***********************************************************************************************************************
 *  FblDiagResetServiceFlags
 **********************************************************************************************************************/
/*! \brief       Clear all status flags relevant for service handling.
 *  \pre         Service response shall be finished.
 **********************************************************************************************************************/
static void FblDiagResetServiceFlags( void )
{
   /* Clear negative response indicator */
   DiagClrError();

   /* Reset internal state in case no response was sent */
   FblDiagClrServiceInProgress();
   FblDiagClrRcrRpInProgress();
   /* Reset flag for functional request, default is physical request */
   FblDiagClrFunctionalRequest();
   FblDiagClrExecutePostHandler();
   /* Reset flag for suppress positive response bit */
   FblDiagClrSuppressPosRspMsg();

   /* diagResponse state back to idle */
   diagResponseFlag = kDiagResponseIdle;
}

/***********************************************************************************************************************
 *  FblDiagConfirmation
 **********************************************************************************************************************/
/*! \brief       Clear all status flags and conclude service transmission.
 *  \pre         Service response has been sent recently.
 **********************************************************************************************************************/
static void FblDiagConfirmation( void )
{
   /* Buffer is now available */
   FblDiagClrBufferLocked();
   /* Clear all diagnostic states */
   ClrP2Timer();

#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
   /* Check for a valid service indicator */
   if( FblDiagGetExecutePostHandler() && ( fblDiagServicePostHandler != (tFblDiagPostHandler)0u ))
   {
      /* Call the postHandler of the last received service */
      (*fblDiagServicePostHandler)( diagPostParam );
   }
#endif   /* FBL_DIAG_ENABLE_SERVICE_POSTHANDLER */

   /* Clear all flags for service handling */
   FblDiagResetServiceFlags();

   /* Restart the tester present timer if not in default session */
#if defined( FBL_ENABLE_STAY_IN_BOOT )
   if (FblDiagGetProgrammingSession() || FblDiagGetExtendedSession() || FblMainGetStartMessageReceived())
#else
   if (FblDiagGetProgrammingSession() || FblDiagGetExtendedSession())
#endif /* FBL_ENABLE_STAY_IN_BOOT */
   {
      ResetTesterTimeout();
   }
}

/* End section to execute code from RAM */
#define FBLDIAG_RAMCODE_STOP_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */

/***********************************************************************************************************************
 *  FblDiagResponseProcessor
 **********************************************************************************************************************/
/*! \brief       Prepares response message and hands it over to ComWrapper.
 *  \details     Prepares either a positive or negative response. Depending on the response type,
 *               the service ID is set to SID + 0x40 or 0x7F.
 *  \pre         Current SID must be in DiagBuffer[0], diagErrorCode must be initialized.
 **********************************************************************************************************************/
static void FblDiagResponseProcessor( void )
{
   /* Service will be concluded under this condition */
   if(( diagResponseFlag & kDiagResponseActive ) != 0u )
   {
      /* Stop transmission of response pending messages */
      FblDiagClrRcrRpAllowed();
      ClrP2Timer();
      FblDiagClrRcrRpInProgress();

      /* Discard all service requests received during processing this one */
      FblDiagDiscardReception();
      FblErrStatSetSId( diagServiceCurrent );

      /* Negative response has highest priority.
       * Must be checked first. */
      if(( diagResponseFlag & kDiagPutNegResponse) != 0u )
      {
         /* Set response type */
         diagPostParam = kDiagPostNegResponse;

         /* Prepare response message */
         DiagBuffer[kDiagFmtSubparam]     = diagServiceCurrent;
         DiagBuffer[kDiagFmtServiceId]    = kDiagRidNegativeResponse;
         DiagBuffer[kDiagFmtNegResponse]  = DiagGetError();

         assertFblUser(DiagGetError() != kDiagErrorNone, kFblSysAssertParameterOutOfRange);  /* PRQA S 2214 */ /* MD_FblDiag_2214 */
         FblCwTransmit(DiagBuffer, 3u, kFblCwTxCallNegResponse);

         /* diagResponse state back to idle */
         diagResponseFlag = kDiagResponseIdle;
      }
      else if (( diagResponseFlag & kDiagPutNoResponse ) != 0u )
      {
         /* Set response type */
         diagPostParam = kDiagPostNoResponse;

         /* Service conclusion */
         FblDiagConfirmation();

         /* Check if a reset shall be generated without message transmission */
         if (FblDiagGetWaitEcuReset())
         {
            /* If so, generate the reset immediately */
            FblDiagSetResetMsgConfirmed();
         }

         FblCwResetRxBlock();
      }
      else if (( diagResponseFlag & kDiagPutPosResponse ) != 0u )
      {
         /* Set response type */
         diagPostParam = kDiagPostPosResponse;

         /* Transmit positive response if not suppressed */
         DiagBuffer[kDiagFmtServiceId] = (vuint8)(diagServiceCurrent + 0x40u);
         FblCwTransmit(DiagBuffer, (tCwDataLengthType)( diagResLen + 1u ), kFblCwTxCallPosResponse );

         /* diagResponse state back to idle */
         diagResponseFlag = kDiagResponseIdle;
      }
      else
      {
         /* This is an illegal state and should not occur */
         assertFblInternal(0u, kFblSysAssertParameterOutOfRange );  /* PRQA S 2214 */ /* MD_FblDiag_2214 */

         /* Set response type */
         diagPostParam = kDiagResponseIdle;
      }

      /* Should be an assertion */
      if( diagPostParam == kDiagPostRcrRp )
      {
        /* diagResponse state back to idle */
        diagResponseFlag = kDiagResponseIdle;
      }
   }
}  /* PRQA S 6080 */ /* MD_MSR_STMIF */

/***********************************************************************************************************************
 *  ChkSuppressPosRspMsgIndication
 **********************************************************************************************************************/
/*! \brief       This functions checks if a diagnostic response has to be sent.
 *  \details     If the request is received with Suppress Positive Response Bit (0x80), the response is cancelled.
 *  \param[in]   subparam Diagnostic sub-parameter
 **********************************************************************************************************************/
static void ChkSuppressPosRspMsgIndication( vuint8 *subparam )
{
   if (((*(subparam)) & (kDiagSuppressPosRspMsgIndicationBit)) != 0)
   {
      /* Set internal flag to suppress positive response */
      DiagSetNoResponse();
      FblDiagSetSuppressPosRspMsg();

      /* Clear suppress bit in request message */
      (*(subparam)) &= FblInvert8Bit( kDiagSuppressPosRspMsgIndicationBit );
   }
}
#if defined( FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING )
/***********************************************************************************************************************
 *  FblDiagSegmentNext
 **********************************************************************************************************************/
/*! \brief       Keep track of segment counter
 *  \return      kSwmOutOfRange if maximum segment count exceeded, number of segments otherwise
 **********************************************************************************************************************/
vuint8 FblDiagSegmentNext( void )
{
   vuint8 result;

   /* Compare current used NOAR against max. value */
   if (segmentCount >= SWM_DATA_MAX_NOAR)
   {
      /* ALL PMA processed. Stop reading data */
      result =  kSwmOutOfRange;
   }
   else
   {
      /* Increment segment count */
      segmentCount++;

      result = segmentCount;
   }

   return result;
}
#endif /* FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING */

#if defined( FBL_DIAG_ENABLE_CORE_GETBLOCKFROMADDR )
/***********************************************************************************************************************
 *  FblGetBlockNrFromAddress
 **********************************************************************************************************************/
/*! \brief       Get logical block number from address and length
 *  \details     Get the number of the corresponding logical block for the given block address.
 *  \param[in]   blockAddress Start address of address range
 *  \param[in]   blockLength Length of address range
 *  \param[out]  pLogicalBlock Logical block number
 *  \return      kFblOk if blockIndex has been found, otherwise kFblFailed
 **********************************************************************************************************************/
tFblResult FblGetBlockNrFromAddress(tFblAddress blockAddress, tFblLength blockLength, vuint8* pLogicalBlock)
{
   tFblResult result;
   vuint8 tempCount;

   /* Initialize variables */
   result = kFblFailed;
   *pLogicalBlock = 0xFFu;

   if( blockLength > 0u)
   {
      (void)FblLookForWatchdog();
      tempCount = 0u;
      /* Search logical block table for blockIndex */
      while ((result == kFblFailed) && (tempCount < FblLogicalBlockTable.noOfBlocks))
      {
         /* Check if requested addresses lie within a logical block */
         result = FblCheckRangeContained(blockAddress,
                                     blockLength,
                                     FblLogicalBlockTable.logicalBlock[tempCount].blockStartAddress,
                                     FblLogicalBlockTable.logicalBlock[tempCount].blockLength
                                    );

         /* Logical block found - save block number */
         if (result == kFblOk)
         {
            *pLogicalBlock = tempCount;
         }

         /* Increment loop counter */
         tempCount++;
      }
   }

   return result;
}
#endif /* FBL_DIAG_ENABLE_CORE_GETBLOCKFROMADDR */

#if defined( FBL_DIAG_ENABLE_CORE_GETBLOCKFROMID )
/***********************************************************************************************************************
 *  FblGetBlockNrFromIndex
 **********************************************************************************************************************/
/*! \brief       Get logical block number for a given block index
 *  \details     Get the number of the corresponding logical block for the given block index.
 *  \param[in]   blockIndex Index of logical block
 *  \param[out]  pLogicalBlock Logical block number
 *  \return      kFblOk if blockIndex has been found, otherwise kFblFailed
 **********************************************************************************************************************/
tFblResult FblGetBlockNrFromIndex(vuint8 blockIndex, vuint8* pLogicalBlock)
{
   tFblResult result;
   vuint8 tempCount;

   /* Initialize variables */
   result = kFblFailed;
   *pLogicalBlock = 0xFFu;

   (void)FblLookForWatchdog();
   tempCount = 0u;
   /* Search logical block table for blockIndex */
   while ((result == kFblFailed) && (tempCount < FblLogicalBlockTable.noOfBlocks))
   {
      /* Check if requested index matches a logical block index */
      if (blockIndex == FblLogicalBlockTable.logicalBlock[tempCount].blockIndex)
      {
         result = kFblOk;
         *pLogicalBlock = tempCount;
      }

      /* Increment loop counter */
      tempCount++;
   }

   return result;
}
#endif /* FBL_DIAG_ENABLE_CORE_GETBLOCKFROMID */

/***********************************************************************************************************************
 *  DIAGNOSTIC MODULE CONTROL ROUTINES
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Initialization handling of diagnostic module
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagInitPowerOn
 **********************************************************************************************************************/
/*! \brief       Initialize module variables
 **********************************************************************************************************************/
void FblDiagInitPowerOn(void)
{
   vuint8 i;

   /* Initialize diagnostic states */
   for (i = 0u; i < (FBL_STATE_INDEX(kFblDiagNumberOfStates - 1u) + 1u); i++)
   {
      fblDiagStates[i] = (tFblStateBitmap)0x00u;
   }

   /* Initialize diagnostic variables */
   memSegment = -1;
   diagResponseFlag = kDiagResponseIdle;

   DiagClrError();

#if defined( FBL_DIAG_ENABLE_SERVICE_PREHANDLER )
   fblDiagServicePreHandler = (tFblDiagPreHandler)0u;
#endif /* FBL_DIAG_ENABLE_SERVICE_PREHANDLER */
   fblDiagServiceMainHandler = (tFblDiagMainHandler)0u;
   fblDiagServiceNrcHandler = (tFblDiagNrcHandler)0u;
#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
   fblDiagServicePostHandler = (tFblDiagPostHandler)0u;
#endif /* FBL_DIAG_ENABLE_SERVICE_POSTHANDLER */

   /* Initialize diagnostic session */
   FblDiagSetDefaultSession();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

   /* Stop tester present timer */
   StopTesterTimeout();

   /* Clear timer for response pending transmission */
   ClrP2Timer();

   /* Make sure to turn off the programming voltage */
   ApplFblResetVfp();

   StopEcuResetTimeout();

#if defined( FBL_ENABLE_SLEEPMODE )
   /* Sleep Timer not running during startup */
   FblSleepCounterClear();
#endif

#if defined( FBL_DIAG_ENABLE_OEM_SEGMENTNRGET )
#else
   /* FblMemSegmentNrGet caching - initialize cache with a valid entry */
   cachedSegment = 0;
   cachedBlock.begin = FlashBlock[0].begin;
   cachedBlock.end = FlashBlock[0].end;
#endif /* FBL_DIAG_ENABLE_OEM_SEGMENTNRGET */

#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
   diagTaskState = 0u;
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */

#if defined( FBL_DIAG_ENABLE_OEM_INITPOWERON )
   /* OEM-specific initializations */
   FblDiagOemInitPowerOn();
#endif /* FBL_DIAG_ENABLE_OEM_INITPOWERON */
}

/***********************************************************************************************************************
 *  FblDiagInit
 **********************************************************************************************************************/
/*! \brief       Initialize diagnostic module
 *  \details     Sets the diagnostic module to default session and initializes depending modules. The diagnostic
 *               module is working from here, but NV-memory dependent features are not available in all configurations.
 **********************************************************************************************************************/
void FblDiagInit(void)
{

   /* Get aligned diag buffer pointer from memory library */
   DiagBuffer = FblMemInitPowerOn();

#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
# if defined( FBL_DIAG_ENABLE_CORE_READ_SECACCESSDELAY_FLAG )
   /* Pre-init security access delay status - no NV-memory available here */
   SetSecurityAccessDelay();
# endif /* FBL_DIAG_ENABLE_CORE_READ_SECACCESSDELAY_FLAG */
#endif

#if defined( FBL_ENABLE_DEBUG_STATUS )
   /* Initialize error status */
   ApplFblInitErrStatus();
#endif

#if defined ( FBL_DIAG_ENABLE_CORE_SEGMENTHANDLING )
   /* Initialize segment counter */
   FblDiagSegmentInit();
#endif

   /* Initialize security module */
   (void)ApplFblSecurityInit();

   /* Init ComWrapper */
   FblCwSetTxInit();

#if defined( FBL_DIAG_ENABLE_OEM_INIT )
   /* OEM-specific initializations */
   FblDiagOemInit();
#endif /* FBL_DIAG_ENABLE_OEM_INIT */
}

/***********************************************************************************************************************
 *  FblDiagPostInit
 **********************************************************************************************************************/
/*! \brief       Initialize diagnostic module
 *  \details     Finishes the initialization of the diagnostic module. NV-memory dependent initialization can
 *               be done here.
 **********************************************************************************************************************/
void FblDiagPostInit(void)
{
#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
vuint8 resetCauseRead;
vuint8 resetCauseWrite;
#endif

#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
# if defined( FBL_DIAG_ENABLE_CORE_READ_SECACCESSDELAY_FLAG )
   /* Initialize security access delay timer after nv-memory access is possible. */
   if (!ApplFblGetSecAccessDelayFlag())
   {
      ClrSecurityAccessDelay();
   }
# endif /* FBL_DIAG_ENABLE_CORE_READ_SECACCESSDELAY_FLAG */
#endif

#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
         /* Check if reset has been caused by diagnostic service
          * StartDiagnosticSession (Default Session) or EcuReset */
         if (   (kFblOk == ApplFblReadResetResponseFlag(&resetCauseRead))
             && (resetCauseRead != RESET_RESPONSE_NOT_REQUIRED))
         {
            /* FBL started from reset
             * Clear flag before sending response to be ready for the next request */
            resetCauseWrite = RESET_RESPONSE_NOT_REQUIRED;

            (void)ApplFblWriteResetResponseFlag(&resetCauseWrite);   /* PRQA S 3425 */ /* MD_FblDiag_3425 */

            if (FblMainGetStartFromAppl())
            {
               /* Both reprogramming flag and reset response flag are set.
                * Clear reset response flag but prioritize the reprogramming flag. */
               assertFblInternal(0u, kFblOemAssertInvalidError);  /* PRQA S 2214 */ /* MD_FblDiag_2214 */
            }
            else
            {
               /* Send response for StartDiagnosticSession or EcuReset request */
               FblDiagInitStartFromReset(resetCauseRead);
            }
         }
#endif /* FBL_ENABLE_RESPONSE_AFTER_RESET */

         /* Send programming session request response */
         if (FblMainGetStartFromAppl())
         {
            /* FBL started from application */
            FblDiagInitStartFromAppl();
         }

#if defined( FBL_ENABLE_SLEEPMODE )
         /* Initialize sleep mode counter */
         FblSleepCounterReload();
#endif

#if defined( FBL_DIAG_ENABLE_OEM_POSTINIT )
   /* OEM-specific initializations */
   FblDiagOemPostInit();
#endif /* FBL_DIAG_ENABLE_OEM_POSTINIT */
}

/***********************************************************************************************************************
 *  FblDiagDeinit
 **********************************************************************************************************************/
/*! \brief       Function prepares for FBL shutdown on diagnostic service request.
 **********************************************************************************************************************/
static void FblDiagDeinit(void)
{
   /* Discard pending FblLib_Mem tasks */
   FblMemDeinit();

#if defined( FBL_DIAG_ENABLE_OEM_DEINIT )
   /* Shutdown OEM-specific service handling */
   FblDiagOemDeinit();
#endif /* FBL_DIAG_ENABLE_OEM_DEINIT */

   if (FblDiagGetProgrammingSession())
   {
      /* ECU programming mode is active */
      FblDiagClrProgrammingSession();
      if (FblDiagGetMemDriverInitialized())
      {
         /* Remove memory driver from RAM-buffer */
         FblCwSetOfflineMode();

         (void)MemDriver_DeinitSync(V_NULL);

         FblCwSetOnlineMode();

         /* Reinitialize memory driver flash */
         FblDiagClrMemDriverInitialized();
      }

      /* Turn off programming voltage */
      ApplFblResetVfp();
   }

   FblDiagClrWaitEcuReset();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

   /* Clear all flags for service handling */
   FblDiagResetServiceFlags();

   /* Request shutdown from ComWrapper */
   FblCwShutdownRequest(kFblCwResetEcuRegularCase);
}

/***********************************************************************************************************************
 *  FblDiagServiceProcessingTask
 **********************************************************************************************************************/
/*! \brief       Service processing done in FblDiagStateTask
 *  \details     If a service should be processed, this function performs service processing tasks
 *  \param[in]   pbDiagData Pointer to data in the diagBuffer (without SID).
 *  \param[in]   diagReqDataLen Length of data (without SID).
 *  \return      Result of service processing
 **********************************************************************************************************************/
static tFblResult FblDiagServiceProcessingTask(vuint8 *pbDiagData, tCwDataLengthType diagReqDataLen)
{
   tFblResult result;
   vuint32 serviceProcessingErrors;

   /* Prepare control variable */
   serviceProcessingErrors = kFblDiagServiceOk;

   /* This is the initial call after reception of a service request */
   if (diagResponseFlag == kDiagResponseRequested)
   {
      /* Update security seed each time a service is received. */
      (void)ApplFblSecuritySeedInit();

      /* Advance service processing state to repeated service
       * This will ensure that the service handler will be
       * called until it has been concluded, e.g. by DiagProcessingDone() */
      diagResponseFlag = kDiagRepeatResponse;

      /* Search for a valid configured service in the service table */
      serviceInfo = FblDiagGetServiceInfo(diagServiceCurrent);

      /* Check if service is supported in service table */
      if (serviceInfo == (V_MEMROM1 tFblDiagServiceTable  V_MEMROM2 V_MEMROM3 *) V_NULL)
      {
         /* No service found. Default NRC handler will be invoked */
         serviceProcessingErrors = kFblDiagServiceNotFound;
         fblDiagServiceNrcHandler = FblDiagDefaultNrcHandler;
      }
      else
      {
         serviceProcessingErrors = FblDiagCheckServiceTableEntry(serviceInfo, pbDiagData, diagReqDataLen);
#if defined( FBL_DIAG_ENABLE_SERVICE_PREHANDLER )
         fblDiagServicePreHandler = serviceInfo->servicePreHandler;
#endif /* FBL_DIAG_ENABLE_SERVICE_PREHANDLER */
         fblDiagServiceMainHandler = serviceInfo->serviceMainHandler;
#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
         fblDiagServicePostHandler = serviceInfo->servicePostHandler;
#endif /* FBL_DIAG_ENABLE_SERVICE_POSTHANDLER */
         fblDiagServiceNrcHandler = serviceInfo->serviceNrcHandler;

         /* Check if further sub-table tests have to be done. Sub-table tests are done only
            if no error on service level has been detected. */
         if ((serviceProcessingErrors & kFblDiagSubTableNotFound) == 0u)
         {
            subServiceInfo = FblDiagFindSubTableEntry(serviceInfo, &serviceProcessingErrors, pbDiagData, diagReqDataLen);

            /* Check if a matching sub-table has been found */
            if ((serviceProcessingErrors & kFblDiagSubTableNotFound) == kFblDiagSubTableNotFound)
            {
               /* If no sub-table is found, the service handler of the service can be invoked
                * in case there are no service level errors. This handler is already configured. */
               if ((serviceProcessingErrors & kFblDiagServiceLevelErrors) == 0u)
               {
                  if (fblDiagServiceMainHandler != (tFblDiagMainHandler)0u)
                  {
                     /* Remove sub-service level errors */
                     serviceProcessingErrors &= kFblDiagServiceLevelErrors;
                  }
               }
            }
            else
            {
               /* Check sub-table parameters */
               serviceProcessingErrors |= FblDiagCheckSubTableEntry(subServiceInfo, pbDiagData, diagReqDataLen);
               fblDiagServiceMainHandler = subServiceInfo->serviceMainHandler;
            }
         }
         else
         {
            /* No sub-table used. Remove error bit. */
            serviceProcessingErrors &= kFblDiagServiceLevelErrors;
         }

#if defined( FBL_DIAG_ENABLE_SERVICE_PREHANDLER )
         /* Use service pre-handler to check service preconditions. */
         if (((serviceProcessingErrors & kFblDiagServiceErrorMask) == 0u) && (fblDiagServicePreHandler != (tFblDiagPreHandler)V_NULL))
         {
            if ((*fblDiagServicePreHandler)(pbDiagData, diagReqDataLen) == kFblFailed)
            {
               serviceProcessingErrors |= kFblDiagServicePrehandlerFailure;
            }
         }
#endif /* FBL_DIAG_ENABLE_SERVICE_PREHANDLER */
      }
   }

   /* Call service main handler or NRC handler */
   if (serviceProcessingErrors == kFblDiagServiceOk)
   {
      /* Allow usage of response pending messages here */
      FblDiagSetRcrRpAllowed();

      /* Call main handler in case no error has been detected. */
      result = (*fblDiagServiceMainHandler)(pbDiagData, diagReqDataLen);
   }
   else
   {
      /* Call NRC handler. This handler is also called in case no service at all has been found. */
      result = (*fblDiagServiceNrcHandler)(serviceProcessingErrors);
   }

#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
   /* Diagnostic service has been processed. Post-handler needs to be invoked. */
   FblDiagSetExecutePostHandler();
#endif /* FBL_DIAG_ENABLE_SERVICS_POST_HANDLER */

   /* Finish service processing if service has been concluded before */
   FblDiagResponseProcessor();

   return result;
}  /* PRQA S 6080 */ /* MD_MSR_STMIF */

/***********************************************************************************************************************
 *  FblDiagStateTask
 **********************************************************************************************************************/
/*! \brief       This function handles the surveillance of the tester present timer.
 *  \details     When a timeout occurs the startup code of the FBL is executed.
 **********************************************************************************************************************/
void FblDiagStateTask( void )
{
#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
   if (!GetDiagStateTaskRunning())
   {
      SetDiagStateTaskRunning();
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */

      /* Diagnostic service dispatcher. Will be executed when a service has to be managed
       * No executed if the diagnostic service is waiting for a reset. */
      if ((!FblDiagGetWaitEcuReset()) && (diagResponseFlag != kDiagResponseIdle))
      {
         /* Call service task. Return value is not used but maintained for future use. */
         (void)FblDiagServiceProcessingTask(&DiagBuffer[kDiagFmtSubparam], (tCwDataLengthType)(DiagDataLength));
      }

#if defined( FBL_MEM_ENABLE_PIPELINING )
      /* Resume stored FblLib_Mem operation */
      FblMemResumeIndication();
#endif /* FBL_MEM_ENABLE_PIPELINING */

      /* Check for reset request
       * The following preconditions have to be fulfilled:
         * Reset message confirmed
         * Pre-reset function (e.g. flag written to NV-memory) has finished */
      if ((FblDiagGetWaitEcuReset()) && (FblDiagGetResetMsgConfirmed()) && (FblDiagGetEcuResetFctFinished()))
      {
         FblDiagDeinit();
      }

#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
      /* Check if Security access delay counter close to expire */
      if (GetSecurityAccessDelay() == 1u)
      {
         /* If so, conclude the decrement and check to clear the flag in EEPROM */
         ClrSecurityAccessDelay();

# if defined( FBL_DIAG_ENABLE_CORE_WRITE_SECACCESSDELAY_FLAG )
         /* Clear security access delay flag */
         (void)ApplFblClrSecAccessDelayFlag();
# endif /* FBL_DIAG_ENABLE_CORE_WRITE_SECACCESSDELAY_FLAG */
      }
#endif

#if defined( FBL_DIAG_ENABLE_OEM_STATETASK )
      FblDiagOemStateTask();
#endif /* FBL_DIAG_ENABLE_OEM_STATETASK */

#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
      /* Allow further task executions */
      ClrFblDiagStateTaskRunning();
   }
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */
}

/***********************************************************************************************************************
 *  Diagnostic tasks
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagTimerTask
 **********************************************************************************************************************/
/*! \brief       This function handles the surveillance of the tester present timer.
 *  \details     When a timeout occurs the startup code of the FBL is executed. Has to be executed from RAM
 *               if diagnostic functionality should be maintained while flashing, e.g. if pipelined programming
 *               is enabled.
 **********************************************************************************************************************/
void FblDiagTimerTask(void)
{
#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
   if (!GetDiagTimerTaskRunning())
   {
      SetDiagTimerTaskRunning();
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */
      /* Tester present timing */
      if (TimeoutTesterValue() != 0u)
      {
         DecTimeoutTesterValue();
         if (TimeoutTesterValue() == 0u)
         {
#if defined( FBL_DIAG_ENABLE_OEM_SESSION_TIMEOUT )
            FblDiagSessionTimeout();
#else
            /* Tester present timer expired, exit bootloader via reset */
            FblDiagEcuReset();
#endif
         }
      }

      /* ECU reset is pending */
      if (TimeoutEcuResetValue() != 0u)
      {
         DecTimeoutEcuResetValue();
         if (TimeoutEcuResetValue() == 0u)
         {
            /* ECU reset timer expired
             * Exit bootloader via Reset, but force no response!! */
            FblDiagEcuReset();
         }
      }

#if defined( FBL_ENABLE_SLEEPMODE )
            /* Sleep mode timer */
            if (FblGetSleepCounter() != 0u)
            {
               FblDecSleepCounter();
               if (FblGetSleepCounter() == 0u)
               {
                  ApplFblBusSleep();
                  FblSleepCounterReload();
               }
            }
#endif /* FBL_ENABLE_SLEEPMODE */

#if defined( FBL_ENABLE_SEC_ACCESS_DELAY )
      /* Security access delay timing */
      if (GetSecurityAccessDelay() > 1u)
      {
         DecSecurityAccessDelay();
      }
#endif

#if defined( FBL_DIAG_ENABLE_OEM_TIMERTASK )
      FblDiagOemTimerTask();
#endif /* FBL_DIAG_ENABLE_OEM_TIMERTASK */

#if defined( FBL_DIAG_ENABLE_TASK_LOCKS )
      /* Allow further task executions */
      ClrDiagTimerTaskRunning();
   }
#endif /* FBL_DIAG_ENABLE_TASK_LOCKS */
}

/***********************************************************************************************************************
 *  Diagnostic kernel handling functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagDefaultNrcHandler
 **********************************************************************************************************************/
/*! \brief       This routine is called as default NRC handler
 *  \details     Called in case no service has been found. User services can be added here.
 *  \param[in]   serviceErrorMask Error bit mask of table check
 *  \return      kFblOk/kFblFailed
 **********************************************************************************************************************/
static tFblResult FblDiagDefaultNrcHandler(vuint32 serviceErrorMask)
{
#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   (void)serviceErrorMask; /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

#if defined( FBL_ENABLE_USERSERVICE )
   FblDiagSetRcrRpAllowed();
   ApplDiagUserService(&DiagBuffer[kDiagFmtSubparam], (tCwDataLengthType)(DiagDataLength));
#else
   DiagNRCServiceNotSupported();
#endif /* FBL_ENABLE_USERSERVICE */

   return kFblOk;
}

/***********************************************************************************************************************
 *  FblDiagFindSubTableEntry
 **********************************************************************************************************************/
/*! \brief       Find a suitable sub-table entry for the currently requested service
 *  \pre         Sub-table is available
 *  \param[in]   pServiceInfo Pointer to service descriptor
 *  \param[out]  pSubtableInfo Pointer to service descriptor
 *  \param[in]   pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]   diagReqDataLen Data length (without SID).
 *  \return      Sub service table
 **********************************************************************************************************************/
static V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3* FblDiagFindSubTableEntry(V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3* pServiceInfo,
                                        V_MEMRAM1 vuint32 V_MEMRAM2 V_MEMRAM3* pServiceErrorMask,
                                        vuint8 *pbDiagData,
                                        tCwDataLengthType diagReqDataLen)
{
   V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3* subTableEntry;
   vuint32 tempServiceErrorMask;

   vuintx subFunctionLength;
   vuintx subParamLength;

   vuintx subTableIndex;
   vuintx subTableIdIndex;

#if defined( V_ENABLE_USE_DUMMY_STATEMENT )
   (void)diagReqDataLen; /* PRQA S 3112 */ /* MD_MSR_14.2 */
#endif

   /* Initialize variables */
   subTableIndex = 0u;
   subTableEntry = (V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3*)V_NULL;

   /* Add sub-table not found, sub-function not found and sub-param not found bits.
      If a matching entry is found, the bits will be removed. */
   *pServiceErrorMask |= (kFblDiagSubTableNotFound | kFblDiagSubfunctionNotFound | kFblDiagSubparamNotFound);

   /* Calculate comparison lengths */
   if ((pServiceInfo->serviceOptions & kFblDiagOptionServiceIsSubfunction) == kFblDiagOptionServiceIsSubfunction)
   {
      subFunctionLength = 1u;
      /* Sub-function may include suppress positive response bit */
      ChkSuppressPosRspMsgIndication(pbDiagData);
   }
   else
   {
      subFunctionLength = 0u;
   }
   subParamLength = pServiceInfo->subTableIdLength - subFunctionLength;

   /* Loop over all sub-table entries. While loop is terminated in case a table entry is found */
   while (((*pServiceErrorMask & kFblDiagSubTableNotFound) == kFblDiagSubTableNotFound) && (subTableIndex < pServiceInfo->subTableLength))
   {
      tempServiceErrorMask = kFblDiagServiceOk;
      subTableEntry = &pServiceInfo->serviceSubTable[subTableIndex];

      /* Check all sub table paramters */
      for (subTableIdIndex = 0u; subTableIdIndex < (subFunctionLength + subParamLength); subTableIdIndex++)
      {
         /* Compare table entries */
         if (subTableEntry->subTableParamTable[subTableIdIndex] != pbDiagData[kDiagLocFmtSubfunction + subTableIdIndex])
         {
            if (subTableIdIndex < subFunctionLength)
            {
               /* Sub-function not supported in this table entry */
               tempServiceErrorMask |= kFblDiagSubfunctionNotFound;
            }
            else
            {
               /* Sub-param not supported in this table entry */
               tempServiceErrorMask |= kFblDiagSubparamNotFound;
            }
         }
      }

      /* Do detail checks (for sub-functions and sub-parameters) */
      if ((tempServiceErrorMask & kFblDiagSubfunctionNotFound) != kFblDiagSubfunctionNotFound)
      {
         /* Matching sub-function found */
         *pServiceErrorMask &= FblInvert32Bit(kFblDiagSubfunctionNotFound);

         /* Sub-function has been found, do session check */
         if ((subTableEntry->serviceOptions & GetCurrentSession()) == 0u)
         {
            *pServiceErrorMask |= kFblDiagSubfuncSessionFailure;
            tempServiceErrorMask |= kFblDiagSubfuncSessionFailure;
         }
         /* Sub-function has been found, do security access check */
         if (((subTableEntry->serviceOptions & kFblDiagOptionSecuredService) == kFblDiagOptionSecuredService) && (!FblDiagGetSecurityUnlock()))
         {
            *pServiceErrorMask |= kFblDiagSubfuncSecAccessFailure;
            tempServiceErrorMask |= kFblDiagSubfuncSecAccessFailure;
         }
      }

      if ((tempServiceErrorMask & kFblDiagSubparamNotFound) != kFblDiagSubparamNotFound)
      {
         /* Matching sub-parameter found */
         *pServiceErrorMask &= FblInvert32Bit(kFblDiagSubparamNotFound);

         /* Sub-parameter has been found, do session check */
         if ((subTableEntry->serviceOptions & GetCurrentSession()) == 0u)
         {
            *pServiceErrorMask |= kFblDiagSubparamSessionFailure;
            tempServiceErrorMask |= kFblDiagSubparamSessionFailure;
         }
         /* Sub-parameter has been found, do security access check */
         if (((subTableEntry->serviceOptions & kFblDiagOptionSecuredService) == kFblDiagOptionSecuredService) && (!FblDiagGetSecurityUnlock()))
         {
            *pServiceErrorMask |= kFblDiagSubparamSecAccessFailure;
            tempServiceErrorMask |= kFblDiagSubparamSecAccessFailure;
         }
      }

      subTableIndex++;
      /* Store result to terminate loop */
      if ((tempServiceErrorMask & (kFblDiagSubfunctionNotFound | kFblDiagSubparamNotFound)) == 0u)
      {
         /* Only keep service level errors. Remove sub-table not found error */
         *pServiceErrorMask &= kFblDiagServiceLevelErrors;
         /* Add detected sub-table error bits. */
         *pServiceErrorMask |= tempServiceErrorMask;
      }
   }

   return subTableEntry;
}  /* PRQA S 6010, 6030 */ /* MD_MSR_STPTH, MD_MSR_STCYC */

/***********************************************************************************************************************
 *  FblDiagCheckSubTableEntry
 **********************************************************************************************************************/
/*! \brief       This function check the basic diagnostic states of the requested sub table entry
 *  \pre         Sub-table has been found for requested service
 *  \param[in]   pSubtableInfo Pointer to service descriptor
 *  \param[in]   pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]   diagReqDataLen Data length (without SID).
 *  \return      Error bit field
 **********************************************************************************************************************/
static vuint32 FblDiagCheckSubTableEntry(V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3* pSubtableInfo,
                                         vuint8 *pbDiagData,
                                         tCwDataLengthType diagReqDataLen)
{
   vuint32 result;

   /* Initialize varibles */
   result = kFblDiagServiceOk;

   /* Check if this sub-table entry is allowed functionally */
   if (FblDiagGetFunctionalRequest() && ((pSubtableInfo->serviceOptions & kFblDiagOptionFunctionalRequest) == 0u))
   {
      result |= kFblDiagSubNoFuncSupport;
   }
   /* Minimum length check based on table entry */
   if (diagReqDataLen < pSubtableInfo->serviceLength)
   {
      result |= kFblDiagSubMinLenFailure;
   }
   /* Complete length check. If a dynamic check is configured, use this check. */
   if (pSubtableInfo->serviceLengthCheck != (tFblDiagLengthCheck)0u)
   {
      if (pSubtableInfo->serviceLengthCheck(pbDiagData, diagReqDataLen) == kFblFailed)
      {
         result |= kFblDiagSubLenFailure;
      }
   }
   else
   {
      /* Match against length in table */
      if (pSubtableInfo->serviceLength != diagReqDataLen)
      {
         result |= kFblDiagSubLenFailure;
      }
   }

   return result;
}

/***********************************************************************************************************************
 *  CheckServiceTableEntry
 **********************************************************************************************************************/
/*! \brief       This function checks the basic diagnostic states of the requested service table entry
 *  \pre         Service has been found in service information table
 *  \param[in]   pServiceInfo Pointer to service descriptor
 *  \param[in]   pbDiagData Pointer to data in diagBuffer (without SID).
 *  \param[in]   diagReqDataLen Data length (without SID).
 *  \return      Error bit field
 **********************************************************************************************************************/
static vuint32 FblDiagCheckServiceTableEntry(V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3* pServiceInfo,
                                             vuint8 *pbDiagData,
                                             tCwDataLengthType diagReqDataLen)
{
   vuint32 result;

   /* Initialize variables */
   result = kFblDiagServiceOk;

   /* Check if a sub-table is present and if it can be processed */
   if (pServiceInfo->subTableLength == 0u)
   {
      result |= kFblDiagSubTableNotFound;
   }
   /* Check if the request length is not sufficient to check sub-table. If not, the sub-table cannot be identified. */
   if (diagReqDataLen < pServiceInfo->subTableIdLength)
   {
      result |= kFblDiagSubTableNotFound;
   }
   /* Check if this service is allowed functionally */
   if (FblDiagGetFunctionalRequest() && ((pServiceInfo->serviceOptions & kFblDiagOptionFunctionalRequest) == 0u))
   {
      result |= kFblDiagServiceNoFuncSupport;
   }
   /* Check session support */
   if ((pServiceInfo->serviceOptions & GetCurrentSession()) == 0u)
   {
      result |= kFblDiagServiceSessionFailure;
   }
   /* Minimum length check - done on service level */
   if (diagReqDataLen < pServiceInfo->serviceLength)
   {
      result |= kFblDiagServiceMinLenFailure;
   }
   /* Security Access on service level */
   if (((pServiceInfo->serviceOptions & kFblDiagOptionSecuredService) != 0u) && (!FblDiagGetSecurityUnlock()))
   {
      result |= kFblDiagServiceSecAccessFailure;
   }
   /* Complete length check. If a dynamic check is configured, use this check. */
   if (pServiceInfo->serviceLengthCheck != (tFblDiagLengthCheck)0u)
   {
      if (pServiceInfo->serviceLengthCheck(pbDiagData, diagReqDataLen) == kFblFailed)
      {
         result |= kFblDiagServiceLenFailure;
      }
   }
   else
   {
      /* Match against length in table (only if there is no sub-table) */
      if ((pServiceInfo->subTableLength == 0u) && (pServiceInfo->serviceLength != diagReqDataLen))
      {
         result |= kFblDiagServiceLenFailure;
      }
   }

   return result;
}  /* PRQA S 6010 */ /* MD_MSR_STPTH */

/***********************************************************************************************************************
 *  FblDiagGetServiceInfo
 **********************************************************************************************************************/
/*! \brief       Find service table entry for received diagnostic service ID
 *  \param[in]   receivedServiceId UDS service ID of requested service
 *  \return      Pointer to service table. If no service has been found, return a NULL pointer
 **********************************************************************************************************************/
static V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 * FblDiagGetServiceInfo(vuint8 receivedServiceId)
{
   V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 * serviceTableEntry;
   vuintx serviceIndex;

   /* Initialize variables */
   serviceTableEntry = (V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 *)V_NULL;

   /* Search for service information */
   for (serviceIndex = 0u; serviceIndex < kSizeOfServiceTable; serviceIndex++)
   {
      /* Check if service id matches id of current table entry */
      if (kFblDiagServiceTable[serviceIndex].serviceId == receivedServiceId)
      {
         /* Service ID matches */
         serviceTableEntry = &kFblDiagServiceTable[serviceIndex];
         break;
      }
   }

   return serviceTableEntry;
}

/***********************************************************************************************************************
 *  Callback functions from fbl_main
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  FblDiagInitStartFromAppl
 **********************************************************************************************************************/
/*! \brief       Starts programming session if the Bootloader is called from the application.
 *  \details     This function prepares a programming session request and starts the diagnostic processing
 *               of this request after the real request has been received by the application software.
 **********************************************************************************************************************/
void FblDiagInitStartFromAppl(void)
{
   FblCwSetRxBlock();

   if (FblDiagRxGetPhysBuffer(kDiagRqlSpsFromAppl + 1u) == DiagBuffer)
   {
      /* Diagnostic session control is processed */
      DiagBuffer[kDiagFmtServiceId] = kDiagSidDiagnosticSessionControl;

#if defined( FBL_DIAG_ENABLE_OEM_SPS_REQUEST )
      /* Use OEM function to prepare programming session request */
      FblDiagOemPrepareSpsRequest(DiagBuffer, &DiagDataLength);
#else
      /* Prepare DiagBuffer for DiagnosticSessionControl service */
      DiagBuffer[kDiagFmtSubparam] = kDiagSubProgrammingSession;
      DiagDataLength = kDiagRqlDiagnosticSessionControl;
#endif /* FBL_DIAG_ENABLE_OEM_SPS_REQUEST */

#if defined( FBL_DIAG_ENABLE_SPS_EXTENDED_SESSION )
      /* Set diagnostic session (extended session) */
      FblDiagSetExtendedSession();  /* PRQA S 3109 */ /* MD_FblDiag_3109 */
#endif /* FBL_DIAG_ENABLE_SPS_EXTENDED_SESSION */

      /* Activate tester present timer */
      ResetTesterTimeout();

      /* Prepare response message address */
      if (FblCwPrepareResponseAddress() == kFblOk)
      {
         /* Indicate prepared buffer */
         FblDiagRxStartIndication();
         FblDiagRxIndication(DiagBuffer, DiagDataLength + 1u);

         /* Dispatch prepared buffer */
         FblDiagStateTask();

         /* Preconditions already checked by application */
         FblDiagSetPreconditionsChecked();
      }
      else
      {  /* PRQA S 3201 */ /* MD_FblDiag_3201 */
         /* No address information found - cancel response */
         FblDiagConfirmation();
         FblCwResetRxBlock();
      }
   }
   else
   {
      FblCwResetRxBlock();
   }
} /* PRQA S 6050 */ /* MD_MSR_STCAL */

#if defined( FBL_ENABLE_RESPONSE_AFTER_RESET )
/***********************************************************************************************************************
 *  FblDiagInitStartFromReset
 **********************************************************************************************************************/
/*! \brief       Process a reset request or default session request received before reset.
 *  \details     This function is called from FblInit() when the FBL is started from reset and the application is not
 *               valid or the application starts the FBL with a reprogramming request.
 *  \param[in]   resetRequestType RESET_RESPONSE_SDS_REQUIRED - Response for default session request
 *                                RESET_RESPONSE_ECURESET_REQUIRED - Response for ECU reset request
 *               No response otherwise
 **********************************************************************************************************************/
void FblDiagInitStartFromReset(vuint8 resetRequestType)
{
   FblCwSetRxBlock();

   if (FblDiagRxGetPhysBuffer(kDiagRqlDiagnosticSessionControl + 1u) == DiagBuffer)
   {
      /* Set diagnostic session (default session) */
      FblDiagSetDefaultSession();   /* PRQA S 3109 */ /* MD_FblDiag_3109 */

      switch (resetRequestType)
      {
         case RESET_RESPONSE_SDS_REQUIRED:
         {
            /* Session control is processed */
            diagServiceCurrent = kDiagSidDiagnosticSessionControl;
# if defined( FBL_DIAG_ENABLE_OEM_SDS_RESPONSE )
            /* Use OEM function to prepare default session request response */
            FblDiagOemPrepareSdsResponse(DiagBuffer, &DiagDataLength);
# else
            /* Prepare DiagBuffer for DiagnosticSessionControl service */
            DiagBuffer[kDiagFmtSubparam] = kDiagSubDefaultSession;
            DiagDataLength = kDiagRqlDiagnosticSessionControl;

            /* Diagnostic session timing */
            DiagBuffer[kDiagFmtSubparam + 1u] = (vuint8)(kDiagSessionTimingP2 >> 8u);
            DiagBuffer[kDiagFmtSubparam + 2u] = (vuint8)(kDiagSessionTimingP2);
            DiagBuffer[kDiagFmtSubparam + 3u] = (vuint8)(kDiagSessionTimingP2Star >> 8u);
            DiagBuffer[kDiagFmtSubparam + 4u] = (vuint8)(kDiagSessionTimingP2Star);
# endif /* FBL_DIAG_ENABLE_OEM_SDS_RESPONSE */

            /* Prepare response message address */
            if (FblCwPrepareResponseAddress() == kFblOk)
            {
# if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
               /* Configure service post handler to be called after response */
               FblDiagSetExecutePostHandler();
               fblDiagServicePostHandler = kFblDiagResponseSdsPostHandler;
# endif
               DiagProcessingDone(kDiagRslDiagnosticSessionControl);
               FblDiagResponseProcessor();
            }
            else
            {  /* PRQA S 3201 */ /* MD_FblDiag_3201 */
               /* No address information found - cancel response */
               FblDiagConfirmation();
               FblCwResetRxBlock();
            }

            break;
         }
         case RESET_RESPONSE_ECURESET_REQUIRED:
         {
            /* Reset request is processed */
            diagServiceCurrent = kDiagSidEcuReset;
# if defined( FBL_DIAG_ENABLE_OEM_RR_RESPONSE )
            /* Use OEM function to prepare default session request response */
            FblDiagOemPrepareRrResponse(DiagBuffer, &DiagDataLength);
# else
            /* Prepare DiagBuffer for EcuReset service */
            DiagBuffer[kDiagFmtSubparam] = kFblDiagSubReset;
            DiagDataLength = kDiagRqlEcuReset;
# endif /* FBL_DIAG_ENABLE_OEM_RR_RESPONSE */
            /* Prepare response message address */
            if (FblCwPrepareResponseAddress() == kFblOk)
            {
# if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
               /* Configure service post handler to be called after response */
               FblDiagSetExecutePostHandler();
               fblDiagServicePostHandler = kFblDiagResponseResetPostHandler;
# endif
               DiagProcessingDone(kDiagRslEcuReset);
               FblDiagResponseProcessor();
            }
            else
            {  /* PRQA S 3201 */ /* MD_FblDiag_3201 */
               /* No address information found - cancel response */
               FblDiagConfirmation();
               FblCwResetRxBlock();
            }

            break;
         }
         default:
         {
            /* No response or parameter not supported */
            FblDiagConfirmation();
            FblCwResetRxBlock();
            break;
         }
      }
   }
}
#endif /* FBL_ENABLE_RESPONSE_AFTER_RESET */

/* Start section to execute code from RAM */
#define FBLDIAG_RAMCODE_START_SEC_CODE
#include "MemMap.h"  /* PRQA S 5087 */ /* MD_MSR_19.1 */
/***********************************************************************************************************************
 *  Callback functions from Communication Wrapper
 **********************************************************************************************************************/

#if defined( FBL_ENABLE_STAY_IN_BOOT )
# if defined( FBL_DIAG_ENABLE_CORE_STAYINBOOT )
/***********************************************************************************************************************
*  FblDiagCheckStartMsg
**********************************************************************************************************************/
/*! \brief       Checks and evaluates the contents of the start message
*  \pre         Start message reception has to be verified by ComWrapper
*  \param[in]   pData Pointer to received data
*  \param[in]   length Length of received data
*  \return      Check result. kFblOk if the start message has been received.
**********************************************************************************************************************/
vuint8 FblDiagCheckStartMsg(const V_MEMRAM1 vuint8 V_MEMRAM2 V_MEMRAM3 *pData, vuintx length)
{
   vuint8 result;
   vuintx index;

   /* Check if received data is long enough */
   if (length < (kSizeOfStayInBootMsg + 1u))
   {
      result = kFblFailed;
   }
   else
   {
      /* Check message length */
      if (kSizeOfStayInBootMsg == pData[0u])
      {
         result = kFblOk;

         /* Check complete message contents for Stay-In-Boot */
         for (index = 0u; index < kSizeOfStayInBootMsg; index++)
         {
            if (fblDiagStayInBootMsg[index] != pData[index + 1u])
            {
               result = kFblFailed;
               break;
            }
         }
      }
      else
      {
         result = kFblFailed;
      }
   }

   return result;
}
# endif /* FBL_DIAG_ENABLE_CORE_STAYINBOOT */
#endif /* FBL_ENABLE_STAY_IN_BOOT */

/***********************************************************************************************************************
 *  FblDiagRxStartIndication
 **********************************************************************************************************************/
/*! \brief       Data reception call-back function to start diagnostic service processing.
 *  \details     Indicates start of a frame to diagnostic module. Tester present timeout is stopped and
 *               the diagnostic layer is locked until the service is processed.
 *  \pre         TP reception successful
 **********************************************************************************************************************/
void FblDiagRxStartIndication(void)
{
   /* Halts the S3 timer while receiving a diagnostic message */
   StopTesterTimeout();

   /* Indicate an ongoing service processing */
   FblDiagSetServiceInProgress();

   DiagClrError();

   /* Reset handler functions */
#if defined( FBL_DIAG_ENABLE_SERVICE_PREHANDLER )
   fblDiagServicePreHandler = (tFblDiagPreHandler)0u;
#endif /* FBL_DIAG_ENABLE_SERVICE_PREHANDLER */
   fblDiagServiceMainHandler = (tFblDiagMainHandler)0u;
   fblDiagServiceNrcHandler = (tFblDiagNrcHandler)0u;
#if defined( FBL_DIAG_ENABLE_SERVICE_POSTHANDLER )
   fblDiagServicePostHandler = FblDiagDefaultPostHandler;
#endif /* FBL_DIAG_ENABLE_SERVICE_POSTHANDLER */
   serviceInfo = (V_MEMROM1 tFblDiagServiceTable V_MEMROM2 V_MEMROM3 *) V_NULL;
   subServiceInfo = (V_MEMROM1 tFblDiagServiceSubTable V_MEMROM2 V_MEMROM3 *) V_NULL;
}

/***********************************************************************************************************************
 *  FblDiagRxIndication
 **********************************************************************************************************************/
/*! \brief       Data reception call-back function to start diagnostic service processing.
 *  \details     Valid TesterPresent requests will simply start the S3 timer and will not be processed.
 *  \pre         TP reception successful
 *  \param[in]   pbDiagBuffer Pointer to diagnostic buffer
 *  \param[in]   rxDataLength number of bytes to be received
 **********************************************************************************************************************/
void FblDiagRxIndication(vuint8* pbDiagBuffer, tCwDataLengthType rxDataLength)
{
   /* Initialize diagBuffer and data length */
   DiagBuffer = pbDiagBuffer;
   DiagDataLength = rxDataLength - 1u;

   /* Reset P2 timer (initial timing) */
#if defined( FBL_DIAG_ENABLE_DYNAMIC_P2_HANDLING )
   SetP2Timer(fblDiagTimeP2Max);
#else
   SetP2Timer(kFblDiagTimeP2);
#endif /* FBL_DIAG_ENABLE_DYNAMIC_P2_HANDLING */

   /* Handle functional TesterPresent request here */
   if (    (kDiagSidTesterPresent == DiagBuffer[kDiagFmtServiceId])
        && (kDiagSuppressPosRspMsgIndicationBit == DiagBuffer[kDiagFmtSubparam])
        && (kDiagRqlTesterPresent == DiagDataLength)
        && FblDiagGetFunctionalRequest()
      )
   {
      /* service processing is not in progress, release the diagBuffer */
      FblDiagConfirmation();
      FblCwResetRxBlock();
   }
   else
   {
      /* Set diagnostic service indication to start service processing */
      diagResponseFlag = kDiagResponseRequested;
      diagServiceCurrent = DiagBuffer[kDiagFmtServiceId];

#if defined( FBL_MEM_ENABLE_PIPELINING )
      if (FblMemTaskIsPending())
      {
         FblMemRxNotification();
      }
#endif
   }

#if defined( FBL_ENABLE_SLEEPMODE )
   /* Request received - reload sleep counter */
   FblSleepCounterReload();
#endif
}

/***********************************************************************************************************************
 *  FblDiagRxGetPhysBuffer
 **********************************************************************************************************************/
/*! \brief       Diagnostic buffer management function for physical requests.
 *  \param[in]   rxDataLength Number of bytes to be received
 *  \return      Pointer to diagnostic buffer
 **********************************************************************************************************************/
vuint8* FblDiagRxGetPhysBuffer(tCwDataLengthType rxDataLength)
{
   vuint8* result;

   /* Initialize variables */
   result = (vuint8*)V_NULL;

   /* Check request length and if buffer is free */
   if ((rxDataLength > 0u )
        && (FBL_DIAG_BUFFER_LENGTH >= rxDataLength)
        && (!FblDiagGetBufferLocked())
      )
   {
      /* Initialize state variables */
      FblDiagClrFunctionalRequest();

      /* Lock buffer  */
      FblDiagSetBufferLocked();

      /* Return diagnostic buffer */
      result = DiagBuffer;
   }

   return result;
}

/***********************************************************************************************************************
 *  FblDiagRxGetFuncBuffer
 **********************************************************************************************************************/
/*! \brief       Diagnostic buffer management function for functional requests.
 *  \param[in]   rxDataLength Number of bytes to be received
 *  \return      Pointer to diagnostic buffer
 **********************************************************************************************************************/
vuint8* FblDiagRxGetFuncBuffer(tCwDataLengthType rxDataLength)
{
   /* Process functional request like physical request */
   vuint8* funcBuffer = FblDiagRxGetPhysBuffer(rxDataLength);

   /* Set functional request indication */
   if (V_NULL != funcBuffer)
   {
      FblDiagSetFunctionalRequest();
   }

   return funcBuffer;
}

/***********************************************************************************************************************
 *  FblDiagRxErrorIndication
 **********************************************************************************************************************/
/*! \brief       Indicates that an error has occurred during reception.
 *  \details     According to ISO-14229, now the S3 timer must be restarted.
 *  \pre         TP must been initialized
 **********************************************************************************************************************/
void FblDiagRxErrorIndication(void)
{
   /* Clear all other flags for service management */
   FblDiagConfirmation();
}

/***********************************************************************************************************************
 *  FblDiagTxErrorIndication
 **********************************************************************************************************************/
/*! \brief       Indicates that an error has occurred during transmission.
 *  \details     According to ISO-14229, now the S3 timer must be restarted.
 *  \pre         TP must been initialized
 *  \param[in]   cwMsgType Message type of indication, if it is a RCR-RP or normal message.
 *               The parameter type is defined in the CW-layer.
 **********************************************************************************************************************/
void FblDiagTxErrorIndication(vuint8 cwMsgType)
{
   /* Do not reset states for RCR-RP confirmation */
   if (cwMsgType != kFblCwMsgTypeRcrRp)
   {
      /* Clear all other flags for service management */
      FblDiagConfirmation();
   }
}

/***********************************************************************************************************************
 *  FblDiagTxConfirmation
 **********************************************************************************************************************/
/*! \brief       TP transmission call-back function.
 *  \pre         TP transmission successful
 *  \param[in]   cwMsgType Message type of indication, if it is a RCR-RP or normal message.
 *               The parameter type is defined in the CW-layer.
 **********************************************************************************************************************/
void FblDiagTxConfirmation(vuint8 cwMsgType)
{
   /* Do not reset states for RCR-RP confirmation */
   if (cwMsgType != kFblCwMsgTypeRcrRp)
   {
      /* Clear all other flags for service management */
      FblDiagConfirmation();
   }

   /* Diag wants to reset FBL after sending an RCR-RP */
   if (FblDiagGetWaitEcuReset())
   {
      FblDiagSetResetMsgConfirmed();
   }
}
/* End section to execute code from RAM */
# define FBLDIAG_RAMCODE_STOP_SEC_CODE
# include "MemMap.h" /* PRQA S 5087 */ /* MD_MSR_19.1 */
/***********************************************************************************************************************
 *  Callback functions from FblLib_Mem
 **********************************************************************************************************************/

#if defined( FBL_DIAG_ENABLE_CORE_PREWRITE )
 /***********************************************************************************************************************
 *  FblDiagMemPreWrite
 **********************************************************************************************************************/
/*! \brief       Function performs actions before FblLibMem calls the memdriver write function.
 *  \return      kFblOk if PreWrite operations are successful, kFblFailed if PreWrite operations failed
 **********************************************************************************************************************/
tFblResult FblDiagMemPreWrite(void)
{
   FblCwSetOfflineMode();

   return kFblOk;
}
#endif /* FBL_DIAG_ENABLE_CORE_PREWRITE  */

#if defined( FBL_DIAG_ENABLE_CORE_POSTWRITE )
/***********************************************************************************************************************
 *  FblDiagMemPostWrite
 **********************************************************************************************************************/
/*! \brief       Checks if message is the platform message or not
 *  \return      kFblOk if post-write operations are successful, kFblFailed if post-write operations failed
 **********************************************************************************************************************/
tFblResult FblDiagMemPostWrite(void)
{
   FblCwSetOnlineMode();

   return kFblOk;
}
#endif /* FBL_DIAG_ENABLE_CORE_POSTWRITE  */

/***********************************************************************************************************************
 *  END OF FILE: FBL_DIAG_CORE.C
 **********************************************************************************************************************/
