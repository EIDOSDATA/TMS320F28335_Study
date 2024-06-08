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

/* file: dnptprt.h
 * description: Implement DNP3 Transport Layer
 */
#include "tmwscl/dnp/dnpdiag.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwscl.h"
#include "tmwscl/utils/tmwlink.h"
#include "tmwscl/utils/tmwcnfg.h"

#include "tmwscl/dnp/dnptprt.h"
#include "tmwscl/dnp/dnpchnl.h"
#include "tmwscl/dnp/dnpmem.h"
#include "tmwscl/dnp/dnpstat.h"

/* Forward Declarations */

static void TMWDEFS_LOCAL _transmitNextFrame(
  DNPTPRT_CONTEXT *pTprtContext);

/* Local Functions */

/* function: _infoCallback
 * purpose: Called from link layer to inform transport layer of an
 *  event.
 * arguments:
 *  pParam - user specified callback paramater, points to transport context
 *  pSession - pointer to session this event pertains to
 *  sesnInfo - specifies the type of event
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _infoCallback(
  void *pParam,
  TMWSESN *pSession,
  TMWSCL_INFO sesnInfo)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParam;

  if((pTprtContext->tmw.pInfoFunc != TMWDEFS_NULL)
    &&(pSession != TMWDEFS_NULL))
  {
    pTprtContext->tmw.pInfoFunc(pParam, pSession, sesnInfo);
  }
}

/* function: _checkDataAvailable 
 * purpose: Called from link layer to see if more data is available
 * arguments:
 *  pParam - user specified callback paramater, points to transport context
 *  pSession - pointer to session from which to get data
 *  classMask - class of data to check for, not used for DNP
 *  buildResponse - should we build the response or just check to see if
 *   data is available. Currently always TRUE for DNP.
 * returns:
 *  TMWDEFS_TRUE if data was sent, else TMWDEFS_FALSE
 */
static TMWTYPES_BOOL TMWDEFS_CALLBACK _checkDataAvailable(
  void *pParam,
  TMWSESN *pSession,
  TMWDEFS_CLASS_MASK classMask,
  TMWTYPES_BOOL buildResponse)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParam;

  /* Avoid warnings */
  TMWTARG_UNUSED_PARAM(classMask);
  
  /* prevent recursion */
  if(pTprtContext->txInProgress)
    return TMWDEFS_FALSE;

  /* First check to see if there are more frames in the
   * current fragment
   */
  if((pTprtContext->pTxDescriptor != TMWDEFS_NULL)
    && ((pSession == TMWDEFS_NULL) || (pTprtContext->pTxDescriptor->pSession == pSession)))
  {
    pTprtContext->txInProgress = TMWDEFS_TRUE;
    if(buildResponse)
      _transmitNextFrame(pTprtContext);
    pTprtContext->txInProgress = TMWDEFS_FALSE;

    return(TMWDEFS_TRUE);
  }

  /* See if the application layer has anything to send */
  if(pTprtContext->tmw.pCheckClassFunc(
    pTprtContext->tmw.pParam, pSession, classMask, buildResponse))
  {
    return(TMWDEFS_TRUE);
  }

  return(TMWDEFS_FALSE);
}

/* function: _dataReady
 * purpose: called by application layer when data is available
 * arguments:
 *  pContext - transport layer context
 *  pSession - session on which data is available
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _dataReady(
  TMWTPRT_CONTEXT *pContext,
  TMWSESN *pSession)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pContext;

  /* See if the channel is busy */
  if(pTprtContext->pTxDescriptor == TMWDEFS_NULL)
  {
    /* Nope, go ahead and call the application layer callback to
     * get the data to transmit
     */
    pContext->pCheckClassFunc(pContext->pParam,
      pSession, TMWDEFS_CLASS_MASK_ALL, TMWDEFS_TRUE);
  }
}

