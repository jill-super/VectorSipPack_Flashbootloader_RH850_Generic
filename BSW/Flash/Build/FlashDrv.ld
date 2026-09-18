MEMORY
 {
  iROM        : ORIGIN = 0x00000000, LENGTH = 3072k
  iRAM        : ORIGIN = 0xFEDE0000, LENGTH = 64k
 }

SECTIONS
 {
/* Start of internal RAM area (iRAM) */
  .sdabase  align(4)               :>iRAM   /* initialize globalpointer GP for SDA addressing */
  .signature               align(4):>.
  .R_FCL_CODE_RAM          align(4):>.
  .text     align(16)              :>.      /* program code area */
  .rozdata                         :>.      /* constant datas in ZDA area */
  .robase   align(4)               :>.      /* initialize textpointer TP for SDA addressing */
  .rosdata  align(4)               :>.      /* constant datas in SDA area */
  .rodata   align(4)               :>.      /* constant datas in normal area */
  .data                            :>.      /* initialized data */
  .bss      align(4)               :>.      /* zero initialized data*/
  .sdata    align(4)               :>.      /* initialized data in SDA area */
  .sbss            align(4)              :>.      /* zero initialized data in SDA area*/
  .tdata    align(4) MAX_SIZE(256) :>.      /* initialized and zero-initialized data in TDA area */
  .zdabase                         :>.
  .zdata                           :>.      /* initialized data in ZDA area*/
  .zbss     align(4)               :>.      /* zero initialized data in ZDA area*/
  .romdata  ROM(.data)             :>.      /* constant data to initialize variables */
  .romzdata ROM(.zdata)            :>.      /* constant data to initialize variables in ZDA area */
  .romsdata ROM(.sdata)            :>.      /* constant data to initialize variables in SDA area */
  .romtdata ROM(.tdata)            :>.      /* constant data to initialize variables in TDA area */
  .secinfo                         :>.      /* constant data to initialize variables in TDA area */
  .R_FCL_CODE_ROMRAM       align(4):>.
  .R_FCL_DATA              align(4):>.
  .R_FCL_CODE_USRINT       align(4):>.      /* Interrupt code located in RAM */
  .R_FCL_CODE_USR          align(4):>.      /* User code located in RAM */
  .R_FCL_CODE_ROM          align(4):>.      /* FCL code in ROM */
  .R_FCL_CONST             align(4):>.      /* FCL constants in ROM */
  .R_FCL_CODE_RAM_EX_PROT  pad(0x20) align(4):>. /* ECC Exception protection */
  .stack    align(4) pad(0x300)    :>.      /* definition of stack size */
 }
-map
-sda
