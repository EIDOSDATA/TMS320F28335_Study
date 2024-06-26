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

/* file: sdnpo030.c
 * description: DNP Slave functionality for Object 30 Analog Inputs
 */
#include "tmwscl/dnp/dnpdiag.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/dnp/dnpdefs.h"
#include "tmwscl/dnp/sdnpo030.h"
#include "tmwscl/dnp/sdnpdata.h"
#include "tmwscl/dnp/sdnputil.h"
#include "tmwscl/dnp/dnputil.h"

#if SDNPDATA_SUPPORT_OBJ30

/* forward declarations */
static void TMWDEFS_GLOBAL _switchVariationInfo(
  SDNPUTIL_STATIC_DESC *pDesc, 
  TMWTYPES_UCHAR variation);

/* function: _isSupportedVariation() */
static TMWTYPES_BOOL TMWDEFS_LOCAL _isSupportedVariation(
  TMWTYPES_UCHAR variation)
{
switch(variation)
  {
#if SDNPDATA_SUPPORT_OBJ30_V1
  case 1:
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V2
  case 2:
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V3
  case 3:
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V4
  case 4:
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V5
  case 5:
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V6
  case 6:
    break;
#endif
  default:
    return(TMWDEFS_FALSE);
  }

  return(TMWDEFS_TRUE);
}

/* function: flagNominalCheck 
 * purpose: Check to see if variation we are switching to requires
 *  flags to be nominal and if flags are nominal. If necessary change
 *  variation to one that supports flags.
 * arguments:
 *  pPoint - point to be read
 *  variation - variation we are trying to switch to
 * returns:
 *  void
 */
static void TMWDEFS_LOCAL flagNominalCheck(
  void *pPoint,
  TMWTYPES_UCHAR *pVariation)     
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_UCHAR flags;

  if((*pVariation == 3) || (*pVariation == 4))
  { 
    /* Read current value */
    sdnpdata_anlgInRead(pPoint, &value, &flags);

    /* See if value is going to cause overflow bit to be set */
    if(*pVariation == 4)
      dnputil_getAnalogValueShort(&value, &flags);
    else
      dnputil_getAnalogValueLong(&value, &flags); 

    /* Check to see whether flags are in nominal state */
    if((flags & 0x7f) != DNPDEFS_DBAS_FLAG_ON_LINE)
    {
      /* change variation from 4->2 3->1 */
      *pVariation = *pVariation -2;
    }
  }
}

/* function: _switchPointDefVariation */
static TMWTYPES_BOOL TMWDEFS_LOCAL _switchPointDefVariation(
  SDNPUTIL_STATIC_DESC *pDesc,
  void *pPoint,
  TMWTYPES_UCHAR currentVariation)                     
{
  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if(pDesc->readVariation == 0)
  {
    TMWTYPES_UCHAR variation;

    if(pDesc->alreadySwitchedVariation)
      return(TMWDEFS_FALSE);

    variation = sdnpdata_anlgInDefVariation(pPoint); 
    if((variation != currentVariation) 
      && (_isSupportedVariation(variation)))
    {
      /* Determine which variation should be used depending on flags */
      flagNominalCheck(pPoint, &variation);
      _switchVariationInfo(pDesc, variation);

      /* if variation is still different than the current variation return true */
      if(variation != currentVariation)
        return(TMWDEFS_TRUE);
    }
  }
  return(TMWDEFS_FALSE);
}

#if SDNPDATA_SUPPORT_OBJ30_V1
/* function: _readv1 */
static SDNPSESN_READ_STATUS TMWDEFS_CALLBACK _readv1(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_ULONG ulvalue;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);

  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 1)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);

  /* Get value, setting over range bit in flags properly */
  ulvalue = dnputil_getAnalogValueLong(&value, &flags); 
  
  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);
  
  /* Store flags */
  pResponse->pMsgBuf[pResponse->msgLength++] = flags;

  /* Store value */
  tmwtarg_store32(&ulvalue, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 4;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif

#if SDNPDATA_SUPPORT_OBJ30_V2
/* function: _readv2 */
static SDNPSESN_READ_STATUS TMWDEFS_CALLBACK _readv2(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_USHORT usvalue;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);
  
  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 2)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);

  /* Get value, setting over range bit in flags properly */
  usvalue = dnputil_getAnalogValueShort(&value, &flags);

  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);

  /* Store flags */
  pResponse->pMsgBuf[pResponse->msgLength++] = flags;

  /* Store value */
  tmwtarg_store16(&usvalue, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 2;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif

#if SDNPDATA_SUPPORT_OBJ30_V3
/* function: _readv3 */
static SDNPSESN_READ_STATUS TMWDEFS_CALLBACK _readv3(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_ULONG ulvalue;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);

  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 3)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);

  /* Get value, setting over range bit in flags properly */
  ulvalue = dnputil_getAnalogValueLong(&value, &flags);

