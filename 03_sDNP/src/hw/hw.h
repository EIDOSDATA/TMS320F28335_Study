/*
 * hw.h
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#include "hw_def.h"

#include "decoder.h"
#include "flash.h"
#include "fram.h"
#include "uart.h"
#include "led.h"
#include "cli.h"
#include "parser.h"
#include "socket.h"

#include "internalFlashHandle.h"
#include "externalFlashHandle.h"

void hwInit(void);

#endif /* SRC_HW_HW_H_ */
