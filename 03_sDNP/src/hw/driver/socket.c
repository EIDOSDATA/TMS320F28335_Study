/*
 * socket.c
 *
 *  Created on: 2024. 5. 31.
 *      Author: ShinSung Industrial Electric
 */

#include <string.h>
#include "socket.h"

#define MAC_ADDRESS_SIZE        (6)
#define IP_ADDR_SIZE            (4)
#define GATEWAY_SIZE            (4)
#define SUBNETMASK_SIZE         (4)
/*W5100 Socket Buffer size config value*/
#define DEVIDE_2KB_PER_SOCKET   (0x55)
#define DEVIDE_4KB_PER_SOCKET   (0x0A)






bool InitEthernet(uint8 *pMacAddress)
{
    uint8 Check[MAC_ADDRESS_SIZE];
    memset(&Check[0], 0, sizeof(uint8) * MAC_ADDRESS_SIZE);

    /* W5100S Device Reset */
    iinchip_init();
    /* RX/TX buffer 4KB per Sockets  socket 0,1 4 KB, socket 2,3 O KB*/
    sysinit(DEVIDE_4KB_PER_SOCKET, DEVIDE_4KB_PER_SOCKET);
    /* Disable Interrupt */
    setIMR(0);

    /* Mac address write*/
    setSHAR(pMacAddress);
    /* Mac address read*/
    getSHAR(Check);

    if(memcmp(pMacAddress, &Check[0], MAC_ADDRESS_SIZE) == 0)
    {
        return TRUE;
    }

    return FALSE;
}

NETWORK_ERROR_CODE NetworkConfig(uint8 *pIpAddr, uint8* pGateway, uint8* pSubnetmask)
{
    uint8 IpAddr[IP_ADDR_SIZE];
    uint8 Gateway[GATEWAY_SIZE];
    uint8 Subnetmask[SUBNETMASK_SIZE];

    memset(&IpAddr[0],      0, sizeof(uint8) * IP_ADDR_SIZE);
    memset(&Gateway[0],     0, sizeof(uint8) * GATEWAY_SIZE);
    memset(&Subnetmask[0],  0, sizeof(uint8) * SUBNETMASK_SIZE);

    /*IP ADDR*/
    setSIPR(pIpAddr);
    getSIPR(IpAddr);

    if(memcmp(&IpAddr[0], pIpAddr, IP_ADDR_SIZE) != 0)
    {
        /*error*/
        return IPADDR_ERR;
    }

    /*GATEWAY*/
    setGAR(pGateway);
    getGAR(Gateway);

    if(memcmp(&Gateway[0], pGateway, GATEWAY_SIZE) != 0)
    {
        /*error*/
        return GATEWAY_ERR;
    }

    /*SUBNET MASK*/
    setSUBR(pSubnetmask);
    getSUBR(Subnetmask);

    if(memcmp(&Subnetmask[0], pSubnetmask, SUBNETMASK_SIZE) != 0)
    {
        /*error*/
        return SUBMASK_ERR;
    }

    return NONE_ERR;
}

SOCKET_STATUS getSocketStatus(SOCKET s)
{
    return getSn_SR(s);
}

