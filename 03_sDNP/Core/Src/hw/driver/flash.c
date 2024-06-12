/*
 * Flash.c
 *
 *  Created on : 2023. 11. 15.
 *      Author : ShinSung Industrial Electric
 *
 *  Description: External FLASH Device (SST39VF1601C)
 *  Total Block 35
 *  Uniform 2K Word Sectors
 *
 *
 */

#include "def.h"
#include "decoder.h"
#include "flash.h"


#define SECTOR_ERASE_SHIFT(Sector)      (Sector<<11)
#define BLOCK_ERASE_SHIFT(Block)        (Block<<15)
#define FLASH_ADDR(Address)             ((volatile uint16 *)(0x200000 + Address))
#define FLASH_ADDR_MASKING              0xFFFFF

#define FLASH_SECTOR_MAX        512
#define FLASH_DELAY             15
#define MAX_TIMEOUT             10000

uint16 FLASH_Read(uint32 addr)
{
   uint16 Rtn = 0;

   addr &= FLASH_ADDR_MASKING;

   Rtn = *FLASH_ADDR(addr);

   return Rtn;

}

//bool FLASH_Write(uint32 Addr, uint16 Data)
//{
//
//    uint16 timeout = 0;
//    uint16 ToggleBit1 = 0;
//    uint16 ToggleBit2 = 0;
//
//    Zone7BusTake(FLASH);
//
//    *FLASH_ADDR(0x555) = 0x00AA;
//    *FLASH_ADDR(0x2AA) = 0x0055;
//    *FLASH_ADDR(0x555) = 0x00A0;
//
//    *FLASH_ADDR(Addr) = Data;
//
//    DELAY_US(FLASH_DELAY);
//
//    while(timeout <MAX_TIMEOUT)
//    {
//        ToggleBit1 = *FLASH_ADDR(Addr);
//
//        ToggleBit2 = *FLASH_ADDR(Addr);
//
//        if(ToggleBit1 == ToggleBit2)
//         {
//            Zone7BusRelease();
//            return true;
//         }
//        timeout++;
//    }
//
//    Zone7BusRelease();
//
//    return false;
//}
//
//bool FLASH_SectorErase(uint32 Sector)
//{
//    uint32 SAx = SECTOR_ERASE_SHIFT(Sector);
//    uint16 timeout = 0;
//    uint16 ToggleBit1 = 0;
//    uint16 ToggleBit2 = 0;
//
//    Zone7BusTake(FLASH);
//
//    /*Erase Command*/
//    *FLASH_ADDR(0x555) = 0x00AA;
//    *FLASH_ADDR(0x2AA) = 0x0055;
//    *FLASH_ADDR(0x555) = 0x0080;
//    *FLASH_ADDR(0x555) = 0x00AA;
//    *FLASH_ADDR(0x2AA) = 0x0055;
//
//    *FLASH_ADDR(SAx) = 0x0050;
//
//    /*Sector Erase Time 18ms(Typical)*/
//
//    Zone7BusRelease();
//
//    TASK_SLEEP(18);
//
//    Zone7BusTake(FLASH);
//
//    while(timeout < MAX_TIMEOUT)
//    {
//        ToggleBit1 = *FLASH_ADDR(SAx);
//        ToggleBit2 = *FLASH_ADDR(SAx);
//
//        if(ToggleBit1 == ToggleBit2)
//        {
//            Zone7BusRelease();
//            return true;
//        }
//        timeout++;
//    }
//
//    Zone7BusRelease();
//    return false;
//}
//
//bool FLASH_BlockErase(uint32 Block)
//{
//    uint32 BAx = BLOCK_ERASE_SHIFT(Block);
//    uint16 timeout = 0;
//    uint16 ToggleBit1 = 0;
//    uint16 ToggleBit2 = 0;
//
//    Zone7BusTake(FLASH);
//
//    /*Erase Command*/
//    *FLASH_ADDR(0x555) = 0x00AA;
//    *FLASH_ADDR(0x2AA) = 0x0055;
//    *FLASH_ADDR(0x555) = 0x0080;
//    *FLASH_ADDR(0x555) = 0x00AA;
//    *FLASH_ADDR(0x2AA) = 0x0055;
//
//    *FLASH_ADDR(BAx) = 0x0030;
//
//    /*Block Erase Time 18ms(Typical)*/
//    Zone7BusRelease();
//
//    TASK_SLEEP(18);
//
//    Zone7BusTake(FLASH);
//
//    while(timeout <MAX_TIMEOUT)
//    {
//       ToggleBit1 = *FLASH_ADDR(BAx);
//       ToggleBit2 = *FLASH_ADDR(BAx);
//
//       if(ToggleBit1 == ToggleBit2)
//       {
//           Zone7BusRelease();
//           return true;
//       }
//
//         timeout++;
//    }
//   Zone7BusRelease();
//   return false;
//}
