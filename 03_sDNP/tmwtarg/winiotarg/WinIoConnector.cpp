/*****************************************************************************/
/* Triangle MicroWorks, Inc.                         Copyright(c) 1997-2017 */
/*****************************************************************************/
/*                                                                           */
/* This file is the property of:                                             */
/*                                                                           */
/*                       Triangle MicroWorks, Inc.                           */
/*                      Raleigh, North Carolina USA                          */
/*                       www.TriangleMicroWorks.com                          */
/* (919) 870 - 6615                                                          */
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

/* file: WinIoConnector.cpp
 * description: This class implements a connector 
 *  that re-trys the openChannel function and sets a flag when it 
 *  successfully opens a channel (i.e. establishes a connection
 *  this class is created for TCP clients, MBP clients, and all serial ports
 */

#include "StdAfx.h"

#include "WinIoTarg/include/WinIoTarg.h"
#include "WinIoInterface.h"
#include "WinIoConnector.h"


/**********************************************************************************\
	Function :			WinIoConnector::WinIoConnector
	Description : [none]	
	Return :			constructor	-	
	Parameters :
	Note : [none]
\**********************************************************************************/
WinIoConnector::WinIoConnector()
{
  DeleteThis = false;
  m_stopEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
  m_threadIsRunning = false;
  m_connectorThreadHandle = INVALID_HANDLE_VALUE;
  m_iConnectDelay = 0;
}

/**********************************************************************************\
	Function :			WinIoConnector::~WinIoConnector
	Description : [none]	
	Return :			destructor	-	
	Parameters :
	Note : [none]
\**********************************************************************************/
WinIoConnector::~WinIoConnector()
{
  StopConnectorThread();
  CloseHandle(m_stopEvent);
  if (m_connectorThreadHandle != INVALID_HANDLE_VALUE)
  {
    CloseHandle(m_connectorThreadHandle);
  }
}

 
/**********************************************************************************\
	Function :			WinIoConnector::connectorThread
	Description : Entry point for connector thread
	Return :			unsigned int	-	
	Parameters :
			void *pParam	-	
	Note : [none]
\**********************************************************************************/
unsigned int __stdcall WinIoConnector::connectorThread(void *pParam)
{
  WinIoConnector *pWinIoConnector = (WinIoConnector *)pParam;
  WinIoInterface *pWinIoInterface = (WinIoInterface *)pWinIoConnector->m_pWinIoInterface;

  if (pWinIoConnector->m_iConnectDelay > 0)
  {
    Sleep(pWinIoConnector->m_iConnectDelay);
  }

#pragma warning(disable:4127)
  while(1)
#pragma warning(default:4127)
  {

    if(pWinIoInterface->openWinIoChannel(pWinIoInterface->getRecvCallbackFunc(), 
      pWinIoInterface->getCheckAddrCallbackFunc(), pWinIoInterface->getChanContextCBData(), WINIO_OPEN_MODE_NONE))
    { 
      // Connected, break out of loop.
      break;
    }
    ULONG waitStatus = WaitForSingleObject(pWinIoConnector->getStopEvent(),100);
    if (waitStatus == WAIT_OBJECT_0)
    {
      break; // stop event was set, 
    }
  }

  pWinIoConnector->m_threadIsRunning = false;

  if(pWinIoConnector->DeleteThis)
  {
    WINIO_CONTEXT *temp = pWinIoInterface->pWinIoContext;
    delete(pWinIoInterface);
    delete(temp);
  }
  return 0;
}


/**********************************************************************************\
	Function :			WinIoConnector::StartConnectorThread
	Description : [none]	
	Return :			bool	-	
	Parameters :
			WinIoInterface *pWinIoInterface	-	
	Note : [none]
\**********************************************************************************/
bool WinIoConnector::StartConnectorThread(WinIoInterface *pWinIoInterface, TMWTYPES_ULONG delay)
{
  // Start thread retrying connections
  m_iConnectDelay = delay;

  m_pWinIoInterface = pWinIoInterface;
  if (m_threadIsRunning == false)
  {
    if (m_connectorThreadHandle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(m_connectorThreadHandle);
    }
    m_threadIsRunning = true;
    m_connectorThreadHandle = (HANDLE)WinIoTarg_startThread(connectorThread,this,&m_threadID,THREAD_PRIORITY_ABOVE_NORMAL);
    if(m_connectorThreadHandle == INVALID_HANDLE_VALUE)
    {
      m_threadIsRunning = false;
      return false;
    }
  
    // give up rest of time slice to allow connector thread to run. 
    Sleep(0);
  }
  return true;
}

/**********************************************************************************\
	Function :			WinIoConnector::StopConnectorThread
	Description : [none]	
	Return :			void	-	
	Parameters :
	Note : [none]
\**********************************************************************************/
void WinIoConnector::StopConnectorThread()
{
  if (m_threadIsRunning == true)
  { 
    SetEvent(m_stopEvent);

    // give up rest of time slice to allow connector thread to run. 
    Sleep(0);

    if (m_connectorThreadHandle != INVALID_HANDLE_VALUE)
    {
      CloseHandle(m_connectorThreadHandle);
      m_connectorThreadHandle = INVALID_HANDLE_VALUE;
    }
  }
}
