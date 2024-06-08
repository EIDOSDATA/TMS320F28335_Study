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

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

 /* Type for the handle that the OS passes back for the created thread */
#define TMW_ThreadId            pthread_t

 /* Declaration for the entry point of a thread */
#define TMW_ThreadDecl          void *

/* The type definition of the entry point of a thread */
/* In most cases it will be the same as TMW_ThreadDecl */
#define TMW_ThreadPtr           void *

/* The type defintion the OS expects to pass as an arguement when starting a thread */
#define TMW_ThreadArg           void *

/* A Macro is used here instead of a function call because of Windows __stdcall calling convention */
#define TMW_ThreadCreate(pId, pFunc, pFuncArg, priority, stackSize) \
  (pthread_create(pId, NULL, pFunc, (TMW_ThreadArg)pFuncArg))

//#define LINIOTARG_SUPPORT_LEGACY_CONFIG 1
#if LINIOTARG_SUPPORT_LEGACY_CONFIG
/* These definitions have been depricated in favor of OS independent values. TMW recommends updating
* applications to use the new definitions. However the depricated definitions can optionally be
* enabled here by defining LINIOTARG_SUPPORT_LEGACY_CONFIG.
*/
#define liniotarg_initConfig tmwtargio_initConfig
#define linTCP targTCP
#define Sleep(ms100) tmwtarg_sleep(ms100/100)
#define TMW_CreateThread(pId, pFunc, pFuncParams, stackSize) \
                        (pthread_create(pId, NULL, pFunc, (TMW_ThreadArg)pFuncParams))
  

/* Define data types used to interface to the TCP target library. */
#define LINTCP_MODE_SERVER        TMWTARGTCP_MODE_SERVER
#define LINTCP_MODE_CLIENT        TMWTARGTCP_MODE_CLIENT
#define LINTCP_MODE_DUAL_ENDPOINT TMWTARGTCP_MODE_DUAL_ENDPOINT
#define LINTCP_MODE_UDP           TMWTARGTCP_MODE_UDP

#define LINTCP_ROLE_MASTER        TMWTARGTCP_ROLE_MASTER
#define LINTCP_ROLE_OUTSTATION    TMWTARGTCP_ROLE_OUTSTATION

#define LINIO_TYPE_232            TMWTARGIO_TYPE_232
#define LINIO_TYPE_TCP            TMWTARGIO_TYPE_TCP
#define LINIO_TYPE_NONE           TMWTARGIO_TYPE_NONE

#endif
#endif // tmwtargos_DEFINED
