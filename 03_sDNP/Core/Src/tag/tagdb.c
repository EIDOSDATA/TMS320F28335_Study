/*
 * tagdb.c
 *
 *  Created on: 2024. 6. 11.
 *      Author: ShinSung Industrial Electric
 */

#include "tagdb.h"


static TAG_DB TagDb;

void TagDbInit(void)
{
    uint16  i;
    float32 dummyData = 0.0F;

    for(i=0; i<TAG_AI_INDEX_MAX; i++)
    {
        /*dummy data*/
        TagDb.AI.AnalogInput[i] = dummyData;
        dummyData += 1.0F;
    }
}
void* getTagDb(void)
{
    return (void*)&TagDb;
}

void* getTagDataAddr(TAG_GROUP Group, uint16 Index)
{
    switch(Group)
    {
        case TAG_GRP_AI:
            return &TagDb.AI.AnalogInput[Index];
        default:
            return NULL;
    }
}
