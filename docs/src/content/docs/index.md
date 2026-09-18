---
title: FlashBootloader RH850 Generic
template: splash
hero:
  title: FlashBootloader RH850 Generic
  tagline: A generic automotive flash bootloader for Renesas RH850 microcontrollers — Vector FBL SLP3, SIP 07.03.01.
  actions:
    - text: Read the docs
      link: asw/
      icon: right-arrow
      variant: primary
    - text: Vector SIP overview
      link: sip/
      icon: open-book
---

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](%REPO_URL%/blob/main/LICENSE)
[![HW: Renesas RH850](https://img.shields.io/badge/HW-Renesas%20RH850-blue)](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rh850-automotive-microcontroller)
[![Vector SIP 07.03.01](https://img.shields.io/badge/SIP-07.03.01-red)](sip/)
[![Source](https://img.shields.io/badge/source-GitHub-blue?logo=github)](%REPO_URL%)

## What is this?

An ECU flash bootloader: it resides in on-chip flash alongside (or instead of) the application, receives a new firmware image over **CAN** using **UDS (ISO 14229)** diagnostics, validates it (CRC/signature), and writes it to flash. Key properties:

- **Target:** Renesas RH850 (demonstrated on R7F701313EAFP), RV40 code flash
- **Compiler:** Green Hills 2015.1.7 · **Config tool:** GENy 01.04.50
- **Stack:** Vector FBL SLP3 07.03.01 — bootloader core, ISO-TP, CAN wrapper, HIS security module, NV wrapper, EEPROM abstraction, RH850 flash driver

## Vector vs. custom — read this first

Almost all of `BSW/` carries a **Vector Informatik GmbH** copyright header: it is third-party SIP code, licensed for the Nexteer EPS project (`CBD1701035`) — **not** MIT. The MIT license in this repository covers the documentation site, build glue, and OEM adaptation points built on top.

| Origin | Meaning | Examples |
|---|---|---|
| ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) | Third-party, proprietary Vector license. Do not modify; update via Vector deliveries. | `BSW/Fbl/fbl_main.c`, `BSW/SecMod/Sec*.c`, `BSW/WrapNv/*`, `BSW/Eep/*`, `BSW/Flash/flashdrv.*`, `Generators/*` |
| ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) | Vector example code explicitly meant for OEM adaptation (liability excluded). | `BSW/Fbl/_Template/*`, `Demo/*/Appl/Source/fbl_ap*.c`, `BSW/SecMod/_Sec_*`, `BSW/WrapNv/_WrapNv_*` |
| ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) | Tool output (GENy, HexView). Regenerate, don't hand-edit. | `*/GenData/*`, `BSW/Flash/flashrom.*`, `BSW/Fbl/v_ver.h` |
| ![Renesas](https://img.shields.io/badge/origin-Renesas%20FCL-yellow) | Renesas Flash C Library, third-party. | `BSW/Flash/FlashLib/*` |
| ![Custom](https://img.shields.io/badge/origin-Custom%2FOEM-blue) | Project-owned, MIT-licensed. | `docs/*`, OEM logic in `Demo/` |

Full inventory: [Vector SIP](sip/) · per-layer module tables below.

## Navigate by AUTOSAR layer

| Layer | Contents |
|---|---|
| [Application Software (ASW)](asw/) | Demo application and demo-bootloader application (OEM adaptation) |
| [Complex Device Drivers (CDD)](cdd/) | Bootloader core: main state machine, UDS diagnostics, ISO-TP, CAN wrapper, hardware, watchdog |
| [Services](services/) | Security module (SecM) and NV wrapper (WrapNv) |
| [ECU Abstraction](ecu-abstraction/) | EEPROM abstraction (EepIO) |
| [MCAL & Hardware (MCAL)](mcal/) | RH850 RV40 flash driver, encrypted flash image, Renesas FCL, common types |
| [Tools](tools/) | GENy generator, vFlash, HexView, MakeSupport build system |
| [General](general/) | User manual, CAN-wakeup note, Vector-tools disclaimer |
| [Vector SIP](sip/) | SIP overview, delivery documents (license CBD1701035), per-module SIP pages |

## Build in 30 seconds

```sh
cd BSW/Flash/Build
make
```

Prerequisites: Green Hills 2015.1.7 toolchain, GNU make (Windows helpers in `MakeSupport/cmd/`), RH850 hardware or simulator. Details: [Build System](tools/build-system/) · [Flash Driver](modules/mcal/flash/) · [Demo Application](modules/asw/demo-application/).

## Source layout

```text
BSW/          Basic Software (mostly Vector SIP — see origin badges)
  Fbl/        Bootloader core (CDD) + _Template/ OEM callbacks
  SecMod/     HIS security module (Services)
  WrapNv/     NV-memory wrapper (Services)
  Eep/        EEPROM abstraction (ECU Abstraction)
  Flash/      Flash driver, encrypted image, Renesas FCL, Build/ (MCAL)
  _Common/    Shared Vector types (v_def.h)
Demo/         Demo bootloader + demo application + keys
Doc/          Original PDFs/HTML (sources for the converted pages)
Generators/   GENy generator components (Vector tooling)
FlashTool/    vFlash template + seed/key material
MakeSupport/  Make-based build system (Vector PES Makesupport)
Misc/HexView  HexView post-build tool (Vector, as-is)
docs/         Astro/Starlight site published to GitHub Pages
```

---

[Back to top](#flashbootloader-rh850-generic)
