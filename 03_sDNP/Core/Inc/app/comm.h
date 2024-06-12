/*
 * comm.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_APP_COMM_H_
#define SRC_APP_COMM_H_

#include "hw.h"

#define DNP_STACK       true

#if DNP_STACK
#include "tmwscl/dnp/dnpchnl.h"
#include "tmwscl/dnp/dnptprt.h"
#include "tmwscl/dnp/dnplink.h"
#include "tmwscl/dnp/sdnpsesn.h"

#include "tmwscl/utils/tmwappl.h"
#include "tmwscl/utils/tmwchnl.h"
#include "tmwscl/utils/tmwphys.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwsesn.h"
#include "tmwscl/utils/tmwpltmr.h"

/*physical interface*/
#include "tmwscl/tmwtarg/targ232.h"
#include "tmwscl/tmwtarg/targtcp.h"

/*database interface*/
#include "tmwscl/tmwdb/targdb.h"
#endif


#if DNP_STACK

typedef enum
{
    SERIAL_SCADA_CH,    // PORT SCI-B
    SERIAL_LOCAL_CH,    // PORT SCI-C

    TCP_SCADA_CH,       // SOCKET 0
    TCP_LOCAL_CH,       // SOCKET 1

    SDNP_CH_MAX,

} SdnpChType;


typedef struct
{
    /*Channel type*/
    TMWTARGIO_TYPE_ENUM     Type;
    /*get handle from tmw stack*/
    TMWCHNL*                pTmwChHd;
    TMWSESN*                pTmwSesnHd;

    /* My communication channel handle
     * Type : UART, TCP
     * this channel handle return from TargInit(Targ232.c, TargTcp.c) function
     */
    void*                   pMyChConfig;
    void*                   pMyCommCh;


} SdnpChHandle;

typedef struct
{
    TMWAPPL*            pApplContext;
    SdnpDbHandle        SdnpDatabaseHd;
    SdnpChHandle        SdnpChannelHd[SDNP_CH_MAX];
    SdnpFileContext*    pSdnpFileContext;

} SdnpContext;
#endif

typedef struct
{
    uint16              taskPeriod;
#if DNP_STACK
    SdnpContext         SdnpStack;
#endif
} commContext;

#if 0
typedef struct
{
  TMWTARGIO_TYPE_ENUM   Type;

  void*                 pTargIoConfig;

  void*                 pTargIOChannel;

} TargIoConfig;
#endif



#endif /* SRC_APP_COMM_H_ */
