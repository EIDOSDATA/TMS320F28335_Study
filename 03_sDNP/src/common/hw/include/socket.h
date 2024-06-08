/*
 * socket.h
 *
 *  Created on: 2024. 5. 31.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_COMMON_HW_INCLUDE_SOCKET_H_
#define SRC_COMMON_HW_INCLUDE_SOCKET_H_

#include "hw_def.h"
#include "wsocket.h"

typedef enum
{
    SOCKET0,
    SOCKET1,
    SOCKET2,
    SOCKET3,
    SOCKET_MAX,

} SOCKET_NUM;

typedef enum
{
    PROTOCOL_TCP   = 1,
    PROTOCOL_UDP   = 2,
    PROTOCOL_IPRAW = 3,

} PROTOCOL;

typedef enum
{
    SOCKET_CLOSE      = 0x00,    SOCKET_INIT      = 0x13,    SOCKET_LISTEN     = 0x14,
    SOCKET_SYNSENT    = 0x15,    SOCKET_SYNRECV   = 0x16,    SOCKET_ESTABLISED = 0x17,
    SOCKET_FIN_WAIT   = 0x18,    SOCKET_CLOSING   = 0x1A,    SOCKET_TIME_WAIT  = 0x1B,
    SOCKET_CLOSE_WAIT = 0x1C,    SOCKET_LAST_ACK  = 0x1D,    SOCKET_UDP        = 0x22,
    SOCKET_IPRAW      = 0x32,    SOCKET_MACRAW    = 0x42,    SOCKET_PPOE       = 0x5F

} SOCKET_STATUS;

typedef enum
{
    NONE_ERR,
    IPADDR_ERR,
    GATEWAY_ERR,
    SUBMASK_ERR,

} NETWORK_ERROR_CODE;

typedef struct
{
    bool            IsOpen;
    SOCKET          Socket;
    PROTOCOL        Protocol;
    uint16          Portnumber;

    SOCKET_STATUS   Status;

} SocketHandle;

/*Socket API W5100 wrapper*/
#define Socket(s, protocol, portNumber)                 socket(s, protocol, portNumber)
#define Close(s)                                        wclose(s)
#define Listen(s)                                       listen(s)
#define Send(s, pbuf, writeSize)                        send(s, pbuf, writeSize)
#define Recv(s, pbuf, readSize)                         recv(s, pbuf, readSize)


#define GetIpAddr(out)                                  getSIPR(out)
#define GetGateway(out)                                 getGAR(out)
#define GetSubnetmask(out)                              getSUBR(out)

#define SetIpAddr(in)                                   setSIPR(in)
#define SetGateway(in)                                  setGAR(in)
#define SetSubnetmask(in)                               setSUBR(in)

#define GetSocketStatus(s)                              getSn_SR(s)


bool InitEthernet(uint8 *pMacAddress);
NETWORK_ERROR_CODE NetworkConfig(uint8 *pIpAddr, uint8* pGateway, uint8* pSubnetmask);


SOCKET_STATUS getSocketStatus(SOCKET s);
#endif /* SRC_COMMON_HW_INCLUDE_SOCKET_H_ */
