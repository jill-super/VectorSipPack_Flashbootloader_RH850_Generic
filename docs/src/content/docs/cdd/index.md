---
title: "Complex Device Drivers (CDD)"
---

In AUTOSAR terms the flash bootloader is effectively a **Complex Device Driver**: hardware-near, non-standardized functionality spanning diagnostics, transport, memory and MCU control.

| Module | Path | Origin |
|---|---|---|
| [Bootloader Core (FBL)](../modules/cdd/fbl-core/) | `BSW/Fbl/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) core + ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) callbacks |

The core implements the bootloader state machine (`fbl_main`), UDS diagnostics (`fbl_diag*`), ISO-TP transport (`fbl_tp`), CAN communication wrapper (`fbl_cw`), flash-memory abstraction (`fbl_mem`, `fbl_mio`, `fbl_flio`), RH850 hardware setup (`fbl_hw`, `fbl_vect`, `fbl_sfr`), and watchdog (`fbl_wd`).

---

[Back to top](#complex-device-drivers-cdd)
