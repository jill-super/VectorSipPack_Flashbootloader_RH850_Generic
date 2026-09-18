---
title: "Services"
---

AUTOSAR Services-layer modules: cryptographic/validation services and non-volatile memory services used by both bootloader and application.

| Module | Path | Origin |
|---|---|---|
| [Security Module (SecM)](../modules/services/secmod/) | `BSW/SecMod/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |
| [NV Wrapper (WrapNv)](../modules/services/wrapnv/) | `BSW/WrapNv/` | ![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) |

- **SecM** implements the HIS security API: seed/key exchange, CRC computation, and signature verification of downloaded images.
- **WrapNv** abstracts NV-memory access (single/list/structure/table stores) so bootloader and application share a common NV interface.

---

[Back to top](#services)
