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
 *                                                   Renamed file name (was SecMSK.c)
 *                                                   Split into source and library parts
 *                                                   Added extended seed/key API (externally provided key and workspace)
 *                                                   Added extended seed and key types
 *                                                   Unified handling of default and extended seed/key variants
 *                                                   API version defined at integration time
 *  02.00.01   2014-01-20  JHg     ESCAN00073010    Resolved compiler warnings
 *  02.01.00   2014-05-28  JHg     ESCAN00075895    No changes
 *  02.02.00   2014-09-18  JHg     ESCAN00078475    No changes
 *  02.03.00   2015-02-09  CB      ESCAN00079854    Changed DAG seed-key constants
 *  03.00.00   2015-11-03  JHg     ESCAN00086163    Vendor specific algorithms moved to callout SecM_ComputeKeyVendor
 *  03.01.00   2016-06-29  JHg     ESCAN00089876    Extended seed/key API changed to compile-time switch
 *                         JHg     ESCAN00090732    Added support for FIPS-186 PRNG
 *                                                  Added entropy pool with option for explicit passing of entropy
 *  03.01.01   2016-07-04  JHg     ESCAN00090820    Only pass entropy pool to random generator when configured
 *  03.01.02   2017-01-23  HRf     ESCAN00093667    No changes
 *  03.02.00   2017-05-09  JHg     ESCAN00096092    Added API for external key comparision
 *                         JHg     ESCAN00095087    No changes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

/* Security module configuration settings */
#include "Sec_Inc.h"       /* PRQA S 0883 */ /* MD_SecInc_0883 */

/* Global definitions for security module */
#include "Sec_Types.h"

/* Interfaces of seed-key */
#include "Sec_SeedKey.h"   /* PRQA S 0883 */ /* MD_SecInc_0883 */

#include "Sec.h"           /* PRQA S 0883 */ /* MD_SecInc_0883 */

/***********************************************************************************************************************
 *   VERSION
 **********************************************************************************************************************/

#if ( SYSSERVICE_SECMODHIS_SEEDKEY_VERSION != 0x0302u ) || \
    ( SYSSERVICE_SECMODHIS_SEEDKEY_RELEASE_VERSION != 0x00u )
# error "Error in SEC_SEEDKEY.C: Source and header file are inconsistent!"
#endif

/* Interface version compatibility check */
# if defined( SECACCESS_API_REFERENCE_VERSION_MAJOR ) && \
     defined( SECACCESS_API_REFERENCE_VERSION_MINOR )
# else
#   error "Error in Sec_SeedKey.c: Interface version requirements not defined!"
# endif

# if ( SECACCESS_API_REFERENCE_VERSION_MAJOR != SECACCESS_API_VERSION_MAJOR) || \
     ( SECACCESS_API_REFERENCE_VERSION_MINOR > SECACCESS_API_VERSION_MINOR)
#  error "Error in Sec_SeedKey.c: Interface version compatibility check failed!"
# endif

/**********************************************************************************************************************
 *  DEFINES
 *********************************************************************************************************************/

/* PRQA S 3453 TAG_SecSeeKey_3453_1 */ /* MD_CBD_19.7 */

#if defined( SEC_ENABLE_SEED_KEY_PRNG_LCG )
/** LCG parameters for random number generator (taken from BCPL language) */
# define SEC_LCG_RANDOM_MULTIPLIER  2147001325ul
# define SEC_LCG_RANDOM_INCREMENT    715136305ul
#endif /* SEC_ENABLE_SEED_KEY_PRNG_LCG */

/* Clear by default */
#if defined( SEC_COMPARE_SIZE )
# undef SEC_COMPARE_SIZE
#endif /* SEC_COMPARE_SIZE */

/* Adaptation of seed/key comparison in SecM_CompareKey */
#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
# if defined( SEC_BYTE_ARRAY_SEED )
/** Array size of compared seed */
#  define SEC_COMPARE_SIZE    SEC_SEED_LENGTH
# elif defined( SEC_WORD_ARRAY_SEED )
/** Array size of compared seed */
#  define SEC_COMPARE_SIZE    SEC_SEED_WORD_LENGTH
# endif /* SEC_<type>_ARRAY_SEED */
/** Variable to be compared with computed value */
# define SEC_COMPARE_VALUE    lastSeed
#else
# if defined( SEC_BYTE_ARRAY_KEY )
/** Array size of compared key */
#  define SEC_COMPARE_SIZE    SEC_KEY_LENGTH
# endif /* SEC_BYTE_ARRAY_KEY */
# if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API )
/** Variable member to be compared with computed value */
#  define SEC_COMPARE_VALUE   key.data
# else
/** Variable to be compared with computed value */
#  define SEC_COMPARE_VALUE   key
# endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */
#endif /* SEC_ASYMMETRIC_SEED_KEY */

