/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        Implementation of the HIS security module - Seed/key authentication
 *
 *  \description  Implements OEM specific challenge/response scheme
 *  -------------------------------------------------------------------------------------------------------------------
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \par Copyright
 *  \verbatim
 *  Copyright (c) 2015 by Vector Informatik GmbH.                                                  All rights reserved.
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
 *  01.00.00   2015-11-03  JHg     ESCAN00086209    Initial release
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/

/* Security module interfaces */
#include "Sec.h"

/***********************************************************************************************************************
 *   VERSION
 **********************************************************************************************************************/

#if ( SECACCESS_HIS_VECTOR_VERSION != 0x0100u ) || \
    ( SECACCESS_HIS_VECTOR_RELEASE_VERSION != 0x00u )
# error "Error in SEC_SEEDKEYVENDOR.C: Source and header file are inconsistent!"
#endif

/***********************************************************************************************************************
 *  CONFIGURATION CHECKS
 **********************************************************************************************************************/

#if defined( SEC_BYTE_ARRAY_SEED ) || \
    defined( SEC_WORD_ARRAY_SEED )
/* Extended API with byte/word array for seed and byte array for key used */
# if defined( SEC_BYTE_ARRAY_KEY )
#  if ( SEC_SEED_LENGTH != SEC_KEY_LENGTH )
#   error "Only identical seed and key lengths (in bytes) are supported by the example code!"
#  endif
# else
#  error "Only a combination of array for both seed and key is supported by the example code!"
# endif /* SEC_BYTE_ARRAY_KEY */
#else
/* Standard HIS API used */
# if defined( SEC_BYTE_ARRAY_KEY )
#  error "Only a combination of array for both seed and key is supported by the example code!"
# else
#  if ( SEC_SEED_LENGTH != 4u ) && \
      ( SEC_SEED_LENGTH != 8u )
#   error "Only seed lengths (in bytes) of 4 or 8 are supported by the example code!"
#  endif /* SEC_SEED_LENGTH */
#  if ( SEC_KEY_LENGTH != 4u )
#   error "Only a key length (in bytes) of 4 is supported by the example code!"
#  endif /* SEC_KEY_LENGTH */
# endif /* SEC_BYTE_ARRAY_KEY */
#endif /* SEC_<type>_ARRAY_SEED */

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 *********************************************************************************************************************/

#if defined( SEC_ENABLE_SEED_KEY_ASYMMETRIC )
/***********************************************************************************************************************
 *  SecM_ComputeKeyVendor
 **********************************************************************************************************************/
/*! \brief       Calculates a seed value based on the provided key and constant value
 *  \details     API is a deviation from the HIS specification
 *               Modified to support asymmetric key calculation where only the tester is able to generate the key from
 *               the original seed value, e.g. using a private key.
 *  \param[in]   inputKey The received key on which calculation shall be based
 *  \param[in]   constant A fixed constant used by the key calculation algorithm
 *  \param[out]  computedSeed Pointer to resulting seed as a formula of seed = f(key,k)
 *  \return      SECM_OK     if calculation was successful
 *               SECM_NOT_OK if calculation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeKeyVendor( SecM_KeyType inputKey, SecM_ConstType constant, /* PRQA S 3673 */ /* MD_SecSeedKeyVendor_3673 */
   V_MEMRAM1 SecM_SeedStorageType V_MEMRAM2 V_MEMRAM3 * computedSeed )                 /* PRQA S 3673 */ /* MD_SecSeedKeyVendor_3673 */
{
   SecM_StatusType   result;
   SecM_ByteFastType seedIndex;
   V_MEMRAM1 SecM_SeedBaseType V_MEMRAM2 V_MEMRAM3 * seedOutput;
# if defined( SEC_BYTE_ARRAY_SEED )
   SecM_ByteType     byteConstant[SEC_WORD_TYPE_SIZE];
# elif defined( SEC_WORD_ARRAY_SEED )
   SecM_WordType     temp;
   SecM_ByteFastType keyIndex;
# endif /* SEC_BYTE_ARRAY_SEED */

   result = SECM_OK;

   /* Dereference output seed for easier access */
   seedOutput  = *computedSeed;

   /* Implementation example for key calculation */
# if defined( SEC_BYTE_ARRAY_SEED )
   /* Extended API with byte arrays for seed and key used */

   /* Serialize key constant (up to 4 bytes) */
   SecM_SetInteger(SEC_INITIAL_SEED_LENGTH, constant, byteConstant);

   /* Loop all seed bytes */
   for (seedIndex = 0u; seedIndex < SEC_SEED_LENGTH; seedIndex++)
   {
      /* Bit-wise XOR of each seed byte with key constant (MSB first) */
      seedOutput[seedIndex] = inputKey[seedIndex] ^ byteConstant[seedIndex & (SEC_WORD_TYPE_SIZE - 1u)];
   }
# elif defined( SEC_WORD_ARRAY_SEED )
   /* Extended API with word array for seed and byte array for key used */

   /* Index into key byte array */
   keyIndex    = 0u;

   /* Loop all seed words */
   for (seedIndex = 0u; seedIndex < SEC_SEED_WORD_LENGTH; seedIndex++)
   {
      /* De-serialize big-endian byte array into 32-bit value */
      temp = SecM_GetInteger(SEC_WORD_TYPE_SIZE, &inputKey[keyIndex]);

      /* Move to next part of key */
      keyIndex += SEC_WORD_TYPE_SIZE;

      /* Bit-wise XOR of each 4 byte of word with key constant */
      seedOutput[seedIndex] = temp ^ constant;
   }
# endif /* SEC_<type>_ARRAY_SEED */

   return result;
}
#else /* SEC_DISABLE_SEED_KEY_ASYMMETRIC */
/***********************************************************************************************************************
 *  SecM_ComputeKeyVendor
 **********************************************************************************************************************/
