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

/* file: WinIoInterface.cpp
* description: Interface to the target I/O routines. This abstract base class provides
* a generic interface to the various low level target interfaces 
* (see the pure virtual functions below). Currently this includes the Modbus Plus Interface, 
* RS232 interface and the TCP/IP interface.
*
*/

#include "StdAfx.h"
#include "WinIoTarg/WinIoInterface.h"
#include "WinIoTarg/include/WinIoTarg.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

/*****************************************************************************/
extern "C" void WinIoAssertion(const char   *expr, const char   *fileName, int lineNum)
{
  TCHAR   buf[1024]; 
  _stprintf_s
    (
    buf, 1024,
    __T("%hs\nFile: %hs, Line: %d"),
    expr,
    fileName,
    (int) lineNum
    );
  
  ::MessageBox
    (
    NULL,
    buf,
   _T("WinIO Assertion Failed. Breaking!"),
    MB_OK | MB_ICONSTOP
    );

  ::DebugBreak();

  return;
}


void WinIoInterface::ProtoAnaLog(TMWDIAG_ID sourceID, const char   *format, ...)
{
  if (WinIoTarg_IsProtoAnaLogEnabled() == false)
  {
    return;
  }
  va_list va;
  va_start(va, format);
  if (WinIoTargProtoAnaLogFun != WINIOTARG_NULL)
  {
    WinIoTargProtoAnaLogFun(TMWDEFS_NULL, TRACE_MASK_NONE, sourceID, "", format, va);
  }
  va_end(va);
}

/**********************************************************************************\
Function :			WinIoInterface::WinIoInterface
Description : [none]	
Return :			constructor	-	
Parameters :
Note : [none]
\**********************************************************************************/
WinIoInterface::WinIoInterface()
{
  m_pWinIoConnector = new WinIoConnector;

  pWinIoContext = WINIOTARG_NULL;

  m_pStatusCallback = WINIOTARG_NULL;
  m_pStatusCallbackParam = WINIOTARG_NULL;

  m_pRecvDataFunc = WINIOTARG_NULL;
  m_pCheckAddrFunc = WINIOTARG_NULL;
  m_pChanContextCBData = WINIOTARG_NULL;
  m_tryToConnect = true;
  m_forceDisconnected  = false;
  m_bUseConnectorThread = false;
  setChanEnabled(true); // initialy on-line redial limit reached will take it off line
}

/**********************************************************************************\
Function :			WinIoInterface::~WinIoInterface
Description : [none]	
Return :			destructor	-	
Parameters :
Note : [none]
\**********************************************************************************/
WinIoInterface::~WinIoInterface()
{
  delete m_pWinIoConnector;
}