/** Access seed parameter directly */
# define SEC_BASE_SEED(seed)  (seed)

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API )
/** Access base type of key parameter */
# define SEC_BASE_KEY(key)    ((key).data)
#else
/** Access key parameter directly */
# define SEC_BASE_KEY(key)    (key)
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */

#if defined( SEC_ENABLE_SEED_KEY_PRNG_LCG )
# define SEC_PRNG_POOL                                                                    /**< No entropy pool used */
/** Generation function of PRNG */
# define SEC_PRNG_GENERATE_RANDOM(pPool, pRandom, length)   SecM_GenerateRandomLcg((pRandom), (length))
#endif /* SEC_ENABLE_SEED_KEY_PRNG_ */

/* PRQA L:TAG_SecSeeKey_3453_1 */

/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 *********************************************************************************************************************/

/** Base type of seed/key comparison in SecM_CompareKey */
#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
typedef SecM_SeedStorageType  SecM_CompareType;
#else
typedef SecM_KeyStorageType   SecM_CompareType;
#endif /* SEC_ASYMMETRIC_SEED_KEY */

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 *********************************************************************************************************************/

#if defined( SEC_ENABLE_SEED_KEY_PRNG_LCG )
static SecM_WordType SecM_LcgRandom( SecM_WordType input );
static SecM_StatusType SecM_GenerateRandomLcg( V_MEMRAM1 SecM_ByteType V_MEMRAM2 V_MEMRAM3 * pRandom, SecM_LengthType length );
#endif /* SEC_ENABLE_SEED_KEY_PRNG_LCG */

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API ) 
# if defined( SEC_ENABLE_WORKSPACE_INTERNAL )
static void SecM_SetDefaultWorkspace( SecM_VoidPtrType pContext, SecM_WorkspacePtrType pWorkspace, SecM_LengthType workspaceSize );
# endif /* SEC_ENABLE_WORKSPACE_INTERNAL */
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */

/***********************************************************************************************************************
 *  LOCAL DATA
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 *********************************************************************************************************************/

#if defined( SEC_ENABLE_SEED_KEY_PRNG_LCG )
/***********************************************************************************************************************
 *  SecM_LcgRandom
 **********************************************************************************************************************/
/*! \brief       Pseudo random number generator based on a linear congruential generator (LCG)
 *  \details     Implementation uses a constant modulus value of 2^32
 *               Attention: The quality of the "randomness" highly depends on the choice of the multiplication and
 *               increment modifier.
 *  \param[in]   input Previous random number or seed value
 *  \return      Next random number
 **********************************************************************************************************************/
static SecM_WordType SecM_LcgRandom( SecM_WordType input )
{
   SecM_WordType  output;

   /* Multiplication may lead to overflow, making this equal to a modulo operation with 2^32 */
   output = (input * SEC_LCG_RANDOM_MULTIPLIER) + SEC_LCG_RANDOM_INCREMENT;

   return output;
}

/***********************************************************************************************************************
 *  SecM_GenerateRandomLcg
 **********************************************************************************************************************/
/*! \brief       Generate random numbers using LCG
 *  \details     Fills the provided array with pseudo random numbers
 *  \param[out]  pRandom Buffer for generated random numbers
 *  \param[in]   length Number of random bytes to be generated
 *  \return      SECM_OK     if generation was successful
 *               SECM_NOT_OK if generation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
static SecM_StatusType SecM_GenerateRandomLcg( V_MEMRAM1 SecM_ByteType V_MEMRAM2 V_MEMRAM3 * pRandom, SecM_LengthType length )
{
   SecM_ShortFastType   index;
   SecM_ShortFastType   remainder;
   SecM_ShortFastType   byteCount;
   SecM_WordType        randomWord;

   /* Limit seed value to size of word */
   byteCount = length;
   if (byteCount > SEC_WORD_TYPE_SIZE)
   {
      byteCount = SEC_WORD_TYPE_SIZE;
   }

   /* Initialize current random value with initial seed value (up to 4 bytes) */
   randomWord  = SecM_GetInteger(byteCount, pRandom);

   /* Remaining random bytes to be generated */
   remainder   = length;

   /* Initialize index into byte seed array */
   index       = 0u;

   /* Populate array starting with initial seed stored in random[0-3] */
   while (remainder > 0u)
   {
      /* Generate next pseudo random number */
      randomWord = SecM_LcgRandom(randomWord);

      /* Limit serialized length to remainder */
      if (remainder < SEC_WORD_TYPE_SIZE)
      {
         byteCount = remainder;
      }
      else
      {
         byteCount = SEC_WORD_TYPE_SIZE;
      }

      /* Store serialized word in output value */
      SecM_SetInteger(byteCount, randomWord, &pRandom[index]);

      index       += SEC_WORD_TYPE_SIZE;
      remainder   -= byteCount;  /* PRQA S 3382 */ /* MD_SecSeedKey_3382 */
   }

   /* Generation always succeeds */
   return SECM_OK;
}
#endif /* SEC_ENABLE_SEED_KEY_PRNG_LCG */

