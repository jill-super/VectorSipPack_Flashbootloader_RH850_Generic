/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        Implementation of the HIS security module - Seed/key authentication
 *
 *  \description  Offers challenge/response authentication interface
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
 *  Christian Baeuerle            CB            Vector Informatik GmbH
 *  Ralf Haegenlaeuer             HRf           Vector Informatik GmbH
 *  --------------------------------------------------------------------------------------------------------------------
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Version    Date        Author  Change Id        Description
 *  --------------------------------------------------------------------------------------------------------------------
 *  01.00.00   2013-06-20  JHg     ESCAN00068213    Initial release based on FblSecMod_Vector 02.02.00
 *  02.00.00   2013-11-08  JHg     ESCAN00071177    Major refactoring
 *                                                   Moved to separate sub-package
 *                                                   Renamed file name (was SecMSK.h)
 *                                                   Split into source and library parts
 *                                                   Added extended seed/key API (externally provided key and workspace)
 *                                                   Added extended seed and key types
 *                                                   Unified handling of default and extended seed/key variants
 *                                                   API version defined at integration time
 *  02.00.01   2014-01-20  JHg     ESCAN00073010    No changes
 *  02.01.00   2014-05-28  JHg     ESCAN00075895    No changes
 *  02.02.00   2014-09-18  JHg     ESCAN00078475    Support for generic HMAC SHA-1 based seed/key
 *  02.03.00   2015-02-09  CB      ESCAN00079854    No changes
 *  03.00.00   2015-11-03  JHg     ESCAN00086163    Vendor specific algorithms moved to callout SecM_ComputeKeyVendor
 *  03.01.00   2016-06-29  JHg     ESCAN00089876    Extended seed/key API changed to compile-time switch
 *                                                   Updated interface version
 *                         JHg     ESCAN00090732    Added support for FIPS-186 PRNG
 *                                                  Added entropy pool with option for explicit passing of entropy
 *  03.01.01   2016-07-04  JHg     ESCAN00090820    Only pass entropy pool to random generator when configured
 *  03.01.02   2017-01-23  HRf     ESCAN00093667    Change filter rule to support use-case with FIPS-186
 *  03.02.00   2017-05-09  JHg     ESCAN00096092    Added API for external key comparision
 *                         JHg     ESCAN00095087    No changes
 **********************************************************************************************************************/

#ifndef __SEC_SEEDKEY_H__
#define __SEC_SEEDKEY_H__

/***********************************************************************************************************************
 *   VERSION
 **********************************************************************************************************************/

/* ##V_CFG_MANAGEMENT ##CQProject : SysService_SecModHis CQComponent : Impl_SeedKey */
#define SYSSERVICE_SECMODHIS_SEEDKEY_VERSION             0x0302u
#define SYSSERVICE_SECMODHIS_SEEDKEY_RELEASE_VERSION     0x00u

/* Interface version */
/** Major interface version identifies incompatible changes */
#define SECACCESS_API_VERSION_MAJOR       0x01u
/** Minor interface version identifies backward compatible changes */
#define SECACCESS_API_VERSION_MINOR       0x02u
/** Release interface version identifies cosmetic changes */
#define SECACCESS_API_VERSION_RELEASE     0x00u

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

#include "Sec_Inc.h"

# include "Sec_SeedKey_Cfg.h"

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* Default values for 32 bit seed and key */
# if defined( SEC_SEED_LENGTH )
# else
/** Seed length in bytes */
#  define SEC_SEED_LENGTH           0x04u
# endif /* SEC_SEED_LENGTH */

# if defined( SEC_KEY_LENGTH )
# else
/** Key length in bytes */
#  define SEC_KEY_LENGTH            0x04u
# endif /* SEC_KEY_LENGTH */

#if defined( SEC_BYTE_ARRAY_SEED )
/** Length of initial seed value */
# if (SEC_SEED_LENGTH < SEC_WORD_TYPE_SIZE)
#  define SEC_INITIAL_SEED_LENGTH   SEC_SEED_LENGTH
# else
#  define SEC_INITIAL_SEED_LENGTH   SEC_WORD_TYPE_SIZE
# endif /* SEC_SEED_LENGTH < SEC_WORD_TYPE_SIZE */
#elif defined( SEC_WORD_ARRAY_SEED )
# define SEC_INITIAL_SEED_LENGTH    SEC_WORD_TYPE_SIZE
/** Convert seed length in bytes to length in words */
# define SEC_SEED_WORD_LENGTH       (((SEC_SEED_LENGTH - 1u) / SEC_WORD_TYPE_SIZE) + 1u)
#else
# define SEC_INITIAL_SEED_LENGTH    (SEC_WORD_TYPE_SIZE * 2u)
#endif /* SEC_<type>_ARRAY_SEED */

