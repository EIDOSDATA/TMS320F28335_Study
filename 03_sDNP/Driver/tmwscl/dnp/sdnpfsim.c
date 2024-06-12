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

/* file: sdnpsim.c
 * description: Example file I/O implementation.
 *
 * Simple file transfer target implementation for Windows. This
 * implementation only supports accessing one file at a time.
 * Trying to open a second file will result in a too many files
 * error.
 */

#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/dnp/dnpdiag.h"

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#include <io.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

/* to get rid of a deprecation warning in VS 2005 .NET */ 
#if defined(_MSC_VER)
#define _STRNICMP _strnicmp
#else
#define _STRNICMP strnicmp
#endif

#if TMWCNFG_USE_SIMULATED_DB

#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/dnp/dnpcnfg.h"
#include "tmwscl/dnp/sdnpcnfg.h"
#include "tmwscl/dnp/sdnpdata.h"
#include "tmwscl/dnp/sdnpsim.h"
#include "tmwscl/dnp/sdnpxml2.h"
#if SDNPDATA_SUPPORT_OBJ70
#include "tmwscl/dnp/sdnpfsim.h"
#include "tmwscl/dnp/sdnpxml.h"

#define DEVICE_PROFILE_XML2_FILE "dnpDPCfg.xml"
#define DEVICE_DESCRIPTION_FILE  "DNPDeviceConfiguration.xml"
#define VIRTUAL_FILE_DRIVE       "V:"

/* function: _buildInfoResponse */
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
static TMWTYPES_BOOL _buildInfoResponse(
  TMWSESN                  *pSession,
  struct _finddata_t       *pFileData,
  const char               *pFilename,
  DNPDEFS_FILE_TYPE        *pType,
  TMWTYPES_ULONG           *pSize,
  TMWDTIME                 *pTimeOfCreation,
  DNPDEFS_FILE_PERMISSIONS *pPermissions)
{
  if(pFileData == TMWDEFS_NULL)
  { 
    tmwdtime_getDateTime(pSession, pTimeOfCreation);
    *pType = DNPDEFS_FILE_TYPE_SIMPLE;

#if SDNPDATA_SUPPORT_XML2 
    if(strstr(pFilename, DEVICE_PROFILE_XML2_FILE) != 0)
    {
      SDNPSESN *pSDNPSession = (SDNPSESN *)pSession;
      pSDNPSession->xml2IndentSize = 0;
      *pSize = sdnpxml2_saveDatabaseGetSize(pSession);
      if(*pSize == 0)
      {
        /* Return error */
        return TMWDEFS_FALSE;
      }
    }
#endif
#if SDNPDATA_SUPPORT_XML
    if(strstr(pFilename, DEVICE_DESCRIPTION_FILE) != 0)
    {
      *pSize = sdnpxml_saveDatabaseStaticGetSize(pSession);
      if(*pSize == 0)
      {
        /* Return error */
        return TMWDEFS_FALSE;
      }
    }
#endif
    *pPermissions = 0x1ff;
  }
  else
  {
#if _MSC_VER >= 1400
    struct tm fileTime;

    /* convert file time of creation to tm structure */
    localtime_s(&fileTime, &pFileData->time_create);

    /* convert tm structure to TMWDTIME structure */
    pTimeOfCreation->mSecsAndSecs = (TMWTYPES_USHORT)(fileTime.tm_sec * 1000);
    pTimeOfCreation->minutes      = (TMWTYPES_UCHAR)fileTime.tm_min;
    pTimeOfCreation->hour         = (TMWTYPES_UCHAR)fileTime.tm_hour;
    pTimeOfCreation->dayOfMonth   = (TMWTYPES_UCHAR)fileTime.tm_mday;
    pTimeOfCreation->month        = (TMWTYPES_UCHAR)(fileTime.tm_mon+1);
    pTimeOfCreation->year         = (TMWTYPES_USHORT)(fileTime.tm_year+1900);
    pTimeOfCreation->dstInEffect  = (TMWTYPES_BOOL)((fileTime.tm_isdst == 0) ? TMWDEFS_FALSE : TMWDEFS_TRUE);

    /* Monday through Friday is equivalent but Sunday = 0 */
    /* in tm structure but = 7 in TMWDTIME structure      */
    if(fileTime.tm_wday == 0)
      pTimeOfCreation->dayOfWeek = 7;
    else
      pTimeOfCreation->dayOfWeek = (TMWTYPES_UCHAR)fileTime.tm_wday;

    /* See what type of file this is */
    if((pFileData->attrib & _A_SUBDIR) != _A_SUBDIR)
    {
      /* Simple file */
      *pType = DNPDEFS_FILE_TYPE_SIMPLE;
      *pSize = pFileData->size;

      if((pFileData->attrib & _A_RDONLY) == _A_RDONLY)
        *pPermissions = DNPDEFS_WORLD_READ_ALLOWED|DNPDEFS_GROUP_READ_ALLOWED|DNPDEFS_OWNER_READ_ALLOWED;
      else
        *pPermissions = 0x1ff;
    }
    else
    {
      /* Subdirectory, file size should be number of files in directory */
      TMWTYPES_USHORT numFiles = 0;
      struct _finddata_t fileData;
      char filename[SDNPFSIM_MAX_PATH];
      intptr_t fileHandle;

      STRCPY(filename, SDNPFSIM_MAX_PATH, pFilename);
      STRCAT(filename, SDNPFSIM_MAX_PATH, "/*.*");

      /* Count files in 'pFilename\*.*' */
      if((fileHandle = _findfirst(filename, &fileData)) > 0)
      {
        numFiles = 1;

        while(_findnext(fileHandle, &fileData) == 0)
          numFiles += 1;

        _findclose(fileHandle);
      }

      /* exclude links to itself and parent "."  ".." according to Application Layer Draft H 6.74 */
      *pSize = numFiles-2;
      *pType = DNPDEFS_FILE_TYPE_DIRECTORY;
      if((pFileData->attrib & _A_RDONLY) == _A_RDONLY)
        *pPermissions = DNPDEFS_WORLD_READ_ALLOWED|DNPDEFS_GROUP_READ_ALLOWED|DNPDEFS_OWNER_READ_ALLOWED;
      else
        *pPermissions = 0x1ff;
    }
#else
    return TMWDEFS_FALSE;
#endif
  }
  return TMWDEFS_TRUE;
}
#endif

