/*
 * parser.h
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_COMMON_HW_INCLUDE_PARSER_H_
#define SRC_COMMON_HW_INCLUDE_PARSER_H_

#include "hw_def.h"

typedef struct
{
    Uint16 len;
    Uint16 address;
    Uint16 type;
    Uint16 data[32];
    Uint16 checksum;
    Uint32 b32Address;

} intel_hex_record_t;


bool parseIntelHexRecord(const char *str, intel_hex_record_t *record);
bool compareChecksum(intel_hex_record_t *record);

#endif /* SRC_COMMON_HW_INCLUDE_PARSER_H_ */
