
/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 2008-2011 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                       www.TriangleMicroWorks.com                          */
/*                          (919) 870-6615                                   */
/*                                                                           */
/* This Source Code and the associated Documentation contain proprietary     */
/* information of Triangle MicroWorks, Inc. and may not be copied or         */
/* distributed in any form without the written permission of Triangle        */
/* MicroWorks, Inc.  Copies of the source code may be made only for backup   */
/* purposes.                                                                 */
/*                                                                           */
/* Your License agreement may limit the installation of this source code to  */
/* specific products.  Before installing this source code on a new           */
/* application, check your license agreement to ensure it allows use on the  */
/* product in question.  Contact Triangle MicroWorks for information about   */
/* extending the number of products that may use this source code library or */
/* obtaining the newest revision.                                            */
/*                                                                           */
/*****************************************************************************/


/* file: linTCP.cpp
* description: Implementation of TCP/IP target routines for Linux
*/

#include "liniodefs.h"
#include "tmwtargio.h"
#include "linuxio.h"
#include "lintcp.h"
#include "liniodiag.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwpltmr.h"
#include <ifaddrs.h>

TMW_ThreadDecl linTCP_socketThread(void *pVoidArg);

#if LINIOTARG_SUPPORT_TCP
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

extern LINIO_CONTEXT LinIoContext;


/* function: linTCP_initChannel */
void * TMWDEFS_GLOBAL linTCP_initChannel(
  const void *pUserConfig,
  TMWTARG_CONFIG *pTmwConfig)
{
  LINIO_CONFIG *pIOConfig = (LINIO_CONFIG *) pUserConfig;
  TCP_IO_CHANNEL *pTcpChannel;

  pTcpChannel = (TCP_IO_CHANNEL *) malloc(sizeof(TCP_IO_CHANNEL));
  if (pTcpChannel == NULL)
    return TMWDEFS_NULL;

  memset(pTcpChannel, 0, sizeof(TCP_IO_CHANNEL));

  pTcpChannel->bChannelFree = TMWDEFS_FALSE;
  pTcpChannel->dataSocket = INVALID_SOCKET;
  pTcpChannel->udpSocket = INVALID_SOCKET;

  pTcpChannel->ipPort = pIOConfig->targTCP.ipPort;
  pTcpChannel->mode = pIOConfig->targTCP.mode;
  pTcpChannel->role = pIOConfig->targTCP.role;

  strncpy(pTcpChannel->nicName, pIOConfig->targTCP.nicName, TMWTARG_IF_NAME_LENGTH);
  strncpy(pTcpChannel->ipAddress, pIOConfig->targTCP.ipAddress, TMWTARG_IP_ADDR_LENGTH);
  strncpy(pTcpChannel->localIpAddress, pIOConfig->targTCP.localIpAddress, TMWTARG_IP_ADDR_LENGTH);
  strncpy(pTcpChannel->chnlName, pIOConfig->targTCP.chnlName, LINIOCNFG_MAX_NAME_LEN);

  pTcpChannel->validateUDPAddress = pIOConfig->targTCP.validateUDPAddress;
  pTcpChannel->validUDPAddress  = 0;
  pTcpChannel->afInet = AF_INET;
  if (pIOConfig->targTCP.ipVersion == TMWTARG_IPV6)
  {
    pTcpChannel->afInet = AF_INET6;
  }
  strncpy(pTcpChannel->udpBroadcastAddress, pIOConfig->targTCP.udpBroadcastAddress, TMWTARG_IP_ADDR_LENGTH);
  pTcpChannel->destUDPPort = pIOConfig->targTCP.destUDPPort;
  pTcpChannel->initUnsolUDPPort = pIOConfig->targTCP.initUnsolUDPPort;
  pTcpChannel->localUDPPort = pIOConfig->targTCP.localUDPPort;
  pTcpChannel->ipConnectTimeout = pIOConfig->targTCP.ipConnectTimeout;
  pTcpChannel->disconnectOnNewSyn = pIOConfig->targTCP.disconnectOnNewSyn;
  pTcpChannel->dualEndPointIpPort = pIOConfig->targTCP.dualEndPointIpPort;

  pTcpChannel->pChannelCallback = pTmwConfig->pChannelCallback;
  pTcpChannel->pChannelCallbackParam = pTmwConfig->pCallbackParam;
  pTcpChannel->pChannelReadyCallback = pTmwConfig->pChannelReadyCallback;
  pTcpChannel->pChannelReadyCbkParam = pTmwConfig->pChannelReadyCbkParam;

  pthread_mutex_lock(LinIoContext.tcpSemaphore);
  pTcpChannel->pNext = LinIoContext.pTcpChannels;
  LinIoContext.pTcpChannels = pTcpChannel;
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);

  return pTcpChannel;
}

/* function: linTCP_deleteChannel */
void TMWDEFS_GLOBAL linTCP_deleteChannel(TCP_IO_CHANNEL *pTcpChannel)
{
  linTCP_closeChannel(pTcpChannel);

  pthread_mutex_lock(LinIoContext.tcpSemaphore);

  /* Mark this channel to be freed by socketThread */
  pTcpChannel->bChannelFree = TMWDEFS_TRUE;

  /* If this was using a listener, see if any other channels still need that listener */
  if ((pTcpChannel->mode == TMWTARGTCP_MODE_SERVER)
    ||(pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT))
  { 
    TMWTYPES_BOOL foundAnotherChannel = TMWDEFS_FALSE;
    TMWTYPES_USHORT listenPort = pTcpChannel->ipPort;

    pTcpChannel = LinIoContext.pTcpChannels;
    while (pTcpChannel != NULL)
    {
      if ((!pTcpChannel->bChannelFree)
        &&((pTcpChannel->mode == TMWTARGTCP_MODE_SERVER)
        ||(pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)))
      {
        if(listenPort == pTcpChannel->ipPort)
        {
          foundAnotherChannel = TMWDEFS_TRUE;
          break;
        }
      }
      pTcpChannel = pTcpChannel->pNext;
    }

    /* If no other channel needs this listener, mark it for deletion */
    if(foundAnotherChannel == TMWDEFS_FALSE)
    {
      TCP_LISTENER *pTcpListener = LinIoContext.pTcpListeners;
      while (pTcpListener != NULL)
      {
        if ((!pTcpListener->bChannelFree)
            &&(pTcpListener->listenPort == listenPort))
        { 
          /* Mark this listener to be deleted by socketThread */
          pTcpListener->bChannelFree = TMWDEFS_TRUE;
          close(pTcpListener->listenSocket);
          pTcpListener->listenSocket = INVALID_SOCKET; 
          break;
        }
        pTcpListener = pTcpListener->pNext;
      }
    } 
  }
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);
}

