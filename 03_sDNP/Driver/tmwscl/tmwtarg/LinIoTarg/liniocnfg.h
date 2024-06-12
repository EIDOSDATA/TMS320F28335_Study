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

/* file: linioCnfg.h
 * description: Configuration parameters for Linux Target layer
 */

#ifndef linioCnfg_DEFINED
#define linioCnfg_DEFINED

#define LINIOCNFG_MAX_NAME_LEN            256

/* Currently only support one serial port */
#define LINIOCNFG_NUM_SERIAL_PORTS        1

#define LINIOCNFG_SERIAL_INBUF_SIZE       512
#define LINIOCNFG_SERIAL_OUTBUF_SIZE      512
#define LINIOCNFG_UDP_BUFFER_SIZE         8000
#define LINIOCNFG_MAX_UDP_FRAME           2500

/* set this to TMWDEFS_FALSE to remove TCP support */
#define LINIOTARG_SUPPORT_TCP TMWDEFS_TRUE

/* set this to TMWDEFS_FALSE to remove serial support */
#define LINIOTARG_SUPPORT_232 TMWDEFS_TRUE

#endif /* linioCnfg_DEFINED */
