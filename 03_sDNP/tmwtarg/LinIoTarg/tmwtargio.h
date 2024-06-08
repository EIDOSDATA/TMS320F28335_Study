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

/* file: twmtargosdefs.h
 * description: Definitions of target IO configuration for Linux
 */
#ifndef tmwtargio_DEFINED
#define tmwtargio_DEFINED

#include "liniocnfg.h"
#include "liniodefs.h"
#include "liniotarg.h"
#include "tmwtargio.h"

typedef LINIO_CONFIG TMWTARGIO_CONFIG;

/* function: tmwtargio_initConfig
* purpose: Initialize the TMW target IO configuration structure.
*  This routine should be called to configure all the members of the
*  TMWTARGIO_CONFIG data structure. The caller should then set any
*  values that need to be modified from the default values.
* arguments :
*  pConfig - pointer to the TMW target IO configuration structure
* returns :
*  void
*/
TMWDEFS_SCL_API void TMWDEFS_GLOBAL tmwtargio_initConfig(
  TMWTARGIO_CONFIG *pConfig);

#endif // tmwtargio_DEFINED
