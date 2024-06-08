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

/* file: lin232.h
 * description: Definition of RS232 target routines for Linux
 */
#ifndef lin232_DEFINED
#define lin232_DEFINED

#if LINIOTARG_SUPPORT_232

#include "linuxio.h"

#include "tmwscl/utils/tmwtypes.h"
#include "tmwscl/utils/tmwdefs.h"
#include "tmwscl/utils/tmwtarg.h"


/* function: lin232_initChannel */
void * TMWDEFS_GLOBAL lin232_initChannel(
  const void *pUserConfig,
  TMWTARG_CONFIG *pTmwConfig);

/* function: lin232_deleteChannel */
void TMWDEFS_GLOBAL lin232_deleteChannel(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_getChannelName */
const char * TMWDEFS_GLOBAL lin232_getChannelName(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_getChannelInfo */
const char * TMWDEFS_GLOBAL lin232_getChannelInfo(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_openChannel */
TMWTYPES_BOOL TMWDEFS_GLOBAL lin232_openChannel(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_closeChannel */
void TMWDEFS_GLOBAL lin232_closeChannel(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_getTransmitReady */
TMWTYPES_MILLISECONDS TMWDEFS_GLOBAL lin232_getTransmitReady(
  SERIAL_IO_CHANNEL *pSerialChannel);

/* function: lin232_receive */
TMWTYPES_USHORT TMWDEFS_GLOBAL lin232_receive(
  SERIAL_IO_CHANNEL *pSerialChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT maxBytes,
  TMWTYPES_MILLISECONDS maxTimeout,
  TMWTYPES_BOOL *pInterCharTimeoutOccurred);

/* function: lin232_transmit */
TMWTYPES_BOOL TMWDEFS_GLOBAL lin232_transmit(
  SERIAL_IO_CHANNEL *pSerialChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes);

#endif // #if LINIOTARG_SUPPORT_232

#endif // lin232_DEFINED
