---
title: "Application Software (ASW)"
---

The application layer holds the OEM-owned software: the **demo application** (a minimal ECU application that coexists with the bootloader) and the **demo bootloader application** (the OEM callback layer on top of the Vector core).

| Module | Path | Origin |
|---|---|---|
| [Demo Application](../modules/asw/demo-application/) | `Demo/DemoAppl/` | ![Custom](https://img.shields.io/badge/origin-Custom%2FOEM-blue) on Vector base |
| [Demo Bootloader Application](../modules/asw/demo-fbl/) | `Demo/DemoFbl/` | ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) |

> ASW code is where you add product logic (application validation responses, OEM diagnostics like `appl_diag_oem.c`, fingerprint/DID handling). The Vector core underneath stays untouched.

---

[Back to top](#application-software-asw)
