<p align="center">
  <a href="../../actions/workflows/deploy-pages.yml"><img alt="Docs build & deploy" src="../../actions/workflows/deploy-pages.yml/badge.svg"></a>
  <a href="LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/License-MIT-green.svg"></a>
  <a href="docs/sip/"><img alt="Vector SIP 07.03.01" src="https://img.shields.io/badge/SIP-07.03.01-red"></a>
  <img alt="HW: Renesas RH850" src="https://img.shields.io/badge/HW-Renesas%20RH850-blue">
  <img alt="Language: C" src="https://img.shields.io/badge/language-C-lightgrey?logo=c">
  <img alt="Diagnostics: UDS" src="https://img.shields.io/badge/diagnostics-UDS%20%2F%20ISO%2014229-orange">
</p>

# FlashBootloader_RH850_Generic

A generic automotive **flash bootloader** for **Renesas RH850** microcontrollers, built on the **Vector Flash Bootloader (SLP3)** delivery — SIP 07.03.01, license `CBD1701035`. Receives firmware over **CAN** via **UDS (ISO 14229)**, validates it (CRC/signature), and programs on-chip flash.

📖 **Full documentation: [`docs/`](docs/src/content/docs/)** — AUTOSAR-layered, searchable, with all Vector PDFs converted to Markdown. To publish it, enable GitHub Pages (Settings → Pages → Source: *GitHub Actions*); the rendered site is then served at `https://<your-user>.github.io/<your-repo>/`. No configuration needed — the workflow derives paths from the repository, so forks and renames just work.

<details>
<summary><strong>Table of contents</strong></summary>

