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

/* file: LinioDefs.h
 * description: Definition of target types and macros for Linux
 */

#ifndef LinioDefs_DEFINED
#define LinioDefs_DEFINED

#include <termios.h>
#include "tmwtargos.h"
#include "liniocnfg.h"

#include "tmwscl/utils/tmwtypes.h"
#include "tmwscl/utils/tmwdefs.h"
#include "tmwscl/utils/tmwtarg.h"

#if LINIOTARG_SUPPORT_232

typedef enum Lin232ParityEnum {
  LIN232_PARITY_NONE,
  LIN232_PARITY_EVEN,
  LIN232_PARITY_ODD
} LIN232_PARITY;

typedef enum Lin232StopBitsEnum {
  LIN232_STOP_BITS_1,
  LIN232_STOP_BITS_2
} LIN232_STOP_BITS;

typedef enum Lin232DataBitsEnum {
  LIN232_DATA_BITS_7,
  LIN232_DATA_BITS_8
} LIN232_DATA_BITS;

typedef enum Lin232PortMode
{
  LIN232_MODE_NONE,
  LIN232_MODE_HARDWARE
} LIN232_PORT_MODE;

typedef enum Lin232BaudRate
{
  LIN232_BAUD_110     = B110,
  LIN232_BAUD_300     = B300,
  LIN232_BAUD_600     = B600,
  LIN232_BAUD_1200    = B1200,
  LIN232_BAUD_2400    = B2400,
  LIN232_BAUD_4800    = B4800,
  LIN232_BAUD_9600    = B9600,
  LIN232_BAUD_19200   = B19200,
  LIN232_BAUD_38400   = B38400,
  LIN232_BAUD_57600   = B57600,
  LIN232_BAUD_115200  = B115200,
  LIN232_BAUD_230400  = B230400,
  LIN232_BAUD_576000  = B576000,
  LIN232_BAUD_921600  = B921600,
  LIN232_BAUD_1152000 = B1152000
} LIN232_BAUD_RATE;


typedef struct Lin232ConfigStruct {
  /* User specified channel name */
  TMWTYPES_CHAR    chnlName[LINIOCNFG_MAX_NAME_LEN]; 

  /* "/dev/ttyS0", "/dev/ttyS1", etc. */
  TMWTYPES_CHAR    portName[LINIOCNFG_MAX_NAME_LEN]; 

  /* flow control: none, hardware */
  LIN232_PORT_MODE portMode;

  /* baud rate */
  LIN232_BAUD_RATE baudRate;

  /* parity */
  LIN232_PARITY    parity;
 
  /* 7 or 8 */
  LIN232_DATA_BITS numDataBits;

  /* 1 or 2 */
  LIN232_STOP_BITS numStopBits;

  /* MODBUS RTU or not */
  TMWTYPES_BOOL    bModbusRTU;
} LIN232_CONFIG;

/* Define serial port channel */
typedef struct SerialIOChannel {
  
  /* Callback function for this channel */
  TMWTARG_CHANNEL_CALLBACK_FUNC     pChannelCallback;      /* From TMWTARG_CONFIG  */
  void                             *pChannelCallbackParam; /* From TMWTARG_CONFIG  */
  TMWTARG_CHANNEL_READY_CBK_FUNC    pChannelReadyCallback; /* From TMWTARG_CONFIG  */
  void                             *pChannelReadyCbkParam; /* From TMWTARG_CONFIG  */
  TMWTARG_CHANNEL_RECEIVE_CBK_FUNC  pReceiveCallbackFunc;  /* From openChannel    */
  TMWTARG_CHECK_ADDRESS_FUNC        pCheckAddrCallbackFunc;/* From openChannel    */
  void                             *pCallbackParam;        /* From openChannel, used by both above */

  /* parity */
  LIN232_PARITY      parity;

  /* flow control: none, hardware */
  LIN232_PORT_MODE   portMode;

  /* 7 or 8 */
  LIN232_DATA_BITS   numDataBits;

  /* 1 or 2 */
  LIN232_STOP_BITS   numStopBits;

  /* MODBUS RTU or not */
  TMWTYPES_BOOL      bModbusRTU; 
   
  /* 
   * Specifies the amount of time (in character times) to use to 
   * determine that a frame has been completed.  For modbus RTU this 
   * value is 3.5 (i.e. 4 will be used)
   */
  TMWTYPES_USHORT numCharTimesBetweenFrames;

  /*
   * Specifies the amount of time to use to 
   * determine that an inter character timeout has occured.  
   * For modbus RTU this value is 1.5 character times (i.e. 2 would be used)
   */
  TMWTYPES_USHORT interCharTimeout;
  
  /* Linux tty fd must be carried around */
  TMWTYPES_INT       ttyfd;
 
  LIN232_BAUD_RATE   baudRate;

  TMWTYPES_CHAR      chnlName[LINIOCNFG_MAX_NAME_LEN]; 
  TMWTYPES_CHAR      portName[LINIOCNFG_MAX_NAME_LEN];

  TMWTYPES_CHAR      chanInfoBuf[128];

  struct SerialIOChannel   *pNext;
} SERIAL_IO_CHANNEL;


