---
title: "NV Wrapper (WrapNv)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · v2.1 · `BSW/WrapNv/`

## Purpose and responsibility

The NV wrapper in [`BSW/WrapNv/`](%REPO_URL%/tree/main/BSW/WrapNv) provides a uniform, small-footprint API for non-volatile data shared between bootloader and application (fingerprints, programming counters, validation flags). It supports **single**, **list**, **structure**, and **table** data layouts and can sit on top of EEPROM or Fee/NvM.

## Key files

| File(s) | Responsibility |
|---|---|
| `WrapNv.h` | Public interface (v2.1, GENy-configurable, Fee/NvM capable) |
| `_WrapNv_Cfg.h`, `_WrapNv_inc.h` | ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) configuration/include templates |
| `Demo/*/Appl/GenData/WrapNv_cfg.*` | ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) generated configuration for each demo |
| `Demo/*/Appl/Include/WrapNv_inc.h` | Application-side include |

## Usage example

```c
#include "WrapNv.h"

/* NV access uses generated blocks; single/list/structure/table layouts
   are selected in GENy and materialize as WrapNv_cfg.h/.c */
WrapNv_InitPowerOn();
/* read-modify-write of shared NV data, e.g. programming attempt counter */
```

> Sharing blocks between application and bootloader requires matching NvM/FEE layouts on both sides — follow the application note below exactly.

## Dependencies

- ← used by [Bootloader Core](../../cdd/fbl-core/) and [Demo Application](../../asw/demo-application/)
- → [EEPROM Abstraction](../../ecu-abstraction/eep/) / NvM/FEE stack
- Configured via [GENy](../../../tools/geny/)

## Converted documentation

- [TR: NV-Wrapper](technical-reference-nv-wrapper/) — architecture, data structures, integration, API
- [AN: Share FEE Blocks (App and Bootloader)](an-isc-8-1173-share-fee-blocks/) — DaVinci/NvM/FEE sharing workflow, `FeeFblConfig`

## Source

[BSW/WrapNv/ on GitHub](%REPO_URL%/tree/main/BSW/WrapNv)

---

[Back to top](#nv-wrapper-wrapnv)