/* function: _beforeTxCallback
 * purpose: Called by link layer right before a link layer frame
 *  is transmitted.
 * arguments:
 *  pParam - user specified callback paramater, points to transport context
 *  pTxData - pointer to link layer from transmit data
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _beforeTxCallback(
  void *pParam,
  TMWSESN_TX_DATA *pTxData)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParam;
  TMWTARG_UNUSED_PARAM(pTxData);

  /* See if this is the first frame of the current fragment */
  if(pTprtContext->txFirstFrame)
  {
    pTprtContext->txFirstFrame = TMWDEFS_FALSE;

    /* Call application layer callback */
    if((pTprtContext->pTxDescriptor != TMWDEFS_NULL)
      && (pTprtContext->pTxDescriptor->pBeforeTxCallback))
    {
      pTprtContext->pTxDescriptor->pBeforeTxCallback(
        pTprtContext->pTxDescriptor->pCallbackData, pTprtContext->pTxDescriptor);
    }
  }
}

/* function: _afterTxCallback
 * purpose: Called by link layer after a link layer frame has been 
 *  transmitted.
 * arguments:
 *  pParam - user specified callback paramater, points to transport context
 *  pTxData - pointer to link layer from transmit data
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _afterTxCallback(
  void *pParam,
  TMWSESN_TX_DATA *pTxData)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParam;
  TMWTARG_UNUSED_PARAM(pTxData);

  /* More bytes in current fragment ? */
  if(pTprtContext->txFragmentOffset < pTprtContext->pTxDescriptor->msgLength)
  {
    /* Tell link layer we have another frame to transmit */
    pTprtContext->tmw.pChannel->pLink->pLinkDataReady(
      pTprtContext->tmw.pChannel->pLinkContext, pTprtContext->pTxDescriptor->pSession);
  }
  else
  {
    /* Done with this fragment */
    TMWSESN_TX_DATA *pOldTxDescriptor = pTprtContext->pTxDescriptor;
    pTprtContext->pTxDescriptor = TMWDEFS_NULL;

    /* Update statistics */
    DNPSTAT_CHNL_FRAGMENT_SENT(pTprtContext->tmw.pChannel);

    /* Call application layer callback */
    if(pOldTxDescriptor->pAfterTxCallback != TMWDEFS_NULL)
    {
      pOldTxDescriptor->pAfterTxCallback(
        pOldTxDescriptor->pCallbackData, pOldTxDescriptor);
    }
  }
}

/* function: _failedTxCallback
 * purpose: Called by link layer after a failed transmission of a link 
 *  layer frame.
 * arguments:
 *  pParam - user specified callback paramater, points to transport context
 *  pTxData - pointer to link layer from transmit data
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _failedTxCallback(
  void *pParam,
  TMWSESN_TX_DATA *pTxData)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParam;
  TMWSESN_TX_DATA *pOldTxDescriptor = pTprtContext->pTxDescriptor;
  TMWTARG_UNUSED_PARAM(pTxData);

  /* Done with this transmission */
  pTprtContext->pTxDescriptor = TMWDEFS_NULL;

  /* Call application layer callback */
  if(pOldTxDescriptor->pFailedTxCallback)
  {
    pOldTxDescriptor->pFailedTxCallback(
      pOldTxDescriptor->pCallbackData, pOldTxDescriptor);
  }
}

/* function: _openSession
 * purpose: Open a new session on this channel
 * arguments:
 *  pContext - transport layer context
 *  pSession - pointer to session to open
 * returns:
 *  TMWDEFS_TRUE if successful, else TMWDEFS_FALSE
 */
static TMWTYPES_BOOL TMWDEFS_CALLBACK _openSession(
  TMWTPRT_CONTEXT *pContext,
  TMWSESN *pSession)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pContext;
  DNPTPRT_SESSION_INFO *pTprtSessionInfo;

  /* Setup callbacks first */
  pTprtContext->tmw.pChannel->pLink->pLinkSetCallbacks(
    pTprtContext->tmw.pChannel->pLinkContext,
    pTprtContext, _infoCallback, dnptprt_parseFrame, _checkDataAvailable,
    TMWDEFS_NULL, TMWDEFS_NULL, TMWDEFS_NULL);

  /* Initialize DNP Transport specific session info */
  pTprtSessionInfo = (DNPTPRT_SESSION_INFO *)dnpmem_alloc(DNPMEM_TPRT_SESSION_INFO_TYPE);
  if(pTprtSessionInfo == TMWDEFS_NULL)
  {
    return(TMWDEFS_FALSE);
  }

  pSession->pTprtSession = pTprtSessionInfo;
  pTprtSessionInfo->txSequenceNumber = 0;
  pTprtSessionInfo->rxSequenceNumber = 0;

  /* Try to open session */
  if(!tmwtprt_openSession(pContext, pSession))
  {
    dnpmem_free(pSession->pTprtSession);
    return(TMWDEFS_FALSE);
  }

  return(TMWDEFS_TRUE);
}

