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


/* file: lin232.cpp
 * description: Implementation of RS232 target routines for Linux
 */

#include "liniodefs.h"
#include "tmwtargio.h"
#include "lin232.h"
#include "liniodiag.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwpltmr.h"

#if LINIOTARG_SUPPORT_232

extern LINIO_CONTEXT LinIoContext;

static TMWTYPES_CHAR * _getBaudString(LIN232_BAUD_RATE baudRate)
{
  switch(baudRate)
  {
  case LIN232_BAUD_110:
    return("110");
  case LIN232_BAUD_300:
    return("300");
  case LIN232_BAUD_600:
    return("600");
  case LIN232_BAUD_1200:
    return("1200");
  case LIN232_BAUD_2400:
    return("2400");
  case LIN232_BAUD_4800:
    return("4800");
  case LIN232_BAUD_9600:
    return("9600");
  case LIN232_BAUD_19200:
    return("19200");
  case LIN232_BAUD_38400:
    return("38400");
  case LIN232_BAUD_57600:
    return("57600");
  case LIN232_BAUD_115200:
    return("115200");
  case LIN232_BAUD_230400:
    return("230400");
  case LIN232_BAUD_576000:
    return("576000");
  case LIN232_BAUD_921600:
    return("921600");
  case LIN232_BAUD_1152000:
    return("1152000");
  default:
    return("unknown");
  }
}

static TMWTYPES_CHAR * _getParityString(LIN232_PARITY parity)
{
  if(parity == LIN232_PARITY_NONE)
    return("none");
  else if(parity == LIN232_PARITY_EVEN)
    return("even");
  else
    return("odd");
}

/* function: lin232_initChannel */
void * TMWDEFS_GLOBAL lin232_initChannel(
  const void *pUserConfig,
  TMWTARG_CONFIG *pTmwConfig)
{
  LINIO_CONFIG *pIOConfig = (LINIO_CONFIG *) pUserConfig;
  SERIAL_IO_CHANNEL *pSerialChannel;

  pSerialChannel = (SERIAL_IO_CHANNEL *) malloc(sizeof(SERIAL_IO_CHANNEL));
  if (pSerialChannel == NULL)
    return TMWDEFS_NULL;

  memset(pSerialChannel, 0, sizeof(SERIAL_IO_CHANNEL));

  pSerialChannel->pChannelCallback = pTmwConfig->pChannelCallback;
  pSerialChannel->pChannelCallbackParam = pTmwConfig->pCallbackParam;
  pSerialChannel->pChannelReadyCallback = pTmwConfig->pChannelReadyCallback;
  pSerialChannel->pChannelReadyCbkParam = pTmwConfig->pChannelReadyCbkParam;
  pSerialChannel->numCharTimesBetweenFrames = pTmwConfig->numCharTimesBetweenFrames; 
  pSerialChannel->interCharTimeout = pTmwConfig->interCharTimeout;

  strcpy(pSerialChannel->chnlName, pIOConfig->lin232.chnlName);
  strcpy(pSerialChannel->portName, pIOConfig->lin232.portName);

  pSerialChannel->portMode     = pIOConfig->lin232.portMode;
  pSerialChannel->baudRate     = pIOConfig->lin232.baudRate;
  pSerialChannel->parity       = pIOConfig->lin232.parity;
  pSerialChannel->numDataBits  = pIOConfig->lin232.numDataBits;
  pSerialChannel->numStopBits  = pIOConfig->lin232.numStopBits;
  pSerialChannel->bModbusRTU   = pIOConfig->lin232.bModbusRTU;


  pthread_mutex_lock(LinIoContext.serialSemaphore);
  pSerialChannel->pNext = LinIoContext.pSerialChannels;
  LinIoContext.pSerialChannels = pSerialChannel;
  pthread_mutex_unlock(LinIoContext.serialSemaphore);

  return pSerialChannel;
}

/* function: lin232_deleteChannel */
void TMWDEFS_GLOBAL lin232_deleteChannel(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  SERIAL_IO_CHANNEL *pChannel;
  SERIAL_IO_CHANNEL **pChannelAnchor;

  pthread_mutex_lock(LinIoContext.serialSemaphore);
  pChannel = LinIoContext.pSerialChannels;
  pChannelAnchor = &LinIoContext.pSerialChannels;
  while (pChannel != NULL)
  {
    if(pChannel == pSerialChannel)
    {
      /* Remove the channel from the list and free it */
      *pChannelAnchor = pChannel->pNext;
      free(pChannel);
      break;
    }
    pChannelAnchor = &pChannel->pNext;
    pChannel = pChannel->pNext;
  } 
  pthread_mutex_unlock(LinIoContext.serialSemaphore);
}