- [Vector vs. custom — read this first](#vector-vs-custom--read-this-first)
- [Repository structure](#repository-structure)
- [AUTOSAR module map](#autosar-module-map)
- [Documentation](#documentation)
- [Installation & build](#installation--build)
- [Flashing & usage](#flashing--usage)
- [Contributing](#contributing)
- [License](#license)
- [Disclaimer](#disclaimer)

</details>

---

## Vector vs. custom — read this first

Almost all of `BSW/` carries a **Vector Informatik GmbH** copyright header: it is a third-party SIP delivery licensed for the Nexteer EPS project (`CBD1701035`) — **not MIT**. The [MIT license](LICENSE) in this repository covers the documentation site (`docs/`), build/CI glue (`.github/`), and OEM adaptation code built on top.

| Origin | Meaning | Examples |
|---|---|---|
| ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) | Third-party, proprietary Vector license. Do not modify; update via Vector deliveries. | `BSW/Fbl/fbl_main.c`, `BSW/SecMod/Sec*.c`, `BSW/WrapNv/*`, `BSW/Eep/*`, `BSW/Flash/flashdrv.*`, `Generators/*` |
| ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) | Vector example code explicitly meant for OEM adaptation (liability excluded). | `BSW/Fbl/_Template/*`, `Demo/*/Appl/Source/fbl_ap*.c`, `BSW/SecMod/_Sec_*`, `BSW/WrapNv/_WrapNv_*` |
| ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) | Tool output (GENy, HexView). Regenerate, don't hand-edit. | `*/GenData/*`, `BSW/Flash/flashrom.*`, `BSW/Fbl/v_ver.h` |
| ![Renesas](https://img.shields.io/badge/origin-Renesas%20FCL-yellow) | Renesas Flash C Library, third-party. | `BSW/Flash/FlashLib/*` |
| ![Custom](https://img.shields.io/badge/origin-Custom%2FOEM-blue) | Project-owned, MIT-licensed. | `docs/*`, `.github/*`, OEM logic in `Demo/` |

## Repository structure

<details>
<summary>Click to expand directory tree</summary>

```text
BSW/            Basic Software (mostly Vector SIP)
  Fbl/          Bootloader core (CDD) + _Template/ OEM callbacks + v_ver.h (SIP versions)
  SecMod/       HIS security module: seed/key, CRC, signature verification (Services)
  WrapNv/       NV-memory wrapper: single/list/structure/table (Services)
  Eep/          EEPROM driver wrapper (ECU Abstraction)
  Flash/        HIS flash driver + encrypted image + Renesas FCL + Build/ (MCAL)
  _Common/      Shared Vector platform types v_def.h (MCAL)
Demo/           Demo bootloader (DemoFbl) + demo application (DemoAppl) + keys (DemoKeys)
Doc/            Original Vector PDFs/HTML — sources for the converted docs/
Generators/     GENy generator plug-ins (Vector tooling)
FlashTool/      vFlash template installer + seed/key material
MakeSupport/    Vector PES Makesupport build system + Windows GNU helpers
Misc/HexView    HexView post-build tool (as-is) + reference manual
docs/           Astro/Starlight site published to GitHub Pages (`npm ci && npm run dev` inside `docs/` to preview)
.github/        Pages deployment, Dependabot, auto-merge workflows
LICENSE         MIT license (project-owned content; Vector files keep their license)
```

</details>

## AUTOSAR module map

| Layer | Module | Path | Origin |
|---|---|---|---|
| ASW | Demo Application | `Demo/DemoAppl/` | ![Custom](https://img.shields.io/badge/origin-Custom%2FOEM-blue) on Vector base |
| ASW | Demo Bootloader Application | `Demo/DemoFbl/` | ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) |
| CDD | Bootloader Core (FBL) | `BSW/Fbl/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| Services | Security Module (SecM) | `BSW/SecMod/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| Services | NV Wrapper (WrapNv) | `BSW/WrapNv/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| ECU Abstraction | EEPROM Abstraction (EepIO) | `BSW/Eep/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| MCAL | Flash Driver (RH850 RV40) | `BSW/Flash/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) + ![Renesas](https://img.shields.io/badge/origin-Renesas%20FCL-yellow) |
| MCAL | Common Types (v_def) | `BSW/_Common/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| Tools | GENy / vFlash / HexView / Build | `Generators/`, `FlashTool/`, `Misc/HexView/`, `MakeSupport/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |

Per-module docs (purpose, API, examples, dependencies): [docs site sources](docs/src/content/docs/) · [Vector SIP section](docs/src/content/docs/sip/).

## Documentation

- 🌐 Rendered site: [`docs/`](docs/src/content/docs/) published via GitHub Pages (Astro Starlight: sidebar, search, dark mode) — see note at the top
- 📄 Converted Vector references live alongside their modules, e.g.:
  - [TR: Bootloader OEM (SLP3)](docs/src/content/docs/modules/cdd/fbl-core/technical-reference-fbl-vector-slp3.md) · [TR: Hardware RH850](docs/src/content/docs/modules/mcal/flash/technical-reference-fbl-rh850.md)
  - [TR: Security Module](docs/src/content/docs/modules/services/secmod/technical-reference-security-module.md) · [TR: NV-Wrapper](docs/src/content/docs/modules/services/wrapnv/technical-reference-nv-wrapper.md)
  - [User Manual](docs/src/content/docs/general/user-manual-flash-bootloader.md) · [HexView Manual](docs/src/content/docs/tools/hexview/reference-manual-hexview.md)
  - [Delivery / Test / Issue reports](docs/src/content/docs/sip/general/) for license `CBD1701035`
- Originals remain in [`Doc/`](Doc/) and [`Misc/HexView/`](Misc/HexView/).

## Installation & build

### Prerequisites

| Hardware | Compiler | Config tool | Host tools |
|---|---|---|---|
| RH850, e.g. R7F701313EAFP | Green Hills 2015.1.7 | GENy 01.04.50 | GNU make (bundled for Windows in `MakeSupport/cmd/`) |

### Build instructions

```sh
# 1. Standalone flash driver (+ encrypted image)
cd BSW/Flash/Build
make

# 2. Demo bootloader
cd ../../../Demo/DemoFbl/Appl
make

# 3. Demo application (+ validation checksums)
cd ../../DemoAppl/Appl
make
./_generate_checksums.bat
```

On Windows the `m.bat` / `b.bat` / `MkFlashRom.bat` wrappers provide the same flows. Compiler/linker options per this delivery: [Project Info §5](docs/src/content/docs/sip/general/readme-cbd1701035.md). Details: [Build System docs](docs/src/content/docs/tools/build-system/).

## Flashing & usage

1. Program the bootloader (and optionally the demo app) to the RH850 with your standard flash tool.
2. Connect over CAN and run the UDS download sequence (see [TR SLP3](docs/src/content/docs/modules/cdd/fbl-core/technical-reference-fbl-vector-slp3.md)); vFlash template in [`FlashTool/`](FlashTool/).
3. The bootloader validates (CRC/signature via SecM), programs flash, and hands over to the application.
4. ⚠️ Known issue ESCAN00097115: keep `FBL_MEM_GAP_FILL_SEGMENTATION >= FBL_MEM_SEGMENT_SIZE` — see [Issue Report](docs/src/content/docs/sip/general/issue-report-cbd1701035.md).

## Contributing

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -am 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a pull request

Please respect origin boundaries: do not relicense or modify Vector SIP files — put OEM changes in templates, `Demo/`, and `docs/`. Documentation PRs touching `docs/` trigger a preview build of the Pages site.

## License

This project uses the [MIT License](LICENSE) for project-owned content (documentation site, CI/build glue, OEM adaptation code).

> **Third-party exception:** files carrying a Vector Informatik GmbH (or Renesas) copyright — essentially all of `BSW/`, `Generators/`, `FlashTool/`, `Misc/HexView/`, and `Doc/` — remain under their proprietary licenses (SIP license `CBD1701035`, Vector tools furnished as-is). They are **not** covered by the MIT license.

## Disclaimer

Some files and code are © Vector Informatik GmbH or provided as-is.
See [Misc/HexView/disclaimer.txt](Misc/HexView/disclaimer.txt) ([rendered](docs/src/content/docs/general/disclaimer-vector-tools.md)) for details. Use of this product can be dangerous — use with care.

---

<p align="center"><a href="#flashbootloader_rh850_generic">Back to top ↑</a></p>