/* Pseudo random number generator */

#if defined( SEC_PRNG_LCG )
#else
/** Use linear congruential generator (LCG) for pseudo random number generation  */
# define SEC_PRNG_LCG               0u
#endif /* SEC_PRNG_LCG */
#if defined( SEC_PRNG_FIPS186 )
#else
/** Use FIPS-186 for pseudo random number generation  */
# define SEC_PRNG_FIPS186           (SEC_PRNG_LCG + 1u)
#endif /* SEC_PRNG_FIPS186 */

#if defined( SEC_PRNG )
#else
/** Algorithm used for pseudo random number generation */
#  define SEC_PRNG                  SEC_PRNG_LCG
#endif /* SEC_PRNG */

#if ( SEC_PRNG == SEC_PRNG_FIPS186 )
/** Use FIPS-186 for pseudo random number generation  */
# define SEC_ENABLE_SEED_KEY_PRNG_FIPS186
#elif ( SEC_PRNG == SEC_PRNG_LCG )
/** Use linear congruential generator (LCG) for pseudo random number generation  */
# define SEC_ENABLE_SEED_KEY_PRNG_LCG
#endif /* SEC_PRNG */

/**********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

/* Supporting data types for easier handling */

#if defined( SEC_BYTE_ARRAY_KEY )
/* This is a deviation from the HIS specification! */
/** Base type for key data */
typedef SecM_ByteType      SecM_KeyBaseType;
/** Type for actual key data storage */
typedef SecM_KeyBaseType   SecM_KeyStorageType[SEC_KEY_LENGTH];
#else
/* According to HIS specification */
/** Base type for key data */
typedef SecM_WordType      SecM_KeyBaseType;
/** Type for actual key data storage */
typedef SecM_KeyBaseType   SecM_KeyStorageType;
#endif /* SEC_BYTE_ARRAY_KEY */

#if defined( SEC_BYTE_ARRAY_SEED )
/* This is a deviation from the HIS specification! */
/** Base type for seed data */
typedef SecM_ByteType      SecM_SeedBaseType;
/** Type for seed values */
typedef SecM_SeedBaseType  SecM_SeedStorageType[SEC_SEED_LENGTH];
#elif defined( SEC_WORD_ARRAY_SEED )
/* This is a deviation from the HIS specification! */
/** Base type for seed data */
typedef SecM_WordType      SecM_SeedBaseType;
/** Type for seed values */
typedef SecM_SeedBaseType  SecM_SeedStorageType[SEC_SEED_WORD_LENGTH];
#else
/* According to HIS specification */
/** Base type for key constant */
typedef struct
{
   SecM_WordType seedX;
   SecM_WordType seedY;
} SecM_SeedBaseType;
/** Type for actual key data storage */
typedef SecM_SeedBaseType  SecM_SeedStorageType;
#endif /* SEC_<type>_ARRAY_SEED */

typedef SecM_WordType      SecM_ConstBaseType;

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API ) 
/** Common information required by extended API */
typedef struct
{
   /** Workspace required by key computation */
   SecM_WorkspaceType   workspace;
   /** Watchdog trigger function */
   FL_WDTriggerFctType  wdTriggerFct;
} SecM_SeedKeyContextType;
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API || SEC_ENABLE_SEED_KEY_EXPLICIT_ENTROPY */

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API )
/* Extended seed/key API */

typedef SecM_SeedKeyContextType  SecM_ComputeKeyParamType;

/** Information for symmetric key algorithm */
typedef struct
{
   /** Common information */
   SecM_ComputeKeyParamType   common;
   /** Secret key */
   SecM_SymKeyType            secretKey;
} SecM_ComputeKeySymParamType;

/** Information for asymmetric key algorithm */
typedef struct
{
   /** Common information */
   SecM_ComputeKeyParamType   common;
   /** Public key pair */
   SecM_AsymKeyType           publicKey;
} SecM_ComputeKeyAsymParamType;

/* This is a deviation from the HIS specification! */
/** Type for key constants */
typedef struct
{
   /** Constant for key calculation */
   SecM_ConstBaseType   constant;
   /** Additional context information, e.g. workspace */
   SecM_VoidPtrType     context;
} SecM_ConstType;
#else
/** Type for key constants */
typedef SecM_ConstBaseType SecM_ConstType;
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */

/*********************************************************************************************************************/

/* Types specified by HIS specification */

/** Type for seed values */
typedef SecM_SeedStorageType  SecM_SeedType;

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API )
/* Extended seed/key API */

