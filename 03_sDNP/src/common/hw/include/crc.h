/*
 * crc.h
 *
 *  Created on: 2024. 2. 11.
 *      Author: ShinSung Industrial Electric
 */

#ifndef CRC_H_
#define CRC_H_

#include <string.h>
#include "types.h"


uint16 Compute_DNP_CRC(uint16 *pBuf, uint16 length);
uint16 CRC_Update(uint16 *pBuf, uint16 length, uint16 PrevCrc);

#endif /* CRC_H_ */
