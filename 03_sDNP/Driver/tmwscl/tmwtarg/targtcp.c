/*
 * targtcp.c
 *
 *  Created on: 2024. 6. 4.
 *      Author: ShinSung Industrial Electric
 */

#include "targtcp.h"

/*Target mem config*/
#include "stack_mem.h"

typedef struct
{
    uint16          SocketRemaing;

    TCP_IO_CHANNEL  ETH_Device[SOCKET_MAX];

} DeviceModule;

static DeviceModule EthObject;

void TargTCP_initPort(void)
{
    memset(&EthObject, 0, sizeof(DeviceModule));

    EthObject.SocketRemaing = SOCKET_MAX;

}

/*TCP Channel initialization function*/
TCP_IO_CHANNEL* TargTCP_initChannel(TCP_CONFIG *pMyTcpConfig, TMWTARG_CONFIG *pTmwConfig)
{
    if((pMyTcpConfig == NULL) || (pTmwConfig == NULL))
        return NULL;

    TCP_IO_CHANNEL* pEthernetHandle = NULL;

    if(EthObject.SocketRemaing)
    {
        uint16 Index = SOCKET_MAX - EthObject.SocketRemaing;
        pEthernetHandle = &EthObject.ETH_Device[Index];

        uint16 PriorIp[IOCNFG_IP_ADDRESS_SIZE];
        uint16 PriorGateway[IOCNFG_GATEWAY_SIZE];
        uint16 PriorSubnet[IOCNFG_SUBNETMASK_SIZE];

        /*applying ethernet configuation data*/
        memcpy(pEthernetHandle->SoreceIp,   &pMyTcpConfig->SoureceIp[0],    IOCNFG_IP_ADDRESS_SIZE);
        memcpy(pEthernetHandle->Gateway,    &pMyTcpConfig->GateWay[0],      IOCNFG_GATEWAY_SIZE);
        memcpy(pEthernetHandle->Subnetmask, &pMyTcpConfig->SubnetMask[0],   IOCNFG_SUBNETMASK_SIZE);

        pEthernetHandle->TargChType = TMWTARGIO_TYPE_TCP;
        pEthernetHandle->Socket     = pMyTcpConfig->Socket[Index];
        pEthernetHandle->PortNumber = pMyTcpConfig->PortNumber[Index];

        /*TMW Dnp stack config*/
        pEthernetHandle->pChannelCallback       = pTmwConfig->pChannelCallback;
        pEthernetHandle->pChannelCallbackParam  = pTmwConfig->pCallbackParam;
        pEthernetHandle->pChannelReadyCallback  = pTmwConfig->pChannelReadyCallback;
        pEthernetHandle->pChannelReadyCbkParam  = pTmwConfig->pChannelReadyCbkParam;

        /*If the settings have changed compared to the previous values, reapply the settings*/

        /*ip address*/
        GetIpAddr(&PriorIp[0]);
        if(memcmp(&PriorIp[0], &pEthernetHandle->SoreceIp[0], IOCNFG_IP_ADDRESS_SIZE) != 0)
            SetIpAddr((uint8*)&pEthernetHandle->SoreceIp[0]);

        /*gateway*/
        GetGateway(&PriorGateway[0]);
        if(memcmp(&PriorGateway[0], &pEthernetHandle->Gateway[0], IOCNFG_GATEWAY_SIZE) != 0)
            SetGateway((uint8*)&pEthernetHandle->Gateway[0]);

        /*subnetmask*/
        GetSubnetmask(&PriorSubnet[0]);
        if(memcmp(&PriorSubnet[0], &pEthernetHandle->Subnetmask[0], IOCNFG_SUBNETMASK_SIZE) != 0)
            SetSubnetmask((uint8*)&pEthernetHandle->Subnetmask[0]);

        EthObject.SocketRemaing--;
    }

    return pEthernetHandle;
}

TMWTYPES_BOOL TargTCP_openChannel(TCP_IO_CHANNEL *pTcpChannel)
{
    if(pTcpChannel == NULL)
        return (0);

    switch(pTcpChannel->Status)
    {
        case SOCKET_CLOSE:
            Socket(pTcpChannel->Socket, PROTOCOL_TCP, pTcpChannel->PortNumber);

            Listen(pTcpChannel->Socket);

            if(GetSocketStatus(pTcpChannel->Socket) == SOCKET_ESTABLISED)
            {
                pTcpChannel->Status = SOCKET_ESTABLISED;
                return TMWDEFS_TRUE;
            }
            break;
        case SOCKET_INIT:
            Listen(pTcpChannel->Socket);

            if(GetSocketStatus(pTcpChannel->Socket) == SOCKET_ESTABLISED)
            {
                pTcpChannel->Status = SOCKET_ESTABLISED;
                return TMWDEFS_TRUE;
            }

            break;
        case SOCKET_CLOSING:
        case SOCKET_CLOSE_WAIT:
            Close(pTcpChannel->Socket);
            break;
        case SOCKET_ESTABLISED:
            return TMWDEFS_TRUE;

    }

    return TMWDEFS_FALSE;

}

uint16 TargTCP_Receive(TCP_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 maxBytes)
{
    if(pChannelInfo == NULL)
        return (0);

    /*read packet from socket*/
    return Recv(pChannelInfo->Socket, pBuf, maxBytes);
}

bool TargTCP_Trasmit(TCP_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 numBytes)
{
    if(pChannelInfo == NULL)
        return (0);

    /*write packet to socket*/
    return Send(pChannelInfo->Socket, pBuf, numBytes);
}

void TargTCP_Monitoring(TCP_IO_CHANNEL* pChannelInfo)
{
    if(pChannelInfo == NULL)
        return ;

    SOCKET_STATUS PriorStatus = pChannelInfo->Status;
    SOCKET_STATUS Status      = (SOCKET_STATUS)GetSocketStatus(pChannelInfo->Socket);

    if(PriorStatus != Status)
    {
        pChannelInfo->Status = Status;

        switch(Status)
        {
            case SOCKET_ESTABLISED:
                pChannelInfo->pChannelCallback(pChannelInfo->pChannelCallbackParam, TMWDEFS_TRUE, TMWDEFS_TARG_OC_SUCCESS);
                break;
            case SOCKET_CLOSE:
            case SOCKET_CLOSING:
            case SOCKET_CLOSE_WAIT:
                pChannelInfo->pChannelCallback(pChannelInfo->pChannelCallbackParam, TMWDEFS_FALSE, TMWDEFS_TARG_OC_FAILURE);
                break;

        }
    }

}


