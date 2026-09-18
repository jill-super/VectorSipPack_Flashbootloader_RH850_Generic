/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        Implementation of the HIS security module - CRC calculation
 *
 *  \description  Offers CRC calculation (CRC-16 / CRC-32 / CRC-64)
 *  -------------------------------------------------------------------------------------------------------------------
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \par Copyright
 *  \verbatim
 *  Copyright (c) 2017 by Vector Informatik GmbH.                                                  All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 */
/*********************************************************************************************************************/

/***********************************************************************************************************************
 *  AUTHOR IDENTITY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Name                          Initials      Company
 *  --------------------------------------------------------------------------------------------------------------------
 *  Joern Herwig                  JHg           Vector Informatik GmbH
 *  --------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  01.00.00   2013-06-20  JHg     ESCAN00068213    Initial release based on FblSecMod_Vector 02.02.00
 *  02.00.00   2013-11-08  JHg     ESCAN00071176    Major refactoring
 *                                                   Moved to separate sub-package
 *                                                   Renamed file name (was SecMCrc.c)
 *                                                   Added CRC-16 CCITT implementation
 *                                                   Function for CRC table generation made static
 *  02.00.01   2014-01-20  JHg     ESCAN00073016    Resolved compiler warnings
 *  02.01.00   2014-02-10  JHg     ESCAN00073559    CRC algorithm selectable at compile time
 *  02.02.00   2015-11-11  JHg     ESCAN00085510    Added SecM_InitPowerOn, intializes state of lookup table
 *  02.03.00   2015-11-25  JHg     ESCAN00086750    Allow overwrite of CRC parameters in configuration
 *  02.04.00   2017-01-18  JHg     ESCAN00093623    Added support for CRC-64 (defaults to ISO-3309)
 *                                                  Multiple CRC widths available at runtime
 *                                                  Allow overwrite of bit order mode in configuration
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

/* Security module configuration settings */
#include "Sec_Inc.h"

/* Global type definitions for security module */
#include "Sec_Types.h"

/* Security module interface */
#include "Sec.h"

/***********************************************************************************************************************
 *   VERSION
 **********************************************************************************************************************/

#if ( SYSSERVICE_SECMODHIS_CRC_VERSION != 0x0204u ) || \
    ( SYSSERVICE_SECMODHIS_CRC_RELEASE_VERSION != 0x00u )
# error "Error in SEC_CRC.C: Source and header file are inconsistent!"
#endif

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* PRQA S 3453 TAG_SecCrc_3453_1 */ /* MD_CBD_19.7 */

#if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )
/* Large lookup table in RAM */
/** Number of bits used to index entries */
# define CRC_INDEX_BITS    8u
/** Mask for table indices */
# define CRC_INDEX_MASK    0xFFul
/** Size of lookup table */
# define CRC_TABLE_SIZE    256u
#elif ( SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED )
/* Small lookup table in ROM */
/** Number of bits used to index entries */
# define CRC_INDEX_BITS    4u
/** Mask for table indices */
# define CRC_INDEX_MASK    0x0Ful
/** Size of lookup table */
# define CRC_TABLE_SIZE    16u
#endif /* SEC_CRC_OPT */

/* Allow override of CRC parameters by configuration (e.g. don't complement CRC value during finalization) */

#if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
# if defined( SEC_CRC_32_INITIAL )
# else
/** Initialization value for CRC */
#  define SEC_CRC_32_INITIAL        0xFFFFFFFFul
# endif /* SEC_CRC_32_INITIAL */
# if defined( SEC_CRC_32_POLYNOMIAL )
# else
/** Reflected CRC-32 polynomial. This value can be found in literature in reverse bit order: 0x04C11DB7 */
#  define SEC_CRC_32_POLYNOMIAL     0xEDB88320ul
# endif /* SEC_CRC_32_POLYNOMIAL */
# if defined( SEC_CRC_32_MODE )
# else
/* Operation mode of CRC (reflected) */
#  define SEC_CRC_32_MODE           SEC_CRC_MODE_REFLECTED
# endif /* SEC_CRC_32_MODE */
# if defined( SEC_CRC_32_FINAL )
# else
/** Finalization value for CRC: Complement CRC value */
#  define SEC_CRC_32_FINAL          0xFFFFFFFFul
# endif /* SEC_CRC_32_FINAL */

