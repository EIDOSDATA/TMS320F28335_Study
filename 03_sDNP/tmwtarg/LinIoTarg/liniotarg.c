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


/* file: liniotarg.c
* description: Implementation of target I/O routines for Linux
*/
#include "liniotarg.h"
#include "liniodefs.h"
#include "liniocnfg.h"
#include "lin232.h"
#include "lintcp.h"

#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwpltmr.h"


/* function: tmwtargio_initConfig */
void TMWDEFS_GLOBAL tmwtargio_initConfig(LINIO_CONFIG *pConfig)
{

#if (LINIOTARG_SUPPORT_TCP == TMWDEFS_FALSE) && (LINIOTARG_SUPPORT_232 == TMWDEFS_FALSE)
  pConfig->type = TMWTARGIO_TYPE_NONE;
#endif

#if (LINIOTARG_SUPPORT_TCP == TMWDEFS_FALSE) && (LINIOTARG_SUPPORT_232 == TMWDEFS_TRUE)
  pConfig->type = TMWTARGIO_TYPE_232;
#endif

#if (LINIOTARG_SUPPORT_TCP == TMWDEFS_TRUE) && (LINIOTARG_SUPPORT_232 == TMWDEFS_FALSE)
  pConfig->type = TMWTARGIO_TYPE_TCP;
#endif

#if (LINIOTARG_SUPPORT_TCP == TMWDEFS_TRUE) && (LINIOTARG_SUPPORT_232 == TMWDEFS_TRUE)
  pConfig->type = TMWTARGIO_TYPE_232;
#endif

#if LINIOTARG_SUPPORT_232 
  strncpy(pConfig->lin232.chnlName, "/dev/ttyS0", LINIOCNFG_MAX_NAME_LEN);
  strncpy(pConfig->lin232.portName, "/dev/ttyS0", LINIOCNFG_MAX_NAME_LEN);
  pConfig->lin232.baudRate    = LIN232_BAUD_9600;
  pConfig->lin232.numDataBits = LIN232_DATA_BITS_8;
  pConfig->lin232.numStopBits = LIN232_STOP_BITS_1;
  pConfig->lin232.parity      = LIN232_PARITY_NONE;
  pConfig->lin232.portMode    = LIN232_MODE_NONE;
  pConfig->lin232.bModbusRTU  = TMWDEFS_FALSE;
#endif

#if LINIOTARG_SUPPORT_TCP
  strncpy(pConfig->targTCP.chnlName, "", LINIOCNFG_MAX_NAME_LEN);
  pConfig->targTCP.mode = TMWTARGTCP_MODE_CLIENT;
  pConfig->targTCP.role = TMWTARGTCP_ROLE_MASTER;
  pConfig->targTCP.ipVersion = TMWTARG_IPV4;
  strncpy(pConfig->targTCP.ipAddress, "127.0.0.1", TMWTARG_IP_ADDR_LENGTH);
  strncpy(pConfig->targTCP.udpBroadcastAddress, "192.168.1.255", TMWTARG_IP_ADDR_LENGTH);
  pConfig->targTCP.ipPort                 = 20000;
  pConfig->targTCP.dualEndPointIpPort     = 20000;
  pConfig->targTCP.destUDPPort            = 20000;
  pConfig->targTCP.initUnsolUDPPort       = 20000;
  pConfig->targTCP.localUDPPort           = TMWTARG_UDP_PORT_NONE;
  pConfig->targTCP.validateUDPAddress     = TMWDEFS_TRUE;
  pConfig->targTCP.disconnectOnNewSyn     = TMWDEFS_FALSE;
  pConfig->targTCP.ipConnectTimeout       = 1000;
  strncpy(pConfig->targTCP.nicName, "", TMWTARG_IF_NAME_LENGTH);
  strncpy(pConfig->targTCP.localIpAddress, "*.*.*.*", TMWTARG_IP_ADDR_LENGTH);
#endif
}