/* function: sdnpfsim_initDb */
void TMWDEFS_GLOBAL sdnpfsim_initDb(
  SDNPFSIM_DATABASE *pFDBHandle)
{ 
  pFDBHandle->findHandle = 0;
  pFDBHandle->blockSize = 0;
  pFDBHandle->pCurrentFile = TMWDEFS_NULL;
  pFDBHandle->bInternalDriveInfo = TMWDEFS_FALSE;

  pFDBHandle->fileAuthenticationKey = 0;
  pFDBHandle->filePassword[0] = '\0';
  pFDBHandle->fileUserName[0] = '\0';
  pFDBHandle->fileEventClass = TMWDEFS_CLASS_MASK_THREE;

#if SDNPDATA_SUPPORT_XML
  memset(pFDBHandle->xmlStaticDescriptionBuf,0,SDNPCNFG_XML_SAVE_BUF_SIZE);
  pFDBHandle->xmlStaticDescriptionBufLen = 0;
  pFDBHandle->xmlStaticDescriptionBufIndex = 0;
#endif

#if SDNPDATA_SUPPORT_XML_STATIC
  pFDBHandle->xmlStaticDescriptionBufOpen = TMWDEFS_FALSE;
  pFDBHandle->xmlStaticDescriptionBufState = SDNPXML_SAVE_NOT_DONE;
#endif

  pFDBHandle->fileTestingMode = TMWSIM_TESTINGMODE_SUCCESS;
  tmwtimer_init(&pFDBHandle->fileTestingTimer); 
}

/* function: sdnpfsim_clearDb */
void TMWDEFS_GLOBAL sdnpfsim_clearDb(
  SDNPFSIM_DATABASE *pFDBHandle)
{
  tmwtimer_cancel(&pFDBHandle->fileTestingTimer); 
}

/* function: sdnpfsim_getFileInfo */
DNPDEFS_FILE_CMD_STAT TMWDEFS_GLOBAL sdnpfsim_getFileInfo(
  TMWSESN *pSession,
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_CHAR *pFilename,
  DNPDEFS_FILE_TYPE *pType,
  TMWTYPES_ULONG *pSize,
  TMWDTIME *pTimeOfCreation,
  DNPDEFS_FILE_PERMISSIONS *pPermissions)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  struct _finddata_t fileData;
  intptr_t fileHandle;

  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_CMD_STAT_LOST_COMM);
  }
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 3)
      return(DNPDEFS_FILE_CMD_STAT_ASYNC);

    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }

  if (strcmp(pFilename,"") == 0)
  { /* return disk drive names */
    return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
  }
  else
  {
    /* Get file info */
    char tmpFileNameBuf[DNPCNFG_MAX_FILENAME+1];

    STRCPY(tmpFileNameBuf, (DNPCNFG_MAX_FILENAME+1), pFilename);

    if (tmpFileNameBuf[strlen(tmpFileNameBuf)-1] == '/' || tmpFileNameBuf[strlen(tmpFileNameBuf)-1] == '\\')
    { /* if last char is a '/' remove it */
      tmpFileNameBuf[strlen(tmpFileNameBuf)-1] = 0;
    }

    if((fileHandle = _findfirst(tmpFileNameBuf, &fileData)) > 0)
    {
      /* Close find since we are done with this one */
      _findclose(fileHandle);

      /* Get file info for response */
      _buildInfoResponse(pSession, &fileData, tmpFileNameBuf, pType, pSize, pTimeOfCreation, pPermissions);

      return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
    }
  }
