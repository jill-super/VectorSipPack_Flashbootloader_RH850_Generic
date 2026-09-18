---
title: "SIP Modules"
---

Per-module SIP views. Each page identifies the SIP component version, the repository files that carry it, and links to the canonical module documentation and converted Vector references. (The SIP has no separate folder in this repo — see [Vector SIP](../) for the stated assumption.)

| Module | Layer | SIP component |
|---|---|---|
| [SIP: Bootloader Core (FBL)](fbl-core/) | CDD | FblMain 03.04.00, FblLib_Mem 04.03.01, FblTp_Iso 03.21.00, FblWrapperCom_Can 02.06.00, … |
| [SIP: Security Module (SecM)](secmod/) | Services | SysService_SecModHis 02.09.00 |
| [SIP: NV Wrapper (WrapNv)](wrapnv/) | Services | WrapNv 2.1 (SysService_WrapperNv) |
| [SIP: EEPROM (EepIO)](eep/) | ECU Abstraction | EepIO wrapper |
| [SIP: Flash Driver (RH850)](flash/) | MCAL | FblWrapperFlash_Rh850Rv40His 01.10.00 + flashdrv + FCL |
| [SIP: Common (v_def)](common/) | MCAL | Shared Vector platform types |

---

[Back to top](#sip-modules)