#if SDNPDATA_SUPPORT_OBJ30_V1
  /* Check to see whether flags are in nominal state. If not variation 2 must
   * be used.
   */
  if((flags & 0x7f) != DNPDEFS_DBAS_FLAG_ON_LINE)
  {
    /* Change the variation to the one with flags */
    _switchVariationInfo(pDesc, 1);

    return(SDNPSESN_READ_NEW_VARIATION);
  }
#else
  TMWTARG_UNUSED_PARAM(pDesc);
#endif

  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);

  /* Store value */
  tmwtarg_store32(&ulvalue, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 4;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif

#if SDNPDATA_SUPPORT_OBJ30_V4
/* function: _readv4 */
static SDNPSESN_READ_STATUS TMWDEFS_CALLBACK _readv4(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_USHORT usvalue;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);

  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 4)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);
  
  /* Get value, setting over range bit in flags properly */
  usvalue = dnputil_getAnalogValueShort(&value, &flags);
 
 #if SDNPDATA_SUPPORT_OBJ30_V2
  /* Check to see whether flags are in nominal state. If not variation 2 must
   * be used.
   */
  if((flags & 0x7f) != DNPDEFS_DBAS_FLAG_ON_LINE)
  {
    /* Change the variation to the one with flags */
    _switchVariationInfo(pDesc, 2);

    return(SDNPSESN_READ_NEW_VARIATION);
  }
#else
  TMWTARG_UNUSED_PARAM(pDesc);
