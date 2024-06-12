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

/* file: linTCP.h
 * description: Definition of TCP/IP target routines for Linux
 */
#ifndef linTCP_DEFINED
#define linTCP_DEFINED

#if LINIOTARG_SUPPORT_TCP

#include "tmwscl/utils/tmwtypes.h"
#include "tmwscl/utils/tmwdefs.h"
#include "tmwscl/utils/tmwtarg.h"


/* function: linTCP_initChannel */
void * TMWDEFS_GLOBAL linTCP_initChannel(
  const void *pUserConfig,
  TMWTARG_CONFIG *pTmwConfig);

/* function: linTCP_deleteChannel */
void TMWDEFS_GLOBAL linTCP_deleteChannel(TCP_IO_CHANNEL *pChannel);

/* function: linTCP_getChannelName */
const char * TMWDEFS_GLOBAL linTCP_getChannelName(TCP_IO_CHANNEL *pChannel);

/* function: linTCP_getChannelInfo */
const char * TMWDEFS_GLOBAL linTCP_getChannelInfo(TCP_IO_CHANNEL *pChannel);

/* function: linTCP_openChannel */
TMWTYPES_BOOL TMWDEFS_GLOBAL linTCP_openChannel(TCP_IO_CHANNEL *pChannel);

/* function: linTCP_closeChannel */
void TMWDEFS_GLOBAL linTCP_closeChannel(TCP_IO_CHANNEL *pChannel);

/* function: linTCP_getTransmitReady */
TMWTYPES_MILLISECONDS TMWDEFS_GLOBAL linTCP_getTransmitReady(
  TCP_IO_CHANNEL *pChannel);

/* function: linTCP_receive */
TMWTYPES_USHORT TMWDEFS_GLOBAL linTCP_receive(
  TCP_IO_CHANNEL *pChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT maxBytes,
  TMWTYPES_MILLISECONDS maxTimeout,
  TMWTYPES_BOOL *pInterCharTimeoutOccurred);

/* function: linTCP_transmit */
TMWTYPES_BOOL TMWDEFS_GLOBAL linTCP_transmit(
  TCP_IO_CHANNEL *pChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes);

TMWTYPES_BOOL TMWDEFS_GLOBAL linTCP_transmitUDP(
  TCP_IO_CHANNEL *pContext,
  TMWTYPES_UCHAR UDPPort,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes);

#endif // #if LINIOTARG_SUPPORT_TCP

#endif // linTCP_DEFINED
