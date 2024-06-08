/*
 * targtcp.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_LIB_TMWSCL_TMWTARG_TARGTCP_H_
#define SRC_LIB_TMWSCL_TMWTARG_TARGTCP_H_

/*Target header*/
#include "hw_def.h"
#include "ringbuf.h"
#include "socket.h"

/*Target IO(Input Output) Configuration*/
#include "tmwscl/tmwtarg/targiocnfg.h"

/*TMW Stack header*/
#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwpltmr.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwtarg.h"

/* Grouped various configuration values for TCP channel initialization into a structure*/
typedef struct
{
    TMWTYPES_UINT           SoureceIp[IOCNFG_IP_ADDRESS_SIZE];
    TMWTYPES_UINT           GateWay[IOCNFG_GATEWAY_SIZE];
    TMWTYPES_UINT           SubnetMask[IOCNFG_SUBNETMASK_SIZE];

    TMWTYPES_UINT           PortNumber[2];
    TMWTYPES_UINT           Socket[2];

} TCP_CONFIG;

/* Define TCP port channel */
typedef struct
{
    TMWTARGIO_TYPE_ENUM     TargChType;

    TMWTYPES_CHAR           chnlName[IOCNFG_MAX_NAME_LEN];
    TMWTYPES_CHAR           portName[IOCNFG_MAX_NAME_LEN];

    TMWTYPES_UINT           Socket;

    /*W5100*/
    SOCKET_STATUS           Status;

    TMWTARGTCP_MODE         Mode;

    TMWTYPES_BYTE           SoreceIp[IOCNFG_IP_ADDRESS_SIZE];
    TMWTYPES_BYTE           DestinationIp[IOCNFG_IP_ADDRESS_SIZE];

    TMWTYPES_BYTE           Gateway[IOCNFG_GATEWAY_SIZE];
    TMWTYPES_BYTE           Subnetmask[IOCNFG_SUBNETMASK_SIZE];

    TMWTYPES_UINT           PortNumber;

    TMWTYPES_UINT           SocketRxBufSize;
    TMWTYPES_UINT           SocketTxBufSize;

    /* Callback function for this channel */
    TMWTARG_CHANNEL_CALLBACK_FUNC     pChannelCallback;      /* From TMWTARG_CONFIG  */
    void                             *pChannelCallbackParam; /* From TMWTARG_CONFIG  */

    TMWTARG_CHANNEL_READY_CBK_FUNC    pChannelReadyCallback; /* From TMWTARG_CONFIG  */
    void                             *pChannelReadyCbkParam; /* From TMWTARG_CONFIG  */

    TMWTARG_CHANNEL_RECEIVE_CBK_FUNC  pReceiveCallbackFunc;  /* From openChannel    */
    TMWTARG_CHECK_ADDRESS_FUNC        pCheckAddrCallbackFunc;/* From openChannel    */
    void                             *pCallbackParam;        /* From openChannel, used by both above */

} TCP_IO_CHANNEL;


void TargTCP_initPort(void);
TCP_IO_CHANNEL* TargTCP_initChannel(TCP_CONFIG *pMyTcpConfig, TMWTARG_CONFIG *pTmwConfig);
TMWTYPES_BOOL   TargTCP_openChannel(TCP_IO_CHANNEL *pTcpChannel);
uint16  TargTCP_Receive(TCP_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 maxBytes);
bool    TargTCP_Trasmit(TCP_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 numBytes);
void    TargTCP_Monitoring(TCP_IO_CHANNEL* pChannelInfo);




#endif /* SRC_LIB_TMWSCL_TMWTARG_TARGTCP_H_ */
