---
title: "Common Types (v_def)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · `BSW/_Common/v_def.h`

## Purpose and responsibility

[`v_def.h`](%REPO_URL%/tree/main/BSW/_Common) declares the platform types and memory-section qualifiers (`vuint8`, `V_MEMROM0/1/2`, `V_MEMRAM1/2/3`, `V_API_NEAR`, …) shared by all Vector modules. It also carries hardware-platform-specific settings — **never mix copies from other platforms** (note in the file header).

## Dependencies

- Included by virtually every `BSW/` module; must match the RH850 + Green Hills derivative settings in the [Build System](../../../tools/build-system/).

## Source

[BSW/_Common/ on GitHub](%REPO_URL%/tree/main/BSW/_Common)

---

[Back to top](#common-types-v_def)