/* function: _closeSession
 * purpose: Close an existing session on this channel
 * arguments:
 *  pContext - transport layer context
 *  pSession - pointer to session to close
 * returns:
 *  TMWDEFS_TRUE if successful, else TMWDEFS_FALSE
 */
static TMWTYPES_BOOL TMWDEFS_CALLBACK _closeSession(
  TMWTPRT_CONTEXT *pContext,
  TMWSESN *pSession)
{
  dnpmem_free(pSession->pTprtSession);
  return(tmwtprt_closeSession(pContext, pSession));
}

/* function: _getSessions
 * purpose: return a list containing the current sessions on
 *  this channel.
 * arguments:
 *  pContext - transport layer context
 * returns:
 *  Pointer to TMWDLIST containing sessions
 */
static TMWDLIST * TMWDEFS_CALLBACK _getSessions(
  TMWTPRT_CONTEXT *pContext)
{
  return(tmwtprt_getSessions(pContext));
}

/* function: _setCallbacks
 * purpose: set application layer callbacks for this session
 * arguments:
 *  pContext - transport layer context
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _setCallbacks(
  TMWTPRT_CONTEXT *pContext,
  void *pParam,
  TMWTPRT_INFO_FUNC pInfoFunc,
  TMWTPRT_PARSE_FUNC pParseFunc,
  TMWTPRT_CHECK_CLASS_FUNC pCheckClassFunc)
{
  tmwtprt_setCallbacks(pContext, pParam, pInfoFunc, pParseFunc, pCheckClassFunc);
}

/* function: _transmitFragment
 * purpose:
 * arguments:
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _transmitFragment(
  TMWTPRT_CONTEXT *pContext,
  TMWSESN_TX_DATA *pTxFragment)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pContext;

  /* Initialize transmission */
  pTprtContext->txFragmentOffset = 0;
  pTprtContext->pTxDescriptor = pTxFragment;
  pTprtContext->txFirstFrame = TMWDEFS_TRUE;

  /* If user has registered a callback function call it */
  if(pTprtContext->pUserTxCallback != TMWDEFS_NULL)
  {
    pTprtContext->pUserTxCallback(pTprtContext->pCallbackParam, pTxFragment);
  }

  /* Tell link layer we have a frame to transmit */
  pTprtContext->tmw.pChannel->pLink->pLinkDataReady(
    pTprtContext->tmw.pChannel->pLinkContext, pTxFragment->pSession);
}

/* function: _cancelFragment
 * purpose:
 * arguments:
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _cancelFragment(
  TMWTPRT_CONTEXT *pContext,
  TMWSESN_TX_DATA *pTxFragment)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pContext;

  if(pTxFragment == pTprtContext->pTxDescriptor)
  {
    /* Initialize transmission */
    pTprtContext->txFragmentOffset = 0;
    pTprtContext->pTxDescriptor = TMWDEFS_NULL;
    pTprtContext->txFirstFrame = TMWDEFS_TRUE;

    /* Tell link layer any outstanding frame is canceled. */
    pTprtContext->tmw.pChannel->pLink->pLinkCancel(
      pTprtContext->tmw.pChannel->pLinkContext, &pTprtContext->linkTxDescriptor);
  }
}

/* function: dnptprt_parseFrame
 * purpose:
 * arguments:
 * returns:
 *  void
 */
