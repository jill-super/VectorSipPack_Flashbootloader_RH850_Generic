--------------------------------------------------------
- Port configuration for external CAN wakeup interrupt -
--------------------------------------------------------

RH850C1x: Port_Group_B1
-----------------------
Channel 0: use INTP2 on Port 0_5 for external interrupt
Channel 1: use INTP3 on Port 0_6 for external interrupt
Channel 2: use INTP4 on Port 0_7 for external interrupt
Channel 3: use INTP5 on Port 0_8 for external interrupt


RH850E1x: Port_Group_D
----------------------
Channel 0: use IRQ0 on Port 2_0 for external interrupt
Channel 1: use IRQ1 on Port 2_1 for external interrupt
Channel 2: use IRQ2 on Port 2_2 for external interrupt
Channel 3: use IRQ3 on Port 2_3 for external interrupt


Note: If your derivative belongs to another derivative group, 
      the RX-pin of the CAN channels are also suitable for external interrupts.
      This is handled automatically by DrvCan and behaves like an internal
      controller wakeup interrupt.