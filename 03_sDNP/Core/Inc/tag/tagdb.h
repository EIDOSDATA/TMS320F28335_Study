/*
 * tagdb.h
 *
 *  Created on: 2024. 6. 11.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_COMMON_TAG_TAGDB_H_
#define SRC_COMMON_TAG_TAGDB_H_

#include "def.h"
#include "tag_header.h"

#define TAG_AI_INDEX_MAX    10

typedef struct
{
    float32 AnalogInput[TAG_AI_INDEX_MAX];

} TAG_AI;

typedef struct
{
    TAG_AI      AI;

} TAG_DB;

void  TagDbInit(void);
void* getTagDb(void);
void* getTagDataAddr(TAG_GROUP Group, uint16 Index);
#endif /* SRC_COMMON_TAG_TAGDB_H_ */