void TMWDEFS_GLOBAL dnptprt_parseFrame(
  void *pParseParam,
  TMWSESN *pSession,
  TMWSESN_RX_DATA *pRxFrame)
{
  DNPCHNL *pDNPChannel;
  DNPTPRT_SESSION_INFO *pTprtSessionInfo = (DNPTPRT_SESSION_INFO *)pSession->pTprtSession;
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pParseParam;
  TMWTYPES_UCHAR transportHeader = pRxFrame->pMsgBuf[0];

  /* Diagnostics */
  DNPDIAG_TPRT_FRAME_RECEIVED(
    pTprtContext->tmw.pChannel, pSession,
    pRxFrame->pMsgBuf, pRxFrame->msgLength);

  /* See if this is the first segment (frame) of a new fragment */
  if(transportHeader & DNPDEFS_TH_FIRST)
  {
    /* If we were already in the middle of a fragment abort it
     * and start a new one.
     */
    if(pTprtContext->rxWaitingForFirst != TMWDEFS_TRUE)
    {
      /* Log error but go ahead and reset to read new fragment */
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_FRAG_RESTART);
    }

    pTprtContext->rxFragmentOffset = 0;
    pTprtContext->pRxSession = pSession;
    pTprtSessionInfo->rxSequenceNumber = (TMWTYPES_UCHAR)(transportHeader & DNPDEFS_TH_SEQUENCE_MASK);
    pTprtContext->rxWaitingForFirst = TMWDEFS_FALSE;
    pTprtContext->rxFragment.firstByteTime = pRxFrame->firstByteTime;
    pTprtContext->rxFragment.isBroadcast = pRxFrame->isBroadcast;
    pTprtContext->rxFragment.rxAddress = pRxFrame->rxAddress;
  }
  else
  {
    /* If we are not in the middle of receiving a fragment ignore
     * this frame since the FIR bit was not set.
     */
    if(pTprtContext->rxWaitingForFirst == TMWDEFS_TRUE)
    {
      pTprtContext->rxFragmentOffset = 0;
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_SEGDISCARD);
      return;
    }

    /* Make sure this frame is for the same session */
    if(pTprtContext->pRxSession != pSession)
    {
      /* Log error, reset receive state and return */
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_FRAME_SESSION);
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_DISCARD); 
      pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
      pTprtContext->rxFragmentOffset = 0;
      return;
    }

    /* Make sure sequence number of this transport segment matches */
    if(pTprtSessionInfo->rxSequenceNumber != (transportHeader & DNPDEFS_TH_SEQUENCE_MASK))
    {
      TMWTYPES_BOOL discardEntireFragment = TMWDEFS_TRUE;

      /* Log error, decide whether to discard segment or entire fragment, and return */
      DNPSTAT_CHNL_ERROR(pTprtContext->tmw.pChannel, pSession, TMWCHNL_ERROR_TPRT_SEQUENCE_ERROR);
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_SEQ_INVALID);
      /* 
       * If the FIN bit is set, the sequence MUST be the expected one or the entire fragment is discarded, 
       * according to Rule 6 in 2.1 of Transport Function Spec
       * If this segment does not have the FIN bit set 
       * and it has the sequence number of the previous segment, just discard this segment, not the whole fragment 
       */
      if(((transportHeader & DNPDEFS_TH_FINAL)==0)
        &&(transportHeader & DNPDEFS_TH_SEQUENCE_MASK) == ((pTprtSessionInfo->rxSequenceNumber -1) & DNPDEFS_TH_SEQUENCE_MASK))
      {
        DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_SEGDISCARD);
        discardEntireFragment = TMWDEFS_FALSE; 
      }

      if(discardEntireFragment)
      {
        DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_DISCARD); 
        pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
        pTprtContext->rxFragmentOffset = 0;
      }
      return;
    }
  } 
  pDNPChannel = (DNPCHNL *)pTprtContext->tmw.pChannel;
  if((pTprtContext->rxFragmentOffset + pRxFrame->msgLength - DNPDEFS_TH_SIZE) > pDNPChannel->rxFragmentSize)
  {
    DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_FRAG_SIZE);
    DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_TPRT_DISCARD); 
    pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
    pTprtContext->rxFragmentOffset = 0;
    return;
  }

  /* Increment expected sequence number */
  pTprtSessionInfo->rxSequenceNumber = (TMWTYPES_UCHAR)
    ((pTprtSessionInfo->rxSequenceNumber + 1) & DNPDEFS_TH_SEQUENCE_MASK);

  /* Copy received frame into receive fragment buffer and update length */
  memcpy(pTprtContext->rxFragment.pMsgBuf + pTprtContext->rxFragmentOffset,
    pRxFrame->pMsgBuf + DNPDEFS_TH_SIZE, pRxFrame->msgLength - DNPDEFS_TH_SIZE);

  pTprtContext->rxFragmentOffset = (TMWTYPES_USHORT)
    (pTprtContext->rxFragmentOffset + pRxFrame->msgLength - DNPDEFS_TH_SIZE);

  /* See if this is the last frame in this fragment */
  if(transportHeader & DNPDEFS_TH_FINAL)
  {
    if(pTprtContext->rxFragmentOffset  < 2)
    {
      DNPDIAG_ERROR(pTprtContext->tmw.pChannel, pSession, DNPDIAG_INVALID_SIZE);
      pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
      pTprtContext->rxFragmentOffset = 0;
      return;
    }

    /* Store last byte time for this fragment */
    pTprtContext->rxFragment.lastByteTime = pRxFrame->lastByteTime;

    /* Update statistics */
    DNPSTAT_CHNL_FRAGMENT_RCVD(pTprtContext->tmw.pChannel);


    /* Send fragment up to the application layer */
    if(pTprtContext->tmw.pParseFunc)
    {
      pTprtContext->rxFragment.msgLength = pTprtContext->rxFragmentOffset;
      
#if defined(TMW_PRIVATE)
      if(pTprtContext->tmw.pUserParseFunc)
        pTprtContext->tmw.pUserParseFunc(pTprtContext->tmw.pUserParseParam, &pTprtContext->rxFragment);
#endif

      pTprtContext->tmw.pParseFunc(pTprtContext->pRxSession, &pTprtContext->rxFragment);
    }

    /* Get ready for next fragment */
    pTprtContext->rxFragmentOffset = 0;
    pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
  }
}

