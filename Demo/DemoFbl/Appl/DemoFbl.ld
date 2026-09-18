MEMORY
 {
  /* FLASH */
  iVect       : ORIGIN = 0x00000000, LENGTH = 0x100
  iFbl        : ORIGIN = 0x0000200, LENGTH = 0x10000-0x200
  iAppl       : ORIGIN = 0x00018000, LENGTH = 0x010000
  iFlashDrv   : ORIGIN = 0xFEDE0000, LENGTH = 0x3000
  iRAM        : ORIGIN = 0xFEDE3000, LENGTH = 0x1C000
  iStack      : ORIGIN = 0xFEDFF000, LENGTH = 0x1000
}

SECTIONS {
   /* FLASH (User Area) */
  .intvect                            :>iVect /* Start of interrupt vector table */
  .vectram                            :>.     /* constant datas in ZDA area */

  .FBLHEADER                          :>iFbl  /* Bootloader header table */
  .startup .: { 
    startup.o(.text)
  }

  /* START OF CONST AREA */
  .rozdata                               :>. /* constant datas in ZDA area */
  .robase                align(4)        :>. /* initialize textpointer TP for SDA addressing */
  .rosdata               align(4)        :>. /* constant datas in SDA area */
  .rodata                align(4)        :>. /* constant datas in normal area */
  /* END OF CONST AREA */

  .text                    align(4)   :>.     /* program code area */

  .fixaddr               align(4)        :>. /* ghs internal (compiler) */
  .fixtype               align(4)        :>. /* ghs internal (compiler) */
  .secinfo               align(4)        :>. /* ghs internal (runtime library) */
  .syscall               align(4)        :>. /* ghs internal (linker) */
  .romtext  ROM(.RAMCODE)           :>.     /* RAM text section (copied to RAM at startup)*/
  .romconst ROM(.RAMCONST)          :>.     /* constant data to initialize variables (copied to RAM at startup)*/
  .ROM.ramcopy align(4) ROM(.ramcopy) :>.     /* ROM copy to initialize text in ramcopy area */
  .romdata  ROM(.data)                :>.     /* constant data to initialize variables (copied to RAM at startup)*/
  .romzdata ROM(.zdata)        :>.   /* constant data to initialize variables in ZDA area (copied to RAM at startup)*/ 
  .romsdata ROM(.sdata)        :>.   /* constant data to initialize variables in SDA area (copied to RAM at startup)*/ 
  .romtdata ROM(.tdata)        :>.   /* constant data to initialize variables in TDA area (copied to RAM at startup)*/ 

  .APPLVECT                           :>iAppl /* Application interrupt vector table if FBL is used */

  /* RAM*/
  .sdabase                 align(4)   :>iFlashDrv /* initialize globalpointer GP for SDA addressing */
  .FLASHDRV                align(4)   :>.         /* Space for FlashCode */ 


  .EEPDATA      align(4)               :>iRAM     /* Space for DummyEep */
  .data                                :>.  /* initialized data */
  .FBLMAGIC                align(4)    :>.     /*  */ 
  .ramcopy                 align(4)    :>.       /* Initialized code segment in RAM */
  .RAMCODE  align(4)               :>.     /* RAM code */
  .RAMCONST align(4)               :>.     /* RAM const */
  .bss      align(4)               :>.     /* zero initialized data*/
  .sdata                 align(4)        :>. /* initialized data in SDA area */
  .sbss                    align(4)    :>.     /* zero initialized data in SDA area*/
  .sda_end                               :>. /* end of SDA RAM area */
  /* END OF SDA RAM AREA */

  .zdata                 align(4)                :>. /* initialized data in ZDA area */
  .zbss                  align(4) :>.           /* zero initialized data in ZDA area */
  .tdata                 align(4) MAX_SIZE(256)  :>. /* initialized and zero-initialized data in TDA area */

  .stack        align(4) pad(0x1000)   :>iStack  /* definition of stack size */
  .ram_end               align(16)               :>.

 }
-sda
