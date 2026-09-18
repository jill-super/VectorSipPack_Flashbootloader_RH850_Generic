---
title: "Code Generator (GENy)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · licensed configuration tool · `Generators/`

## Purpose

GENy (v01.04.50 in this delivery) generates the bootloader's configuration from `Demo/DemoFbl/Config/DemoFbl_CBD1701035.gny`: CAN IDs/timing, UDS service tables (`fbl_mtab.*`, `fbl_cw_cfg.*`), memory layout, SecM/WrapNv parameters. `Generators/Components/` ships the generator plug-ins (`GenTool_Geny*`, `FblCan_14229_Vector.dll`, `FblDrvCan_Rh850RscanCrx.dll`, `SysService_SecModHis.dll`, …) plus BSWMD ARXML (`GenTool_GenyFblCanBase_bswmd.arxml`).

## Workflow

1. Edit `Demo/DemoFbl/Config/DemoFbl_CBD1701035.gny` (and `user_config.cfg`) in GENy.
2. Generate → `*/GenData/*` (`fbl_cfg.h`, `v_cfg.h`, `v_par.*`, `SecMPar.*`, `WrapNv_cfg.*`, `ftp_cfg.h`, …).
3. Rebuild bootloader/application ([Build System](../build-system/)).
4. Never hand-edit `GenData/` or `flashrom.*` — regenerate instead.

See [Project Info CBD1701035](../../sip/general/readme-cbd1701035/) §6 for the GENy setup used in this delivery.

## Source

[Generators/ on GitHub](%REPO_URL%/tree/main/Generators)

---

[Back to top](#code-generator-geny)