/* function: _transmitNextFrame
 * purpose:
 * arguments:
 * returns:
 *  void
 */
static void TMWDEFS_LOCAL _transmitNextFrame(
  DNPTPRT_CONTEXT *pTprtContext)
{
  DNPTPRT_SESSION_INFO *pTprtSessionInfo;

  TMWTYPES_UCHAR transportHeader;
  TMWTYPES_USHORT bytesToTransmit;
  TMWTYPES_USHORT bytesLeft;
  TMWTYPES_USHORT offset;

  /* Initialize link layer transmit descriptor */
  pTprtContext->linkTxDescriptor.pChannel = pTprtContext->pTxDescriptor->pChannel;
  pTprtContext->linkTxDescriptor.pSession = pTprtContext->pTxDescriptor->pSession;
  pTprtContext->linkTxDescriptor.destAddress = pTprtContext->pTxDescriptor->destAddress;

  /* Figure out how many bytes we can transmit */
  offset = pTprtContext->txFragmentOffset;
  bytesLeft = (TMWTYPES_USHORT)(pTprtContext->pTxDescriptor->msgLength - offset);
  bytesToTransmit = (TMWTYPES_USHORT)
    (((pTprtContext->txFrameSize - DNPDEFS_TH_SIZE) < bytesLeft) 
    ? pTprtContext->txFrameSize  - DNPDEFS_TH_SIZE : bytesLeft);

  /* Get pointer to transport specific session info */
  pTprtSessionInfo = (DNPTPRT_SESSION_INFO *)pTprtContext->pTxDescriptor->pSession->pTprtSession;

  /* Set sequence number */
  transportHeader = pTprtSessionInfo->txSequenceNumber;
  pTprtSessionInfo->txSequenceNumber = (TMWTYPES_UCHAR)
    ((pTprtSessionInfo->txSequenceNumber + 1) & DNPDEFS_TH_SEQUENCE_MASK);

  /* Set FIR bit if first frame */
  if(offset == 0)
    transportHeader |= DNPDEFS_TH_FIRST;

  /* Set FIN bit if last frame */
  if(bytesToTransmit == bytesLeft)
    transportHeader |= DNPDEFS_TH_FINAL;

  /* Set frame header */
  pTprtContext->linkTxDescriptor.pMsgBuf[0] = transportHeader;

  /* Copy user data into frame and update length */
  memcpy(pTprtContext->linkTxDescriptor.pMsgBuf + DNPDEFS_TH_SIZE,
    pTprtContext->pTxDescriptor->pMsgBuf + offset, bytesToTransmit);

  pTprtContext->txFragmentOffset =
    (TMWTYPES_USHORT)(pTprtContext->txFragmentOffset + bytesToTransmit);

  /* Setup callbacks */
  pTprtContext->linkTxDescriptor.pCallbackData = pTprtContext;
  pTprtContext->linkTxDescriptor.pBeforeTxCallback = _beforeTxCallback;
  pTprtContext->linkTxDescriptor.pAfterTxCallback = _afterTxCallback;
  pTprtContext->linkTxDescriptor.pFailedTxCallback = _failedTxCallback;

  /* Setup message buffer */
  pTprtContext->linkTxDescriptor.msgLength = (TMWTYPES_USHORT)(bytesToTransmit + DNPDEFS_TH_SIZE);

  /* Setup transmit flags */
  pTprtContext->linkTxDescriptor.txFlags = pTprtContext->pTxDescriptor->txFlags;

  /* Diagnostics */
  DNPDIAG_TPRT_FRAME_SENT(pTprtContext->tmw.pChannel, pTprtContext->pTxDescriptor->pSession,
    pTprtContext->linkTxDescriptor.pMsgBuf, pTprtContext->linkTxDescriptor.msgLength);

  /* Transmit frame */
  pTprtContext->tmw.pChannel->pLink->pLinkTransmit(
    pTprtContext->tmw.pChannel->pLinkContext, &pTprtContext->linkTxDescriptor);
}

