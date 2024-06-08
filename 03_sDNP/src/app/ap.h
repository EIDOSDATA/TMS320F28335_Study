/*
 * ap.h
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_APP_AP_H_
#define SRC_APP_AP_H_

#include "hw.h"
#include "def.h"

void apInit(void);
void apMain(void);


void CliTask(UArg arg0, UArg arg1);
#endif /* SRC_APP_AP_H_ */
