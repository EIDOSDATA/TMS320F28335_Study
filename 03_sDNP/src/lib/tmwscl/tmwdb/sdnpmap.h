/*
 * sdnpmap.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_LIB_TMWSCL_TMWDB_SDNPMAP_H_
#define SRC_LIB_TMWSCL_TMWDB_SDNPMAP_H_

/*for mapping*/
#include "tagdb.h"

#define LOCAL_DB_AI_POINT_MAX            10
#define LOCAL_DB_AO_POINT_MAX            10
#define LOCAL_DB_BI_POINT_MAX            10
#define LOCAL_DB_BO_POINT_MAX            10

#define SCADA_DB_AI_POINT_MAX            10
#define SCADA_DB_AO_POINT_MAX            10
#define SCADA_DB_BI_POINT_MAX            10
#define SCADA_DB_BO_POINT_MAX            10


typedef enum
{
    ANALOG_TYPE_INT16,
    ANALOG_TYPE_UINT16,
    ANALOG_TYPE_INT32,
    ANALOG_TYPE_INT8,
    ANALOG_TYPE_UINT8,
    ANALOG_TYPE_UINT32,
    ANALOG_TYPE_FLOAT32,
    ANALOG_TYPE_FLOAT64,

} ANALOG_TYPE;

typedef struct
{
    uint16      AnalogType;

    TAG_GROUP   TagGroup;
    uint16      TagIndex;

} DnpAnalogMap;

typedef struct
{
    TAG_GROUP   TagGroup;
    uint16      TagIndex;

} DnpBinaryMap;

/*For test*/
const DnpAnalogMap DnpAiMap[LOCAL_DB_AI_POINT_MAX] =
{
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VA,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VB,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VC,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VR,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VS,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_VT,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_IA,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_IB,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_IC,
  ANALOG_TYPE_FLOAT32,    TAG_GRP_AI,    ALS_AI_RMS_IN
};

const DnpAnalogMap DnpAoMap[LOCAL_DB_AO_POINT_MAX];
const DnpBinaryMap DnpBiMap[LOCAL_DB_BI_POINT_MAX];
const DnpBinaryMap DnpBoMap[LOCAL_DB_BO_POINT_MAX];

#endif /* SRC_LIB_TMWSCL_TMWDB_SDNPMAP_H_ */