#else
  TMWTARG_UNUSED_PARAM(pSession);
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(pFilename);
  TMWTARG_UNUSED_PARAM(pType);
  TMWTARG_UNUSED_PARAM(pSize);
  TMWTARG_UNUSED_PARAM(pTimeOfCreation);
  TMWTARG_UNUSED_PARAM(*pPermissions);
#endif

  return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
}
    
/* function: sdnpfsim_fileEventClass */
TMWDEFS_CLASS_MASK TMWDEFS_GLOBAL sdnpfsim_fileEventClass(
  SDNPFSIM_DATABASE *pFDBHandle)
{
  return(pFDBHandle->fileEventClass);
}

/* function: sdnpfsim_setFileEventClass */
TMWTYPES_BOOL TMWDEFS_GLOBAL sdnpfsim_setFileEventClass(
  SDNPFSIM_DATABASE *pFDBHandle, 
  TMWDEFS_CLASS_MASK eventClass)
{
  pFDBHandle->fileEventClass = eventClass;
  return(TMWDEFS_TRUE);
}

/* function: sdnpfsim_setAuthentication */
TMWTYPES_BOOL sdnpfsim_setAuthentication(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_ULONG authenticationKey, 
  TMWTYPES_CHAR *pUserName, 
  TMWTYPES_CHAR *pPassword)
{
  if(pUserName == TMWDEFS_NULL)
  {
    pFDBHandle->fileUserName[0] = '\0';
  } 
  else if(strlen(pUserName) < SDNPSIM_FILE_AUTH_SIZE)
  {    
    STRCPY(pFDBHandle->fileUserName, SDNPSIM_FILE_AUTH_SIZE, pUserName);
  }
  else 
  {
    return(TMWDEFS_FALSE);
  }

  if(pPassword == TMWDEFS_NULL)
  {
    pFDBHandle->filePassword[0] = '\0';
  }
  else if(strlen(pPassword) < SDNPSIM_FILE_AUTH_SIZE)
  {
    STRCPY(pFDBHandle->filePassword, SDNPSIM_FILE_AUTH_SIZE, pPassword);
  }
  else 
  {
    return(TMWDEFS_FALSE);
  }

  pFDBHandle->fileAuthenticationKey = authenticationKey; 

  return(TMWDEFS_TRUE);
}

/* function: sdnpfsim_getAuthenticationData */
TMWTYPES_BOOL sdnpfsim_getAuthenticationData(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_ULONG *pAuthenticationKey, 
  TMWTYPES_CHAR *pUserName, 
  TMWTYPES_USHORT maxUserNameSize,
  TMWTYPES_CHAR *pPassword,
  TMWTYPES_USHORT maxPasswordSize)
{
  STRCPY(pUserName, maxUserNameSize, pFDBHandle->fileUserName);
  STRCPY(pPassword, maxPasswordSize, pFDBHandle->filePassword); 
  *pAuthenticationKey = pFDBHandle->fileAuthenticationKey;
  return(TMWDEFS_TRUE);
}