#endif

  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);

  /* Store value */
  tmwtarg_store16(&usvalue, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 2;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif /* SDNPDATA_SUPPORT_OBJ30_V4 */

#if SDNPDATA_SUPPORT_OBJ30_V5
/* function: _readv5 */
static SDNPSESN_READ_STATUS TMWDEFS_LOCAL _readv5(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_SFLOAT fvalue;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);  
  
  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 5)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);
 
  /* Get value, setting over range bit in flags properly */
  fvalue = dnputil_getAnalogValueFloat(&value, &flags);

  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);

  /* Store flags */
  pResponse->pMsgBuf[pResponse->msgLength++] = flags;
 
  /* Store value */
  tmwtarg_storeSFloat(&fvalue, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 4;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif /* SDNPDATA_SUPPORT_OBJ30_V5 */

#if SDNPDATA_SUPPORT_OBJ30_V6
/* function: _readv6 */
static SDNPSESN_READ_STATUS TMWDEFS_LOCAL _readv6(
  TMWSESN *pSession,
  TMWSESN_TX_DATA *pResponse,
  TMWTYPES_USHORT messageIndex,
  TMWTYPES_USHORT pointNum,
  void *pPoint,
  SDNPUTIL_STATIC_DESC *pDesc)
{
  TMWTYPES_ANALOG_VALUE value;
  TMWTYPES_DOUBLE dval;
  TMWTYPES_UCHAR flags;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;

  TMWTARG_UNUSED_PARAM(messageIndex);
  
  /* If session default variation is zero, check the default variation
   * for this specific point 
   */
  if((pSDNPSession->obj30DefaultVariation == 0) 
     &&(_switchPointDefVariation(pDesc, pPoint, 6)))
  {
    return(SDNPSESN_READ_NEW_VARIATION);  
  }

  /* Read current value */
  sdnpdata_anlgInRead(pPoint, &value, &flags);

  /* Diagnostics */
  DNPDIAG_SHOW_ANALOG_INPUT(pSession, pointNum, &value, flags, TMWDEFS_FALSE, TMWDEFS_NULL);

  /* Store flags */
  pResponse->pMsgBuf[pResponse->msgLength++] = flags;

  /* Store value */
  dval = dnputil_getAnalogValueDouble(&value);
  tmwtarg_store64(&dval, &pResponse->pMsgBuf[pResponse->msgLength]);
  pResponse->msgLength += 8;

  pDesc->alreadySwitchedVariation = TMWDEFS_FALSE;
  return(SDNPSESN_READ_COMPLETE);
}
#endif /* SDNPDATA_SUPPORT_OBJ30_V6 */

/* function: _setVariationInfo() */
static void TMWDEFS_LOCAL _setVariationInfo(
  SDNPUTIL_STATIC_DESC *pDesc, 
  TMWTYPES_UCHAR variation)
{
  switch(variation)
  {
  default:
    /* Fall through to first one that is supported */ 
  /* 3 is the preferred default */
#if SDNPDATA_SUPPORT_OBJ30_V3
  case 3:
    pDesc->variation = 3;
    pDesc->pReadIntoRespFunc = _readv3;
    pDesc->sizeInBytes = 4;
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V1
  case 1:
    pDesc->variation = 1;
    pDesc->pReadIntoRespFunc = _readv1;
    pDesc->sizeInBytes = 5;
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V2
  case 2:
    pDesc->variation = 2;
    pDesc->pReadIntoRespFunc = _readv2;
    pDesc->sizeInBytes = 3;
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V4
  case 4:
    pDesc->variation = 4;
    pDesc->pReadIntoRespFunc = _readv4;
    pDesc->sizeInBytes = 2;
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V5
  case 5:
    pDesc->variation = 5;
    pDesc->pReadIntoRespFunc = _readv5;
    pDesc->sizeInBytes = 5;
    break;
#endif
#if SDNPDATA_SUPPORT_OBJ30_V6
  case 6:
    pDesc->variation = 6;
    pDesc->pReadIntoRespFunc = _readv6;
    pDesc->sizeInBytes = 9;
    break;
#endif
  }
}

/* function: _switchVariationInfo() */
static void TMWDEFS_LOCAL _switchVariationInfo(
  SDNPUTIL_STATIC_DESC *pDesc, 
  TMWTYPES_UCHAR variation)
{
  _setVariationInfo(pDesc, variation);
  pDesc->alreadySwitchedVariation = TMWDEFS_TRUE;
}

/* functions: sdnpo030_readObj30 */
SDNPSESN_READ_STATUS TMWDEFS_CALLBACK sdnpo030_readObj30(
  TMWSESN *pSession,
  DNPUTIL_RX_MSG *pRequest,
  TMWSESN_TX_DATA *pResponse, 
  DNPUTIL_OBJECT_HEADER *pObjHeader,
  SDNPSESN_QUAL qualifier)
{
  SDNPUTIL_STATIC_DESC desc;
  SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;
  TMWTYPES_UCHAR variation = pObjHeader->variation;

  if(variation == 0)
  { 
    variation = pSDNPSession->obj30DefaultVariation;
  }
  else
  {
    if(!_isSupportedVariation(variation))
    {
      pSDNPSession->iin |= DNPDEFS_IIN_OBJECT_UNKNOWN; 
      return (SDNPSESN_READ_FAILED);
    }
  }

  desc.group = DNPDEFS_OBJ_30_ANA_INPUTS;
  desc.readVariation = variation; 
  desc.readBits = TMWDEFS_FALSE;
  desc.pQuantityFunc = sdnpdata_anlgInQuantity;
  desc.pGetPointFunc = sdnpdata_anlgInGetPoint;
  desc.alreadySwitchedVariation = TMWDEFS_FALSE;
#if SDNPDATA_SUPPORT_CLASS0_POINT
  desc.pIsInClass0Func = sdnpdata_anlgInIsClass0;
#endif

  _setVariationInfo(&desc, variation);

  return(sdnputil_readStatic(pSession,
    pRequest, pResponse, pObjHeader, qualifier, &desc));
}

#if SDNPDATA_SUPPORT_OBJ32
#if SDNPDATA_SUPPORT_ASSIGN
TMWTYPES_BOOL TMWDEFS_CALLBACK sdnpo030_assignClass(
  TMWSESN *pSession, 
  DNPUTIL_RX_MSG *pRxFragment,
  DNPUTIL_OBJECT_HEADER *pObjHeader, 
  TMWDEFS_CLASS_MASK classMask)
{
  SDNPSESN *pSDNPSession;
  TMWTYPES_BOOL status;
  TMWTYPES_USHORT i;

  pSDNPSession = (SDNPSESN *)pSession;

  /* limit requested number of points to quantity in database 
   * if status == TMWDEFS_FALSE IN2.2 will be set in response.
   */
  status = sdnputil_validateAssignQualifier(pObjHeader, sdnpdata_anlgInQuantity(pSDNPSession->pDbHandle));

  for(i = 0; i < pObjHeader->numberOfPoints; i++)
  {
    void *pPoint;
    TMWTYPES_USHORT pointNumber;
    dnputil_getPointNumber(pRxFragment, pObjHeader, i, &pointNumber);

    pPoint = sdnpdata_anlgInGetPoint(pSDNPSession->pDbHandle, pointNumber);
    if(pPoint == TMWDEFS_NULL)
    {
      /* Failure of any point for startstop or indexed qualifiers should set IIN2.2 in response */
      if((pObjHeader->qualifier < DNPDEFS_QUAL_ALL_POINTS)
        ||(pObjHeader->qualifier > DNPDEFS_QUAL_16BIT_LIMITED_QTY))
      {
        status = TMWDEFS_FALSE;
      }
      continue;
    }

    if(!sdnpdata_anlgInAssignClass(pPoint, classMask))  
      status = TMWDEFS_FALSE;
  }

  return(status);
}
#endif
#endif /* SDNPDATA_SUPPORT_OBJ32 */
#endif /* SDNPDATA_SUPPORT_OBJ30 */