#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API ) 
# if defined( SEC_ENABLE_WORKSPACE_INTERNAL )
/***********************************************************************************************************************
 *  SecM_SetDefaultWorkspace
 **********************************************************************************************************************/
/*! \brief       Set the default workspace
 *  \details     Workspace referenced in passed context is either replaced unconditionally (SEC_DISABLE_WORKSPACE_EXTERNAL)
 *               or when size is set to SEC_DEFAULT_WORKSPACE_SIZE (SEC_ENABLE_WORKSPACE_EXTERNAL)
 *  \param[in,out] pContext Pointer to key parameter structure derived from SecM_ComputeKeyParamType
 *  \param[in]   pWorkspace Pointer to default workspace
 *  \param[in]   workspaceSize Size of default workspace
 **********************************************************************************************************************/
static void SecM_SetDefaultWorkspace( SecM_VoidPtrType pContext, SecM_WorkspacePtrType pWorkspace, SecM_LengthType workspaceSize )
{
   V_MEMRAM1 SecM_SeedKeyContextType V_MEMRAM2 V_MEMRAM3 * pKeyParam;

   /* Convert void pointer to base type for key parameter structures */
   pKeyParam = (V_MEMRAM1 SecM_SeedKeyContextType V_MEMRAM2 V_MEMRAM3 *)pContext;

#  if defined( SEC_ENABLE_WORKSPACE_EXTERNAL )
   /* Only overwrite workspace when not explicitly set by caller */
   if (SEC_DEFAULT_WORKSPACE_SIZE == pKeyParam->workspace.size)
#  endif /* SEC_ENABLE_WORKSPACE_EXTERNAL */
   {
      /* Store default workspace */
      pKeyParam->workspace.data = pWorkspace;
      pKeyParam->workspace.size = workspaceSize;
   }
}
# endif /* SEC_ENABLE_WORKSPACE_INTERNAL */
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API || SEC_ENABLE_SEED_KEY_ENTROPY_POOL */

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

/***********************************************************************************************************************
 *  SecM_GenerateSeed
 **********************************************************************************************************************/