/** Value to indicate uninitialized RAM lookup table */
# define SEC_CRC_32_UNINIT_PATTERN  0xA5A5A5A5ul
/** Magic value to indicate initialized RAM lookup table */
# define SEC_CRC_32_INIT_PATTERN    0xC2C3274Bul
#endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

#if defined( SEC_ENABLE_CRC_STRUCT_TYPE )
# define SEC_INITIAL_VAL(val)       { 0x00ul, (val) }
# define SEC_GET_BASE_CRC(crc)      ((crc).lowWord)
#else
# define SEC_INITIAL_VAL(val)       (val)
# define SEC_GET_BASE_CRC(crc)      (crc)
#endif /* SEC_ENABLE_CRC_STRUCT_TYPE */

#if ( SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED )
/* Macros for compile-time generation of small lookup table in ROM */

# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
#  if ( SEC_CRC_32_MODE == SEC_CRC_MODE_NON_REFLECTED )
/** Update CRC-32 value according to next input bit */
#   define CRC_32_BIT(in)        ((((in) & 0x7FFFFFFFul) << 1u) ^ ((((in) >> 31u) & 0x01u) * SEC_CRC_32_POLYNOMIAL))
/** Calculate CRC-32 table entry for given input index */
#   define CRC_32_CALC(in)       CRC_32_NIBBLE(((in) & 0x0Ful) << 28u)
#  else /* SEC_CRC_32_MODE == SEC_CRC_MODE_REFLECTED */
/** Update CRC-32 value according to next input bit */
#   define CRC_32_BIT(in)        (((in) >> 1u) ^ (((in) & 0x01u) * SEC_CRC_32_POLYNOMIAL))
/** Calculate CRC-32 table entry for given input index */
#   define CRC_32_CALC(in)       CRC_32_NIBBLE((in))
#  endif /* SEC_CRC_32_MODE */

/** Calculate entry for CRC-32 calculation table based on 4-bit algorithm */
#  define CRC_32_NIBBLE(in)      (CRC_32_BIT(CRC_32_BIT(CRC_32_BIT(CRC_32_BIT(in)))))

/** Calculate 4 entries of CRC-32 calculation table */
#  define CRC_32_ENTRIES_4(i)    \
   CRC_32_CALC((i)),             \
   CRC_32_CALC((i) + 1u),        \
   CRC_32_CALC((i) + 2u),        \
   CRC_32_CALC((i) + 3u)

/** Calculate 16 entries of CRC-32 calculation table */
#  define CRC_32_ENTRIES_16(i)   \
   CRC_32_ENTRIES_4((i)),        \
   CRC_32_ENTRIES_4((i) + 4u),   \
   CRC_32_ENTRIES_4((i) + 8u),   \
   CRC_32_ENTRIES_4((i) + 12u)
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

#endif /* SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED */

/* PRQA L:TAG_SecCrc_3453_1 */

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

#if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )
# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
static void SecM_GenerateLookupCrc32( FL_WDTriggerFctType pWatchdog );
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */
#endif /* SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED */

#if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
static void SecM_UpdateCrc32( V_MEMRAM1 SecM_CRCParamType V_MEMRAM2 V_MEMRAM2 * pCrcParam );
#endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

/***********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/

 /* PRQA S 3218 TAG_SecCrc_3218_1 */ /* MD_SecCrc_3218 */

#if ( SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED )
# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/** CRC-32 calculation table based on 4-bit algorithm */
V_MEMROM0 static V_MEMROM1 SecM_Crc32Type V_MEMROM2 lookupCrc32[CRC_TABLE_SIZE] =
{
   CRC_32_ENTRIES_16(0x00u)
};
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */
#elif ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )
/* For speed optimization, the CRC table is dynamically generated in RAM */
# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/** CRC-32 calculation table based on 8-bit algorithm */
V_MEMRAM0 static V_MEMRAM1 SecM_Crc32Type V_MEMRAM2 lookupCrc32[CRC_TABLE_SIZE + 1u];
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

#endif /* SEC_CRC_OPT */

# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/** Initialization value for CRC */
V_MEMROM0 static V_MEMROM1 SecM_CRCType   V_MEMROM2 initialValueCrc32   = SEC_INITIAL_VAL(SEC_CRC_32_INITIAL);
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

/* PRQA L:TAG_SecCrc_3218_1 */

/**********************************************************************************************************************
 *  GLOBAL DATA
 *********************************************************************************************************************/

/** Zero CRC value */
V_MEMROM0 V_MEMROM1 SecM_CRCType          V_MEMROM2 secCrcZeroValue     = SEC_INITIAL_VAL(0u);

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/

#if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )

# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/***********************************************************************************************************************
 *  SecM_GenerateLookupCrc32
 **********************************************************************************************************************/
/*! \brief       Generates the CRC-32 lookup table
 *  \param[in]   pWatchdog Pointer to watchdog trigger function
 **********************************************************************************************************************/
static void SecM_GenerateLookupCrc32( FL_WDTriggerFctType pWatchdog )
{
   /* Create CRC table in RAM */
   SecM_ShortFastType   tableIndex;
   SecM_ByteFastType    bitIndex;
   SecM_Crc32Type       crc;

   /* Fill lookup table */
   for (tableIndex = 0u; tableIndex < CRC_TABLE_SIZE; tableIndex++)
   {
      /* Serve watchdog (every n-th cycle) */
      SEC_WATCHDOG_CYCLE_TRIGGER(pWatchdog, tableIndex); /* PRQA S 3109 */ /* MD_MSR_14.3 */

      /* Start value equals part of table index */
#  if ( SEC_CRC_32_MODE == SEC_CRC_MODE_NON_REFLECTED )
      /* Non-reflected algorithm */
      crc = (SecM_Crc32Type)(tableIndex << (32u - CRC_INDEX_BITS));
#  else /* SEC_CRC_32_MODE == SEC_CRC_MODE_REFLECTED */
      /* Reflected algorithm */
      crc = tableIndex;
#  endif /* SEC_CRC_32_MODE */

      /* Iterate all index bits */
      for (bitIndex = 0u; bitIndex < CRC_INDEX_BITS; bitIndex++)
      {
#  if ( SEC_CRC_32_MODE == SEC_CRC_MODE_NON_REFLECTED )
         /* Non-reflected algorithm takes most significant bit */
         if ((crc & (0x01u << 31u)) != 0x00u)
         {
            crc <<= 1u;
            crc ^= SEC_CRC_32_POLYNOMIAL;
         }
         else
         {
            crc <<= 1u;
         }
#  else /* SEC_CRC_32_MODE == SEC_CRC_MODE_REFLECTED */
         /* Reflected algorithm takes least significant bit */
         if ((crc & 0x01u) != 0x00u)
         {
            crc >>= 1u;
            crc ^= SEC_CRC_32_POLYNOMIAL;
         }
         else
         {
            crc >>= 1u;
         }
#  endif /* SEC_CRC_32_MODE */
      }

      /* Store calculated lookup value */
      lookupCrc32[tableIndex] = crc;
   }

   /* Mark table as initialized by setting magic value */
   lookupCrc32[CRC_TABLE_SIZE] = SEC_CRC_32_INIT_PATTERN;
}
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

#endif /* SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED */

#if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/***********************************************************************************************************************
 *  SecM_UpdateCrc32
 **********************************************************************************************************************/
/*! \brief       Computes the CRC upon the given data buffer
 *  \param[in,out] crcParam Pointer to parameter structure
 **********************************************************************************************************************/
