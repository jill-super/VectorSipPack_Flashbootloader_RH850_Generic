---
title: "Bootloader Core (FBL)"
---

![Vector SIP](https://img.shields.io/badge/origin-Vector%20SIP-red) core · ![Vector template](https://img.shields.io/badge/origin-Vector%20template%20%E2%80%94%20customize-orange) callbacks · FblMain 03.04.00 / SIP 07.03.01

## Purpose and responsibility

The bootloader core in [`BSW/Fbl/`](%REPO_URL%/tree/main/BSW/Fbl) implements the complete reprogramming sequence of the ECU: start-up and application-validity decision, UDS diagnostic session handling, segmented data transfer over ISO-TP/CAN, flash-memory programming through `FblMem`, and post-programming validation and reset. It is Vector's `FblMain`/`FblKbApi`/`FblLib_Mem`/`FblWrapperCom_Can`/`FblTp_Iso` feature set, configured with GENy for CAN, UDS SLP3, and the RH850.

## Key files

| File(s) | Responsibility |
|---|---|
| `fbl_main.c/.h` | Bootloader state machine, start-up (`FblStart`), stay-in-boot / start-message handling |
| `fbl_diag_core.c/.h`, `fbl_diag_oem.c/.h`, `fbl_diag.h` | UDS service dispatch (core) + OEM services/fingerprint/DID handling |
| `fbl_tp.c/.h` | ISO 15765-2 transport protocol (`FblTpInitPowerOn`, `FblTpTask`, `FblTpTransmit`, `FblTpPrecopy`) |
| `fbl_cw.c/.h` | CAN communication wrapper (`FblCwInit`, `FblCwTimerTask`, `FblCwStateTask`, baud-rate switching) |
| `fbl_mem.c/.h`, `fbl_mio.c/.h`, `fbl_flio.c/.h`, `fbl_mem_oem.h` | Memory logic: segments/blocks, gap fill, pipelining; `FblMemInit`, `FblMemTask`, `FblMemDataIndication`, erase/program/verify indications |
| `fbl_hw.c/.h`, `fbl_vect.c`, `applvect.h`, `fbl_sfr.h` | RH850 hardware init, interrupt vector tables, SFR definitions |
| `fbl_wd.c/.h` | Watchdog handling |
| `fbl_assert*.h`, `fbl_def.h`, `iotypes.h` | Assertions, base types, IO types |
| `_Template/*` | OEM adaptation templates (`_fbl_ap*`, `_MemMap.h`, `_applvect.c`) — copy into the application and customize |
| `v_ver.h` | ![Generated](https://img.shields.io/badge/origin-Generated-lightgrey) SIP/component version manifest (license `CBD1701035`, Nexteer) |

## Public API (selection)

```c
/* Life cycle */
void FblMain(void);                    /* fbl_main.h — bootloader main loop/state machine */
void FblDiagInitPowerOn(void);         /* fbl_diag_core.h */
void FblDiagTimerTask(void);           /* 1 ms diagnostic task */
void FblDiagStateTask(void);
/* Memory programming */
tFblMemRamData FblMemInit(void);
void           FblMemTask(void);       /* background program/verify state machine */
tFblMemStatus  FblMemDataIndication(tFblMemConstRamData buffer, tFblLength offset, tFblLength length);
/* Transport / communication */
void   FblTpInitPowerOn(void);
void   FblTpTask(void);
vuint8 FblTpTransmit(tTpDataLength count);
void   FblCwInit(void);
void   FblCwTimerTask(void);
void   FblCwStateTask(void);
```

## Usage example

The bootloader is driven by its tasks from the main loop / timer ISR (see `Demo/DemoFbl`):

```c
FblMainInitPowerOn();   /* generated + core power-on init */
for (;;)
{
   FblMain();           /* state machine incl. FblMemTask */
   FblDiagTimerTask();  /* 1 ms cycle — enforced, see ESCAN history in fbl_main.h */
   FblTpTask();
   FblCwTimerTask();
   SecM_Task();
   (void)FblLookForWatchdogVoid();
}
```

OEM behavior is injected through `_Template` callbacks (`FblAppl*` / `ApplFbl*` in `fbl_ap.c`, `fbl_apdi.c`, `fbl_apnv.c`, `fbl_apwd.c`) — never by editing core files.

## Dependencies

- → [Security Module](../../services/secmod/) (`SecM_*` verification, seed/key)
- → [NV Wrapper](../../services/wrapnv/) (NV data during programming)
- → [EEPROM Abstraction](../../ecu-abstraction/eep/) (where NvM/FEE is used)
- → [Flash Driver](../../mcal/flash/) (`flashdrv` HIS API + `flashrom` image + Renesas FCL)
- → [GENy](../../../tools/geny/) generated `GenData/` (`fbl_cfg.h`, `fbl_mtab.*`, `v_cfg.h`, …)

## Converted documentation

- [TR: Flash Bootloader OEM (SLP3)](technical-reference-fbl-vector-slp3/) — UDS services, download sequences, configuration options
- [Project Info CBD1701035](../../../sip/general/readme-cbd1701035/) · [Test Report](../../../sip/general/test-report-fbl-vector-slp3/) · [Issue Report](../../../sip/general/issue-report-cbd1701035/) — incl. known issue ESCAN00097115 (gap-fill buffer overflow workaround: keep `FBL_MEM_GAP_FILL_SEGMENTATION >= FBL_MEM_SEGMENT_SIZE`)

## Source

[BSW/Fbl/ on GitHub](%REPO_URL%/tree/main/BSW/Fbl) · original PDFs in [`Doc/`](%REPO_URL%/tree/main/Doc)

---

[Back to top](#bootloader-core-fbl)
