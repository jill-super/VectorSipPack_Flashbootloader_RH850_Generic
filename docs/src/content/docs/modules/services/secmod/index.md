---
title: "Security Module (SecM)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · HIS Security Module · `BSW/SecMod/`

## Purpose and responsibility

The security module in [`BSW/SecMod/`](%REPO_URL%/tree/main/BSW/SecMod) implements the HIS security API used during reprogramming: **seed/key authentication** (SecurityAccess), **CRC/checksum computation** over downloaded data, and **signature verification** of the image before the bootloader accepts it. `SecM.h` is kept as a backward-compatibility header over the `Sec_*` implementation.

## Key files

| File(s) | Responsibility |
|---|---|
| `SecM.h`, `SecM_Inc.h` | Public/compatibility interface |
| `Sec.h`, `Sec_Inc.h`, `Sec_Types.h`, `Sec.c` | Core types and implementation |
| `Sec_Verification.h/.c` | Verification state machine: `SecM_InitVerification`, `SecM_Verification`, `SecM_VerifySignature`, vendor/DDD classes |
| `Sec_Crc.h/.c` | CRC engine: `SecM_InitPowerOnCRC`, `SecM_ComputeCRC` |
| `Sec_SeedKey.h/.c` | Seed/key exchange framework |
| `_SecMPar.h`, `_SecM_cfg.h`, `_Sec_SeedKey_Cfg.h` | ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) configuration templates (GENy input) |
| `_Sec_SeedKeyVendor.c` | ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) vendor algorithm template — **OEM must provide the real algorithm** (see `Demo/*/Appl/Source/Sec_SeedKeyVendor.c`) |

## Public API (selection)

```c
extern SecM_StatusType SecM_InitPowerOn(SecM_InitType initParam);
extern void            SecM_Task(void);                       /* cyclic verification task */
SecM_StatusType SecM_InitVerification(SecM_VerifyInitType init);
SecM_StatusType SecM_Verification(SecM_VerifyParamType *pVerifyParam);
SecM_StatusType SecM_VerifySignature(SecM_SignatureParamType *pVerifyParam);
SecM_StatusType SecM_VerifyChecksumCrc(SecM_SignatureParamType *pVerifyParam);
void            SecM_InitPowerOnCRC(void);
SecM_StatusType SecM_ComputeCRC(SecM_CRCParamType *crcParam);
```

## Usage example

```c
/* 1. Power-on init from bootloader startup */
(void)SecM_InitPowerOn(kSecM_Init_Verification);

/* 2. Cyclic call from main loop */
SecM_Task();

/* 3. Verify downloaded image (CRC or signature class) */
if (SecM_Verification(&verifyParam) != SECM_OK)
{
   /* reject image, stay in bootloader */
}
```

Application images are prepared offline with [HexView](../../../tools/hexview/) (checksums, signatures, validation structures) — see the Security Module TR and the validation-strategies note below.

## Dependencies

- ← used by [Bootloader Core](../../cdd/fbl-core/) (verification at `RequestTransferExit` / validation)
- → [HexView](../../../tools/hexview/) offline counterpart (checksum/signature generation)
- Configured via [GENy](../../../tools/geny/) (`SecM_cfg.h`, `SecMPar.*` in `GenData/`)

## Converted documentation

- [TR: Security Module Basic](technical-reference-security-module/) — architecture, GENy config, HexView application preparation, API
- [AN: Bootloader Validation Strategies](an-isc-8-1143-validation-strategies/) — pattern vs. checksum/CRC vs. signatures

## Source

[BSW/SecMod/ on GitHub](%REPO_URL%/tree/main/BSW/SecMod)

---

[Back to top](#security-module-secm)