/* function: sdnpfsim_readFileInfo */
DNPDEFS_FILE_TFER_STAT TMWDEFS_GLOBAL sdnpfsim_readFileInfo(
  TMWSESN *pSession,
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_USHORT maxNameSize,
  TMWTYPES_CHAR *pName,
  TMWTYPES_BOOL *pLast,
  DNPDEFS_FILE_TYPE *pType,
  TMWTYPES_ULONG *pSize,
  TMWDTIME *pTimeOfCreation,
  DNPDEFS_FILE_PERMISSIONS *pPermissions)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  char tmpName[SDNPFSIM_MAX_PATH];
  size_t fileNameLen;

  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_CMD_STAT_LOST_COMM);
  }
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 3)
      return(DNPDEFS_FILE_CMD_STAT_ASYNC);

    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }

  if(pFDBHandle->bInternalDriveInfo == TMWDEFS_TRUE)
  { 
    /* If SCL supports new shema, return that one */
    if(strstr(pFDBHandle->findFilename, DEVICE_PROFILE_XML2_FILE)!=0)
    {
      _buildInfoResponse(pSession, TMWDEFS_NULL, DEVICE_PROFILE_XML2_FILE, pType, pSize, pTimeOfCreation, pPermissions);
      STRCPY(pName, maxNameSize, DEVICE_PROFILE_XML2_FILE);
      STRCPY(pFDBHandle->findFilename, SDNPFSIM_MAX_PATH, DEVICE_DESCRIPTION_FILE); 
#if SDNPDATA_SUPPORT_XML
      *pLast = TMWDEFS_FALSE;
#else
      *pLast = TMWDEFS_TRUE;
#endif
    } 
    else
    {
      _buildInfoResponse(pSession, TMWDEFS_NULL, DEVICE_DESCRIPTION_FILE, pType, pSize, pTimeOfCreation, pPermissions);
      STRCPY(pName, maxNameSize, DEVICE_DESCRIPTION_FILE); 
      *pLast = TMWDEFS_TRUE;
    }
  }
  else
  {
    if(pFDBHandle->findHandle <= 0)
    {
      return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
    }

    STRCPY(tmpName, SDNPFSIM_MAX_PATH, pFDBHandle->findFilename);
    STRCAT(tmpName, SDNPFSIM_MAX_PATH, "/");
    STRCAT(tmpName, SDNPFSIM_MAX_PATH, pFDBHandle->findData.name);

    _buildInfoResponse(pSession, &pFDBHandle->findData, tmpName, pType, pSize, pTimeOfCreation, pPermissions);

    fileNameLen = strlen(pFDBHandle->findData.name);
    if(fileNameLen >= maxNameSize)
    {
      /* Name won't fit in buffer */
      return(DNPDEFS_FILE_TFER_STAT_MISC);
    }
    
    STRNCPY(pName, maxNameSize, pFDBHandle->findData.name, fileNameLen);

    /* NULL terminate name */
    pName[fileNameLen] = 0x00;

    if(_findnext(pFDBHandle->findHandle, &pFDBHandle->findData) == 0)
    {
     *pLast = TMWDEFS_FALSE;
    }
    else
    {
     *pLast = TMWDEFS_TRUE;
    }
  }

  return(DNPDEFS_FILE_TFER_STAT_SUCCESS);
#else
  TMWTARG_UNUSED_PARAM(pSession);
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(maxNameSize);
  TMWTARG_UNUSED_PARAM(pName);
  TMWTARG_UNUSED_PARAM(pLast);
  TMWTARG_UNUSED_PARAM(pType);
  TMWTARG_UNUSED_PARAM(pSize);
  TMWTARG_UNUSED_PARAM(pTimeOfCreation);
  TMWTARG_UNUSED_PARAM(pPermissions);
  return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
#endif
}


/* function: sdnpfsim_deleteFile */
DNPDEFS_FILE_CMD_STAT TMWDEFS_GLOBAL sdnpfsim_deleteFile(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_CHAR *pFilename,
  TMWTYPES_ULONG authKey)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)  
  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_CMD_STAT_MISC);
  }
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 3)
      return(DNPDEFS_FILE_CMD_STAT_ASYNC);

    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }
  
  if(authKey != pFDBHandle->fileAuthenticationKey)
  {
    return(DNPDEFS_FILE_CMD_STAT_DENIED);
  }

  /* Delete requested file */
  if(remove(pFilename))
    return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);

  /* Return success */
  return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
#else
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(pFilename);
  TMWTARG_UNUSED_PARAM(authKey);
  return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
#endif
}

/* function: sdnpfsim_getAuthentication */
TMWTYPES_BOOL TMWDEFS_GLOBAL sdnpfsim_getAuthentication(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_CHAR *pUserName,
  TMWTYPES_CHAR *pPassword,
  TMWTYPES_ULONG *pAuthKey)
{
  if((strcmp(pUserName, pFDBHandle->fileUserName) == 0)
    &&(strcmp(pPassword, pFDBHandle->filePassword) == 0))
  {
    *pAuthKey = pFDBHandle->fileAuthenticationKey;
  }
  else
  {
    /* Return authentication value of zero. */ 
    *pAuthKey = 0;
  }

  return(TMWDEFS_TRUE);
}

/* function: sdnpfsim_deviceProfileFile */
TMWTYPES_BOOL TMWDEFS_GLOBAL sdnpfsim_deviceProfileFile(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_CHAR *pFileName,
  TMWTYPES_ULONG authKey,
  TMWTYPES_BOOL *pAuthKeyOK)
{
  if(strstr(pFileName, DEVICE_PROFILE_XML2_FILE) != 0)
  {
    if(authKey == pFDBHandle->fileAuthenticationKey)
      *pAuthKeyOK = TMWDEFS_TRUE;
    else
      *pAuthKeyOK = TMWDEFS_FALSE;
    return(TMWDEFS_TRUE);
  }
  else
  {
    return(TMWDEFS_FALSE);
  }
}

#include "tmwscl/dnp/sdnpo070.h"
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
static void TMWDEFS_CALLBACK _fileTestingTimeout(
  void *pCallbackParam)
{
  sdnpo070_fileCmdComplete((TMWSESN *)pCallbackParam, 0);
}