/* function: _updateMsg
 * purpose:
 * arguments:
 * returns:
 *  void
 */
static void TMWDEFS_CALLBACK _updateMsg(
  TMWTPRT_CONTEXT *pContext,
  TMWTYPES_USHORT offset,
  TMWTYPES_UCHAR *pData,
  TMWTYPES_USHORT length)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pContext;

  /* Pass on to the link layer, adding one for the transport header.
   * Note that this will only work for the first frame in a fragment,
   * but since this feature is currently only required for the first
   * frame of a fragment this is OK.
   */
  pTprtContext->tmw.pChannel->pLink->pLinkUpdateMsg(
    pTprtContext->tmw.pChannel->pLinkContext, (TMWTYPES_USHORT)(offset + 1), pData, length);
}

/* Global Functions */

/* function: dnptprt_initConfig */
void TMWDEFS_GLOBAL dnptprt_initConfig(
  DNPTPRT_CONFIG *pConfig)
{
  TMWTARG_UNUSED_PARAM(pConfig);
}

/* dnptprt_initChannel */
DNPTPRT_CONTEXT * TMWDEFS_GLOBAL dnptprt_initChannel(
  TMWCHNL *pChannel,
  const DNPTPRT_CONFIG *pConfig)
{
  DNPTPRT_CONTEXT *pTprtContext;

  static const TMWTPRT_INTERFACE _dnptprtInterface = {
    (TMWTPRT_OPEN_SESSION_FUNC)_openSession,
    (TMWTPRT_CLOSE_SESSION_FUNC)_closeSession,
    (TMWTPRT_GET_SESSIONS_FUNC)_getSessions,
    (TMWTPRT_DATA_READY_FUNC)_dataReady,
    (TMWTPRT_SET_CALLBACKS_FUNC)_setCallbacks,
    (TMWTPRT_TRANSMIT_FUNC)_transmitFragment,
    (TMWTPRT_UPDATE_MSG_FUNC)_updateMsg,
    (TMWTPRT_CANCEL_FUNC)_cancelFragment
  };

  TMWTARG_UNUSED_PARAM(pConfig);

  pTprtContext = (DNPTPRT_CONTEXT *)dnpmem_alloc(DNPMEM_TPRT_CONTEXT_TYPE);
  if(pTprtContext != TMWDEFS_NULL)
  {
    pTprtContext->tmw.pChannel = pChannel;
    pChannel->pTprt = &_dnptprtInterface;
    pChannel->pTprtContext = (TMWTPRT_CONTEXT *)pTprtContext;

    /* Configuration */
    pTprtContext->txFrameSize = dnputil_linkFrameSizeToTprt(((DNPLINK_CONTEXT *)pChannel->pLinkContext)->txFrameSize); 

    /* Parsing function */
    pTprtContext->tmw.pParseFunc = TMWDEFS_NULL;
#if defined(TMW_PRIVATE)
    pTprtContext->tmw.pUserParseFunc = TMWDEFS_NULL;
#endif

    /* Transmit info */
    pTprtContext->pUserTxCallback = TMWDEFS_NULL;
    pTprtContext->txFirstFrame = TMWDEFS_TRUE;
    pTprtContext->txInProgress = TMWDEFS_FALSE;
    pTprtContext->pTxDescriptor = TMWDEFS_NULL;

#if TMWCNFG_USE_DYNAMIC_MEMORY && !TMWCNFG_ALLOC_ONLY_AT_STARTUP 
    pTprtContext->linkTxDescriptor.pMsgBuf = (TMWTYPES_UCHAR *)tmwtarg_alloc(pTprtContext->txFrameSize);
    if(pTprtContext->linkTxDescriptor.pMsgBuf == TMWDEFS_NULL)
    {
      DNPDIAG_ERROR(pChannel, TMWDEFS_NULL, DNPDIAG_TX_BUFFER);
      dnpmem_free(pTprtContext);
      return(TMWDEFS_NULL);
    }
#else
    pTprtContext->linkTxDescriptor.pMsgBuf = pTprtContext->txFrameBuffer;
#endif

    /* Receive info */
    pTprtContext->rxWaitingForFirst = TMWDEFS_TRUE;
#if TMWCNFG_USE_DYNAMIC_MEMORY && !TMWCNFG_ALLOC_ONLY_AT_STARTUP 
    pTprtContext->rxFragment.pMsgBuf = (TMWTYPES_UCHAR *)tmwtarg_alloc(((DNPCHNL *)pChannel)->rxFragmentSize);
    if(pTprtContext->rxFragment.pMsgBuf == TMWDEFS_NULL)
    {
      DNPDIAG_ERROR(pChannel, TMWDEFS_NULL, DNPDIAG_RX_BUFFER);
      tmwtarg_free(pTprtContext->linkTxDescriptor.pMsgBuf);
      dnpmem_free(pTprtContext);
      return(TMWDEFS_NULL);
    }
#else
    pTprtContext->rxFragment.pMsgBuf = pTprtContext->rxFragmentBuffer;
#endif
  }

  return(pTprtContext);
}

