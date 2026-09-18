/**********************************************************************************************************************
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/** \file
 *  \brief        Configuration of the HIS security module - Seed/key authentication
 *
 *  \description  Offers challenge/response authentication interface
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

#ifndef __SEC_SEEDKEY_CFG_H__
#define __SEC_SEEDKEY_CFG_H__

/***********************************************************************************************************************
 *   VERSION
 **********************************************************************************************************************/

/* ##V_CFG_MANAGEMENT ##CQProject : SecAccess_His_Vector CQComponent : Implementation */
#define SECACCESS_HIS_VECTOR_VERSION            0x0100u
#define SECACCESS_HIS_VECTOR_RELEASE_VERSION    0x00u

/* Reference interface version */
/** Major interface version identifies incompatible changes */
#define SECACCESS_API_REFERENCE_VERSION_MAJOR      0x01u
/** Minor interface version identifies backward compatible changes */
#define SECACCESS_API_REFERENCE_VERSION_MINOR      0x00u
/** Release interface version identifies cosmetic changes */
#define SECACCESS_API_REFERENCE_VERSION_RELEASE    0x00u

/***********************************************************************************************************************
 *  DEFINES
 **********************************************************************************************************************/

/* --- Configuration of seed/key interface --- */

/** Seed length in bytes */
#define SEC_SEED_LENGTH       4u
/** Key length in bytes */
#define SEC_KEY_LENGTH        4u

/** Seed data stored as array of bytes */
/* #define SEC_BYTE_ARRAY_SEED */
/** Seed data stored as array of words */
/* #define SEC_WORD_ARRAY_SEED */
/** Key data stored as array of bytes */
/* #define SEC_BYTE_ARRAY_KEY */

/** Key generated using asymmetric algorithm */
/* #define SEC_ENABLE_SEED_KEY_ASYMMETRIC */

#endif /* __SEC_SEEDKEY_CFG_H__ */

/***********************************************************************************************************************
 *  END OF FILE: SEC_SEEDKEY_CFG.H
 **********************************************************************************************************************/