static void SecM_UpdateCrc32( V_MEMRAM1 SecM_CRCParamType V_MEMRAM2 V_MEMRAM2 * pCrcParam )
{
   SecM_ShortFastType      tableIndex;    /* Index for CRC table access */
   SecM_LengthFastType     sourceIndex;   /* Index for source data buffer */
   SecM_LengthFastType     byteCount;
   SecM_Crc32Type          tmpCrc;
   SecM_ByteType           currentData;
   FL_WDTriggerFctType     pWatchdog;

   /* Operate on local copy of CRC */
   tmpCrc      = (SecM_Crc32Type)SEC_GET_BASE_CRC(pCrcParam->currentCRC);
   byteCount   = pCrcParam->crcByteCount;
   pWatchdog   = pCrcParam->wdTriggerFct;

   /* Loop all input bytes */
   for (sourceIndex = 0u; sourceIndex < byteCount; sourceIndex++)
   {
      /* Serve watchdog (every n-th cycle) */
      SEC_WATCHDOG_CYCLE_TRIGGER(pWatchdog, sourceIndex); /* PRQA S 3109 */ /* MD_MSR_14.3 */

      /* Read current data byte */
      currentData = pCrcParam->crcSourceBuffer[sourceIndex];

# if ( SEC_CRC_32_MODE == SEC_CRC_MODE_NON_REFLECTED )
#  if (SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED)
      /* Calculate CRC with small table in ROM */
      /* Calculate high-nibble first */
      tableIndex  = (SecM_ShortFastType)(((currentData >> (8u - CRC_INDEX_BITS)) ^ (tmpCrc >> (32u - CRC_INDEX_BITS))) & CRC_INDEX_MASK);
      tmpCrc      = (SecM_Crc32Type)((tmpCrc << CRC_INDEX_BITS) ^ lookupCrc32[tableIndex]);
#  endif /* SEC_CRC_OPT */
      tableIndex  = (SecM_ShortFastType)((currentData ^ (tmpCrc >> (32u - CRC_INDEX_BITS))) & CRC_INDEX_MASK);
      tmpCrc      = (SecM_Crc32Type)((tmpCrc << CRC_INDEX_BITS) ^ lookupCrc32[tableIndex]);
# else /* SEC_CRC_32_MODE == SEC_CRC_MODE_REFLECTED */
      /* Reflected */
      tableIndex  = (SecM_ShortFastType)((currentData ^ tmpCrc) & CRC_INDEX_MASK);
      tmpCrc      = (SecM_Crc32Type)((tmpCrc >> CRC_INDEX_BITS) ^ lookupCrc32[tableIndex]);
#  if ( SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED )
      /* Calculate CRC with small table in ROM */
      /* Calculate low-nibble */
      tableIndex  = (SecM_ShortFastType)(((currentData >> (8u - CRC_INDEX_BITS)) ^ tmpCrc) & CRC_INDEX_MASK);
      tmpCrc      = (SecM_Crc32Type)((tmpCrc >> CRC_INDEX_BITS) ^ lookupCrc32[tableIndex]);
#  endif /* SEC_CRC_OPT */
# endif /* SEC_CRC_32_MODE */
   }

   /* Update external CRC value */
   SEC_GET_BASE_CRC(pCrcParam->currentCRC) = tmpCrc;
}
#endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  SecM_InitPowerOnCRC
 *********************************************************************************************************************/
/*! \brief         Initialize CRC module
 *********************************************************************************************************************/
void SecM_InitPowerOnCRC( void )
{
#if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )
   /* Mark lookup table as potentially uninitialized */
# if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
   lookupCrc32[CRC_TABLE_SIZE] = SEC_CRC_32_UNINIT_PATTERN;
# endif /* SEC_ENABLE_CRC_TYPE_CRC32 */
#endif /* SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED */
}

/***********************************************************************************************************************
 *  SecM_ComputeCRC
 **********************************************************************************************************************/
/*! \brief       Function that manages the state of CRC computation
 *  \param[in,out] crcParam Pointer to parameter structure
 *  \return      SECM_OK if operation was successful
 *               SECM_NOT_OK
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeCRC( V_MEMRAM1 SecM_CRCParamType V_MEMRAM2 V_MEMRAM2 * crcParam )
{
#if ( SEC_CRC_TYPE == SEC_CRC16 )
   return SecM_ComputeCrc16(crcParam);
#elif ( SEC_CRC_TYPE == SEC_CRC32 )
   return SecM_ComputeCrc32(crcParam);
#else /* SEC_CRC_TYPE == SEC_CRC64 */
   return SecM_ComputeCrc64(crcParam);
#endif /* SEC_CRC_TYPE */
}

#if defined( SEC_ENABLE_CRC_TYPE_CRC32 )
/***********************************************************************************************************************
 *  SecM_ComputeCrc32
 **********************************************************************************************************************/
