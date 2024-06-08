/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright (c) 1997-2017 */
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

/* file: sdnpo122.h
 * description: This file is intended for internal SCL use only.
 *   DNP Slave functionality for Object 122 Security Statistics Change events.
 */
#ifndef SDNPO122_DEFINED
#define SDNPO122_DEFINED

#include "tmwscl/utils/tmwdefs.h"
#include "tmwscl/utils/tmwdtime.h"

#include "tmwscl/dnp/sdnpsesn.h"
#include "tmwscl/dnp/sdnpdata.h"
#include "tmwscl/dnp/sdnpevnt.h"

#if SDNPDATA_SUPPORT_OBJ120 && SDNPCNFG_SUPPORT_SA_VERSION5

#ifdef __cplusplus
extern "C" {
#endif

  /* function: sdnpo122_addEvent
   * purpose: Add a security statistic event to event queue
   * arguments:
   *  pSession - pointer to session
   *  point - point number of point that generated the event
   *  value - value of counter
   *  flags - current DNP flags
   *  pTimeStamp - pointer to time of event
   *   This should be gotten by calling sdnputil_getDateTime(). This will
   *   set the invalid field of the time structure properly.
   * returns:
   *  void
   */
  TMWDEFS_SCL_API void TMWDEFS_GLOBAL sdnpo122_addEvent(
    TMWSESN *pSession, 
    TMWTYPES_USHORT assocId,
    TMWTYPES_USHORT point, 
    TMWTYPES_ULONG value, 
    TMWTYPES_UCHAR flags, 
    TMWDTIME *pTimeStamp);

  /* function: sdnpo122_init
   * purpose: Initialize security statistic change events
   * arguments:
   *  pSession - pointer to session
   * returns:
   *  void
   */
  void TMWDEFS_GLOBAL sdnpo122_init(
    TMWSESN *pSession);

  /* function: sdnpo122_close
   * purpose: 
   * arguments:
   * returns:
   *  void
   */ 
  void TMWDEFS_GLOBAL sdnpo122_close(
    TMWSESN *pSession);

  /* function: sdnpo122_countEvents
   * purpose: Count the number of security statistic change events in
   *  queue
   * arguments:
   *  pSession - pointer to session
   *  classMask - count events in this class or classes
   * returns:
   *  void
   */
  TMWTYPES_USHORT TMWDEFS_GLOBAL sdnpo122_countEvents(
    TMWSESN *pSession, 
    TMWDEFS_CLASS_MASK classMask,
    TMWTYPES_BOOL countAll,
    TMWTYPES_USHORT threshold);

  /* function: sdnpo122_cleanupEvents
   * purpose: Either remove events that have been sent or clear the
   *  sent flags if message was not acknowledged.
   * arguments:
   *  pSession - pointer to session
   *  deleteEvents - remove events if true, else clear sent flag
   * returns:
   *  void
   */
  TMWTYPES_BOOL TMWDEFS_GLOBAL sdnpo122_cleanupEvents(
    TMWSESN *pSession, 
    TMWTYPES_BOOL deleteEvents);

  /* function: sdnpo122_readObj122
   * purpose: Read security statistic change events using specified variation
   * arguments:
   *  pSession - pointer to session
   *  pResponse - response buffer to read into
   *  pObjHeader - current request object header
   * returns:
   *  void
   */
  SDNPSESN_READ_STATUS TMWDEFS_CALLBACK sdnpo122_readObj122(
    TMWSESN *pSession, 
    DNPUTIL_RX_MSG *pRequest,
    TMWSESN_TX_DATA *pResponse, 
    DNPUTIL_OBJECT_HEADER *pObjHeader,
    SDNPSESN_QUAL qualifier);

  /* function: sdnpo122_readObj122v0ByClass
   * purpose: Read security statistic change events using variation 0 for a specific 
   *  class
   * arguments:
   *  pSession - pointer to session
   *  pResponse - response buffer to read into
   *  pObjHeader - current request object header
   *  classMask - class of event to read
   * returns:
   *  void
   */
  SDNPSESN_READ_STATUS TMWDEFS_CALLBACK sdnpo122_readobj122v0ByClass(
    TMWSESN *pSession, 
    DNPUTIL_RX_MSG *pRequest,
    TMWSESN_TX_DATA *pResponse, 
    DNPUTIL_OBJECT_HEADER *pObjHeader, 
    TMWDEFS_CLASS_MASK classMask);

#ifdef __cplusplus
}
#endif
#endif /* SDNPDATA_SUPPORT_OBJ120 && SDNPCNFG_SUPPORT_SA_VERSION5 */
#endif /* SDNPO122_DEFINED */