/* function: _directorySize */
static size_t TMWDEFS_LOCAL _directorySize(
  char *pDirName)
{
  struct _finddata_t dirData;
  intptr_t dirHandle;
  size_t length = 0;

  if((dirHandle = _findfirst(pDirName, &dirData)) <= 0)
  {
    return(0);
  }
  /* For a directory, size is the number of octets that would be transferred by 
   * the read requests. This is (the number of files contained in the directory * 20)
   * + (sum of the lengths of the names of all of the files in the directory).
   */

  /* exclude links to itself and parent "."  ".." since they are not sent */
  _findnext(dirHandle, &dirData);
  while(_findnext(dirHandle, &dirData) == 0)
  {
    length += 20;
    length += strlen(dirData.name);
  }
  return(length);
}
#endif

/* function: sdnpfsim_openFile */
DNPDEFS_FILE_CMD_STAT TMWDEFS_GLOBAL sdnpfsim_openFile(
  TMWSESN *pSession,
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_CHAR *pFilename,
  TMWTYPES_ULONG authKey,
  DNPDEFS_FILE_MODE mode,
  TMWTYPES_USHORT *pMaxBlockSize,
  DNPDEFS_FILE_PERMISSIONS *pPermissions,
  TMWDTIME *pTimeOfCreation,
  TMWTYPES_ULONG *pFileHandle,
  TMWTYPES_ULONG *pSize,
  DNPDEFS_FILE_TYPE *pType)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  char *pFileMode;

  /* If file already open return error */
  if((pFDBHandle->pCurrentFile != TMWDEFS_NULL) || (pFDBHandle->findHandle > 0))
  {
    return(DNPDEFS_FILE_CMD_STAT_TOO_MANY);
  }

  if(authKey != pFDBHandle->fileAuthenticationKey)
  {
    return(DNPDEFS_FILE_CMD_STAT_DENIED);
  }

  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_CMD_STAT_MISC);
  }

  /*.. removing this tests file read delay */
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 1)
    {
      /* Start a timer, to have database call sdnpo070_fileCmdComplete */
      tmwtimer_start(&pFDBHandle->fileTestingTimer, 
        200,
        pSession->pChannel,
        _fileTestingTimeout,
        pSession); 

      return(DNPDEFS_FILE_CMD_STAT_ASYNC);
    }
    if(pFDBHandle->fileDelayCount++ < 2)
    {
      /* Let SCL retry open request periodically */
      return(DNPDEFS_FILE_CMD_STAT_ASYNC);
    }
    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }
/* SPM */
  /* if this is a request for the old XML device configuration file
   */
  if(strstr(pFilename, DEVICE_DESCRIPTION_FILE) != 0)
  {
#if SDNPDATA_SUPPORT_XML
    /* can only read the device description */
    if (mode != DNPDEFS_FILE_MODE_READ)
    {
      return(DNPDEFS_FILE_CMD_STAT_INV_MODE);
    }
    pFDBHandle->blockSize = *pMaxBlockSize;
    *pSize = sdnpxml_saveDatabaseStaticGetSize(pSession);
    if(*pSize == 0)
    {
      /* Return error */
      return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
    }
    sdnpxml_saveDatabaseStaticInit(pSession);
    pFDBHandle->xmlStaticDescriptionBufOpen = TMWDEFS_TRUE;  

    *pFileHandle = SDNPFSIM_HANDLE;
    *pType = DNPDEFS_FILE_TYPE_SIMPLE;
    /* Return success */
    return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
#else
    *pSize = 0;
    return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
#endif
  }
  else if(_STRNICMP(VIRTUAL_FILE_DRIVE, pFilename, strlen(VIRTUAL_FILE_DRIVE)) == 0
    && ((strlen(pFilename) == strlen(VIRTUAL_FILE_DRIVE))
    || (pFilename[strlen(pFilename)-1] == '/')
    || (pFilename[strlen(pFilename)-1] == '\\')))
  {  
    /* open directory of the virtual drive */
    if(mode != DNPDEFS_FILE_MODE_READ)
    {
      /* can only read the directory */
      return(DNPDEFS_FILE_CMD_STAT_INV_MODE);
    }
    else
    {
      pFDBHandle->bInternalDriveInfo = TMWDEFS_TRUE;
      pFDBHandle->bInternalFileCount = 0;
#if SDNPDATA_SUPPORT_XML2
      /* If standard schema is supported, show it first */
      STRCPY(pFDBHandle->findFilename, SDNPFSIM_MAX_PATH, DEVICE_PROFILE_XML2_FILE);
#else
      /* If just this schema is supported, show it */
      STRCPY(pFDBHandle->findFilename, SDNPFSIM_MAX_PATH, DEVICE_DESCRIPTION_FILE);
#endif
      *pType = DNPDEFS_FILE_TYPE_DIRECTORY;
      *pFileHandle = SDNPFSIM_HANDLE;
      tmwdtime_getDateTime(pSession, pTimeOfCreation);
      *pPermissions = 0x1ff;

      *pSize = 0;

#if SDNPDATA_SUPPORT_XML
      /* xml file on the virtual drive, size is
       * 20 + length of file name
       */
      *pSize += 20 + (TMWTYPES_ULONG)strlen(DEVICE_DESCRIPTION_FILE); 
#endif

#if SDNPDATA_SUPPORT_XML2 
      /* xml2 file on the virtual drive, size is
       * 20 + length of file name
       */
      *pSize += 20 + (TMWTYPES_ULONG)strlen(DEVICE_PROFILE_XML2_FILE);
#endif
    }
    /* Return success */
    return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
  }
  else
  { /* regular file processing */
    /* If mode is read, see if the requested file is a directory */
    if(mode == DNPDEFS_FILE_MODE_READ)
    {
      DNPDEFS_FILE_CMD_STAT cmdStat = sdnpfsim_getFileInfo(pSession, pFDBHandle, pFilename, pType, pSize, pTimeOfCreation, pPermissions);
      if (cmdStat != DNPDEFS_FILE_CMD_STAT_SUCCESS)
      {
        return (cmdStat);
      }
      if(*pType == DNPDEFS_FILE_TYPE_DIRECTORY)
      {
        /* Yep, it's a directory setup a directory read */
        char filename[SDNPFSIM_MAX_PATH];

        STRCPY(pFDBHandle->findFilename, SDNPFSIM_MAX_PATH, pFilename);
        STRCPY(filename, SDNPFSIM_MAX_PATH, pFilename);
        STRCAT(filename, SDNPFSIM_MAX_PATH, "/*.*");

        if((pFDBHandle->findHandle = (long)_findfirst(filename, &pFDBHandle->findData)) <= 0)
        {
          *pFileHandle = 0;
          return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
        }

        /* exclude links to itself and parent "."  ".." according to Application Layer Draft H 6.74 */
        _findnext(pFDBHandle->findHandle, &pFDBHandle->findData);
        _findnext(pFDBHandle->findHandle, &pFDBHandle->findData);

        *pSize = (TMWTYPES_ULONG)_directorySize(filename);

        pFDBHandle->blockSize = *pMaxBlockSize;
        *pFileHandle = SDNPFSIM_HANDLE;
        return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
      }
    }

    /* Get file mode */
    switch(mode)
    {
    case DNPDEFS_FILE_MODE_READ:
      pFileMode = "rb";
      break;

    case DNPDEFS_FILE_MODE_WRITE:
      pFileMode = "wb";
      break;

    case DNPDEFS_FILE_MODE_APPEND:
      pFileMode = "ab";
      break;

    default:
      return(DNPDEFS_FILE_CMD_STAT_INV_MODE);
    }
    
#if _MSC_VER >= 1400
    /* Open file */
    if(fopen_s(&pFDBHandle->pCurrentFile, pFilename, pFileMode) == 0)
    {
      /* Initialize file state */
      pFDBHandle->blockSize = *pMaxBlockSize;
      *pFileHandle = SDNPFSIM_HANDLE;

      /* Return success */
      return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
    }
#endif
  }
  /* Clear file handle */
  *pFileHandle = 0;

  /* Return error */
  return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
#else
  TMWTARG_UNUSED_PARAM(pSession);
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(pFilename);
  TMWTARG_UNUSED_PARAM(authKey);
  TMWTARG_UNUSED_PARAM(mode);
  TMWTARG_UNUSED_PARAM(pMaxBlockSize);
  TMWTARG_UNUSED_PARAM(pPermissions);
  TMWTARG_UNUSED_PARAM(pTimeOfCreation);
  TMWTARG_UNUSED_PARAM(*pFileHandle);
  TMWTARG_UNUSED_PARAM(pSize);
  TMWTARG_UNUSED_PARAM(pType);
  return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
#endif
}

/* function: sdnpfsim_closeFile */
DNPDEFS_FILE_CMD_STAT TMWDEFS_GLOBAL sdnpfsim_closeFile(
  TMWSESN *pSession,
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_ULONG fileHandle)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
 
  /* check for fileTestingMode after close is done */
  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 1)
    {
      /* Start a timer, to have database call sdnpo070_fileCmdComplete */
      tmwtimer_start(&pFDBHandle->fileTestingTimer, 
        200,
        pSession->pChannel,
        _fileTestingTimeout,
        pSession); 

      return(DNPDEFS_FILE_CMD_STAT_ASYNC);
    }
    if(pFDBHandle->fileDelayCount++ < 2)
    {
      /* Let SCL retry close request periodically */
      return(DNPDEFS_FILE_CMD_STAT_ASYNC);
    }
    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }

#if SDNPDATA_SUPPORT_XML
  if(pFDBHandle->xmlStaticDescriptionBufOpen == TMWDEFS_TRUE)
  {
    /* Validate file state */
    if(fileHandle != SDNPFSIM_HANDLE)
      return(DNPDEFS_FILE_CMD_STAT_INV_HANDLE);
    
    memset(pFDBHandle->xmlStaticDescriptionBuf,0,SDNPCNFG_XML_SAVE_BUF_SIZE);
    pFDBHandle->xmlStaticDescriptionBufLen = 0;
    pFDBHandle->xmlStaticDescriptionBufIndex = 0;
    pFDBHandle->xmlStaticDescriptionBufState = SDNPXML_SAVE_NOT_DONE;
    pFDBHandle->xmlStaticDescriptionBufOpen = TMWDEFS_FALSE;

    /* Return success */
    return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
  }
  else
#endif
  if(pFDBHandle->pCurrentFile != TMWDEFS_NULL)
  {
    /* Validate file state */
    if(fileHandle != SDNPFSIM_HANDLE)
      return(DNPDEFS_FILE_CMD_STAT_INV_HANDLE);

    if(pFDBHandle->pCurrentFile != TMWDEFS_NULL)
    {
      /* Close file */
      fclose(pFDBHandle->pCurrentFile);

      /* Clear current file pointer */
      pFDBHandle->pCurrentFile = TMWDEFS_NULL;
      
      /* For test purposes only */
      if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
      {
        return(DNPDEFS_FILE_CMD_STAT_MISC);
      }

      /* Return success */
      return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
    }
    else
      return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
  }
  else if(pFDBHandle->findHandle > 0)
  {
    /* Close find */
    _findclose(pFDBHandle->findHandle);

    /* Clear current find fileHandle */
    pFDBHandle->findHandle = 0;

    /* For test purposes only */
    if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
    {
      return(DNPDEFS_FILE_CMD_STAT_MISC);
    }

    /* Return success */
    return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
  }
  else
  { /* close for directory */
    pFDBHandle->bInternalDriveInfo = TMWDEFS_FALSE;
    return(DNPDEFS_FILE_CMD_STAT_SUCCESS);
  }
#else
  TMWTARG_UNUSED_PARAM(pSession);
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(fileHandle);
  return(DNPDEFS_FILE_CMD_STAT_NOT_FOUND);
#endif
}
/* function: sdnpfsim_readFile */
DNPDEFS_FILE_TFER_STAT TMWDEFS_GLOBAL sdnpfsim_readFile(
  TMWSESN *pSession,
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_ULONG fileHandle,
  TMWTYPES_BOOL *pLast,
  TMWTYPES_USHORT *pBytesRead,
  TMWTYPES_UCHAR *pBuf)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_TFER_STAT_MISC);
  }
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 3)
      return(DNPDEFS_FILE_TFER_STAT_ASYNC);

    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }
  
#if SDNPDATA_SUPPORT_XML
  /* If this pointer is not NULL XML description "file" has been opened
   * and xml description has been generated.
   */ 
  if(pFDBHandle->xmlStaticDescriptionBufOpen == TMWDEFS_TRUE)
  {
    TMWTYPES_UINT memCpyLen;

    /* Validate file state against request */
    if(fileHandle != SDNPFSIM_HANDLE)
      return(DNPDEFS_FILE_TFER_STAT_INV_HANDLE);

    do 
    {
      if (pFDBHandle->xmlStaticDescriptionBufLen == 0)
      { 
        pFDBHandle->xmlStaticDescriptionBufState = sdnpxml_saveDatabaseStatic(pSession, pFDBHandle->xmlStaticDescriptionBuf, SDNPCNFG_XML_SAVE_BUF_SIZE, &pFDBHandle->xmlStaticDescriptionBufLen);  
        if(pFDBHandle->xmlStaticDescriptionBufState == SDNPXML_SAVE_FAILED)
          return(DNPDEFS_FILE_TFER_STAT_MISC);
      }

      if((pFDBHandle->xmlStaticDescriptionBufIndex + pFDBHandle->blockSize) <= pFDBHandle->xmlStaticDescriptionBufLen)
      {
        memCpyLen = pFDBHandle->blockSize;
        *pLast = TMWDEFS_FALSE;
      }
      else
      {
        memCpyLen = pFDBHandle->xmlStaticDescriptionBufLen - pFDBHandle->xmlStaticDescriptionBufIndex;
        if (pFDBHandle->xmlStaticDescriptionBufState == SDNPXML_SAVE_DONE)
        {
          *pLast = TMWDEFS_TRUE;
        }
        else
        {
          *pLast = TMWDEFS_FALSE;
        }
      }
      if (memCpyLen == 0)
      {
        pFDBHandle->xmlStaticDescriptionBufIndex = 0;
        pFDBHandle->xmlStaticDescriptionBufLen = 0;
        memset(pFDBHandle->xmlStaticDescriptionBuf,0,SDNPCNFG_XML_SAVE_BUF_SIZE);
      }
    }
    while(memCpyLen == 0);

    memcpy(pBuf, &pFDBHandle->xmlStaticDescriptionBuf[pFDBHandle->xmlStaticDescriptionBufIndex], memCpyLen);
    pFDBHandle->xmlStaticDescriptionBufIndex += memCpyLen;
    if (memCpyLen < pFDBHandle->blockSize)
    {
      pFDBHandle->xmlStaticDescriptionBufIndex = 0;
      pFDBHandle->xmlStaticDescriptionBufLen = 0;
      memset(pFDBHandle->xmlStaticDescriptionBuf,0,SDNPCNFG_XML_SAVE_BUF_SIZE);
    }
    *pBytesRead = (TMWTYPES_USHORT)memCpyLen;
  }
