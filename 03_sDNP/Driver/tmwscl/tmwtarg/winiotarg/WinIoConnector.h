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

// WinIoConnector.h: interface for the WinIoConnector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINIOCONNECTOR_H__78CD06E9_1914_4C88_B16F_AAA65FECF8FF__INCLUDED_)
#define AFX_WINIOCONNECTOR_H__78CD06E9_1914_4C88_B16F_AAA65FECF8FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "tmwscl/utils/tmwdefs.h"
#include "tmwscl/utils/tmwtypes.h"
#include "WinIoTarg/WinThreading.h"

class WinIoInterface;

class WinIoConnector  
{
public:
	WinIoConnector();
	virtual ~WinIoConnector();
  
  HANDLE getStopEvent()
  {
    return m_stopEvent;
  }

  bool isRunning()
  {
    return m_threadIsRunning;
  }

  void StopConnectorThread();
	bool StartConnectorThread(WinIoInterface *pWinIoInterface, TMWTYPES_ULONG delay);

  bool DeleteThis;

private:
  HANDLE m_stopEvent;   // handle to stop event
  WinIoInterface *m_pWinIoInterface;
  // Is thread currently running
  bool m_threadIsRunning;
  TMWTYPES_ULONG m_iConnectDelay;
  
  unsigned int m_threadID;
  HANDLE m_connectorThreadHandle;

  // Connector thread entry point
  static unsigned int __stdcall connectorThread(void *pParam);

};

#endif // !defined(AFX_WINIOCONNECTOR_H__78CD06E9_1914_4C88_B16F_AAA65FECF8FF__INCLUDED_)
