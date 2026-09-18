---
title: "Demo Bootloader Application"
---

![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) · `Demo/DemoFbl/`

## Purpose and responsibility

The demonstration bootloader in [`Demo/DemoFbl/`](%REPO_URL%/tree/main/Demo/DemoFbl) wires the Vector core to concrete OEM callbacks and a GENy configuration (`DemoFbl_CBD1701035.gny`, DBC `DemoFBL_Vector_SLP3.dbc`). It shows exactly which files an integrator copies and adapts.

## Key files

| File(s) | Responsibility |
|---|---|
| `Appl/Source/fbl_ap.c` | Application-dependent routines (main adaptation point) |
| `Appl/Source/fbl_apdi.c` | Diagnostic callbacks (DIDs, routines, fingerprint) |
| `Appl/Source/fbl_apnv.c` | NV-memory callbacks |
| `Appl/Source/fbl_apwd.c` | Watchdog callbacks |
| `Appl/Source/Sec_SeedKeyVendor.c` | Vendor seed/key algorithm (demo — replace with OEM algorithm) |
| `Appl/Source/startup.c` | Startup code |
| `Appl/GenData/*` | ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) GENy output |
| `Config/DemoFbl_CBD1701035.gny`, `Config/DemoFBL_Vector_SLP3.dbc` | GENy project + CAN database (CAN IDs, timing) |
| `Appl/DemoFbl.hex/.elf/.map` | Build artifacts |

```sh
cd Demo/DemoFbl/Appl
make   # or m.bat / b.bat on Windows
```

## Dependencies

- → [Bootloader Core](../../cdd/fbl-core/) · [Security Module](../../services/secmod/) · [NV Wrapper](../../services/wrapnv/) · [GENy](../../../tools/geny/)

## Source

[Demo/DemoFbl/ on GitHub](%REPO_URL%/tree/main/Demo/DemoFbl) · [Project Info (CBD1701035)](../../../sip/general/readme-cbd1701035/) (demo build §7, memory map §7.2)

---

[Back to top](#demo-bootloader-application)