#endif
  else
  {
    int handle;

    /* Validate file state against request */
    if(fileHandle != SDNPFSIM_HANDLE)
    {
      return(DNPDEFS_FILE_TFER_STAT_INV_HANDLE);
    }

    if(pFDBHandle->pCurrentFile == TMWDEFS_NULL)
    {
      return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
    }

    handle = _fileno(pFDBHandle->pCurrentFile);
    
    /* Read data */
    *pBytesRead = (TMWTYPES_USHORT)_read(handle, pBuf, pFDBHandle->blockSize);  

    /* Check for errors */
    if(ferror(pFDBHandle->pCurrentFile))
      return(DNPDEFS_FILE_TFER_STAT_BAD_FILE);

    /* Check for end of file */
    *pLast = (TMWTYPES_BOOL)(_eof(handle) ? TMWDEFS_TRUE : TMWDEFS_FALSE);
  }

  /* Return success */
  return(DNPDEFS_FILE_TFER_STAT_SUCCESS);
#else
  TMWTARG_UNUSED_PARAM(pSession);
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(fileHandle);
  TMWTARG_UNUSED_PARAM(pLast);
  TMWTARG_UNUSED_PARAM(pBytesRead);
  TMWTARG_UNUSED_PARAM(pBuf);
  return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
#endif
}

/* function: sdnpfsim_writeFile */
DNPDEFS_FILE_TFER_STAT TMWDEFS_GLOBAL sdnpfsim_writeFile(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWTYPES_ULONG fileHandle,
  TMWTYPES_BOOL last,
  TMWTYPES_USHORT numBytes,
  TMWTYPES_UCHAR *pBuf)
{
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  TMWTARG_UNUSED_PARAM(last);
  
  if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_FAILURE)
  {
    return(DNPDEFS_FILE_TFER_STAT_MISC);
  }
  else if(pFDBHandle->fileTestingMode == TMWSIM_TESTINGMODE_DELAY)
  {
    if(pFDBHandle->fileDelayCount++ < 3)
      return(DNPDEFS_FILE_TFER_STAT_ASYNC);

    if(pFDBHandle->fileDelayCount++ >=10)
      pFDBHandle->fileDelayCount = 0;
  }

  /* Validate file state */
  if(fileHandle != SDNPFSIM_HANDLE)
  {
    return(DNPDEFS_FILE_TFER_STAT_INV_HANDLE);
  }

  if(pFDBHandle->pCurrentFile == TMWDEFS_NULL)
  {
    return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
  }

  if(numBytes > pFDBHandle->blockSize)
  {
    return(DNPDEFS_FILE_TFER_STAT_OVERRUN);
  }

  /* Write data */
  if(fwrite(pBuf, 1, numBytes, pFDBHandle->pCurrentFile) != numBytes)
  {
    return(DNPDEFS_FILE_TFER_STAT_BAD_FILE);
  }

  /* Return success */
  return(DNPDEFS_FILE_TFER_STAT_SUCCESS);
#else
  TMWTARG_UNUSED_PARAM(pFDBHandle);
  TMWTARG_UNUSED_PARAM(fileHandle);
  TMWTARG_UNUSED_PARAM(last);
  TMWTARG_UNUSED_PARAM(numBytes);
  TMWTARG_UNUSED_PARAM(pBuf);
  return(DNPDEFS_FILE_TFER_STAT_NOT_OPEN);
#endif
}

/* function: sdnpfsim_setFileTestingMode */
void TMWDEFS_GLOBAL sdnpfsim_setFileTestingMode(
  SDNPFSIM_DATABASE *pFDBHandle,
  TMWSIM_TESTINGMODE testingMode)
{
#if SDNPDATA_SUPPORT_OBJ70
  pFDBHandle->fileTestingMode = testingMode;
  pFDBHandle->fileDelayCount = 0;
#endif
}

#endif

#endif /* #if TMWCNFG_USE_SIMULATED_DB */
