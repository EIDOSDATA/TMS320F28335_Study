/*
 * wsocket.h
 *
 *  Created on: 2023. 11. 1.
 *      Author: ShinSung Industrial Electric
 */

#ifndef BSP_ZONE7_ETHERNET_WSOCKET_H_
#define BSP_ZONE7_ETHERNET_WSOCKET_H_

#include "w5100.h"

/*Opens a socket(TCP or UDP or IP_RAW mode)*/
extern uint8 socket(SOCKET s, uint8 protocol, uint16 port);
/*Close socket*/
extern void wclose(SOCKET s);
/* Establish TCP connection (Active connection) */
extern uint8 connect(SOCKET s, uint8 * addr, uint16 port);
/*Disconnect the connection*/
extern void disconnect(SOCKET s);
/*Establish TCP connection (Passive connection)*/
extern uint8 listen(SOCKET s);
/*Send data (TCP)*/
extern uint16 send(SOCKET s, const uint8 * buf, uint16 len);
/*Receive data (TCP)*/
extern uint16 recv(SOCKET s, uint8 * buf, uint16 len);
/*Send data (UDP/IP RAW)*/
extern uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port);
/*Receive data (UDP/IP RAW)*/
extern uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port);

#endif /* BSP_ZONE7_ETHERNET_WSOCKET_H_ */