/*! \brief       Calculates random seed value
 *  \detail      Uses a pseudo random number generator and an initial seed to generate a seed value
 *               In case of HIS specification 1.0.1 or earlier the last seed is stored in an internal structure.
 *  \pre         Implicit entropy: The values of seedX and seedY (default API), seed[0-3] (byte array) or seed[0] (word array)
 *                should have a random start value
 *               Explicit entropy: Entropy pool passed in seed parameter has to be initialized and must hold at least
 *                one byte of entropy
 *  \param[in,out]  Pointer to seed, where the random values shall be stored
 *  \return      SECM_OK     if generation was successful
 *               SECM_NOT_OK if generation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
SecM_StatusType SecM_GenerateSeed( V_MEMRAM1 SecM_SeedType V_MEMRAM2 V_MEMRAM3 * seed ) /* PRQA S 3673 */ /* MD_SecSeedKey_3673_1 */
{
   SecM_StatusType result;
   V_MEMRAM1 SecM_SeedBaseType V_MEMRAM2 V_MEMRAM3 * pBaseSeed;
   V_MEMRAM1 SecM_ByteType V_MEMRAM2 V_MEMRAM3 * pRandom;
#if defined( SEC_BYTE_ARRAY_SEED )
#else
   SecM_ByteType        randomBuffer[SEC_INITIAL_SEED_LENGTH];
# if defined( SEC_WORD_ARRAY_SEED )
   SecM_ShortFastType   index;
# endif /* SEC_WORD_ARRAY_SEED */
#endif /* SEC_BYTE_ARRAY_SEED */

   /* Access seed base type through local variable */
   pBaseSeed = (V_MEMRAM1 SecM_SeedBaseType V_MEMRAM2 V_MEMRAM3 *)seed;

#if defined( SEC_BYTE_ARRAY_SEED )
   /* Directly store generated random numbers in seed buffer */
   pRandom = pBaseSeed;
#else
   /* Store generated random numbers in temporary buffer */
   pRandom = randomBuffer;

   /* Serialize implicit seed provided as input into temporary buffer */
#  if defined( SEC_WORD_ARRAY_SEED )
   SecM_SetInteger(SEC_INITIAL_SEED_LENGTH, pBaseSeed[0], randomBuffer);
#  else
   SecM_SetInteger(SEC_WORD_TYPE_SIZE, pBaseSeed->seedX, &randomBuffer[0u]);
   SecM_SetInteger(SEC_WORD_TYPE_SIZE, pBaseSeed->seedY, &randomBuffer[SEC_WORD_TYPE_SIZE]);
#  endif /* SEC_WORD_ARRAY_SEED */
#endif /* SEC_BYTE_ARRAY_SEED */

   {
#if defined( SEC_BYTE_ARRAY_SEED )
      /* Fill array with pseudo random numbers */
      result = SEC_PRNG_GENERATE_RANDOM(SEC_PRNG_POOL, pRandom, SEC_SEED_LENGTH);

#elif defined( SEC_WORD_ARRAY_SEED )
      /* Populate seed array */
      for (index = 0u; index < SEC_SEED_WORD_LENGTH; index++)
      {
         /* Generate next pseudo random number */
         result = SEC_PRNG_GENERATE_RANDOM(SEC_PRNG_POOL, pRandom, SEC_WORD_TYPE_SIZE);

         /* Store in output value */
         pBaseSeed[index]  = SecM_GetInteger(SEC_WORD_TYPE_SIZE, pRandom);

         if (SECM_OK != result)
         {
            break;
         }
      }
#else
      /* Generate pseudo random numbers */
      result = SEC_PRNG_GENERATE_RANDOM(SEC_PRNG_POOL, pRandom, SEC_WORD_TYPE_SIZE);
      pBaseSeed->seedX = SecM_GetInteger(SEC_WORD_TYPE_SIZE, pRandom);

      if (SECM_OK == result)
      {
         /* Generate pseudo random numbers */
         result = SEC_PRNG_GENERATE_RANDOM(SEC_PRNG_POOL, pRandom, SEC_WORD_TYPE_SIZE);
         pBaseSeed->seedY = SecM_GetInteger(SEC_WORD_TYPE_SIZE, pRandom);

      }
#endif /* SEC_<type>_ARRAY_SEED */

      if (SECM_OK == result)
      {
         /* Start validity timer of seed if applicable */
         SecM_StartKeyTimer();
      }
   }

   return result;
}

#if defined( SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION )
#else
# if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
/***********************************************************************************************************************
 *  SecM_ComputeKey
 **********************************************************************************************************************/
/*! \brief       Calculates a seed value based on the provided key and constant value
 *  \details     API is a deviation from the HIS specification
 *               Modified to support asymmetric key calculation where only the tester is able to generate the key from
 *               the original seed value, e.g. using a private key.
 *  \pre         Function SecM_GenerateSeed must have been called at least once.
 *  \param[in]   inputKey The received key on which calculation shall be based
 *  \param[in]   constant A fixed constant used by the key calculation algorithm
 *                        Contains additional parameters (workspace, secret key) in case of extended API
 *  \param[out]  computedSeed Pointer to resulting seed as a formula of seed = f(key,k)
 *  \return      SECM_OK     if calculation was successful
 *               SECM_NOT_OK if calculation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeKey( SecM_KeyStorageType inputKey, SecM_ConstType constant,
   V_MEMRAM1 SecM_SeedStorageType V_MEMRAM2 V_MEMRAM3 * computedSeed )
{
   SecM_StatusType result;

   result = SecM_ComputeKeyVendor(inputKey, constant, computedSeed);

   return result;
}
# else
/***********************************************************************************************************************
 *  SecM_ComputeKey
 **********************************************************************************************************************/
/*! \brief       Calculates a key value based on the provided seed and constant value
 *  \pre         Function SecM_GenerateSeed must have been called at least once.
 *  \param[in]   inputSeed The random seed the key calculation shall be based on
 *  \param[in]   constant A fixed constant used by the key calculation algorithm
 *                        Contains additional parameters (workspace, secret key) in case of extended API
 *  \param[out]  computedKey Pointer to resulting key data as a formula of key = f(seed,k)
 *  \return      SECM_OK     if calculation was successful
 *               SECM_NOT_OK if calculation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeKey( SecM_SeedStorageType inputSeed, SecM_ConstType constant,
   V_MEMRAM1 SecM_KeyStorageType V_MEMRAM2 V_MEMRAM3 * computedKey )
{
   SecM_StatusType result;

   result = SecM_ComputeKeyVendor(inputSeed, constant, computedKey);

   return result;
}
# endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */
#endif /* SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION */

/***********************************************************************************************************************
 *  SecM_CompareKey
 **********************************************************************************************************************/
/*! \brief       Compare received key against computed one
 *  \details     Runs the key calculation and compares the calculated key against the received key
 *  \pre         Function SecM_GenerateSeed must have been called before.
 *  \param[in]   key Key for the authorisation
 *                   Contains additional parameters (workspace, secret key) in case of extended API
 *  \param[in]   lastSeed Start value (seed) for the authorisation
 *  \return      SECM_OK if key calculation and comparison was successful
 *               SECM_NOT_OK if key calculation failed or key mismatch
 **********************************************************************************************************************/
SecM_StatusType SecM_CompareKey( SecM_KeyType key, SecM_SeedType lastSeed ) /* PRQA S 3673 */ /* MD_SecSeedKey_3673_1 */
{
   SecM_StatusType      result;
   SecM_ConstType       keyConstant;
#if defined( SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION )
#else
   SecM_CompareType     computeVal;
# if defined( SEC_COMPARE_SIZE )
   SecM_ShortFastType   index;
# endif /* SEC_COMPARE_SIZE */

/* Clear contents computed value */
# if defined( SEC_COMPARE_SIZE )
   for (index = 0u; index < SEC_COMPARE_SIZE; index++)
   {
      computeVal[index] = 0u;
   }
# else
   computeVal = 0u;
# endif /* SEC_COMPARE_SIZE */
#endif /* SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION */

   /* Initialize local variables */
   result = SECM_NOT_OK;

   /* Verify validity of seed if applicable */
   if (SecM_GetKeyTimer() > 0u)
   {
#if defined( SEC_ENABLE_SEED_KEY_EXTENDED_API )
      /* Extended API used */
      /* Pass configured key constant */
      keyConstant.constant = SEC_ECU_KEY;
      /* Pass context required for calculation
         Move from key as this parameter is usually not available in SecM_ComputeKey */
      keyConstant.context  = key.context;
#else
      /* Pass configured key constant */
      keyConstant = SEC_ECU_KEY;
#endif /* SEC_ENABLE_SEED_KEY_EXTENDED_API */

      /* Run key calculation */
#if defined( SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION )
      result = SecM_CompareKeyVendor(SEC_BASE_KEY(key), SEC_BASE_SEED(lastSeed), keyConstant);
#else
# if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
      result = SecM_ComputeKey(SEC_BASE_KEY(key), keyConstant, &computeVal);
# else
      result = SecM_ComputeKey(SEC_BASE_SEED(lastSeed), keyConstant, &computeVal);
# endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */

      /* Compare provided value against computed value
         Perform full comparison regardless of current result to prevent timing analysis */
# if defined( SEC_COMPARE_SIZE )
      for (index = 0u; index < SEC_COMPARE_SIZE; index++)
      {
         if (computeVal[index] != SEC_COMPARE_VALUE[index])
         {
            /* Comparison failed report failure */
            result = SECM_NOT_OK;
         }
      }
# else
      if (computeVal != SEC_COMPARE_VALUE)
      {
         /* Comparison failed report failure */
         result = SECM_NOT_OK;
      }
# endif /* SEC_COMPARE_SIZE */
#endif /* SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION */

      /* Invalidate seed validity */
      SecM_StopKeyTimer();
   }

#if defined( SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION )
#else
   /* Destroy temporary key (un-spy) */
# if defined( SEC_COMPARE_SIZE )
   for (index = 0u; index < SEC_COMPARE_SIZE; index++)
   {
      computeVal[index] = 0u;
   }
# else
   computeVal = 0u;
# endif
#endif /* SEC_ENABLE_SEED_KEY_EXTERNAL_COMPARISION */

   return result;
}

/**********************************************************************************************************************
 *  MISRA
 *********************************************************************************************************************/

/* Module specific MISRA deviations:

   MD_SecSeedKey_3382:
     Reason: Subtraction from unsigned value without explicit range check directly before operation.
      Instead limitation of subtracted value is located above as it is required before.
     Risk: Distance between limitation and subtraction makes correlation less obvious.
     Prevention: Ensure correct limitation of subtracted value through code inspection.

  MD_SecSeedKey_3673_1:
     Reason: Parameter or parts of it are actually modified inside the calling hierarchy.
     Risk: No identifiable risk.
     Prevention: No prevention required.

*/

/***********************************************************************************************************************
 *  END OF FILE: SEC_SEEDKEY.C
 **********************************************************************************************************************/
