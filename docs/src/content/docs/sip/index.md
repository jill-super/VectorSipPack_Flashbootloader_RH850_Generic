---
title: "Vector SIP"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) third-party delivery · SLP3 07.03.01 · license `CBD1701035`

## What is the SIP?

The **Software Integration Package (SIP)** is Vector's tested, versioned delivery of the flash-bootloader basic software for this project (customer: Nexteer Automotive (Suzhou) Co., project EPS, micros R7F701313EAFP, compiler Green Hills 2015.1.7, GENy 01.04.50).

> **Repository assumption (stated):** this repository has no separate `SIP/` folder — the SIP *is* the `BSW/` + `Generators/` + `Demo/*/GenData` content plus the documents in `Doc/`. The pages below map each SIP module to its canonical documentation. Everything under this section is **proprietary Vector material**: use is restricted to the questionnaire configuration; see the [delivery description](general/delivery-description-cbd1701035/) and [project info](general/readme-cbd1701035/).

| Area | Pages |
|---|---|
| [SIP Delivery Documents](general/) | Delivery description, project info, test report, issue report |
| [SIP Modules](modules/) | Per-module SIP pages linking to code + converted docs |

## SIP module map

| SIP component (from `v_ver.h` / delivery description) | Version | Docs |
|---|---|---|
| FblMain | 03.04.00 | [Bootloader Core](../modules/cdd/fbl-core/) · [SIP: FBL](modules/fbl-core/) |
| FblDiag_14229 (Core / OEM UDS) | 03.02.01 / 07.01.00 | [Bootloader Core](../modules/cdd/fbl-core/) · [TR SLP3](../modules/cdd/fbl-core/technical-reference-fbl-vector-slp3/) |
| FblDrvCan_Rh850RscanCrx | 01.22.00 | [Bootloader Core](../modules/cdd/fbl-core/) |
| FblKbApi (+Frame/HW/WD variants) | 01.82.00 … | [Bootloader Core](../modules/cdd/fbl-core/) |
| FblLib_Mem | 04.03.01 | [Bootloader Core](../modules/cdd/fbl-core/) — note ESCAN00097115 |
| FblTp_Iso | 03.21.00 | [Bootloader Core](../modules/cdd/fbl-core/) |
| FblWrapperCom_Can | 02.06.00 | [Bootloader Core](../modules/cdd/fbl-core/) |
| FblWrapperFlash_Rh850Rv40His | 01.10.00 | [Flash Driver](../modules/mcal/flash/) · [SIP: Flash](modules/flash/) |
| Security (SysService_SecModHis) | 02.09.00 | [Security Module](../modules/services/secmod/) · [SIP: SecM](modules/secmod/) |
| FBL Vector SLP3 (OEM package) | 07.03.01 | [Test Report](general/test-report-fbl-vector-slp3/) · [Issue Report](general/issue-report-cbd1701035/) |

---

[Back to top](#vector-sip)