/* This is a deviation from the HIS specification! */
/** Type for key values */
typedef struct
{
   /** Pointer to actual key data */
   V_MEMRAM1 SecM_KeyBaseType V_MEMRAM2 V_MEMRAM3 *   data;
   /** Additional context information, e.g. workspace */
   SecM_VoidPtrType                                   context;
} SecM_KeyType;
#else
/** Type for key values */
typedef SecM_KeyStorageType   SecM_KeyType;
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */

/* Pointer types for API-functions */
typedef SecM_StatusType SEC_CALL_TYPE (* pSecGenerateSeedFct)( V_MEMRAM1 SecM_SeedType V_MEMRAM2 V_MEMRAM3 * seed );
#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
typedef SecM_StatusType SEC_CALL_TYPE (* pSecComputeKeyFct)( SecM_KeyStorageType inputKey, SecM_ConstType constant,
   V_MEMRAM1 SecM_SeedStorageType V_MEMRAM2 V_MEMRAM3 * computedSeed );
#else
typedef SecM_StatusType SEC_CALL_TYPE (* pSecComputeKeyFct)( SecM_SeedStorageType inputSeed, SecM_ConstType constant,
   V_MEMRAM1 SecM_KeyStorageType V_MEMRAM2 V_MEMRAM3 * computedKey );
#endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */
typedef SecM_StatusType SEC_CALL_TYPE (* pSecCompareKeyFct)( SecM_KeyType key, SecM_SeedType lastSeed );

/**********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

#if defined( __cplusplus )
extern "C" {
#endif

/* Functions specified by HIS specification */

extern SecM_StatusType SecM_GenerateSeed( V_MEMRAM1 SecM_SeedType V_MEMRAM2 V_MEMRAM3 * seed );
#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
extern SecM_StatusType SecM_ComputeKey( SecM_KeyStorageType inputKey, SecM_ConstType constant,
   V_MEMRAM1 SecM_SeedStorageType V_MEMRAM2 V_MEMRAM3 * computedSeed );
#else
extern SecM_StatusType SecM_ComputeKey( SecM_SeedStorageType inputSeed, SecM_ConstType constant,
   V_MEMRAM1 SecM_KeyStorageType V_MEMRAM2 V_MEMRAM3 * computedKey );
#endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */
extern SecM_StatusType SecM_CompareKey( SecM_KeyType key, SecM_SeedType lastSeed );

/*********************************************************************************************************************/

/* Library functions */
# if defined( SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION )
extern SecM_StatusType SecM_CompareKeyVendor( SecM_KeyStorageType key, SecM_SeedStorageType seed, SecM_ConstType constant );
# else
#  if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
extern SecM_StatusType SecM_ComputeKeyVendor( SecM_KeyStorageType inputKey, SecM_ConstType constant,
   V_MEMRAM1 SecM_SeedStorageType V_MEMRAM2 V_MEMRAM3 * computedSeed );
#  else
extern SecM_StatusType SecM_ComputeKeyVendor( SecM_SeedStorageType inputSeed, SecM_ConstType constant,
   V_MEMRAM1 SecM_KeyStorageType V_MEMRAM2 V_MEMRAM3 * computedKey );
#  endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */
# endif /* SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION */

#if defined( __cplusplus )
} /* extern "C" */
#endif

/***********************************************************************************************************************
 *  CONFIGURATION CHECKS
 **********************************************************************************************************************/

#if defined( SEC_WORD_ARRAY_SEED )
# if ( (SEC_SEED_LENGTH % 4u) != 0u )
#  error "Error in configuration: Seed lengths (in bytes) has to be a multiple of 4!"
# endif
#endif /* SEC_WORD_ARRAY_SEED */

#if defined( SEC_BYTE_ARRAY_SEED ) && \
    defined( SEC_WORD_ARRAY_SEED )
# error "Error in configuration: Both SEC_BYTE_ARRAY_SEED and SEC_WORD_ARRAY_SEED set!"
#endif /* SEC_BYTE_ARRAY_SEED && SEC_WORD_ARRAY_SEED*/

#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
# if defined( SEC_BYTE_ARRAY_SEED ) || \
     defined( SEC_WORD_ARRAY_SEED )
# else
#  error "Error in configuration: Asymmetric seed/key calculation only supports seed data stored as array!"
# endif /* SEC_BYTE_ARRAY_SEED || SEC_WORD_ARRAY_SEED */
#endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */

#endif /* __SEC_SEEDKEY_H__ */

/***********************************************************************************************************************
 *  END OF FILE: SEC_SEEDKEY.H
 **********************************************************************************************************************/
