/*
 ******************************************************************************************
 * @File            : externalFlashHandle.h
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

#ifndef EXTERNAL_FLASH_HANDLE_H
#define EXTERNAL_FLASH_HANDLE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------- */
// Device Headerfile and Examples Include File
#include "hw_def.h"

/* defines -------------------------------------------------------------------------------- */

/* typedefs ------------------------------------------------------------------------------- */

/* extern variables ----------------------------------------------------------------------- */

/* function prototypes -------------------------------------------------------------------- */
void selectSST39(void);
Uint16 readWordSST39(Uint32);
void eraseSectorSST39(Uint32);
void eraseBlockSST39(Uint32);
void eraseChipSST39(void);
void writeWordSST39(Uint32, Uint16);

#ifdef __cplusplus
extern }
#endif

#endif