/*! \brief       Function that manages the state of CRC-32 computation
 *  \param[in,out] crcParam Pointer to parameter structure
 *  \return      SECM_OK if operation was successful
 *               SECM_NOT_OK
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeCrc32( V_MEMRAM1 SecM_CRCParamType V_MEMRAM2 V_MEMRAM3 * crcParam )
{
   SecM_StatusType result;
   SecM_Crc32Type  crcTransferred;

   result = SECM_NOT_OK;

   switch (crcParam->crcState)
   {
      case SEC_CRC_INIT:
      {
# if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED )
         /* Check magic value to verify whether lookup table was already initialized */
         if (SEC_CRC_32_INIT_PATTERN != lookupCrc32[CRC_TABLE_SIZE])
         {
            /* Initialize lookup table in RAM */
            SecM_GenerateLookupCrc32(crcParam->wdTriggerFct);
         }
# endif /* SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED */

         /* Set initial CRC value */
         crcParam->currentCRC = initialValueCrc32;

         result = SECM_OK;

         break;
      }
      case SEC_CRC_COMPUTE:
      {
         /* Update CRC using provided data */
         SecM_UpdateCrc32(crcParam);

         result = SECM_OK;

         break;
      }
      case SEC_CRC_FINALIZE:
      {
         /* Complement CRC value */
         SEC_GET_BASE_CRC(crcParam->currentCRC) ^= SEC_CRC_32_FINAL;

         result = SECM_OK;

         break;
      }
      case SEC_CRC_VERIFY:
      {
         if (crcParam->crcByteCount == SEC_SIZE_CHECKSUM_CRC_32)
         {
            /* Convert byte array to big-endian CRC value */
            crcTransferred = (SecM_Crc32Type)SecM_GetInteger(SEC_SIZE_CHECKSUM_CRC_32, crcParam->crcSourceBuffer);

            /* Compare given CRC against calculated one */
            if (crcTransferred == SEC_GET_BASE_CRC(crcParam->currentCRC))
            {
               /* CRC match */
               result = SECM_VER_OK;
            }
            else
            {
               /* CRC mismatch */
               result = SECM_VER_CRC;
            }
         }

         break;
      }
# if defined( SEC_ENABLE_CRC_SERIALIZATION )
      case SEC_CRC_GET:
      {
         if (crcParam->crcByteCount >= SEC_SIZE_CHECKSUM_CRC_32)
         {
            SecM_SetInteger(SEC_SIZE_CHECKSUM_CRC_32, SEC_GET_BASE_CRC(crcParam->currentCRC), crcParam->crcOutputBuffer);
            /* Overwrite with actual CRC size */
            crcParam->crcByteCount = SEC_SIZE_CHECKSUM_CRC_32;

            result = SECM_OK;
         }

         break;
      }
# endif /* SEC_ENABLE_CRC_SERIALIZATION */
      default:
      {
         break;
      }
   }

   return result;
}
#endif /* SEC_ENABLE_CRC_TYPE_CRC32 */

/***********************************************************************************************************************
 *  CONFIGURATION CHECKS
 **********************************************************************************************************************/

#if defined( SEC_CRC_OPT )
#else
# error "Error in configuration: SEC_CRC_OPT not defined"
#endif

#if ( SEC_CRC_OPT == SEC_CRC_SPEED_OPTIMIZED ) || \
    ( SEC_CRC_OPT == SEC_CRC_SIZE_OPTIMIZED )
#else
# error "Error in configuration: Either SEC_CRC_SPEED_OPTIMIZED or SEC_CRC_SIZE_OPTIMIZED must be defined"
#endif

/**********************************************************************************************************************
 *  MISRA
 *********************************************************************************************************************/

/* Module specific MISRA deviations:

   MD_SecCrc_3218:
      Reason: The configuration constants of this module are kept at a central location for a better overview and maintenance.
      Scope is larger than required (whole file instead of one function).
      Risk: No identifiable risk.
      Prevention: No prevention required.

   MD_SecCrc_3410:
      Reason: Helper macro to expand comma seperated list. Parameter cannot be enclosed in parentheses.
      Risk: No identifiable risk.
      Prevention: No prevention required.
*/

/***********************************************************************************************************************
 *  END OF FILE: SEC_CRC.C
 **********************************************************************************************************************/
