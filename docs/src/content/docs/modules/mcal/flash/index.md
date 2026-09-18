---
title: "Flash Driver (RH850 RV40)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) driver · ![Renesas](https://img.shields.io/badge/origin-Renesas%20FCL-yellow) FCL · ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) image · `BSW/Flash/`

## Purpose and responsibility

The flash stack in [`BSW/Flash/`](%REPO_URL%/tree/main/BSW/Flash) erases and programs the Renesas RH850 RV40 code flash during reprogramming. It implements the standardized **HIS flash driver API** (`Init/Deinit/Erase/Write`, polled), which is what makes the bootloader portable across flash derivatives — including downloadable/custom drivers.

## Key files

| File(s) | Responsibility |
|---|---|
| `flashdrv.c` / `flashdrv.h` | HIS-API flash driver for RH850 RV40 (Vector): error codes `kFlashOk/kFlashFailed/kFlashVerify/...`, per-function codes (`kFlashFctInit/Erase/Write/…`) |
| `flashrom.c` / `flashrom.h` | ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) encrypted C-array image of the flash driver (`flashDrvBlk0`, XOR `0xCC`, checksum `0xC22B`, load address `0xFEDE0000`) — produced by HexView, do not hand-edit |
| `FlashLib/` | ![Renesas](https://img.shields.io/badge/origin-Renesas%20FCL-yellow) Renesas Flash C Library (`r_fcl_*`, `r_typedefs.h`) — third-party MCU firmware |
| `Build/` | Standalone flash-driver build: `Makefile*`, `MkFlashRom.bat`, `_generate_checksums.bat`, linker scripts (`.ld`), `FlashDrv.hex/.elf/.map`, [CAN-wakeup note](../../../general/readme-can-wakeup/) |

## Public API (HIS flash API shape)

```c
tFlashErrorCode FlashInit (tFlashAddress targetAddress, tFlashLength length, ...);
tFlashErrorCode FlashDeinit(void);
tFlashErrorCode FlashErase(tFlashAddress address, tFlashLength length);
tFlashErrorCode FlashWrite(tFlashAddress address, tFlashLength length, tFlashDataPtr data);
/* long-running calls are polled; watchdog + RCR-RP handled by the caller (FblMem) */
```

Build the standalone driver image:

```sh
cd BSW/Flash/Build
make            # or MkFlashRom.bat / m.bat on Windows
```

## Dependencies

- ← driven by [Bootloader Core](../../cdd/fbl-core/) (`FblMem`/`FblMio` pipelining, gap fill, RCR-RP)
- → Renesas FCL (`FlashLib/`) for register-level sequences
- Image post-processing with [HexView](../../../tools/hexview/)

## Converted documentation

- [TR: Flash Bootloader Hardware (RH850)](technical-reference-fbl-rh850/) — memory model, vector tables, watchdog, linker specifics
- [AN: Custom Flash Drivers](an-isc-8-1188-custom-flash-drivers/) — HIS interface, polling, watchdog, RCR-RP, pipelining, downloadable drivers

> Known issue ESCAN00097115 affects gap-fill sizing in `FblMem`, not the driver itself — see [Issue Report](../../../sip/general/issue-report-cbd1701035/).

## Source

[BSW/Flash/ on GitHub](%REPO_URL%/tree/main/BSW/Flash)

---

[Back to top](#flash-driver-rh850-rv40)
