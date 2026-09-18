---
title: "SIP: Bootloader Core (FBL)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · SIP 07.03.01 · FblMain 03.04.00

SIP components in this module: **FblMain 03.04.00**, FblDiag_14229_Core 03.02.01, FblDiag OEM (Uds2) 07.01.00, FblDrvCan_Rh850RscanCrx 01.22.00, FblKbApi 01.82.00 (+Frame/FrameDiag/FrameNv/FrameWd/HW/OEM variants), **FblLib_Mem 04.03.01**, FblMio 01.46.01, **FblTp_Iso 03.21.00**, FblVtab_Rh850 01.03.01, FblWd 02.11.00, FblWrapperCom_Can 02.06.00 (versions from `BSW/Fbl/v_ver.h`).

- Repository files: `BSW/Fbl/*.c/*.h` (core, Vector-proprietary) · `BSW/Fbl/_Template/*` (OEM adaptation templates) · `BSW/Fbl/v_ver.h` (generated version manifest)
- Canonical docs: [Bootloader Core](../../../modules/cdd/fbl-core/) · [TR SLP3](../../../modules/cdd/fbl-core/technical-reference-fbl-vector-slp3/)
- Known issues: [Issue Report](../../general/issue-report-cbd1701035/) — ESCAN00097115 (gap fill; workaround: `FBL_MEM_GAP_FILL_SEGMENTATION >= FBL_MEM_SEGMENT_SIZE`)
- Test evidence: [Test Report](../../general/test-report-fbl-vector-slp3/) (result OK on R7F701313EAFP)

---

[Back to top](#sip-bootloader-core-fbl)