/*! \brief       Calculates a key value based on the provided seed and constant value
 *  \param[in]   inputSeed The random seed the key calculation shall be based on
 *  \param[in]   constant A fixed constant used by the key calculation algorithm
 *  \param[out]  computedKey Pointer to resulting key data as a formula of key = f(seed,k)
 *  \return      SECM_OK     if calculation was successful
 *               SECM_NOT_OK if calculation has failed (e.g. wrong parameters)
 **********************************************************************************************************************/
SecM_StatusType SecM_ComputeKeyVendor( SecM_SeedType inputSeed, SecM_ConstType constant,  /* PRQA S 3673 */ /* MD_SecSeedKeyVendor_3673 */
   V_MEMRAM1 SecM_KeyStorageType V_MEMRAM2 V_MEMRAM3 * computedKey )                      /* PRQA S 3673 */ /* MD_SecSeedKeyVendor_3673 */
{
   SecM_StatusType   result;
# if defined( SEC_BYTE_ARRAY_KEY )
   SecM_ByteFastType seedIndex;
   V_MEMRAM1 SecM_KeyBaseType V_MEMRAM2 V_MEMRAM3 * keyOutput;
# endif /* SEC_BYTE_ARRAY_KEY */
# if defined( SEC_BYTE_ARRAY_SEED )
   SecM_ByteType     byteConstant[SEC_WORD_TYPE_SIZE];
# elif defined( SEC_WORD_ARRAY_SEED )
   SecM_WordType     temp;
   SecM_ByteFastType keyIndex;
# endif /* SEC_BYTE_ARRAY_SEED */

   result = SECM_OK;

   /* Implementation example for key calculation */
# if defined( SEC_BYTE_ARRAY_SEED )
   /* Extended API with byte arrays for seed and key used */

   /* Dereference output key for easier access */
   keyOutput   = *computedKey;
   /* Serialize key constant (up to 4 bytes) */
   SecM_SetInteger(SEC_INITIAL_SEED_LENGTH, constant, byteConstant);

   /* Loop all seed bytes */
   for (seedIndex = 0u; seedIndex < SEC_SEED_LENGTH; seedIndex++)
   {
      /* Bit-wise XOR of each seed byte with key constant (MSB first) */
      keyOutput[seedIndex] = inputSeed[seedIndex] ^ byteConstant[seedIndex & (SEC_WORD_TYPE_SIZE - 1u)];
   }
# elif defined( SEC_WORD_ARRAY_SEED )
   /* Extended API with word array for seed and byte array for key used */

   /* Dereference output key for easier access */
   keyOutput   = *computedKey;
   /* Index into key byte array */
   keyIndex    = 0u;

   /* Loop all seed words */
   for (seedIndex = 0u; seedIndex < SEC_SEED_WORD_LENGTH; seedIndex++)
   {
      /* Bit-wise XOR of each seed word with key constant */
      temp = inputSeed[seedIndex] ^ constant;

      /* Serialize 32 bit value into big-endian byte array */
      SecM_SetInteger(SEC_WORD_TYPE_SIZE, temp, &keyOutput[keyIndex]);

      /* Move to next part of key */
      keyIndex += SEC_WORD_TYPE_SIZE;
   }
# else
   /* Standard HIS API used */
#  if ( SEC_SEED_LENGTH == 4u )
   /* Bit-wise XOR of x-value of seed and key constant */
   *computedKey = inputSeed.seedX ^ constant;
#  else /* SEC_SEED_LENGTH == 8u */
   /* Bit-wise XOR of both seed values and key constant */
   *computedKey = inputSeed.seedX ^ inputSeed.seedY ^ constant;
#  endif /* SEC_SEED_LENGTH */
# endif /* SEC_<type>_ARRAY_SEED */

   return result;
}
#endif /* SEC_ENABLE_SEED_KEY_ASYMMETRIC */

/**********************************************************************************************************************
 *  MISRA
 *********************************************************************************************************************/

/* Module specific MISRA deviations:

  MD_SecSeedKeyVendor_3673:
     Reason: API defined by HIS specification.
     Risk: No identifiable risk.
     Prevention: No prevention required.

*/

/***********************************************************************************************************************
 *  END OF FILE: SEC_SEEDKEYVENDOR.C
 **********************************************************************************************************************/