/* function: linTCP_getChannelName */
const char * TMWDEFS_GLOBAL linTCP_getChannelName(TCP_IO_CHANNEL *pContext)
{
  return(pContext->chnlName);
}

/* function: linTCP_getChannelInfo */
const char * TMWDEFS_GLOBAL linTCP_getChannelInfo(TCP_IO_CHANNEL *pContext)
{
  sprintf(pContext->chanInfoBuf,"Nic: %s Port: %s:%d",
    pContext->nicName,
    pContext->ipAddress,
    pContext->ipPort
   );
  return(pContext->chanInfoBuf);
}

/* function: _setupUDP */
static TMWTYPES_BOOL _setupUDP(TCP_IO_CHANNEL *pTcpChannel)
{
  char *pIpAddressString;
  struct addrinfo *pAddrInfo;
  struct addrinfo hints;
  int    result;
  struct sockaddr_in my_addr;
  SOCKET udpSocket;

  if (pTcpChannel->udpSocket != INVALID_SOCKET)
  {
    return TMWDEFS_TRUE; 
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_family = pTcpChannel->afInet;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;

  if(pTcpChannel->mode == TMWTARGTCP_MODE_UDP)
    /* If UDP only, then use the configured IP address to send all messages to */
    pIpAddressString = pTcpChannel->ipAddress;
  else
    /* Otherwise use the broadcast address to send only broadcast UDP messages to */
    pIpAddressString = pTcpChannel->udpBroadcastAddress;

  if(strcmp(pIpAddressString, "*.*.*.*") == 0)
  {
    pTcpChannel->destUDPaddr.sin_family = pTcpChannel->afInet;
    pTcpChannel->destUDPaddr.sin_port = htons(pTcpChannel->destUDPPort);
    pTcpChannel->destUDPaddr.sin_addr.s_addr = INADDR_ANY;
  }
  else 
  {
    result = getaddrinfo(pIpAddressString, NULL, &hints, &pAddrInfo);
    if (result != 0)
    {
      TMWTYPES_CHAR buf[64];
      tmwtarg_snprintf(buf, sizeof(buf), "UDP address resolution failed with %d", result); 
      LINIODIAG_ERRORMSG(buf);
      return(TMWDEFS_FALSE);
    }
  
    /* Just use the first one */
    memcpy(&pTcpChannel->destUDPaddr, pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
    pTcpChannel->destUDPaddrLen = pAddrInfo->ai_addrlen;
    freeaddrinfo(pAddrInfo);
  }


  /* Now set up UDP socket to send and receive on. */
  udpSocket = socket(pTcpChannel->afInet, SOCK_DGRAM, IPPROTO_UDP);
  if (udpSocket == INVALID_SOCKET)
  {
    LINIODIAG_ERRORMSG("UDP socket failed\n");
    return(TMWDEFS_FALSE);
  }

  my_addr.sin_family = pTcpChannel->afInet;
  my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (pTcpChannel->localUDPPort == TMWTARG_UDP_PORT_ANY)
    my_addr.sin_port = 0;
  else
    my_addr.sin_port = htons(pTcpChannel->localUDPPort);

  if (bind(udpSocket, (struct sockaddr *) &my_addr, sizeof(my_addr)) != 0)
  {
    TMWTYPES_CHAR buf[128];  
    tmwtarg_snprintf(buf, sizeof(buf), "UDP socket bind failed: %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);
    close(udpSocket);
    return(TMWDEFS_FALSE);
  }
  pTcpChannel->udpReadIndex = 0;
  pTcpChannel->udpWriteIndex = 0;
  pTcpChannel->udpSocket = udpSocket; 

  if(pTcpChannel->validateUDPAddress)
  { 
    /* If UDP only, use the configured address */
    if(pTcpChannel->mode == TMWTARGTCP_MODE_UDP)
    {
      pTcpChannel->validUDPAddress = pTcpChannel->destUDPaddr.sin_addr.s_addr;
    }
    else
    {
      /* Set UDPAddress for validation when connection is made */
      pTcpChannel->validUDPAddress = INADDR_NONE; 
    }
  }

  return(TMWDEFS_TRUE);
}

/* function: _connect 
 *  Send a TCP connect request to the server
 */
TMWTYPES_BOOL _connect(TCP_IO_CHANNEL *pTcpChannel)
{
  struct addrinfo     *pAddrInfo;
  struct addrinfo     hints;
  char                buf[64];
  char                portName[10];
  struct sockaddr_in6 localAddr;
  SOCKET              dataSocket;
  struct timeval      timeout;
  fd_set              writeFds;
  int                 result;
  int                 error;
  socklen_t           length;

  if (pTcpChannel->dataSocket != INVALID_SOCKET)
  {
    /* Already connected */
#if defined(DEBUG_LINIOTARG)
    printf("connect, already connected \n");
#endif
    return(TMWDEFS_TRUE);
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_flags = 0;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_family = pTcpChannel->afInet;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;
 
  sprintf(portName, "%d", pTcpChannel->ipPort); 
  result = getaddrinfo(pTcpChannel->ipAddress, portName, &hints, &pAddrInfo);
  if (result != 0)
  {
    tmwtarg_snprintf(buf, sizeof(buf), "TCP address resolution failed with %d", result);
    LINIODIAG_ERRORMSG(buf);
    return(TMWDEFS_FALSE);
  }
 
  tmwtarg_snprintf(buf, sizeof(buf), "TCP, Connecting to %s %s", pTcpChannel->ipAddress, portName);
  LINIODIAG_MSG(buf);

  dataSocket = socket(pTcpChannel->afInet, SOCK_STREAM, IPPROTO_TCP);
  if (dataSocket == INVALID_SOCKET)
  {
    LINIODIAG_ERRORMSG("TCP socket failed");
    return(TMWDEFS_FALSE);
  }

/*
  MAKE_SOCKET_REUSEADDR(dataSocket, result);
  if (result != 0)
  {
    liniotarg_diagErrorMsg("TCP reuse address failed");
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }
*/

  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin6_family = pTcpChannel->afInet;
  localAddr.sin6_port = htons(0);
  localAddr.sin6_addr = in6addr_any;

  if (bind(dataSocket, (struct sockaddr *) &localAddr, sizeof(localAddr)) != 0)
  {
    TMWTYPES_CHAR buf[128]; 
    tmwtarg_snprintf(buf, sizeof(buf), "TCP connect socket bind failed, %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_NON_BLOCKING(dataSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP non-blocking failed");
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_NO_DELAY(dataSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP no-delay failed");
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_KEEPALIVE(dataSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP connect set keepalive failed");
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  if (pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)
  {
    struct sockaddr_in *pSockAddrIn = (struct sockaddr_in *) pAddrInfo->ai_addr;
    pSockAddrIn->sin_port = htons(pTcpChannel->dualEndPointIpPort);
  }

  result = connect(dataSocket, (struct sockaddr *) pAddrInfo->ai_addr, pAddrInfo->ai_addrlen);
  if (result != 0)
  {
    result = errno;
    if ((result != EWOULDBLOCK) && (result != EINPROGRESS))
    { 
      TMWTYPES_CHAR buf[128]; 
      tmwtarg_snprintf(buf, sizeof(buf), "TCP connect bad return code %s", strerror(result));
      LINIODIAG_ERRORMSG(buf);

      close(dataSocket);
      return(TMWDEFS_FALSE);
    }
#if defined(DEBUG_LINIOTARG)
    printf("TCP connect started\n");
#endif
  }
  freeaddrinfo(pAddrInfo);

  /* Wait for connection to come up */
  timeout.tv_sec = 0;
  timeout.tv_usec = pTcpChannel->ipConnectTimeout * 1000;
  FD_ZERO(&writeFds);
  FD_SET(dataSocket, &writeFds);
  result = select(dataSocket + 1, NULL, &writeFds, NULL, &timeout);
  if (result == SOCKET_ERROR)
  {
    TMWTYPES_CHAR buf[128];
    tmwtarg_snprintf(buf, sizeof(buf), "TCP select failed waiting for connection, %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);

    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  /* If no error, see if activity on the correct file descriptor */
  if(!FD_ISSET(dataSocket, &writeFds))
  {
    LINIODIAG_ERRORMSG("TCP connect failed, closing socket\r\n");
    close(dataSocket);
    return(TMWDEFS_FALSE);
  }

  /*if SO_ERROR == 0 then it connected successfully */
  length = sizeof(error);
  result = getsockopt(dataSocket, SOL_SOCKET, SO_ERROR, (void*) &error, &length);
  if(error !=0)
  { 
    TMWTYPES_CHAR buf[128];
    tmwtarg_snprintf(buf, sizeof(buf), "TCP connect failed, %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);

    close(dataSocket);
    return(TMWDEFS_FALSE);
  }
 
  pTcpChannel->dataSocket = dataSocket;

  return(TMWDEFS_TRUE);
}

/* function: linTCP_listen */
TMWTYPES_BOOL linTCP_listen(TCP_IO_CHANNEL *pTcpChannel)
{
  TCP_LISTENER *pTcpListener;
  struct sockaddr_in6  localAddr;
  SOCKET       listenSocket;
  int          result;
  struct ifreq ifr;
#if TMWCNFG_SUPPORT_DIAG
  char         diagBuf[128];
#endif

  /* if there is already a listener on this port, don't listen again */
  pthread_mutex_lock(LinIoContext.tcpSemaphore);
  pTcpListener = LinIoContext.pTcpListeners;
  while (pTcpListener != NULL)
  {
    if ((pTcpListener->listenPort == pTcpChannel->ipPort) &&
        (memcmp(&pTcpListener->listenAddr, &pTcpChannel->ipAddr, sizeof(struct in6_addr)) == 0))
    {
      /* already listening on this port */
      pthread_mutex_unlock(LinIoContext.tcpSemaphore);
      return TMWDEFS_TRUE;
    }
    pTcpListener = pTcpListener->pNext;
  } 
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);

  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin6_family = pTcpChannel->afInet;
  localAddr.sin6_port = htons(pTcpChannel->ipPort);
  localAddr.sin6_addr = in6addr_any;

  if ((strcmp(pTcpChannel->localIpAddress, "*.*.*.*") != 0) && (pTcpChannel->afInet == AF_INET))
  {
    struct sockaddr_in *in = (struct sockaddr_in*)&localAddr;
    inet_pton(pTcpChannel->afInet, pTcpChannel->localIpAddress, &in->sin_addr);
    memcpy(&pTcpChannel->ipAddr, &in->sin_addr, sizeof(struct in_addr));
  }

  listenSocket = socket(pTcpChannel->afInet, SOCK_STREAM, IPPROTO_TCP);
  if (listenSocket == INVALID_SOCKET)
  {
    LINIODIAG_ERRORMSG("TCP listen socket failed");
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_REUSEADDR(listenSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP reuse address failed");
    close(listenSocket);
    return(TMWDEFS_FALSE);
  }

  if(strcmp(pTcpChannel->nicName, "") != 0) {
    char *p_char;
    int   nic_name_len;
    int   alias_len;
    struct ifaddrs *ifa, *ifa_tmp;
    memset(&ifr, 0, sizeof(ifr));       
    nic_name_len = strlen(pTcpChannel->nicName);
    p_char = strchr(pTcpChannel->nicName, ':');
    if (p_char == NULL) {
      alias_len = 0;
    } else {
      alias_len = strlen(p_char);
    } 
    //If the nicName contain an alias (eth0:5), only copy the interface part (eth0)
    strncpy(&ifr.ifr_name[0], &pTcpChannel->nicName[0], nic_name_len - alias_len);
    result = setsockopt(listenSocket, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr));
    if (result != 0) {
      TMWTYPES_CHAR buf[128];
      tmwtarg_snprintf(buf, sizeof(buf), "TCP listen socket bind to interface(%s) failed, %s", ifr.ifr_name, strerror(errno));
      LINIODIAG_ERRORMSG(buf);
      close(listenSocket);
      return(TMWDEFS_FALSE);
    }

    if (getifaddrs(&ifa) == -1) {
      TMWTYPES_CHAR buf[128];
      tmwtarg_snprintf(buf, sizeof(buf), "TCP listen socket getifaddrs failed, %s", strerror(errno));
      LINIODIAG_ERRORMSG(buf);
      return(TMWDEFS_FALSE);
    }

    ifa_tmp = ifa;
    while (ifa_tmp) {
      if ((ifa_tmp->ifa_addr) && (ifa_tmp->ifa_addr->sa_family == pTcpChannel->afInet) &&
        (strncmp(pTcpChannel->nicName, ifa_tmp->ifa_name, TMWTARG_IF_NAME_LENGTH) == 0)) {
        if (ifa_tmp->ifa_addr->sa_family == AF_INET) {
          // store IPv4 addresss
          struct sockaddr_in *in = (struct sockaddr_in*) ifa_tmp->ifa_addr;
          memcpy(&pTcpChannel->ipAddr, &in->sin_addr, sizeof(struct in_addr));
        }
        else
        { 
          // store IPv6 address
          struct sockaddr_in6 *in6 = (struct sockaddr_in6*) ifa_tmp->ifa_addr;
          memcpy(&pTcpChannel->ipAddr, &in6->sin6_addr, sizeof(struct in6_addr));
        }
#if defined(DEBUG_LINIOTARG)
        char addr[50];
        inet_ntop(pTcpChannel->afInet, &pTcpChannel->ipAddr, addr, sizeof(addr));
        printf("name = %s\n", ifa_tmp->ifa_name);
        printf("addr = %s\n", addr);
#endif
        break;
      }
      ifa_tmp = ifa_tmp->ifa_next;
    }
  } 

  if (bind(listenSocket, (struct sockaddr *) &localAddr, sizeof(localAddr)) != 0)
  {
    TMWTYPES_CHAR buf[128]; 
    tmwtarg_snprintf(buf, sizeof(buf), "TCP listen socket bind failed, %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);
    close(listenSocket);
    return(TMWDEFS_FALSE);
  }

  if (listen(listenSocket, LINTCP_BACKLOG) != 0)
  {
    LINIODIAG_ERRORMSG("TCP: listen failed");
    close(listenSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_NON_BLOCKING(listenSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP non-blocking listen failed");
    close(listenSocket);
    return(TMWDEFS_FALSE);
  }

  pTcpListener = malloc(sizeof(TCP_LISTENER));
  if (pTcpListener == NULL)
  {
    LINIODIAG_ERRORMSG("TCP listen could not malloc");
    close(listenSocket);
    return TMWDEFS_FALSE;
  }

  pTcpListener->bChannelFree = TMWDEFS_FALSE;
  pTcpListener->listenSocket = listenSocket;
  pTcpListener->listenPort = pTcpChannel->ipPort;
  pTcpListener->listenAddr = pTcpChannel->ipAddr;

  pthread_mutex_lock(LinIoContext.tcpSemaphore);

  /* Put this on the list of listeners */
  pTcpListener->pNext = LinIoContext.pTcpListeners;
  LinIoContext.pTcpListeners = pTcpListener;

  pthread_mutex_unlock(LinIoContext.tcpSemaphore);

#if TMWCNFG_SUPPORT_DIAG
  sprintf(diagBuf,"TCP listening on port %d",pTcpChannel->ipPort);
  LINIODIAG_MSG(diagBuf);
#endif

  return(TMWDEFS_TRUE);
}

/* function: linTCP_accept */
TMWTYPES_BOOL linTCP_accept(TCP_LISTENER *pTcpListener)
{
  int                 result;
  SOCKET              acceptSocket;
  struct sockaddr_in  remoteAddr;
  socklen_t           remoteAddrLen;
  TCP_IO_CHANNEL      *pTcpChannel;
  TCP_IO_CHANNEL      *pUseThisTcpChannel;

  remoteAddrLen = sizeof(remoteAddr);
  acceptSocket = accept(pTcpListener->listenSocket,
    (struct sockaddr *) &remoteAddr, &remoteAddrLen);
  if (acceptSocket == INVALID_SOCKET)
  {
    LINIODIAG_ERRORMSG("TCP: accept failed\n");
    return(TMWDEFS_FALSE);
  }

#if defined(DEBUG_LINIOTARG)
  {
    int a,b,c,d;
    TMWTYPES_CHAR buf[96];
    unsigned long temp = remoteAddr.sin_addr.s_addr;
    a = (temp >> 24);
    temp = temp << 8;
    b = (temp >> 24);
    temp = temp << 8;
    c = (temp >> 24);
    temp = temp << 8;
    d = (temp >> 24);
    tmwtarg_snprintf(buf, sizeof(buf), "TCP: Accept received connect request from IP address %d.%d.%d.%d", d,c,b,a);
    liniotarg_diagMsg(buf);
  }
#endif

  /* We have a new connection, set up socket parameters for TCP */
  MAKE_SOCKET_NON_BLOCKING(acceptSocket, result);
  if (result != 0)
  {
    TMWTYPES_CHAR buf[64];
    tmwtarg_snprintf(buf, sizeof(buf), "MAKE_SOCKET_N_BLOCKING returned %d", result);
    LINIODIAG_ERRORMSG(buf);
    close(acceptSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_NO_DELAY(acceptSocket, result);
  if (result != 0)
  { 
    TMWTYPES_CHAR buf[64];
    tmwtarg_snprintf(buf, sizeof(buf), "MAKE_SOCKET_NO_DELAY returned %d", result);
    LINIODIAG_ERRORMSG(buf);
    close(acceptSocket);
    return(TMWDEFS_FALSE);
  }

  MAKE_SOCKET_KEEPALIVE(acceptSocket, result);
  if (result != 0)
  {
    LINIODIAG_ERRORMSG("TCP accept set keepalive failed");
    close(acceptSocket);
    return(TMWDEFS_FALSE);
  }

  pUseThisTcpChannel = TMWDEFS_NULL;

  /* Look for a matching channel */
  pTcpChannel = LinIoContext.pTcpChannels;
  while (pTcpChannel != NULL)
  {
    if ((!pTcpChannel->bChannelFree)
      &&((pTcpChannel->mode == TMWTARGTCP_MODE_SERVER)
        ||(pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)))
    {
      /* See if the address and port matches */
      if(((strcmp(pTcpChannel->ipAddress, "*.*.*.*") == 0)
        ||(inet_addr(pTcpChannel->ipAddress) == remoteAddr.sin_addr.s_addr))
        && (pTcpChannel->ipPort == pTcpListener->listenPort))
      { 
        /* If channel is not yet connected */
        if (pTcpChannel->dataSocket == INVALID_SOCKET)
        { 
          pUseThisTcpChannel = pTcpChannel;
          break;
        }
        else /* already connected */
        {
          if (pTcpChannel->disconnectOnNewSyn)
          {
            /* this is for IEC protocols, not DNP */
            /* if configured, mark this channel for disconnect. This will allow
             * new connection to come in next time. This handles not receiving notification
             * of disconnect from remote end, but remote end trying to reconnect.
             *
             * If you want to allow multiple simultaneous connections from any IP address
             * to this particular port, this parameter should be set to FALSE
             */   

            LINIODIAG_MSG("TCP: Accept previously connected to this ip address and a new connection has come in");

            /* Keep looking, but save this channel to use if an unconnected one is not found. */
            pUseThisTcpChannel = pTcpChannel;
          }

          /* If this is a DNP dual end point outstation, 
           * close the existing connection and accept the new one 
           */
          else if ((pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)
                &&(pTcpChannel->role == TMWTARGTCP_ROLE_OUTSTATION))
          {  
            /* Close existing connection and accept the new connection.*/
            LINIODIAG_MSG("TCP: Accept Dual End Point outstation, previously connected to this ip address and a new connection has come in");
            close(pTcpChannel->dataSocket);
            pTcpChannel->dataSocket = INVALID_SOCKET; 
            pUseThisTcpChannel = pTcpChannel;
            break;
          } 
          /* this channel is busy, look at the next one */
        }
      }
    }
    pTcpChannel = pTcpChannel->pNext;
  } 

  if(pUseThisTcpChannel != TMWDEFS_NULL)
  {
    LINIODIAG_MSG("TCP: Accept this connection");

    /* If configured for it, save the address from the master to
     * be used for comparing with src address in UDP requests.
     */
    if(pUseThisTcpChannel->validateUDPAddress)
    {
      pUseThisTcpChannel->validUDPAddress = remoteAddr.sin_addr.s_addr;
    }

    if (pUseThisTcpChannel->pChannelCallback != TMWDEFS_NULL)
    {
      /* TCPChannels are only removed by this thread, release tcpSemaphore to prevent deadlock with SCL channel lock */
      pthread_mutex_unlock(LinIoContext.tcpSemaphore); 

      pUseThisTcpChannel->pChannelCallback(pUseThisTcpChannel->pChannelCallbackParam,
        TMWDEFS_TRUE, /* open */
        TMWDEFS_TARG_OC_SUCCESS);

      pthread_mutex_lock(LinIoContext.tcpSemaphore);
 
      pUseThisTcpChannel->dataSocket = acceptSocket; 
    }
  }
  else
  {
    /* No one interested so close the socket */
    close(acceptSocket);
    return(TMWDEFS_FALSE);
  }

  return(TMWDEFS_TRUE);
}
 
/* function: inputUdpData 
 * get UDP bytes from socket and copy into circular buffer
 */
static void _inputUdpData(TCP_IO_CHANNEL *pTcpChannel)
{
  /* Perform address check */
  struct sockaddr_in fromAddr;
  socklen_t   fromAddrLen;
  int         remain;
  int         result;
  char        tempBuff[ LINIOCNFG_MAX_UDP_FRAME ];

  fromAddrLen = sizeof(fromAddr);
  result = recvfrom(pTcpChannel->udpSocket, tempBuff, LINIOCNFG_MAX_UDP_FRAME, 0, 
    (struct sockaddr *) &fromAddr, &fromAddrLen);

  if (result < 0)
  {
    TMWTYPES_CHAR buf[64];
    tmwtarg_snprintf(buf, sizeof(buf), "Bad return from read of UDP: %d", errno);
    LINIODIAG_ERRORMSG(buf);
    return;
  }
 
  /* If outstation TCP and UDP, verify the src address with TCP connection end point.
   * If outstation UDP only, verify the src address of master
   * If master UDP only, verify src address of slave.
   * These are done by setting validUDPAddress to appropriate address
   *   if validation is enabled.
   */
  if((pTcpChannel->validUDPAddress != 0)
    &&(fromAddr.sin_addr.s_addr != pTcpChannel->validUDPAddress))
  {
    return;
  }

  pthread_mutex_lock(LinIoContext.tcpSemaphore);
  pTcpChannel->sourceUDPPort = ntohs(fromAddr.sin_port);
  remain = LINIOCNFG_UDP_BUFFER_SIZE - pTcpChannel->udpWriteIndex;
  if (remain >= result)
  {
    memcpy(&pTcpChannel->udpBuffer[ pTcpChannel->udpWriteIndex ], tempBuff, result);
    pTcpChannel->udpWriteIndex += result;
  }
  else
  {
    memcpy(&pTcpChannel->udpBuffer[ pTcpChannel->udpWriteIndex ], tempBuff, remain);
    result = result - remain;
    memcpy(&pTcpChannel->udpBuffer[ 0 ], &tempBuff[ remain ], result);
    pTcpChannel->udpWriteIndex = result;
  }
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);

  return;
}

/* function: linTCP_connectThread */
TMW_ThreadDecl linTCP_connectThread(TMW_ThreadArg pVoidArg)
{
  TCP_IO_CHANNEL      *pTcpChannel;
  TMWTARG_UNUSED_PARAM(pVoidArg);

  while (LinIoContext.connectThreadState == LIN_THREAD_RUNNING)
  {
    /* Handle any clients that need connections */
    pthread_mutex_lock(LinIoContext.tcpSemaphore);
    pTcpChannel = LinIoContext.pTcpChannels;
    while (pTcpChannel != NULL)
    {
      if((pTcpChannel->dataSocket == INVALID_SOCKET)
        &&(pTcpChannel->mode == TMWTARGTCP_MODE_CLIENT)
        &&(!pTcpChannel->bChannelFree))
      {
        if(_connect(pTcpChannel))
        {
          TCP_IO_CHANNEL *pConnectedTcpChannel;

          /* Release semaphore to prevent deadlock  */
          pthread_mutex_unlock(LinIoContext.tcpSemaphore); 

          pTcpChannel->pChannelCallback(pTcpChannel->pChannelCallbackParam,
            TMWDEFS_TRUE, /* open */
            TMWDEFS_TARG_OC_SUCCESS);

          pConnectedTcpChannel = pTcpChannel;

          /* 1/2 second */
          tmwtarg_sleep(500);

          pthread_mutex_lock(LinIoContext.tcpSemaphore);

          /* Since TCPChannel could have been removed by socketThread, 
           * see if it is still on the list of channels
           */
          pTcpChannel = LinIoContext.pTcpChannels;
          while (pTcpChannel != NULL)
          {
            if(pTcpChannel == pConnectedTcpChannel)
            {
              break;
            }
            pTcpChannel = pTcpChannel->pNext;
          }
          /* If it was removed, start back at beginning of list */
          if(pTcpChannel == NULL)
          {
            pTcpChannel = LinIoContext.pTcpChannels;
            continue;
          }
        }
      } 
      pTcpChannel = pTcpChannel->pNext;
    } 
    pthread_mutex_unlock(LinIoContext.tcpSemaphore);

    /* 100000 == 1 second */
    tmwtarg_sleep(1000);
  }
  LinIoContext.connectThreadState = LIN_THREAD_EXITED;
  return((TMW_ThreadPtr) NULL);
}

/* function: linTCP_socketThread */
TMW_ThreadDecl linTCP_socketThread(TMW_ThreadArg pVoidArg)
{
  TCP_IO_CHANNEL      *pTcpChannel;
  TCP_IO_CHANNEL      **pChannelAnchor;
  TCP_LISTENER        **pListenerAnchor;
  struct timeval      timeout;
  fd_set              readFds;
  SOCKET              highest;
  int                 result;
  TMWTARG_UNUSED_PARAM(pVoidArg);

  while (LinIoContext.socketThreadState == LIN_THREAD_RUNNING)
  {
    TCP_LISTENER *pTcpListener;
    TMWTYPES_BOOL portsOpen = TMWDEFS_FALSE;

    highest = 0;
    FD_ZERO(&readFds);

    pthread_mutex_lock(LinIoContext.tcpSemaphore);

    /* First add the listeners to the select list */
    pTcpListener = LinIoContext.pTcpListeners;
    pListenerAnchor = &LinIoContext.pTcpListeners;
    while (pTcpListener != NULL)
    { 
      if (pTcpListener->bChannelFree)
      {
        /* This listener is marked for deletion */
        TCP_LISTENER *pTemp;

        /* Remove the listener from the list and free it */
        *pListenerAnchor = pTcpListener->pNext;
        pTemp = pTcpListener;
        pTcpListener = pTcpListener->pNext;
        free(pTemp);
        continue;
      }
      if (pTcpListener->listenSocket != INVALID_SOCKET)
      {
        FD_SET(pTcpListener->listenSocket, &readFds);
        if (pTcpListener->listenSocket > highest)
          highest = pTcpListener->listenSocket;
      }
      pListenerAnchor = &pTcpListener->pNext;
      pTcpListener = pTcpListener->pNext;
    }

    /* Now add the individual channels to the select list */
    pTcpChannel = LinIoContext.pTcpChannels;
    pChannelAnchor = &LinIoContext.pTcpChannels;
    while (pTcpChannel != NULL)
    {
      if (pTcpChannel->bChannelFree)
      {
        /* This channel is marked for deletion */
        TCP_IO_CHANNEL *pTemp;

        /* Remove the channel from the list and free it */
        *pChannelAnchor = pTcpChannel->pNext;
        pTemp = pTcpChannel;
        pTcpChannel = pTcpChannel->pNext;
        free(pTemp);
        continue;
      }

      if (pTcpChannel->udpSocket != INVALID_SOCKET)
      {
        FD_SET(pTcpChannel->udpSocket, &readFds);
        if (pTcpChannel->udpSocket > highest)
          highest = pTcpChannel->udpSocket;
      }
      /* If event driven uncomment this. 
       *if (pTcpChannel->dataSocket != INVALID_SOCKET) 
       *{
       *  FD_SET(pTcpChannel->dataSocket, &readFds);
       *  if (pTcpChannel->dataSocket > highest)
       *    highest = pTcpChannel->dataSocket;
       *}
       */
      pChannelAnchor = &pTcpChannel->pNext;
      pTcpChannel = pTcpChannel->pNext;
    }

    pthread_mutex_unlock(LinIoContext.tcpSemaphore);

    if (highest == 0)
    {
      /* There are no listening channels or UDP channels
       * pause for 1 second 
       */
      tmwtarg_sleep(1000);
      continue;
    }

    highest++;

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;

    result = select(highest, &readFds, NULL, NULL, &timeout);
    if (result == SOCKET_ERROR)
    {
      /* This is probably due to one of the sockets being closed asynchronously */
      TMWTYPES_CHAR buf[128];   
      tmwtarg_snprintf(buf, sizeof(buf), "TCP select failed (%s)", strerror(errno));
      LINIODIAG_ERRORMSG(buf);

      tmwtarg_sleep(1);
      continue;
    }

    if (result > 0)
    {
      TCP_LISTENER *pTcpListener;

      pthread_mutex_lock(LinIoContext.tcpSemaphore);

      /* First check the listeners for incoming connections */
      pTcpListener = LinIoContext.pTcpListeners;
      while (pTcpListener != NULL)
      {
        if ((!pTcpListener->bChannelFree)
          &&(pTcpListener->listenSocket != INVALID_SOCKET))
        {
          if (FD_ISSET(pTcpListener->listenSocket, &readFds))
          {
            linTCP_accept(pTcpListener);
          }
        }
        pTcpListener = pTcpListener->pNext;
      } 
      
      /* Now check the channels for data */
      portsOpen = TMWDEFS_FALSE;
      pTcpChannel = LinIoContext.pTcpChannels;
      while (pTcpChannel != NULL)
      {
        if (!pTcpChannel->bChannelFree) 
        {
          /* If event driven uncomment this.
           * TMWTYPES_BOOL callCallback = TMWDEFS_FALSE; 
           */
          if (pTcpChannel->udpSocket != INVALID_SOCKET)
          {
            portsOpen = TMWDEFS_TRUE;
            if (FD_ISSET(pTcpChannel->udpSocket, &readFds))
            {           
              _inputUdpData(pTcpChannel);
              /* if event driven
               * callCallback = TMWDEFS_TRUE;
               */
            }
          }
          /* If event driven uncomment this.
           * if(pTcpChannel->dataSocket != INVALID_SOCKET)
           *{ 
           *  portsOpen = TMWDEFS_TRUE;
           *  if (FD_ISSET(pTcpChannel->dataSocket, &readFds))
           *  {
           *    callCallback = TMWDEFS_TRUE;
           *  }
           * }
           *if(callCallback)
           *  pTcpChannel->pReceiveCallbackFunc(pTcpChannel->pCallbackParam);
           */
        }
        pTcpChannel = pTcpChannel->pNext;
      }
      pthread_mutex_unlock(LinIoContext.tcpSemaphore);
    }
    else
    {
      /* There are open listeners or UDP ports, but there was no activity
       */
      if(portsOpen)
        tmwtarg_sleep(5); /* 5 milliseconds */
      else
        tmwtarg_sleep(100); /* 100 milliseconds */
    }
  }
  LinIoContext.socketThreadState = LIN_THREAD_EXITED;
  return((TMW_ThreadPtr) NULL);
}

/* function: linTCP_openChannel */
TMWTYPES_BOOL linTCP_openChannel(TCP_IO_CHANNEL *pTcpChannel)
{
  TMWTYPES_BOOL status = TMWDEFS_FALSE;

  /* Check for UDP configured */
  if (pTcpChannel->localUDPPort != TMWTARG_UDP_PORT_NONE)
  { 
    /* This is tolerant of failure, if TCP AND UDP. may want to catch this
     * The problem is both master and slave use 20000 by default...
     */
    status = _setupUDP(pTcpChannel); 
  }

  /* If configured for UDP only, don't listen or connect, return success or failure */
  if(pTcpChannel->mode != TMWTARGTCP_MODE_UDP)
  { 
    if (pTcpChannel->mode == TMWTARGTCP_MODE_SERVER)
    {
      status = linTCP_listen(pTcpChannel);
    
      if(status == TMWDEFS_TRUE)
      {
        /* listening, is it connected yet? */
        if(pTcpChannel->dataSocket == INVALID_SOCKET) 
          status = TMWDEFS_FALSE;
      }
    }

    else if (pTcpChannel->mode == TMWTARGTCP_MODE_CLIENT)
    {
      /* connected or not? */
      if (pTcpChannel->dataSocket == INVALID_SOCKET)
      {
        status = TMWDEFS_FALSE;
      }
      else
      {
        status = TMWDEFS_TRUE;
      }

      /* If connect thread is not yet running, start it */
      if (LinIoContext.connectThreadState == LIN_THREAD_IDLE)
      {
        status = _connect(pTcpChannel);
        if(!status)
        {
          LinIoContext.connectThreadState = LIN_THREAD_RUNNING;
          TMW_ThreadCreate(&LinIoContext.connectThreadHandle,
            linTCP_connectThread, (TMW_ThreadArg) NULL, 0, 0);
        }
      }
    }

    else if (pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)
    {
      /* Setup listener.
       * Return true even though not connected yet
       * so that SCL will attempt to send data when needed
       * which will cause a connection to be attempted.
       */
      status = linTCP_listen(pTcpChannel);
    }
  }

  /* If socket thread is not yet running, start it */
  if (LinIoContext.socketThreadState == LIN_THREAD_IDLE)
  {
    LinIoContext.socketThreadState = LIN_THREAD_RUNNING;
    TMW_ThreadCreate(&LinIoContext.socketThreadHandle,
      linTCP_socketThread, (TMW_ThreadArg) NULL, 0, 0);
  }

  return(status);
}

/* function: linTCP_closeChannel */
void TMWDEFS_GLOBAL linTCP_closeChannel(TCP_IO_CHANNEL *pTcpChannel)
{
  pthread_mutex_lock(LinIoContext.tcpSemaphore);

  if (pTcpChannel->dataSocket != INVALID_SOCKET)
  {
    close(pTcpChannel->dataSocket);
    pTcpChannel->dataSocket = INVALID_SOCKET;
  } 
  if (pTcpChannel->udpSocket != INVALID_SOCKET)
  {
    close(pTcpChannel->udpSocket);
    pTcpChannel->udpSocket = INVALID_SOCKET;
  }
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);
}

/* function: linTCP_getTransmitReady */
TMWTYPES_MILLISECONDS TMWDEFS_GLOBAL linTCP_getTransmitReady(TCP_IO_CHANNEL *pTcpChannel)
{
  if (!pTcpChannel->bChannelFree)
  {
    if ((pTcpChannel->mode == TMWTARGTCP_MODE_UDP)
      && (pTcpChannel->udpSocket != INVALID_SOCKET))
      return(0);

    if (pTcpChannel->dataSocket != INVALID_SOCKET)
      return(0);

    if ((pTcpChannel->mode == TMWTARGTCP_MODE_DUAL_ENDPOINT)
      && (pTcpChannel->dataSocket == INVALID_SOCKET))
    {
      /* Retry connection */
      if (!_connect(pTcpChannel))
        return(1000);
    }
  }
  return(500);    /* Wait for things to happen */
}

/* function: linio_UDPReceive
 *  Copy any received UDP data from circular buffer into return pointer
 */
static TMWTYPES_USHORT _UDPReceive(
  TCP_IO_CHANNEL  *pTcpChannel,
  TMWTYPES_UCHAR  *pBuff,
  TMWTYPES_ULONG  maxBytes)
{
  TMWTYPES_ULONG udpReadIndex;
  TMWTYPES_ULONG udpWriteIndex;
  TMWTYPES_ULONG bytesRead = 0;
  TMWTYPES_UCHAR *readPtr = NULL;

  // See if there are any bytes in the circular buffer
  if(pTcpChannel->udpReadIndex == pTcpChannel->udpWriteIndex)
    return(0);

  pthread_mutex_lock(LinIoContext.tcpSemaphore);

  udpReadIndex = pTcpChannel->udpReadIndex;
  udpWriteIndex = pTcpChannel->udpWriteIndex;
  readPtr = &pTcpChannel->udpBuffer[udpReadIndex];

  if(udpReadIndex < udpWriteIndex)
  {
    bytesRead = udpWriteIndex - udpReadIndex;
    if(bytesRead > maxBytes)
    {
      bytesRead = maxBytes;
    }
    pTcpChannel->udpReadIndex += bytesRead;
  }
  else
  {
    bytesRead = LINIOCNFG_UDP_BUFFER_SIZE - udpReadIndex;
    if(bytesRead > maxBytes)
    {
      bytesRead = maxBytes;
      pTcpChannel->udpReadIndex += bytesRead;
    }
    else
    {
      pTcpChannel->udpReadIndex = 0;
    }
  }
  pthread_mutex_unlock(LinIoContext.tcpSemaphore);

  memcpy(pBuff, readPtr, bytesRead);
  return((TMWTYPES_USHORT)bytesRead);
}

/* function: linTCP_receive, this also receives UDP data */
TMWTYPES_USHORT TMWDEFS_GLOBAL linTCP_receive(
  TCP_IO_CHANNEL *pContext,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT maxBytes,
  TMWTYPES_MILLISECONDS maxTimeout,
  TMWTYPES_BOOL *pInterCharTimeoutOccurred)
{
  TCP_IO_CHANNEL    *pTcpChannel = (TCP_IO_CHANNEL *) pContext;
  int               result;
  fd_set            readFds;
  struct timeval    timeout;
  TMWTARG_UNUSED_PARAM(maxTimeout);
  TMWTARG_UNUSED_PARAM(pInterCharTimeoutOccurred);

  /* first see if there are any bytes received on the UDP socket 
   * Currently only for DNP Networking.
   */
  if(pTcpChannel->udpSocket != INVALID_SOCKET)
  {
    TMWTYPES_USHORT numReceived;
    numReceived = _UDPReceive(pTcpChannel, pBuff, maxBytes);
    if(numReceived > 0)
    {  
      char buf[64];
      tmwtarg_snprintf(buf, sizeof(buf), "TCP Received %d bytes of UDP data", numReceived);
      LINIODIAG_MSG(buf);
      return(numReceived);
    }
  }

  if (pTcpChannel->dataSocket == INVALID_SOCKET)
    return(0);

  FD_ZERO(&readFds);
  FD_SET(pTcpChannel->dataSocket, &readFds);

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

  result = select(pTcpChannel->dataSocket + 1, &readFds, NULL, NULL, &timeout);
  if (result != SOCKET_ERROR)
  {
    if (FD_ISSET(pTcpChannel->dataSocket, &readFds))
    {
      result = recv(pTcpChannel->dataSocket, pBuff, maxBytes, LINTCP_RECV_FLAGS);
      if (result > 0)
        return((TMWTYPES_USHORT) result); 
    }
    else
      return((TMWTYPES_USHORT) 0);
  }

  close(pTcpChannel->dataSocket);
  pTcpChannel->dataSocket = INVALID_SOCKET;

  if (pTcpChannel->pChannelCallback != TMWDEFS_NULL)
    pTcpChannel->pChannelCallback(pTcpChannel->pChannelCallbackParam,
    TMWDEFS_FALSE, /* close */
    TMWDEFS_TARG_OC_FAILURE);

  return((TMWTYPES_USHORT) 0);
}

/* function: linTCP_transmit */
TMWTYPES_BOOL TMWDEFS_GLOBAL linTCP_transmit(
  TCP_IO_CHANNEL *pContext,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes)
{
  TCP_IO_CHANNEL *pTcpChannel = (TCP_IO_CHANNEL *) pContext;
  int result;

  if (pTcpChannel->dataSocket != INVALID_SOCKET)
  {
    result = send(pTcpChannel->dataSocket, (char *) pBuff, numBytes, 0);
    if (result <= 0)
    {
      close(pTcpChannel->dataSocket);
      pTcpChannel->dataSocket = INVALID_SOCKET;
      if (pTcpChannel->pChannelCallback != TMWDEFS_NULL)
        pTcpChannel->pChannelCallback(pTcpChannel->pChannelCallbackParam,
        TMWDEFS_FALSE, 
        TMWDEFS_TARG_OC_FAILURE);
      return(TMWDEFS_FALSE);
    }
  }
  return(TMWDEFS_TRUE);
}

/* function: linTCP_transmitUDP */
TMWTYPES_BOOL TMWDEFS_GLOBAL linTCP_transmitUDP(
  TCP_IO_CHANNEL *pContext,
  TMWTYPES_UCHAR UDPPort,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes)
{
  TCP_IO_CHANNEL      *pTcpChannel = (TCP_IO_CHANNEL *) pContext;
  TMWTYPES_USHORT     nUDPPort;
  int                 result;

  if (pTcpChannel->udpSocket != INVALID_SOCKET)
  {
    if (pTcpChannel->sourceUDPPort == TMWTARG_UDP_PORT_NONE)
    {
      if (UDPPort == TMWTARG_UDP_SEND)
        nUDPPort = pTcpChannel->destUDPPort;
      else
        nUDPPort = pTcpChannel->initUnsolUDPPort;
    }
    else
    {
      /* If configuration allows it, use src port from previous request
       * This would only be allowed on an outstation.
       */
      if (pTcpChannel->destUDPPort == TMWTARG_UDP_PORT_SRC)
        nUDPPort = pTcpChannel->sourceUDPPort;
      else
        nUDPPort = pTcpChannel->destUDPPort;
    }

    pTcpChannel->destUDPaddr.sin_port = htons(nUDPPort);
    result = sendto(pTcpChannel->udpSocket, (char *) pBuff, numBytes, 0,
      (const struct sockaddr *) &pTcpChannel->destUDPaddr,
      pTcpChannel->destUDPaddrLen);
    if (result <= 0)
    {
      close(pTcpChannel->udpSocket);
      pTcpChannel->udpSocket = INVALID_SOCKET;

      if (pTcpChannel->pChannelCallback != TMWDEFS_NULL)
        pTcpChannel->pChannelCallback(pTcpChannel->pChannelCallbackParam,
        TMWDEFS_FALSE, 
        TMWDEFS_TARG_OC_FAILURE);

      return(TMWDEFS_FALSE);
    }
  }
  return(TMWDEFS_TRUE);
}
#endif // #if LINIOTARG_SUPPORT_TCP
