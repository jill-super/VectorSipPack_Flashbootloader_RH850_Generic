---
title: "EEPROM Abstraction (EepIO)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) · `BSW/Eep/`

## Purpose and responsibility

`EepIO` in [`BSW/Eep/`](%REPO_URL%/tree/main/BSW/Eep) wraps EEPROM access behind a synchronous HIS-style driver interface (`Init/Deinit/Verify/Read/Write/Erase`), decoupling the bootloader's NV users ([WrapNv](../../services/wrapnv/)) from the physical EEPROM implementation.

## Key files

| File | Responsibility |
|---|---|
| `EepIO.h` / `EepIO.c` | Driver wrapper: `EepromDriver_InitSync`, `EepromDriver_RReadSync`, `EepromDriver_RWriteSync`, `EepromDriver_REraseSync`, `EepromDriver_VerifySync`, `EepromDriver_DeinitSync` |
| `EepInc.h` | Internal includes/definitions |

## Public API

```c
IO_ErrorType EepromDriver_InitSync(void *);
IO_ErrorType EepromDriver_DeinitSync(void *);
IO_ErrorType EepromDriver_VerifySync(void *);
IO_ErrorType EepromDriver_RReadSync(IO_MemPtrType, IO_SizeType, IO_PositionType);
IO_ErrorType EepromDriver_RWriteSync(IO_MemPtrType, IO_SizeType, IO_PositionType);
IO_ErrorType EepromDriver_REraseSync(IO_SizeType, IO_PositionType);
```

All calls are **synchronous** (`*Sync`) — appropriate for the bootloader's polled execution model.

## Dependencies

- ← used by [NV Wrapper](../../services/wrapnv/)
- Version reporting via `EepromDriver_GetVersionOfDriver()`

## Source

[BSW/Eep/ on GitHub](%REPO_URL%/tree/main/BSW/Eep)

---

[Back to top](#eeprom-abstraction-eepio)
