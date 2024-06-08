/*
 * update.c
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

bool parseIntelHexRecord(const char *str, intel_hex_record_t *record)
{
    if ((str == NULL) || (record == NULL))
    {
        return false;
    }

    /// get len
    if (sscanf ((str + 1), "%2hx", &(record->len)) != 1)
    {
        return false;
    }

    /// get addr
    if (sscanf ((str + 3), "%4hx", &(record->address)) != 1)
    {
        return false;
    }

    /// get type
    if (sscanf ((str + 7), "%2hx", &(record->type)) != 1)
    {
        return false;
    }

    /// get data
    Uint16 i;
    for (i = 0; i < record->len; i++)
    {
        Uint16 data[2];
        if (record->type == 0x04)
        {
            if (sscanf ((str + 9 + (i * 2)), "%2hx", &data[i]) != 1)
            {
                return false;
            }

            record->b32Address = ((data[0] & 0x00FF) << 8 | (data[1] & 0x00FF));
        }
        else if ((record->type == 0x00) || (record->type == 0x01))
        {
            if (sscanf ((str + 9 + (i * 2)), "%2hx", &(record->data[i])) != 1)
            {
                return false;
            }
        }
    }



    /// get checksum
    if (sscanf ((str + 9 + (record->len * 2)), "%2hx", &(record->checksum)) != 1)
    {
        return false;
    }

    return true;
}

bool compareChecksum(intel_hex_record_t *record)
{
    Uint16 i, myChecksum = 0;

    myChecksum += record->len & 0x00FF;
    myChecksum += ((record->address >> 8) & 0x00FF);
    myChecksum += record->address & 0x00FF;
    myChecksum += record->type & 0x00FF;

    for (i = 0; i < record->len; i++)
    {
        myChecksum += record->data[i] & 0x00FF;
    }

    return ((~(myChecksum & 0x00FF) + 1) & 0x00FF) == record->checksum;
}


Uint16 extractHexNumber(const char *str, const char *src) // @suppress("Unused static function")
{
    // "write=" 다음에 오는 부분을 찾음
    const char* numberStart = strstr(str, src);

    // "write=" 문자열을 찾지 못한 경우
    if (numberStart == NULL)
    {
        return 0; // 예외 처리: 기본값으로 0을 반환하거나 오류 처리
    }

    // "write=" 다음에 오는 숫자 부분을 가져옴
    numberStart += strlen(src);

    // 추출한 숫자를 정수로 변환하여 반환
    return (Uint16)(strtol(numberStart, NULL, 16));
}
