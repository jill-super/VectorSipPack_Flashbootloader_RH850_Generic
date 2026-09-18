---
title: "Demo Application"
---

![Custom](https://img.shields.io/badge/origin-Custom%2FOEM-blue) OEM code on Vector base · `Demo/DemoAppl/`

## Purpose and responsibility

A minimal runnable ECU application in [`Demo/DemoAppl/`](%REPO_URL%/tree/main/Demo/DemoAppl) that demonstrates bootloader↔application coexistence: validity handshake, reset vector handover (`applvect.c`), OEM diagnostics (`appl_diag_oem.c`), and application-side NV access. It is the reference starting point for the real product application.

## Key files

| File(s) | Responsibility |
|---|---|
| `Appl/Source/appl_main.c` | Application entry / main loop |
| `Appl/Source/appl_ap.c`, `appl_diag_oem.c` | OEM application callbacks and diagnostics |
| `Appl/Source/applvect.c` | Application vector table |
| `Appl/Source/startup.c` | C startup code |
| `Appl/Source/fbl_ap*.c`, `Sec_SeedKeyVendor.c` | Bootloader-callback / seed-key templates adapted for the demo |
| `Appl/GenData/*` | ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) GENy output (`fbl_cfg.h`, `fbl_mtab.*`, `SecM_cfg.h`, `WrapNv_cfg.*`, `v_cfg.h`, …) |
| `Appl/Include/*` | `fbl_inc.h`, `fbl_ap*.h`, `MemMap.h`, `Sec_SeedKey_Cfg.h`, `WrapNv_inc.h` |
| `Appl/DemoAppl.hex/.elf/.map`, `DemoAppl.crc` | Build artifacts incl. application checksum |
| `Config/` | `user_config.cfg`, DBC references |

Build and checksum:

```sh
cd Demo/DemoAppl/Appl
make                      # or m.bat / b.bat on Windows
./_generate_checksums.bat # application validation data (see SecM docs)
```

## Dependencies

- → [Bootloader Core](../../cdd/fbl-core/) (handover protocol, shared headers)
- → [Security Module](../../services/secmod/) (validation: pattern/CRC/signature — see [validation strategies](../../services/secmod/an-isc-8-1143-validation-strategies/))
- → [NV Wrapper](../../services/wrapnv/) (shared NV blocks)

## Source

[Demo/DemoAppl/ on GitHub](%REPO_URL%/tree/main/Demo/DemoAppl)

---

[Back to top](#demo-application)
