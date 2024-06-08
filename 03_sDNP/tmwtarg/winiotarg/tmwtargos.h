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

/* file: twmtargos.h
 * description: Common definitions used by the SCL that are OS dependent.
 */
#ifndef tmwtargos_DEFINED
#define tmwtargos_DEFINED

#include "tmwscl/tmwtarg/WinioTarg/include/WinIoTargEnums.h"

/* Type for the handle that the OS passes back for the created thread */
#define TMW_ThreadId            unsigned int

/* Declaration for the entry point of a thread */
#define TMW_ThreadDecl          unsigned int __stdcall

/* The type definition of the entry point of a thread */
/* In most cases it will be the same as TMW_ThreadDecl */
#define TMW_ThreadPtr           unsigned int

/* The type defintion the OS expects to pass as an arguement when starting a thread */
#define TMW_ThreadArg           void *

/* A Macro is used here instead of a function call because of Windows __stdcall calling convention */
#define TMW_ThreadCreate(pThreadId, pFunc, funcArg, priority, stackSize)                       \
  HANDLE threadHandle;                                                                         \
  threadHandle = (HANDLE)_beginthreadex(NULL, 0, pFunc, funcArg, CREATE_SUSPENDED, pThreadId); \
  SetThreadPriority(threadHandle, THREAD_PRIORITY_NORMAL);                                     \
  ResumeThread(threadHandle); 

/* Ideally there would be a single enumerated type for all target layers.
* The Windows specific mapping is done to ensure backward compatibility and with .NET.
*/
typedef WINTCP_ROLE TMWTARGTCP_ROLE;
  #define  TMWTARGTCP_ROLE_MASTER       WINTCP_ROLE_MASTER
  #define  TMWTARGTCP_ROLE_OUTSTATION   WINTCP_ROLE_OUTSTATION

typedef  WINTCP_MODE  TMWTARGTCP_MODE;
  #define TMWTARGTCP_MODE_SERVER        WINTCP_MODE_SERVER
  #define TMWTARGTCP_MODE_CLIENT        WINTCP_MODE_CLIENT
  #define TMWTARGTCP_MODE_UDP           WINTCP_MODE_UDP
  #define TMWTARGTCP_MODE_DUAL_ENDPOINT WINTCP_MODE_DUAL_ENDPOINT

/** Ensure compatibility with WINIO_TYPE_ENUM */
typedef WINIO_TYPE_ENUM TMWTARGIO_TYPE_ENUM;
  #define TMWTARGIO_TYPE_232                WINIO_TYPE_232 
  #define TMWTARGIO_TYPE_MODEM_POOL_CHANNEL WINIO_TYPE_MODEM_POOL_CHANNEL
  #define TMWTARGIO_TYPE_MODEM_POOL         WINIO_TYPE_MODEM_POOL
  #define TMWTARGIO_TYPE_MODEM              WINIO_TYPE_MODEM
  #define TMWTARGIO_TYPE_TCP                WINIO_TYPE_TCP
  #define TMWTARGIO_TYPE_UDP_TCP            WINIO_TYPE_UDP_TCP
  #define TMWTARGIO_TYPE_MBP                WINIO_TYPE_MBP
  #define TMWTARGIO_TYPE_MON                WINIO_TYPE_MON
  #define TMWTARGIO_TYPE_NONE               WINIO_TYPE_NONE

//#define WINIOTARG_SUPPORT_LEGACY_CONFIG 1
#if WINIOTARG_SUPPORT_LEGACY_CONFIG
/* These definitions have been depricated in favor of OS independent values. TMW recommends updating
* applications to use the new definitions. However the depricated definitions can optionally be
* enabled here by defining WINIOTARG_SUPPORT_LEGACY_CONFIG.
*/
#define WinIoTarg_initConfig tmwtargio_initConfig
#define winTCP targTCP
#define tmwtargp_Sleep tmwtarg_sleep
#endif

#endif // tmwtargos_DEFINED