/* function: lin232_getChannelName */
const char * TMWDEFS_GLOBAL lin232_getChannelName(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  return(pSerialChannel->chnlName);
}

/* function: lin232_getChannelInfo */
const char * TMWDEFS_GLOBAL lin232_getChannelInfo(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  int stopBits = 1;
  int dataBits = 7;

  if(pSerialChannel->numStopBits == LIN232_STOP_BITS_2)
    stopBits = 2;
 
  if(pSerialChannel->numDataBits == LIN232_DATA_BITS_8)
    dataBits = 8;

  sprintf(pSerialChannel->chanInfoBuf, "baud %s, data bits %d, stop bits %d parity %s",
    _getBaudString(pSerialChannel->baudRate), dataBits, stopBits, _getParityString(pSerialChannel->parity));

  return(pSerialChannel->chanInfoBuf);
}

/* function: lin232_openChannel */
TMWTYPES_BOOL TMWDEFS_GLOBAL lin232_openChannel(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  int ttyfd;
  struct termios settings;
#if TMWCNFG_SUPPORT_DIAG
  TMWTYPES_CHAR buf[128];
#endif

  /* zero out the structure */
  memset(&settings, 0, sizeof(settings));

  if ((ttyfd = open(pSerialChannel->portName, O_RDWR | O_NOCTTY)) < 0)
  {
#if TMWCNFG_SUPPORT_DIAG
    tmwtarg_snprintf(buf, sizeof(buf), "Lin232 Error opening %s, %s",  pSerialChannel->portName, strerror(errno));
    LINIODIAG_ERRORMSG(buf);
#endif
    return TMWDEFS_FALSE;
  }

#if TMWCNFG_SUPPORT_DIAG
  tmwtarg_snprintf(buf, sizeof(buf), "Lin232 opened %s",  pSerialChannel->portName);
  LINIODIAG_MSG(buf);
#endif

  /* read your man page for the meaning of all this. # man termios */
  /* Its a bit to involved to comment here                         */
  cfsetispeed(&settings, pSerialChannel->baudRate);
  cfsetospeed(&settings, pSerialChannel->baudRate);

  settings.c_line = 0;
  settings.c_iflag |= IGNBRK;
  settings.c_iflag &=~ IGNPAR;  /*SPM changed this to not ignore framing and parity errors */
  settings.c_iflag &=~ PARMRK;
  settings.c_iflag |=  INPCK; /* SPM changed to enable input parity checking */
  settings.c_iflag &=~ ISTRIP;
  settings.c_iflag &=~ INLCR;
  settings.c_iflag &=~ IGNCR;
  settings.c_iflag &=~ ICRNL;
  settings.c_iflag &=~ IUCLC;
  settings.c_iflag &=~ IXON;
  settings.c_iflag |= IXANY;
  settings.c_iflag &=~ IXOFF;
  settings.c_iflag &=~ IMAXBEL;

  settings.c_oflag |= OPOST;
  settings.c_oflag &=~ OLCUC;
  settings.c_oflag &=~ ONLCR;
  settings.c_oflag &=~ OCRNL;
  settings.c_oflag &=~ ONOCR;   /*SPM changed to allow output cr at column zero*/
  settings.c_oflag &=~ ONLRET;
  settings.c_oflag &=~ OFILL;
  settings.c_oflag &=~ OFDEL;


  settings.c_cflag &=~ CSIZE;
  if(pSerialChannel->numDataBits == LIN232_DATA_BITS_8)
    settings.c_cflag |= CS8;
  else
    settings.c_cflag |= CS7;

  if(pSerialChannel->numStopBits == LIN232_STOP_BITS_1)
    settings.c_cflag &=~ CSTOPB;
  else
    settings.c_cflag |= CSTOPB;

  settings.c_cflag |= CREAD;

  if(pSerialChannel->parity == LIN232_PARITY_NONE)
  {
    settings.c_cflag &=~ PARENB;
    settings.c_cflag &=~ PARODD;
  }
  else if(pSerialChannel->parity == LIN232_PARITY_EVEN)
  {
    settings.c_cflag |= PARENB;
    settings.c_cflag &=~ PARODD;
  }
  else
  {
    settings.c_cflag |= PARENB;
    settings.c_cflag |= PARODD;
  }

  settings.c_cflag &=~ HUPCL;  /* don't lower modem control lines after last process closed the device */
  settings.c_cflag |= CLOCAL;  /* ignore modem control lines */

  if(pSerialChannel->portMode == LIN232_MODE_NONE)
    settings.c_cflag &=~ CRTSCTS;
  else
    settings.c_cflag |= CRTSCTS;

  settings.c_lflag &=~ ISIG;
  settings.c_lflag &=~ ICANON;
  settings.c_lflag &=~ ECHO;
  settings.c_lflag |= IEXTEN;


  if(pSerialChannel->bModbusRTU == TMWDEFS_TRUE)
  { 
    int bitsPerChar;
    int timeBetweenFrames;

    /* for modbus RTU, a message must be contiguous, with no more than 3 empty frames.
     * For now, we will use this fact to extablish the end of a message. 
     */

    /* 1 start bit, 8 data bits, 1 parity bit + 2 stop bits */
    bitsPerChar = 12;
    
    /* time in deciseconds  */
    timeBetweenFrames = ((pSerialChannel->numCharTimesBetweenFrames*bitsPerChar*10) /9600 /*dcb.BaudRate*/) + 1;

    /* The following two combined mean when there is a gap between chars or 255 chars are    *  received the read will return
     */
    settings.c_cc[VMIN] = 255;
    /* time between frames in deciseconds,  1/10 of a second. */
    settings.c_cc[VTIME] = timeBetweenFrames;
  }
  else
  {
    settings.c_cc[VMIN] = 0;
    settings.c_cc[VTIME] = 0;
  }

  if(tcsetattr(ttyfd, TCSANOW, &settings ) < 0)
  {
#if TMWCNFG_SUPPORT_DIAG
    tmwtarg_snprintf(buf, sizeof(buf), "Lin232 tcsetattr failed %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);
#endif
    return(TMWDEFS_FALSE);
  }

  pSerialChannel->ttyfd = ttyfd;
  return(TMWDEFS_TRUE);
}

/* function: lin232_closeChannel */
void TMWDEFS_GLOBAL lin232_closeChannel(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  close(pSerialChannel->ttyfd);
}

/* function: lin232_getTransmitReady */
TMWTYPES_MILLISECONDS TMWDEFS_GLOBAL lin232_getTransmitReady(
  SERIAL_IO_CHANNEL *pSerialChannel)
{
  TMWTARG_UNUSED_PARAM(pSerialChannel);
  return 0;
}

/* function: lin232_receive */
TMWTYPES_USHORT TMWDEFS_GLOBAL lin232_receive(
  SERIAL_IO_CHANNEL *pSerialChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT maxBytes,
  TMWTYPES_MILLISECONDS maxTimeout,
  TMWTYPES_BOOL *pInterCharTimeoutOccurred)
{
  int bytesReceived;
  fd_set          rfds;
  struct timeval  tv;
  TMWTARG_UNUSED_PARAM(maxTimeout);
  TMWTARG_UNUSED_PARAM(pInterCharTimeoutOccurred);

  bytesReceived = 0;

  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(pSerialChannel->ttyfd, &rfds);

  if(select(pSerialChannel->ttyfd + 1, &rfds, NULL, NULL, &tv) != -1)
  {  
    if(FD_ISSET( pSerialChannel->ttyfd, &rfds))
    {
      if((bytesReceived = read(pSerialChannel->ttyfd, pBuff, maxBytes)) == -1)
      {
        TMWTYPES_CHAR buf[128];
        tmwtarg_snprintf(buf, sizeof(buf), "Lin232 read failed %s", strerror(errno));
        LINIODIAG_ERRORMSG(buf);
        bytesReceived = 0;
      }
    }
  }
  return bytesReceived;
}

/* function: lin232_transmit */
TMWTYPES_BOOL TMWDEFS_GLOBAL lin232_transmit(
  SERIAL_IO_CHANNEL *pSerialChannel,
  TMWTYPES_UCHAR *pBuff,
  TMWTYPES_USHORT numBytes)
{
  int bytesSent = write(pSerialChannel->ttyfd, pBuff, numBytes);
  if(numBytes == (TMWTYPES_USHORT)bytesSent)
    return TMWDEFS_TRUE;
  else
  {
#if TMWCNFG_SUPPORT_DIAG
    TMWTYPES_CHAR buf[128];
    tmwtarg_snprintf(buf, sizeof(buf), "Lin232 write failed %s", strerror(errno));
    LINIODIAG_ERRORMSG(buf);
#endif

    return TMWDEFS_FALSE;
  }
}

#endif // #if LINIOTARG_SUPPORT_232
