/*
 * tag_header.h
 *
 *  Created on: 2024. 6. 11.
 *      Author: hp878
 */

#ifndef SRC_COMMON_TAG_TAG_HEADER_H_
#define SRC_COMMON_TAG_TAG_HEADER_H_

#include "def.h"

#include "alias/Alias_AI.h"

typedef enum
{
    TAG_GRP_NULL,

    TAG_GRP_AI,
    TAG_GRP_DI,
    TAG_GRP_DO,
    TAG_GRP_BV,
    TAG_GRP_NMV_UI,
    TAG_GRP_NMV_F,
    TAG_GRP_NVV_UI,
    TAG_GRP_NVV_F,

    TAG_GRP_RCM,

    TAG_GRP_LS_UI,          TAG_GRP_LS_F,
    TAG_GRP_LS0_UI,         TAG_GRP_LS0_F,          TAG_GRP_LS1_UI,         TAG_GRP_LS1_F,

    TAG_GRP_SC_UI,          TAG_GRP_SC_F,

    TAG_GRP_SIM_DI,         TAG_GRP_SIM_RMS,        TAG_GRP_SIM_ANG,        TAG_GRP_SIM_FREQ,       TAG_GRP_SIM_GPAI,
    TAG_GRP_DG,
    TAG_GRP_MMI,
    TAG_GRP_ACC,
    TAG_GRP_DNP232_UI,      TAG_GRP_DNP232_F,       TAG_GRP_DNPETH_UI,      TAG_GRP_DNPETH_F,

    TAG_GRP_MAX,

} TAG_GROUP;

#endif /* SRC_COMMON_TAG_TAG_HEADER_H_ */