#endif /* #if LINIOTARG_SUPPORT_232 */

#if LINIOTARG_SUPPORT_TCP
#include <netinet/in.h>
#define LINTCP_BACKLOG          5
#define LINTCP_RECV_FLAGS       0
#define SOCKET              int


 
typedef struct TcpIOChannel {
  TMWTYPES_CHAR chnlName[LINIOCNFG_MAX_NAME_LEN];     /* User specified channel name */

  TMWTARGTCP_MODE      mode;
  TMWTARGTCP_ROLE      role;

  /* Callback function for this channel */
  TMWTARG_CHANNEL_CALLBACK_FUNC     pChannelCallback;      /* From TMWTARG_CONFIG */
  void                             *pChannelCallbackParam; /* From TMWTARG_CONFIG */
  TMWTARG_CHANNEL_READY_CBK_FUNC    pChannelReadyCallback; /* From TMWTARG_CONFIG (not used)         */
  void                             *pChannelReadyCbkParam; /* From TMWTARG_CONFIG (not used)         */
  TMWTARG_CHANNEL_RECEIVE_CBK_FUNC  pReceiveCallbackFunc;  /* From openChannel    (for event driven) */
  TMWTARG_CHECK_ADDRESS_FUNC        pCheckAddrCallbackFunc;/* From openChannel    (not used)         */
  void                             *pCallbackParam;        /* From openChannel, used by both above   */

  int                  afInet;
  TMWTYPES_CHAR        ipAddress[TMWTARG_IP_ADDR_LENGTH];
  TMWTYPES_CHAR        localIpAddress[TMWTARG_IP_ADDR_LENGTH]; /* Allows to bind to specific local address */
  TMWTYPES_CHAR        nicName[TMWTARG_IF_NAME_LENGTH];    /* Allows binding to a specific interface */
  TMWTYPES_CHAR        udpBroadcastAddress[TMWTARG_IP_ADDR_LENGTH];
  TMWTYPES_USHORT      ipPort;
  struct in6_addr      ipAddr;
  TMWTYPES_USHORT      dualEndPointIpPort;
  TMWTYPES_ULONG       ipConnectTimeout;
  TMWTYPES_BOOL        disconnectOnNewSyn;

  TMWTYPES_CHAR        chanInfoBuf[64];
  SOCKET               dataSocket;

  TMWTYPES_USHORT      destUDPPort;
  TMWTYPES_USHORT      initUnsolUDPPort;
  TMWTYPES_USHORT      localUDPPort;
  TMWTYPES_USHORT      sourceUDPPort;
  TMWTYPES_BOOL        validateUDPAddress;
  TMWTYPES_ULONG       validUDPAddress;
  SOCKET               udpSocket;
  struct sockaddr_in   destUDPaddr;
  int                  destUDPaddrLen;

  TMWTYPES_UCHAR       udpBuffer[LINIOCNFG_UDP_BUFFER_SIZE];
  int                  udpReadIndex;
  int                  udpWriteIndex;

  TMWTYPES_BOOL        bChannelFree;

  struct TcpIOChannel *pNext;
} TCP_IO_CHANNEL;

 
typedef struct TcpListener {
  SOCKET              listenSocket;
  TMWTYPES_USHORT     listenPort;
  struct in6_addr     listenAddr;
  TMWTYPES_BOOL       bChannelFree;

  struct TcpListener *pNext;
} TCP_LISTENER;

typedef enum LinThreadState
{
  LIN_THREAD_IDLE,
  LIN_THREAD_EXITED,
  LIN_THREAD_RUNNING,
  LIN_THREAD_EXITING,
} LIN_THREAD_STATE;

#endif /* #if LINIOTARG_SUPPORT_TCP */

typedef struct LinIOChannel {
  TMWTARGIO_TYPE_ENUM   type;
  void                 *pChannelInfo;
} LINIO_CHANNEL;

/* Top level control structure */
typedef struct LinIOContext {
  TMWTYPES_BOOL         bStarted;

#if LINIOTARG_SUPPORT_232
  pthread_mutex_t      *serialSemaphore;
  SERIAL_IO_CHANNEL    *pSerialChannels;
#endif

#if LINIOTARG_SUPPORT_TCP
  pthread_mutex_t      *tcpSemaphore;
  TCP_IO_CHANNEL       *pTcpChannels;
  TCP_LISTENER         *pTcpListeners; 
  LIN_THREAD_STATE      socketThreadState;
  LIN_THREAD_STATE      connectThreadState;
  TMW_ThreadId          socketThreadHandle;
  TMW_ThreadId          connectThreadHandle;
#endif

} LINIO_CONTEXT;

/* Configuration structure */
typedef struct LinIOConfig {
  TMWTARGIO_TYPE_ENUM type;

#if LINIOTARG_SUPPORT_232
  LIN232_CONFIG lin232;
#endif

#if LINIOTARG_SUPPORT_TCP
  TMWTARGTCP_CONFIG targTCP;
#endif
} LINIO_CONFIG;

#endif /*  LinioDefs_DEFINED */
