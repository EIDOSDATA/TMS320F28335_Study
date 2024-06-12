/*
 ******************************************************************************************
 * @File            : externalFlashHandle.c
 * @brief           :
 * @author          : thkim
 * @date            : 2024-02-20
 *
 ******************************************************************************************
 * @attention
 *
 *
 *
 *
 *
 ******************************************************************************************
 */

/* Includes ------------------------------------------------------------------------------- */
#include <externalFlashHandle.h>

/* defines -------------------------------------------------------------------------------- */
#define NULL                (0)

#define SST39_ADDR(x)       ((volatile Uint16 *)(0x200000 + x))

/* typedefs ------------------------------------------------------------------------------- */

/* variables ------------------------------------------------------------------------------ */

/* function prototypes -------------------------------------------------------------------- */
static Uint16 checkSST39ToggleReady(Uint32);
static void writeCommandSST39(void);
static void eraseCommandSST39(void);

/* function definitions ------------------------------------------------------------------- */
void selectSST39(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO53 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO52 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;
    GpioDataRegs.GPBSET.bit.GPIO50 = 1;
}

Uint16 readWordSST39(Uint32 address)
{
    address &= 0xFFFFF;

    selectSST39();

//    asm (" ESTOP0 ");

    return *SST39_ADDR(address) & 0xFFFF;
}

void eraseSectorSST39(Uint32 address)
{
    DINT;

    address &= 0xFFFFF;

    selectSST39();

    eraseCommandSST39();

    *(SST39_ADDR(address)) = 0x50;  /// erase sector command

    Uint16 result = checkSST39ToggleReady(address);

    EINT;

//    asm (" ESTOP0 ");
}

void eraseBlockSST39(Uint32 address)
{
    DINT;

    address &= 0xFFFFF;

    selectSST39();

    eraseCommandSST39();

    *(SST39_ADDR(address)) = 0x30;  /// erase block command

    EINT;

    Uint16 result = checkSST39ToggleReady(address);
}

void eraseChipSST39(void)
{
    DINT;

    selectSST39();

    eraseCommandSST39();

    *(SST39_ADDR(0x555)) = 0x10;  /// erase chip command

    Uint16 result = checkSST39ToggleReady(0x555);

    EINT;
}

void writeWordSST39(Uint32 address, Uint16 data)
{
    DINT;

    address &= 0xFFFFF;

    selectSST39();

    writeCommandSST39();

    *(SST39_ADDR(address)) = data;

    Uint16 result = checkSST39ToggleReady(address);

    EINT;

//    asm (" ESTOP0 ");
}

static Uint16 checkSST39ToggleReady(Uint32 dest)
{
    Uint16 prevData = 0, currData = 0, result = 0;
    Uint32 tick = 0;
    Uint32 addr = dest & 0xFFFFF;

    prevData = *(SST39_ADDR(addr));
    prevData &= 0x0040;                 /// read DQ6 (Toggle Pin)

    while (tick < 0x0FFFFFFF)
    {
        currData = *(SST39_ADDR(addr));
        currData &= 0x0040;             /// read DQ6 (Toggle Pin)

        if (prevData == currData)
        {
            result = 1;
            break;
        }

        prevData = currData;
        tick++;
    }

    return result;
}

static void writeCommandSST39(void)
{
    *(SST39_ADDR(0x555)) = 0xAA;
    *(SST39_ADDR(0x2AA)) = 0x55;
    *(SST39_ADDR(0x555)) = 0xA0;
}

static void eraseCommandSST39(void)
{
    *(SST39_ADDR(0x555)) = 0xAA;
    *(SST39_ADDR(0x2AA)) = 0x55;
    *(SST39_ADDR(0x555)) = 0x80;
    *(SST39_ADDR(0x555)) = 0xAA;
    *(SST39_ADDR(0x2AA)) = 0x55;
}
