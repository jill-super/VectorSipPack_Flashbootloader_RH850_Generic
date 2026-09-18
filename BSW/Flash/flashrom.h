/****************************************************************
*
*  Filename:      D:\usr\usage\Delivery\CBD17x\CBD1701035\D00\external\BSW\Flash\Build\..\flashrom.h
*  Project:       Exported definition of C-Array Flash-Driver
*  File created:  Mon Mar 26 11:14:57 2018

*
****************************************************************/

#ifndef __FLASHDRV__
#define __FLASHDRV__
#define FLASHDRV_GEN_RAND 7187

#define FLASHDRV_NUMBLOCKS 1

#define FLASHDRV_DECRYPTVALUE	0xCC
#define FLASHDRV_DECRYPTDATA(a)   (vuint8)((vuint8)(a) ^ (vuint8)FLASHDRV_DECRYPTVALUE)
#define FLASHDRV_BLOCK0_ADDRESS	0xFEDE0000
#define FLASHDRV_BLOCK0_LENGTH	0x1C88
#define FLASHDRV_BLOCK0_CHECKSUM	0xC22Bu

V_MEMROM0 extern V_MEMROM1 vuint8 V_MEMROM2 flashDrvBlk0[FLASHDRV_BLOCK0_LENGTH];


#endif    /* #ifdef __FLASHDRV__ */