/* function: dnptprt_modifyChannel */
TMWTYPES_BOOL TMWDEFS_GLOBAL dnptprt_modifyChannel(
  TMWCHNL *pChannel,
  const DNPTPRT_CONFIG *pConfig,
  TMWTYPES_ULONG configMask)
{
  TMWTARG_UNUSED_PARAM(pChannel);
  TMWTARG_UNUSED_PARAM(pConfig);
  TMWTARG_UNUSED_PARAM(configMask);
  return(TMWDEFS_TRUE);
}

/* function: dnptprt_deleteChannel */
TMWTYPES_BOOL TMWDEFS_GLOBAL dnptprt_deleteChannel(
  TMWCHNL *pChannel)
{
#if TMWCNFG_USE_DYNAMIC_MEMORY && !TMWCNFG_ALLOC_ONLY_AT_STARTUP 
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pChannel->pTprtContext;
  tmwtarg_free(pTprtContext->linkTxDescriptor.pMsgBuf);
  tmwtarg_free(pTprtContext->rxFragment.pMsgBuf);
#endif

  /* Free allocated memory */
  dnpmem_free(pChannel->pTprtContext);

  /* Make sure we don't do this again */
  pChannel->pTprt = TMWDEFS_NULL;
  pChannel->pTprtContext = TMWDEFS_NULL;

  return(TMWDEFS_TRUE);
}

/* function: dnptprt_registerCallback */
void TMWDEFS_GLOBAL dnptprt_registerCallback(
  TMWCHNL *pChannel,
  DNPTPRT_TX_CALLBACK_FUNC pUserTxCallback,
  void *pCallbackParam)
{
  DNPTPRT_CONTEXT *pTprtContext = (DNPTPRT_CONTEXT *)pChannel->pTprtContext;
  pTprtContext->pUserTxCallback = pUserTxCallback;
  pTprtContext->pCallbackParam = pCallbackParam;
}
