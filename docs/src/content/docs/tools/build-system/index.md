---
title: "Build System"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) Vector PES Makesupport 3.13 + project glue · `MakeSupport/`

## Purpose

The build is driven by **Vector PES Makesupport** makefiles for Green Hills 2015.1.7 / RH850: global targets (`MakeSupport/Global.Makefile.target.make.3`), analysis helpers (`Makefile.analysis`), and per-project makefiles (`BSW/Flash/Build/Makefile*`, `Demo/*/Appl/Makefile*`, derivative settings, linker-script defaults, memory maps). `MakeSupport/cmd/` bundles the Windows GNU helpers (`make.exe`, `sh.exe`, `gawk`, `sed`, …). Windows batch wrappers (`m.bat`, `b.bat`, `MkFlashRom.bat`, `_generate_checksums.bat`, `j.bat`) wrap common flows.

## Build instructions

Prerequisites: Green Hills 2015.1.7, GNU make (or use the bundled `MakeSupport/cmd/` on Windows), target RH850 (e.g. R7F701313EAFP).

```sh
# 1. Standalone flash driver (+ encrypted image for the bootloader)
cd BSW/Flash/Build
make

# 2. Demo bootloader
cd ../../../Demo/DemoFbl/Appl
make

# 3. Demo application (+ checksums for validation)
cd ../../DemoAppl/Appl
make
./_generate_checksums.bat
```

Compiler/linker options used for this delivery: [Project Info](../../sip/general/readme-cbd1701035/) §5. CAN-wakeup port setup: [CAN Wakeup note](../../general/readme-can-wakeup/).

## Source

[MakeSupport/ on GitHub](%REPO_URL%/tree/main/MakeSupport)

---

[Back to top](#build-system)
