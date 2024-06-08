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

/* file: tmwcrypto.c
 * description: This file defines the interface between the Triangle
 *  MicroWorks Inc. SCL and Cryptography and Key Management routines.
 *  By default it provides a sample interface with OpenSSL(copyright)
 *  an Open Source Cryptography library. This file should be modified 
 *  for your target crypto library as desired.
 */
#include "tmwscl/utils/tmwtarg.h"
#if defined(_MSC_VER) && !defined(_WIN32_WCE)   
#include <tchar.h>
#endif

#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwcrypto.h"

#if TMWCNFG_SUPPORT_CRYPTO

#if TMWCNFG_USE_MANAGED_SCL
#include "tmwscl/.NET/TMW.SCL/TMWCryptoWrapper.h"
#undef TMWCNFG_USE_SIMULATED_CRYPTO_DB
#define TMWCNFG_USE_SIMULATED_CRYPTO_DB TMWDEFS_FALSE
#endif

#if  TMWCNFG_USE_GATEWAY_DB
#include "gateway/GTWLib/GTWCryptoWrapper.h"
#undef TMWCNFG_USE_SIMULATED_CRYPTO_DB
#define TMWCNFG_USE_SIMULATED_CRYPTO_DB TMWDEFS_FALSE
#endif

#if TMWCNFG_USE_OPENSSL
#undef TMWCNFG_USE_SIMULATED_CRYPTO
#define TMWCNFG_USE_SIMULATED_CRYPTO TMWDEFS_FALSE

/* Use OpenSSL implementation of AES Key Wrap instead of our version which calls the low level AES encrypt */
#define USE_OPENSSL_KEYWRAP 1
 
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#   pragma comment(lib,"libeay32.lib")
#endif

#ifdef __cplusplus
extern "C" {
#endif



#include "e_os2.h"

#include "openssl/ssl.h"
#include "openssl/aes.h"
#if TMWCNFG_SUPPORT_CRYPTO_AESGMAC
#include "openssl/modes.h"
#endif
#include "openssl/engine.h"
#include "openssl/evp.h"


#ifdef __cplusplus
}
#endif

TMWTYPES_BOOL openSSLInited = TMWDEFS_FALSE;
TMWTYPES_BOOL openSSLLockInited = TMWDEFS_FALSE;
TMWDEFS_RESOURCE_LOCK openSSLCryptoLock;

#endif /* TMWCNFG_USE_OPENSSL */

/* set this to 1 to include some code for crypto testing */
#define TMWCRYPTO_TESTING   1
/* set this to 1 to include some code for testing asymmetric algorithms */
#define TMWCRYPTO_ASYMTESTING   0

#if TMWCRYPTO_TESTING
TMWTYPES_BOOL tmwcrypto_tested = TMWDEFS_FALSE;
static TMWTYPES_BOOL TMWDEFS_LOCAL _testCrypto(void *pUserHandle);
#endif

/* If there is no real database to hold keys, provide
 * some storage here.
 * This simulation does NOT provide Cryptography.
 */
#if TMWCNFG_USE_SIMULATED_CRYPTO_DB
TMWTYPES_BOOL simCryptoInitialized = TMWDEFS_FALSE;

#define SIM_MAX_USERS  16
#define SIM_MAX_KEY_LEN 64

typedef struct {
  void *handle;
  TMWTYPES_USHORT keyLen;
  TMWTYPES_USHORT oldRetainedKeyLen;
  TMWTYPES_USHORT asymPubKeyLen;
  TMWTYPES_USHORT asymPrvKeyLen;
  TMWTYPES_UCHAR  key[SIM_MAX_KEY_LEN];
  TMWTYPES_UCHAR  oldRetainedKey[SIM_MAX_KEY_LEN];
  TMWTYPES_UCHAR  asymPubKey[SIM_MAX_KEY_LEN];
  TMWTYPES_UCHAR  asymPrvKey[SIM_MAX_KEY_LEN];
} TMWCRYPTO_SIM_USER;

TMWCRYPTO_SIM_USER simUser[SIM_MAX_USERS];
 
TMWTYPES_UCHAR  simOSAsymPubKey[SIM_MAX_KEY_LEN];
TMWTYPES_USHORT simOSAsymPubKeyLen;
 
TMWTYPES_UCHAR  simOSAsymPrvKey[SIM_MAX_KEY_LEN];
TMWTYPES_USHORT simOSAsymPrvKeyLen;

TMWTYPES_UCHAR  simAuthAsymPubKey[SIM_MAX_KEY_LEN];
TMWTYPES_USHORT simAuthAsymPubKeyLen;

TMWTYPES_UCHAR  simAuthAsymPrvKey[SIM_MAX_KEY_LEN];
TMWTYPES_USHORT simAuthAsymPrvKeyLen;
 
/* This is the default Auth Cert key the test harness uses to simulate the Authority */
TMWTYPES_UCHAR  simAuthCertKey[] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
  0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
  0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06
};

TMWTYPES_USHORT simAuthCertKeyLen = 32;
#endif

/* function: tmwcrypto_init */
void * TMWDEFS_GLOBAL tmwcrypto_init(void *pUserHandle)
{ 
#if TMWCNFG_USE_OPENSSL
  /* apps_startup(); */
  
  if(!openSSLLockInited)
  {
    TMWTARG_LOCK_INIT(&openSSLCryptoLock);
    openSSLLockInited = TMWDEFS_TRUE;
  }

  /* Prevent other threads from using OpenSSL until initialization is complete. */
  TMWTARG_LOCK_SECTION(&openSSLCryptoLock);
  if(!openSSLInited)
  {
#if defined(_MSC_VER) && !defined(_WIN32_WCE)   
    HINSTANCE hinst = LoadLibrary(_T("libeay32.dll"));
    if (hinst == NULL) 
    {
      TMWDIAG_ERROR("tmwcrypto: load libeay32.dll failed");
      return TMWDEFS_NULL;
    }
#endif
    openSSLInited = TMWDEFS_TRUE;
   
    /* 1.0.1 Beta1 and 1.0.1c support aes gmac */
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
    {
      const char *versionString;
      int version = SSLeay(); 
      versionString = SSLeay_version(SSLEAY_VERSION);
      TMWDIAG_MESSAGE("tmwcrypto: OpenSSL version", (TMWDIAG_ID_TARGET|TMWDIAG_ID_SECURITY_DATA|TMWDIAG_ID_PHYS));
      TMWDIAG_MESSAGE(versionString, (TMWDIAG_ID_TARGET|TMWDIAG_ID_SECURITY_DATA|TMWDIAG_ID_PHYS));
      if(version < 0x10001001L)
      {
        TMWDIAG_ERROR("tmwcrypto: OpenSSL version was older than 1.0.1-beta1. There may be missing functionality"); 
      }
    }
#endif

    /* do_pipe_sig(); */
    CRYPTO_malloc_init();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
	  ENGINE_load_builtin_engines();
  	RAND_seed(&pUserHandle, 10);
 	  TMWDIAG_MESSAGE( "SSL init complete", (TMWDIAG_ID_TARGET|TMWDIAG_ID_SECURITY_DATA|TMWDIAG_ID_PHYS));
  }
          
  TMWTARG_UNLOCK_SECTION(&openSSLCryptoLock);
#endif

#if TMWCRYPTO_TESTING
  /* This can be removed once your cryptography interface is tested */
  /* Just test this on first call to tmwcrypto_init */
  if (!tmwcrypto_tested)
  {
  if(!_testCrypto(pUserHandle))
  {
    /* The crypto testing failed */ 
    TMWDIAG_ERROR("tmwcrypto: failed crypto testing"); 
  }
    else
    {
      tmwcrypto_tested = TMWDEFS_TRUE;
    }
  }
#endif

#if TMWCNFG_USE_MANAGED_SCL
  /* If TMW managed implementation (TH and .NET) is being used */
  return(TMWCryptoWrapper_Init(pUserHandle));

#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB
  if(!simCryptoInitialized)
  {
    int i;
    simCryptoInitialized = TMWDEFS_TRUE;
    
    TMWDIAG_ERROR("tmwcrypto: Using simulated cryptography TMWCNFG_USE_SIMULATED_CRYPTO_DB. Must implement a database.");

    for(i=0; i<SIM_MAX_USERS; i++)
    {  
      simUser[i].handle = 0;
      simUser[i].keyLen = 0;
      simUser[i].oldRetainedKeyLen = 0; 
      simUser[i].asymPubKeyLen = 0;
      simUser[i].asymPrvKeyLen = 0;
    }

    /* These are default Outstation RSA Asymmetric keys used for encryption */
    simOSAsymPubKeyLen = (TMWTYPES_USHORT)strlen("TMWTestOSRsa2048PubKey.pem"); 
    memcpy(simOSAsymPubKey, "TMWTestOSRsa2048PubKey.pem", simOSAsymPubKeyLen);
      
    simOSAsymPrvKeyLen = (TMWTYPES_USHORT)strlen("TMWTestOSRsa2048PrvKey.pem"); 
    memcpy(simOSAsymPrvKey, "TMWTestOSRsa2048PrvKey.pem", simOSAsymPrvKeyLen);
     
    /* These are default Authority DSA Asymmetric keys for signing */  
    simAuthAsymPubKeyLen = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa2048PubKey.pem"); 
    memcpy(simAuthAsymPubKey, "TMWTestAuthorityDsa2048PubKey.pem", simAuthAsymPubKeyLen);

    simAuthAsymPrvKeyLen = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa2048PrvKey.pem"); 
    memcpy(simAuthAsymPrvKey, "TMWTestAuthorityDsa2048PrvKey.pem", simAuthAsymPrvKeyLen);
  }
   
  return (void *)1;

#else
  /* Put target code here */
  /* This sample does not use a Crypto Handle 
   * but ensures a non NULL return value to indicate success
   */
  if (pUserHandle == TMWDEFS_NULL)
  return (void *)1;

  return pUserHandle;
#endif
}

/* function: tmwcrypto_close */
void TMWDEFS_GLOBAL tmwcrypto_close(void *pCryptoHandle)
{
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
}

/* function: tmwcrypto_algorithmSupport */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_algorithmSupport(
  void *pCryptoHandle,
  TMWTYPES_ULONG algorithm)
{  
#if TMWCNFG_USE_OPENSSL
  /* This sample does not use a Crypto Handle */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128)
    return TMWDEFS_TRUE;
 
  else if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SYM_AES128)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SYM_AES256)
    return TMWDEFS_FALSE;
  
  else if(algorithm ==  TMWCRYPTO_ALG_ASYM_RSAESOAEP1024)
    return TMWDEFS_TRUE;

  else if(algorithm ==  TMWCRYPTO_ALG_ASYM_RSAESOAEP2048)
    return TMWDEFS_TRUE;

  else if(algorithm ==  TMWCRYPTO_ALG_ASYM_RSAESOAEP3072)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    return TMWDEFS_TRUE;
    
  else if(algorithm == TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1)
    return TMWDEFS_TRUE;
    
  else if(algorithm == TMWCRYPTO_ALG_SIGN_RSA_2048_SHA256)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_SIGN_RSA_3072_SHA256)
    return TMWDEFS_TRUE;

  else if(algorithm == TMWCRYPTO_ALG_MAC_SHA1)
    return TMWDEFS_TRUE;
    
  else if(algorithm == TMWCRYPTO_ALG_MAC_SHA256)
    return TMWDEFS_TRUE;

#if TMWCNFG_SUPPORT_CRYPTO_AESGMAC
  else if(algorithm == TMWCRYPTO_ALG_MAC_AESGMAC)
    return TMWDEFS_TRUE;
#endif
   
  else
    return TMWDEFS_FALSE; 
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  return TMWDEFS_FALSE; 
#endif
}

TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_setIVector(
  void             *pCryptoHandle,
  TMWCRYPTO_KEY    *pKey,
  TMWTYPES_UCHAR   *pIVector,
  TMWTYPES_USHORT   IVectorLength)
{
#if TMWCNFG_USE_OPENSSL || TMWCNFG_USE_SIMULATED_CRYPTO
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  if(IVectorLength <= TMWCRYPTO_MAX_IVECTOR_LENGTH)
  {
    pKey->ivLength = IVectorLength;
    if(IVectorLength > 0)
      memcpy(pKey->iv, pIVector, IVectorLength);
    return TMWDEFS_TRUE;
  }
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pIVector);
  TMWTARG_UNUSED_PARAM(IVectorLength);
#endif 
  return TMWDEFS_FALSE;
}

/* function: tmwcrypto_MACValue */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_MACValue(
  void             *pCryptoHandle,
  TMWTYPES_ULONG    algorithm,
  TMWCRYPTO_KEY    *pKey,
  TMWTYPES_USHORT   requestedLength,
  TMWTYPES_UCHAR   *pData,
  TMWTYPES_USHORT   dataLength,
  TMWTYPES_UCHAR   *pMACValue,
  TMWTYPES_USHORT  *pMACValueLength)
{ 
#if TMWCNFG_USE_OPENSSL
  TMWTYPES_UCHAR *pHash;
  unsigned int hashLength;
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 
  
  if(algorithm == TMWCRYPTO_ALG_MAC_SHA1)
  {
    /* This returns a pointer to a 20 byte hash value */
	  pHash = HMAC(EVP_sha1(), pKey->value, pKey->length,
	     (const unsigned char *)pData, dataLength, TMWDEFS_NULL, &hashLength);

    if(pHash != TMWDEFS_NULL)
    {
      if(requestedLength > 20)
        requestedLength = 20;

      memcpy(pMACValue, pHash, requestedLength);
      *pMACValueLength = requestedLength;
      return TMWDEFS_TRUE;
    } 
  }
  else if(algorithm == TMWCRYPTO_ALG_MAC_SHA256)
  {
    /* This returns a pointer to a 32 byte hash value */
    pHash = HMAC(EVP_sha256(), pKey->value, pKey->length,
      (const unsigned char *)pData, dataLength, TMWDEFS_NULL, &hashLength);
    if(pHash != TMWDEFS_NULL)
    {
      if(requestedLength > 32)
        requestedLength = 32;

      memcpy(pMACValue, pHash, requestedLength);
      *pMACValueLength = requestedLength;
      return TMWDEFS_TRUE;
    } 
  } 
#if TMWCNFG_SUPPORT_CRYPTO_AESGMAC
  else if(algorithm == TMWCRYPTO_ALG_MAC_AESGMAC)
  {
    GCM128_CONTEXT *pCtx;
    AES_KEY aesKey;
   
    /* length needs to be in bits */ 
    AES_set_encrypt_key(pKey->value, pKey->length*8, &aesKey); 

    pCtx = CRYPTO_gcm128_new(&aesKey, (block128_f)AES_encrypt);
    if(pCtx != TMWDEFS_NULL) 
    {
      /*iv has initialization vector in it */ 
      if(pKey->ivLength > 0)
        CRYPTO_gcm128_setiv(pCtx, pKey->iv, pKey->ivLength);	
   
      CRYPTO_gcm128_aad(pCtx, pData, dataLength);
      if(!CRYPTO_gcm128_encrypt(pCtx, TMWDEFS_NULL, TMWDEFS_NULL, 0))
      { 
        CRYPTO_gcm128_tag(pCtx, pMACValue, 12);

        *pMACValueLength = 12;

        CRYPTO_gcm128_release(pCtx);

        return TMWDEFS_TRUE;
      }

      CRYPTO_gcm128_release(pCtx);
    }

  } 
#endif
  return TMWDEFS_FALSE;

#elif TMWCNFG_USE_SIMULATED_CRYPTO
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(dataLength);
  
  TMWDIAG_ERROR("tmwcrypto: Using simulated cryptography TMWCNFG_USE_SIMULATED_CRYPTO.");

  memcpy(pMACValue, pData, requestedLength);
  *pMACValueLength = requestedLength;
  return TMWDEFS_TRUE;

#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(requestedLength);
  TMWTARG_UNUSED_PARAM(pData);
  TMWTARG_UNUSED_PARAM(dataLength);
  TMWTARG_UNUSED_PARAM(pMACValue);
  TMWTARG_UNUSED_PARAM(pMACValueLength);
  return TMWDEFS_FALSE;
#endif
}

/* function: tmwcrypto_getRandomData */ 
TMWTYPES_BOOL TMWDEFS_LOCAL tmwcrypto_getRandomData(
  void            *pCryptoHandle,
  TMWTYPES_USHORT  minLength,
  TMWTYPES_UCHAR  *pBuf,
  TMWTYPES_USHORT *pLength)
{
#if TMWCNFG_USE_OPENSSL
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 

  /*This routine puts minLength random bytes into pBuf.  */
  RAND_bytes(pBuf, minLength);
  *pLength = minLength;
  return TMWDEFS_TRUE;

#elif TMWCNFG_USE_SIMULATED_CRYPTO
  TMWDIAG_ERROR("tmwcrypto: Using simulated cryptography TMWCNFG_USE_SIMULATED_CRYPTO.");
  *pLength = minLength;
  return TMWDEFS_TRUE;
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 
  TMWTARG_UNUSED_PARAM(minLength); 
  TMWTARG_UNUSED_PARAM(pBuf); 
  TMWTARG_UNUSED_PARAM(pLength); 
  return TMWDEFS_FALSE;
#endif
}

typedef struct pw_cb_data
	{
	const void *password;
	const char *prompt_info;
	} PW_CB_DATA;

int password_callback(char *buf, int bufsiz, int verify,
	PW_CB_DATA *cb_data)
{ 
	const char *password = TMWDEFS_NULL;
  TMWTARG_UNUSED_PARAM(verify); 
  if (cb_data->password != TMWDEFS_NULL)
  {
    size_t len;
    password = (char *) cb_data->password;
    len = strlen(password);
    if((int)len>bufsiz)
      len = bufsiz;
    memcpy(buf, password, len);
    return (int)len;
  }
  return 0;
}

/* function: tmwcrypto_getAsymKeyTypeSize */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_getAsymKeyTypeSize(
  void            *pCryptoHandle,
  TMWCRYPTO_KEY   *pKey,
  TMWTYPES_UCHAR  *pType,
  TMWTYPES_ULONG  *pSize)
{
#if TMWCNFG_USE_OPENSSL
  int              keySizeInBits;
  int              keyType;
  EVP_PKEY        *pEVPKey;
  FILE            *fp;
  PW_CB_DATA       passworddata;
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 

  /* Read private key */
  fp = fopen ((char*)pKey->value, "r");
  if (fp == TMWDEFS_NULL) 
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }

  if(pKey->passwordLength != 0)
  {
    pKey->password[pKey->passwordLength] = '\0';
    passworddata.password = pKey->password;
  }
  else
  {
    passworddata.password = TMWDEFS_NULL;
  }
  
  pEVPKey = PEM_read_PrivateKey(fp, NULL, (pem_password_cb *)password_callback, &passworddata);
  
  fclose (fp);

  if (pEVPKey == TMWDEFS_NULL) 
  { 
    ERR_print_errors_fp (stderr); 
    TMWDIAG_ERROR("tmwcrypto: genDigitalSignature could not read private key");
    return TMWDEFS_FALSE;
  }
   
  /* Get the size of key. */
  keySizeInBits = EVP_PKEY_bits(pEVPKey);
  *pSize = keySizeInBits;
  
  keyType = EVP_PKEY_id(pEVPKey);
  if(keyType == EVP_PKEY_DSA)
    *pType = 1;
  else if(keyType == EVP_PKEY_RSA)
    *pType = 2;
  else
    *pType = 0;

  return TMWDEFS_TRUE;
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pType);
  TMWTARG_UNUSED_PARAM(pSize);
  return(TMWDEFS_FALSE);
#endif
}

/* function: tmwcrypto_genDigitalSignature */
/* generate digital signature for TMWCRYPTO_ALG_RSA2048 and TMWCRYPTO_ALG_RSA1024  */ 
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_genDigitalSignature(
  void                *pCryptoHandle, 
  TMWTYPES_ULONG       algorithm,
  TMWCRYPTO_KEY       *pKey,
  TMWTYPES_UCHAR      *pData,
  TMWTYPES_USHORT      dataLength,
  TMWTYPES_UCHAR      *pSignatureData,
  TMWTYPES_USHORT     *pSignatureLength)
{
#if TMWCNFG_USE_OPENSSL
  int              keyType;
  int              keySizeInBits;
  int              requiredSize;
  char            *pRequiredType;
  TMWTYPES_BOOL    error;
  unsigned int     length;
  int              retCode;
  EVP_PKEY        *pEVPKey;
  FILE            *fp;
  EVP_MD_CTX       md_ctx;
  PW_CB_DATA       passworddata;
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 

  /* Read private key */
  fp = fopen ((char*)pKey->value, "r");
  if (fp == TMWDEFS_NULL) 
  {
    /* errno contains error  */
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }

  if(pKey->passwordLength != 0)
  {
    pKey->password[pKey->passwordLength] = '\0';
    passworddata.password = pKey->password;
  }
  else
  {
    passworddata.password = TMWDEFS_NULL;
  }
  
  /* If the key is an RSA key it will use RSA, if DSA key it will use DSA */
  pEVPKey = PEM_read_PrivateKey(fp, NULL, (pem_password_cb *)password_callback, &passworddata);
  
  fclose (fp);

  if (pEVPKey == TMWDEFS_NULL) 
  { 
    ERR_print_errors_fp (stderr);
    TMWDIAG_ERROR("tmwcrypto: genDigitalSignature could not read private key");
    return TMWDEFS_FALSE;
  }
   
  /* Check that the correct type and size key is being used. */
  requiredSize = 0;
  pRequiredType = "";
  error = TMWDEFS_FALSE;
  /* Get the size in bits*/
  keySizeInBits = EVP_PKEY_bits(pEVPKey);

  /* Get the type */
  keyType = EVP_PKEY_id(pEVPKey);
  
  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256))
  {
    if(keyType != EVP_PKEY_DSA)
    {
      pRequiredType = "DSA";
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_RSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_3072_SHA256))
  {
    if(keyType != EVP_PKEY_RSA)
    {
      pRequiredType = "RSA";
      error = TMWDEFS_TRUE;
    }
  }
  else
  {
    return TMWDEFS_FALSE;
  }
  
  if(error)
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: genDigitalSignature, key %s is wrong type expected %s", (char*)pKey->value, pRequiredType);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }

  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1))
  {
    if(keySizeInBits != 1024)
    {
      requiredSize = 1024;
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_2048_SHA256))
  {
    if(keySizeInBits != 2048)
    {
      requiredSize = 2048;
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_3072_SHA256))
  {
    if(keySizeInBits != 3072)
    {
      requiredSize = 3072;
      error = TMWDEFS_TRUE;
    }
  }
  else
  {
    return TMWDEFS_FALSE;
  }
  
  if(error)
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: genDigitalSignature, key %s is wrong size, expected %d bits", (char*)pKey->value, requiredSize);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }

  /* Make sure there is enough room for signature */
  if(EVP_PKEY_size(pEVPKey) > *pSignatureLength)
    return TMWDEFS_FALSE;
  
  /* Do the signature */ 
  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1))
  {
    EVP_SignInit(&md_ctx, EVP_sha1());
  }
  else
  {
    EVP_SignInit(&md_ctx, EVP_sha256());
  }

  EVP_SignUpdate(&md_ctx, pData, dataLength);

  retCode = EVP_SignFinal (&md_ctx, pSignatureData, &length, pEVPKey);

  *pSignatureLength = (TMWTYPES_USHORT)length;
  
  EVP_MD_CTX_cleanup(&md_ctx);
  EVP_PKEY_free (pEVPKey);

  if(retCode == 1)
    return TMWDEFS_TRUE;
  else
    return TMWDEFS_FALSE;

#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pData);
  TMWTARG_UNUSED_PARAM(dataLength);
  TMWTARG_UNUSED_PARAM(pSignatureData);
  TMWTARG_UNUSED_PARAM(pSignatureLength);
  return(TMWDEFS_FALSE);
#endif
}

/* function: tmwcrypto_verifySignature */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_verifySignature(
  void                *pCryptoHandle,
  TMWTYPES_ULONG       algorithm,
  TMWCRYPTO_KEY       *pKey,
  TMWTYPES_UCHAR      *pData,
  TMWTYPES_USHORT      dataLength,
  TMWTYPES_UCHAR      *pSignatureData,
  TMWTYPES_USHORT      signatureLength)
{ 
#if TMWCNFG_USE_OPENSSL
  int            keyType;
  int            keySizeInBits;
  int            requiredSize;
  char          *pRequiredType;
  TMWTYPES_BOOL  error;
  int            retCode;
  EVP_PKEY      *pEVPKey;
  FILE          *fp;
  X509          *x509;
  EVP_MD_CTX     md_ctx;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  /* Read public key */ 
  fp = fopen ((char*)pKey->value, "r");
  if (fp == NULL) 
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }
  
  /* This works if there was a BEGIN CERTIFICATE in file */
  /*X509 *PEM_read_X509(FILE *fp, X509 **x, pem_password_cb *cb, void *u); */
 
  x509 = PEM_read_X509(fp, NULL, NULL, NULL);

  if (x509 != NULL) 
  {
    /* Get public key */
    pEVPKey=X509_get_pubkey(x509);
    X509_free(x509);
    fclose (fp); 
  }
  else
  {
    fp = freopen ((char*)pKey->value, "r", fp);
    if (fp == NULL) 
      return TMWDEFS_FALSE;

    /* This works if there is a BEGIN PUBLIC KEY in file */
    /* EVP_PKEY *PEM_read_PUBKEY(fp, EVP_PKEY **x, pem_password_cb *cb, void *u); */
    pEVPKey = PEM_read_PUBKEY(fp, NULL,	NULL, NULL);
    fclose (fp);
  }
  
  if (pEVPKey == NULL)
    /* ERR_print_errors_fp (stderr); */
    return(TMWDEFS_FALSE);

  /* Check that the correct type and size key is being used. */
  error = TMWDEFS_FALSE;
  requiredSize = 0;
  pRequiredType = "";
  
  /* Get the size in bits*/
  keySizeInBits = EVP_PKEY_bits(pEVPKey);

  /* Get the type */
  keyType = EVP_PKEY_id(pEVPKey);
  
  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256))
  {
    if(keyType != EVP_PKEY_DSA)
    {
      pRequiredType = "DSA";
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_RSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_3072_SHA256))
  {
    if(keyType != EVP_PKEY_RSA)
    {
      pRequiredType = "RSA";
      error = TMWDEFS_TRUE;
    }
  }
  else
  {
    return TMWDEFS_FALSE;
  }
  
  if(error)
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: verifySignature, key %s is wrong type expected %s", (char*)pKey->value, pRequiredType);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }
  
  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1))
  {
    if(keySizeInBits != 1024)
    {
      requiredSize = 1024;
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_2048_SHA256))
  {
    if(keySizeInBits != 2048)
    {
      requiredSize = 2048;
      error = TMWDEFS_TRUE;
    }
  }
  else if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_3072_SHA256))
  {
    if(keySizeInBits != 3072)
    {
      requiredSize = 3072;
      error = TMWDEFS_TRUE;
    }
  }
  else
  {
    return TMWDEFS_FALSE;
  }

  if(error)
  {
#if TMWCNFG_SUPPORT_DIAG
    char buf[128];
    tmwtarg_snprintf(buf, 128, "tmwcrypto: verifySignature, key %s is wrong size, expected %d bits", (char*)pKey->value, requiredSize);
    TMWDIAG_ERROR(buf);
#endif
    return TMWDEFS_FALSE;
  }

  /* Verify the signature */
  if((algorithm == TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1)
    ||(algorithm == TMWCRYPTO_ALG_SIGN_RSA_1024_SHA1))
  {
    /* If the key is an RSA key it will use RSA, if DSA key it will use DSA */
    EVP_VerifyInit(&md_ctx, EVP_sha1());
  }
  else
  {
    EVP_VerifyInit(&md_ctx, EVP_sha256());
  }

  EVP_VerifyUpdate(&md_ctx, pData, dataLength);
  retCode = EVP_VerifyFinal (&md_ctx, pSignatureData, signatureLength, pEVPKey);

  EVP_MD_CTX_cleanup(&md_ctx);
  EVP_PKEY_free(pEVPKey);

  if (retCode == 1) {
    /* "Signature Verified Ok.\n");*/
    return TMWDEFS_TRUE;
  }
  else
  {
	  /* ERR_print_errors_fp (stderr);*/
    return TMWDEFS_FALSE;
  }
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pData);
  TMWTARG_UNUSED_PARAM(dataLength);
  TMWTARG_UNUSED_PARAM(pSignatureData);
  TMWTARG_UNUSED_PARAM(signatureLength);
  return(TMWDEFS_FALSE);
#endif
}

typedef struct {
  TMWTYPES_UCHAR bytes[8];
} MDNPDATA_R_TYPE;

#if TMWCNFG_USE_OPENSSL 
static TMWTYPES_UCHAR m_IV[8] = {0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6};
#endif

#define MAX_BLOCK_SIZE 128
 
/* function: tmwcrypto_encryptData */ 
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_encryptData(
  void                *pCryptoHandle,
  TMWTYPES_ULONG       algorithm, 
  TMWCRYPTO_KEY       *pKey,
  TMWTYPES_UCHAR      *pPlainData, 
  TMWTYPES_USHORT      plainDataLength, 
  TMWTYPES_UCHAR      *pEncryptedData,
  TMWTYPES_USHORT     *pEncryptedLength)
{ 
#if TMWCNFG_USE_OPENSSL
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 
#if USE_OPENSSL_KEYWRAP
  if((algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128) ||(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256))
  {
    /* This calls the OpenSSL implementation of AES KEYWRAP 
     * The else code implements the Key Wrap algorithm.
     */  
    int retCode;
    int n;
    AES_KEY aesKey; 
    int keyBits = 128;
   
    if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256)
      keyBits = 256;

    if(!AES_set_encrypt_key(pKey->value, keyBits, &aesKey)) 
    {

      /* plainText must be multiple of 8bytes (64bits)*/ 
      if((n = plainDataLength%8) != 0)
      { 
        plainDataLength += (TMWTYPES_USHORT)(8-n); 
      }
   
      retCode = AES_wrap_key(&aesKey, m_IV, pEncryptedData, pPlainData, plainDataLength); 
      if (retCode > 0)  
      {
        *pEncryptedLength = (TMWTYPES_USHORT)retCode; 
        return TMWDEFS_TRUE;
      }
    }
  }
#else
  if((algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128) ||(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256))
  {
    /* This code implements the AES Key Wrap Algorithm specified in RFC3394,
     * It calls a function that provides the AES encryption algorithm specified 
     * in FIPS 197 multiple times based on the length of the plain data.
     * You could just call the key wrap algorithm from here.
     */ 
    int i;
    int n;
    int j;
    MDNPDATA_R_TYPE R[20]; 
    TMWTYPES_UCHAR input[16];
    TMWTYPES_UCHAR A[16];
    TMWTYPES_UCHAR B[16];
    unsigned char iv[2*MAX_BLOCK_SIZE/8];
    EVP_CIPHER_CTX ctx;
    int outl;			
    	
    memset(iv,0, sizeof(iv));
      
    EVP_CIPHER_CTX_init(&ctx);
    if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128)
    {
      EVP_EncryptInit_ex(&ctx,EVP_get_cipherbyname("AES-128-ECB"),
        NULL, pKey->value,iv);
    }
    else
    {
      EVP_EncryptInit_ex(&ctx,EVP_get_cipherbyname("AES-256-ECB"),
        NULL, pKey->value,iv);
    }

    /* This turns padding off */
    EVP_CIPHER_CTX_set_padding(&ctx, 0);

    /* plainText must be multiple of 8bytes (64bits)*/
    n = plainDataLength/8;
    if(plainDataLength%8 != 0)
    { 
      n++; 
    }

    /* make sure there is enough room in the array */
    if(n<= 19) 
    {

      /* Section 2.2.1 alternative description of the key wrap algorithm from RFC3394 
       * Set A0 = IV, an initial value (see 2.2.3)
       */
      memcpy(A, m_IV, 8);
      
      /* For i=1 to n, R[i]=Pi] */
      memcpy(&R[1], pPlainData, plainDataLength);
      
      for(j=0; j<=5; j++)
      {  
        int i;
        for(i=1; i<=n; i++)
        {                 
          /* B = AES(K, A | R[i])     */  
          memcpy(input, A, 8);
          memcpy(&input[8], &R[i], 8); 

          /* This calls the AES Encryption function specified in FIPS 197 */
          EVP_EncryptUpdate(&ctx,B,&outl,input,16); /* high level openssl */

          /* The returned encrypted data would also be 16 bytes long */

          /* A = MSB(64, B) ^ t where t = (n*j)+i */
          memcpy(A, B, 8);
          A[7] ^= (n*j) +i;

          /* R[i] = LSB(64, B) */ 
          memcpy(&R[i], &B[8], 8);
        }
      }

          
      EVP_EncryptFinal_ex(&ctx,A,&outl);
      EVP_CIPHER_CTX_cleanup(&ctx);

      /* Set C[0] = A    */
      /* For i = 1 to n  */
      /*    C[i] = R[i]  */
      memcpy(pEncryptedData, A, 16);
      for(i=1; i<=n; i++) 
        memcpy(&pEncryptedData[i*8], &R[i], 8);
     
      *pEncryptedLength = (TMWTYPES_USHORT)(n*8)+8;
      return TMWDEFS_TRUE;
    }
  } 
#endif
  else if((algorithm == TMWCRYPTO_ALG_SYM_AES128) || (algorithm == TMWCRYPTO_ALG_SYM_AES256))
  {
    int encryptedLength;
    TMWTYPES_UCHAR *pEData; 
    unsigned char iv[2*MAX_BLOCK_SIZE/8];
    EVP_CIPHER_CTX ctx; 

    encryptedLength = 0;
    pEData = pEncryptedData; 

    if(pKey->ivLength == 0)
      memset(iv,0, sizeof(iv));
    else
      memcpy(iv, pKey->iv, pKey->ivLength);

    EVP_CIPHER_CTX_init(&ctx);

    if(algorithm & TMWCRYPTO_ALG_SYM_AES128)
      EVP_EncryptInit_ex(&ctx,EVP_get_cipherbyname("AES-128-CBC"),
        NULL, pKey->value,iv);
    else
      EVP_EncryptInit_ex(&ctx,EVP_get_cipherbyname("AES-256-CBC"),
        NULL, pKey->value,iv);

    /* high level openssl */
    if(EVP_EncryptUpdate(&ctx, pEData, &encryptedLength,
      pPlainData, plainDataLength)) 
    {

      pEData += encryptedLength;
      *pEncryptedLength = (TMWTYPES_USHORT)encryptedLength;

      if((plainDataLength-encryptedLength) > 0)
      {
        EVP_EncryptFinal_ex(&ctx, pEData, &encryptedLength);
        *pEncryptedLength += (TMWTYPES_USHORT)encryptedLength;
      }
      EVP_CIPHER_CTX_cleanup(&ctx);

      return TMWDEFS_TRUE;
    }
  }
#if TMWCNFG_SUPPORT_CRYPTO_ASYM
  else if((algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP1024) 
    || (algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP2048)
    || (algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP3072))
  {   
    EVP_PKEY_CTX  *pCtx;
    EVP_PKEY      *pEVPKey;
    FILE          *fp;
    X509          *x509;
    size_t         outlen; 
   
    /* Read public key */ 
    fp = fopen ((char*)pKey->value, "r");
    
    if (fp == NULL)  
    {
#if TMWCNFG_SUPPORT_DIAG
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
      TMWDIAG_ERROR(buf);
#endif
      return TMWDEFS_FALSE;
    }

    /* This works if there was a BEGIN CERTIFICATE in file */
    /*X509 *PEM_read_X509(FILE *fp, X509 **x, pem_password_cb *cb, void *u); */
    x509 = PEM_read_X509(fp, NULL, NULL, NULL);

    if (x509 != NULL) 
    {
      /* Get public key */
      pEVPKey=X509_get_pubkey(x509);
      X509_free(x509);
      fclose (fp);
    }
    else
    {
      fp = freopen ((char*)pKey->value, "r", fp);
      if (fp == NULL) 
        return TMWDEFS_FALSE;

      /* This works if there is a BEGIN PUBLIC KEY in file */
      /* EVP_PKEY *PEM_read_PUBKEY(fp, EVP_PKEY **x, pem_password_cb *cb, void *u); */
      pEVPKey = PEM_read_PUBKEY(fp, NULL,	NULL, NULL);
      fclose (fp);
    }

    if (pEVPKey != NULL) 
    {
      int keySizeInBits; 
      int requiredSize = 0;
      TMWTYPES_BOOL error = TMWDEFS_FALSE;

      /* Check that the correct size key is being used. */
      keySizeInBits = EVP_PKEY_bits(pEVPKey);
      if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP1024)
      {  
        if(keySizeInBits != 1024)
        {
          requiredSize = 1024;
          error = TMWDEFS_TRUE;
        }
      } 
      else if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP2048)
      {
        if(keySizeInBits != 2048)
        {
          requiredSize = 2048;
          error = TMWDEFS_TRUE;
        }
      }
      else if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP3072)
      {
        if(keySizeInBits != 3072)
        {
          requiredSize = 3072;
          error = TMWDEFS_TRUE;
        }
      } 
      else
      {
        return TMWDEFS_FALSE;
      }

      if(error)
      {
#if TMWCNFG_SUPPORT_DIAG
        char buf[128];
        tmwtarg_snprintf(buf, 128, "tmwcrypto: encryptData, key %s is wrong size, expected %d bits", (char*)pKey->value, requiredSize);
        TMWDIAG_ERROR(buf);
#endif
        return TMWDEFS_FALSE;
      }

      pCtx = EVP_PKEY_CTX_new(pEVPKey, NULL);
      if (pCtx != TMWDEFS_NULL)
      { 
     
        if (EVP_PKEY_encrypt_init(pCtx) > 0)
        { 
       
          if (EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_OAEP_PADDING) > 0)
          { 
         
            outlen = *pEncryptedLength;
            if (EVP_PKEY_encrypt(pCtx, pEncryptedData, &outlen, pPlainData, plainDataLength) > 0)
            {
              EVP_PKEY_CTX_free(pCtx);

              /* Encrypted data is outlen bytes written to buffer out */
              *pEncryptedLength = (TMWTYPES_USHORT)outlen;
          
              return TMWDEFS_TRUE;
            }
          }
        }
        /* Error */
        EVP_PKEY_CTX_free(pCtx); 
      }
	    /* Error occurred */
      EVP_PKEY_free (pEVPKey); 
    }
  } 
#endif

#else
  /* Put target code here */
  TMWDIAG_ERROR("tmwcrypto: tmwcrypto_encryptData not implemented");
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pPlainData);
  TMWTARG_UNUSED_PARAM(plainDataLength);
  TMWTARG_UNUSED_PARAM(pEncryptedData);
  TMWTARG_UNUSED_PARAM(pEncryptedLength);
#endif

  TMWDIAG_ERROR("TMWCRYPTO encryption failed.");
  return TMWDEFS_FALSE; 
}

/* function: tmwcrypto_decryptData */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_decryptData(
  void                *pCryptoHandle,
  TMWTYPES_ULONG       algorithm,
  TMWCRYPTO_KEY       *pKey,
  TMWTYPES_UCHAR      *pEncryptedData, 
  TMWTYPES_USHORT      encryptedDataLength,
  TMWTYPES_UCHAR      *pPlainData,
  TMWTYPES_USHORT     *pPlainLength)
{
#if TMWCNFG_USE_OPENSSL 
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
#if USE_OPENSSL_KEYWRAP
  if((algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128) ||(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256))
  {
    /* This calls the OpenSSL implementation of AES KEYWRAP 
     * The else code implements the Key Wrap algorithm.
     */  
    int retCode;
    AES_KEY aesKey; 
    int keyBits = 128;
   
    if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256)
      keyBits = 256;
    
    if(AES_set_decrypt_key(pKey->value, keyBits, &aesKey))
      return TMWDEFS_FALSE;
   
	  retCode = AES_unwrap_key(&aesKey, m_IV, pPlainData, pEncryptedData, encryptedDataLength);
	  if (retCode <= 0) 
      return TMWDEFS_FALSE;

    *pPlainLength = (TMWTYPES_USHORT)retCode; 

    return TMWDEFS_TRUE;
  }
#else
  if((algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128) ||(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES256))
  {   
    /* This code implements the AES Key UnWrap Algorithm specified in RFC3394,
     * It calls a function that provides the AES decryption algorithm specified 
     * in FIPS 197 multiple times based on the length of the encrypted data.
     */ 
    int i;
    int j;
    int n;
    MDNPDATA_R_TYPE R[20]; 
    TMWTYPES_UCHAR input[16];
    TMWTYPES_UCHAR A[16];
    TMWTYPES_UCHAR B[16];  

    unsigned char iv[2*MAX_BLOCK_SIZE/8];
    EVP_CIPHER_CTX ctx;
    int outl;			

    n = (encryptedDataLength/8)-1;
    /* make sure R array is large enough */
    if(n > 19)
      return TMWDEFS_FALSE;

    EVP_CIPHER_CTX_init(&ctx);
    
    if(algorithm == TMWCRYPTO_ALG_KEYWRAP_AES128)
    {
      EVP_DecryptInit_ex(&ctx,EVP_get_cipherbyname("AES-128-ECB"),
        NULL, pKey->value,iv);
    }
    else
    {
      EVP_DecryptInit_ex(&ctx,EVP_get_cipherbyname("AES-256-ECB"),
        NULL, pKey->value,iv);
    }

    /* This turns padding off */
    EVP_CIPHER_CTX_set_padding(&ctx, 0);

    /* Set A= C[0]  */
    memcpy(A, pEncryptedData, 8); 
    
    /* For i=1 to n, R[i]=C[i] */ 
    memcpy(&R[1], pEncryptedData+8, encryptedDataLength-8);

    for(j=5; j>=0; j--)
    {  
      for(i=n; i>0; i--)
      { 
        /* B = AES-1(K, (A ^ t) | R[i]) where t = n*j+i */
        A[7] ^= (n*j) +i;
        memcpy(&input[0], A, 8);
        memcpy(&input[8], &R[i], 8); 
 
        EVP_DecryptUpdate(&ctx,B,&outl,input,16); /* high level openssl */

        /* The returned decrypted data would also be 16 bytes long */
     
        /* A = MSB(64, B) */
        memcpy(A, B, 8); 
          
        /* R[i] = LSB(64, B) */
        memcpy(&R[i], &B[8], 8); 
      }
    }

    EVP_DecryptFinal_ex(&ctx,A,&outl);
    EVP_CIPHER_CTX_cleanup(&ctx);

    for(i=0; i<8; i++)
     if(A[i] != 0xA6)
        return TMWDEFS_FALSE;
 
    /* For i = 1 to n, P[i] = R[i] */
    *pPlainLength = encryptedDataLength-8;
    memcpy(pPlainData, &R[1], encryptedDataLength-8);
    return TMWDEFS_TRUE;
  } 
#endif
  else if((algorithm  == TMWCRYPTO_ALG_SYM_AES128)
    ||(algorithm  == TMWCRYPTO_ALG_SYM_AES256))
  { 
    TMWTYPES_UCHAR *pPData; 
    int plainDLength;
    unsigned char iv[2*MAX_BLOCK_SIZE/8];
    EVP_CIPHER_CTX ctx; 
    EVP_CIPHER_CTX_init(&ctx);

    if(pKey->ivLength == 0)
      memset(iv,0, sizeof(iv));
    else
      memcpy(iv, pKey->iv, pKey->ivLength);

    if(algorithm == TMWCRYPTO_ALG_SYM_AES128)
      EVP_DecryptInit_ex(&ctx,EVP_get_cipherbyname("AES-128-CBC"),
        NULL, pKey->value,iv);
    else
      EVP_DecryptInit_ex(&ctx,EVP_get_cipherbyname("AES-256-CBC"),
        NULL, pKey->value,iv);

    /* This turns padding off */
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
 
    plainDLength = 0; 
    pPData = pPlainData;  

    /* high level openssl */
    if(!EVP_DecryptUpdate(&ctx, pPData, &plainDLength,
      pEncryptedData, encryptedDataLength)) 
    {
      EVP_CIPHER_CTX_cleanup(&ctx);
      return TMWDEFS_FALSE;
    } 

    pPData += plainDLength; 
    *pPlainLength = (TMWTYPES_USHORT)plainDLength;
    
    EVP_DecryptFinal_ex(&ctx, pPData, &plainDLength);
    EVP_CIPHER_CTX_cleanup(&ctx);

    *pPlainLength += (TMWTYPES_USHORT)plainDLength;
    return TMWDEFS_TRUE;
  }
  
#if TMWCNFG_SUPPORT_CRYPTO_ASYM
  /* Diffentiate 1024 and 2048 by the key size in the file */
  else if((algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP1024) 
    || (algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP2048)
    || (algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP3072))
  {   
    int keySizeInBits;
    int requiredSize;
    TMWTYPES_BOOL error;
    EVP_PKEY_CTX *pCtx;
    size_t    outlen; 
    EVP_PKEY *pEVPKey;
    FILE     *fp;
    PW_CB_DATA passworddata;
 
    /* Read private key */
    fp = fopen ((char*)pKey->value, "r");
    if (fp == NULL) 
    {
#if TMWCNFG_SUPPORT_DIAG
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
      TMWDIAG_ERROR(buf);
#endif
      return TMWDEFS_FALSE;
    }

    if(pKey->passwordLength != 0)
    {
      pKey->password[pKey->passwordLength] = '\0';
      passworddata.password = pKey->password;
    }
    else
    {
      passworddata.password = TMWDEFS_NULL;
    }
   
    pEVPKey = PEM_read_PrivateKey(fp, NULL, (pem_password_cb *)password_callback, &passworddata);
    fclose (fp);

    if (pEVPKey == NULL) { 
      /* ERR_print_errors_fp (stderr); */
      return(TMWDEFS_FALSE);
    }

    /* Check that the correct size key is being used. */
    error = TMWDEFS_FALSE;
    requiredSize = 0;
    keySizeInBits = EVP_PKEY_bits(pEVPKey);

    if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP1024)
    {
      if(keySizeInBits != 1024)
      {
        requiredSize = 1024;
        error = TMWDEFS_TRUE;
      }
    } 
    else if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP2048)
    {
      if(keySizeInBits != 2048)
      {
        requiredSize = 2048;
        error = TMWDEFS_TRUE;
      }
    }
    else if(algorithm == TMWCRYPTO_ALG_ASYM_RSAESOAEP3072)
    {
      if(keySizeInBits != 3072)
      {
        requiredSize = 3072;
        error = TMWDEFS_TRUE;
      }
    } 
    else
    {
      return TMWDEFS_FALSE;
    }

    if(error)
    {
#if TMWCNFG_SUPPORT_DIAG
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: decryptData, key %s is wrong size, expected %d bits", (char*)pKey->value, requiredSize);
      TMWDIAG_ERROR(buf); 
#endif
      return TMWDEFS_FALSE;
    }

    pCtx = EVP_PKEY_CTX_new(pEVPKey, NULL);
    if (!pCtx)
    {
      /* Error occurred */
      EVP_PKEY_free (pEVPKey);
      return TMWDEFS_FALSE;
    }

    EVP_PKEY_free (pEVPKey);

    if (EVP_PKEY_decrypt_init(pCtx) <= 0)
    {
      /* Error */
      EVP_PKEY_CTX_free(pCtx);
      return TMWDEFS_FALSE;
    }
 
    if (EVP_PKEY_CTX_set_rsa_padding(pCtx, RSA_PKCS1_OAEP_PADDING) <= 0)
    {
      /* Error */
      EVP_PKEY_CTX_free(pCtx);
      return TMWDEFS_FALSE;
    }

    /* EVP_PKEY_decrypt will check to see if outlen is big enough */
    outlen = *pPlainLength;
    if (EVP_PKEY_decrypt(pCtx, pPlainData, &outlen, pEncryptedData, encryptedDataLength) <= 0)
    {
      /* Error */
      EVP_PKEY_CTX_free(pCtx);
      return TMWDEFS_FALSE;
    }
    
    EVP_PKEY_CTX_free(pCtx);

    *pPlainLength = (TMWTYPES_USHORT)outlen;

    /* Encrypted data is outlen bytes written to buffer out */
    return TMWDEFS_TRUE;

  }  
#endif
  TMWDIAG_ERROR("TMWCRYPTO decryption failed.");
  return TMWDEFS_FALSE; 
#else
  /* Put target code here */
  TMWDIAG_ERROR("tmwcrypto: tmwcrypto_decryptData not implemented");
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(algorithm);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pEncryptedData);
  TMWTARG_UNUSED_PARAM(encryptedDataLength);
  TMWTARG_UNUSED_PARAM(pPlainData);
  TMWTARG_UNUSED_PARAM(pPlainLength);
  return TMWDEFS_FALSE; 
#endif 
}

/* function: tmwcrypto_generateNewKey */ 
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_generateNewKey(
  void              *pCryptoHandle,
  TMWCRYPTO_KEYTYPE  keyType,
  TMWTYPES_USHORT    length,
  TMWCRYPTO_KEY     *pKey)
{  
#if TMWCNFG_USE_OPENSSL 
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  RAND_bytes(pKey->value, length);
  pKey->length = length;
  pKey->passwordLength = 0;
  pKey->ivLength = 0;
  pKey->keyType = keyType;
  return(TMWDEFS_TRUE);
#elif TMWCNFG_USE_SIMULATED_CRYPTO 
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  pKey->length = length;
  pKey->passwordLength = 0;
  pKey->ivLength = 0;
  pKey->keyType = keyType;
  return(TMWDEFS_TRUE);
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyType);
  TMWTARG_UNUSED_PARAM(length);
  TMWTARG_UNUSED_PARAM(pKey);
  return(TMWDEFS_FALSE);
#endif
}

TMWTYPES_UCHAR TMWDEFS_GLOBAL tmwcrypto_getAlgorithm(
  void                *pCryptoHandle,
  TMWCRYPTO_KEYTYPE    keyType,
  void *               keyHandle)
  {
#if  TMWCNFG_USE_GATEWAY_DB
  return GTWCrypto_getAlgorithm(pCryptoHandle, keyType, keyHandle);

#elif TMWCNFG_USE_OPENSSL || TMWCNFG_USE_SIMULATED_CRYPTO
  if(keyType == TMWCRYPTO_USER_UPDATE_KEY)
  {
    TMWCRYPTO_KEY key;
    if(tmwcrypto_getKey(pCryptoHandle, keyType, keyHandle, &key))
    {
      if(key.length == 32)
        return TMWCRYPTO_ALG_KEYWRAP_AES256;
      else
        return TMWCRYPTO_ALG_KEYWRAP_AES128;
    } 
  }
  return TMWCRYPTO_ALG_UNDEFINED;
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyType);
  TMWTARG_UNUSED_PARAM(keyHandle);
  return TMWCRYPTO_ALG_UNDEFINED;
#endif
}

/* function: tmwcrypto_getKey */ 
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_getKey(
  void                *pCryptoHandle,
  TMWCRYPTO_KEYTYPE    keyType,
  void *               keyHandle,
  TMWCRYPTO_KEY       *pKey)
{

#if TMWCNFG_USE_MANAGED_SCL
  /* If TMW managed implementation (TH and .NET) is being used */
  return TMWCryptoWrapper_getKey(pCryptoHandle, keyType, keyHandle, pKey);

#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB
  int             i;
  TMWTYPES_UCHAR *pSimKey;
  TMWTYPES_USHORT keyLen;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyHandle);

  pSimKey = TMWDEFS_NULL;
  keyLen = 0;

  pKey->ivLength = 0;
  pKey->passwordLength = 0;
  pKey->keyType = keyType;

  switch(keyType)
  {
  case TMWCRYPTO_USER_UPDATE_KEY:
  case TMWCRYPTO_USER_ASYM_PUB_KEY:
  case TMWCRYPTO_USER_ASYM_PRV_KEY:
    /* see if we have a key that matches that handle */
    for(i=0; i< SIM_MAX_USERS; i++)
    { 
      if(simUser[i].handle == keyHandle)
      {
        if(keyType == TMWCRYPTO_USER_UPDATE_KEY)
        {
          pSimKey = (TMWTYPES_UCHAR*)&simUser[i].key;
          keyLen = simUser[i].keyLen;
        }
        else if(keyType == TMWCRYPTO_USER_ASYM_PUB_KEY)
        {
          pSimKey = (TMWTYPES_UCHAR*)&simUser[i].asymPubKey;
          keyLen = simUser[i].asymPubKeyLen;
        }
        else if(keyType == TMWCRYPTO_USER_ASYM_PRV_KEY)
        {
          pSimKey = (TMWTYPES_UCHAR*)&simUser[i].asymPrvKey;
          keyLen = simUser[i].asymPrvKeyLen;
        }
        break;
      }
    } 
    
    if(pSimKey == TMWDEFS_NULL)
      return(TMWDEFS_FALSE);

    break;

  case TMWCRYPTO_OS_ASYM_PUB_KEY: 
    pSimKey = (TMWTYPES_UCHAR*)simOSAsymPubKey;
    keyLen = simOSAsymPubKeyLen;
    break;
 
  case TMWCRYPTO_OS_ASYM_PRV_KEY: 
    pSimKey = (TMWTYPES_UCHAR*)simOSAsymPrvKey;
    keyLen = simOSAsymPrvKeyLen;
    break;

  case TMWCRYPTO_AUTH_CERT_SYM_KEY:
    pSimKey = (TMWTYPES_UCHAR*)simAuthCertKey;
    keyLen = simAuthCertKeyLen;
    break;
   
  case TMWCRYPTO_AUTH_ASYM_PRV_KEY:
    pSimKey = (TMWTYPES_UCHAR*)simAuthAsymPrvKey; 
    keyLen = simAuthAsymPrvKeyLen;
    break;

  case TMWCRYPTO_AUTH_ASYM_PUB_KEY:
    pSimKey = (TMWTYPES_UCHAR*)simAuthAsymPubKey; 
    keyLen = simAuthAsymPubKeyLen;
    break;

  default:
    return TMWDEFS_FALSE;
  }

  memcpy(pKey->value, pSimKey, keyLen);
  /* Null terminate this. Cannot use strncpy because there could be a zero in the key */
  pKey->value[keyLen] = 0;
  pKey->length = keyLen;
  return(TMWDEFS_TRUE);
#elif  TMWCNFG_USE_GATEWAY_DB
  return GTWCryptoWrapper_getKey(pCryptoHandle, keyType, keyHandle, pKey);
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyType);
  TMWTARG_UNUSED_PARAM(keyHandle);
  TMWTARG_UNUSED_PARAM(pKey);
  return(TMWDEFS_FALSE);
#endif
}

/* function: tmwcrypto_getKeyData */ 
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_getKeyData(
  void                *pCryptoHandle,
  TMWCRYPTO_KEY       *pKey,
  void				        *pKeyHandle,
  TMWTYPES_UCHAR      *pKeyData,
  TMWTYPES_USHORT     *pKeyLength)
{
#if TMWCNFG_USE_OPENSSL
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

#if TMWCNFG_SUPPORT_CRYPTO_ASYM
  if(pKey->keyType == TMWCRYPTO_USER_ASYM_PUB_KEY)
  { 
    /*  From Jan 6 2011 DNP3 Tech SA Teleconference TC11-01-06-SA Minutes.
     *  The public key should be an octet by octet copy of the
     *  SubjectPublicKeyInfo field from the X509 certificate (RFC 5280).
     */
    EVP_PKEY      *pEVPKey;
    X509          *x509;
    FILE          *fp;
    int            len;
    unsigned char *pp = NULL;  
     
    fp = fopen ((char*)pKey->value, "r");
    if (fp == NULL) 
    {
#if TMWCNFG_SUPPORT_DIAG
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
      TMWDIAG_ERROR(buf);
#endif
      return TMWDEFS_FALSE;
    }

    /* This works if there was a BEGIN CERTIFICATE in file. */
    x509  = PEM_read_X509(fp, NULL, NULL, NULL);
    if (x509 != NULL)
    {
      /* Get public key */
      pEVPKey = X509_get_pubkey(x509);
      X509_free(x509);
      fclose(fp);
    }
    else
    {
      fp = freopen((char*)pKey->value, "r", fp);
      if (fp == NULL)
        return TMWDEFS_FALSE;

      /* This works if there is both a private and public key in the file */
      pEVPKey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
      if (pEVPKey == NULL) 
      {
        fp = freopen((char*)pKey->value, "r", fp);
        if (fp == NULL)
          return TMWDEFS_FALSE;

        /* This works if there is a BEGIN PUBLIC KEY in file */
        /* EVP_PKEY *PEM_read_PUBKEY(fp, EVP_PKEY **x, pem_password_cb *cb, void *u); */
        pEVPKey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
      }

      fclose(fp);

      if (pEVPKey == NULL) {
        /* ERR_print_errors_fp (stderr); */
        return(TMWDEFS_FALSE);
      }
    }
    
    /* This will allocate pp, 
     * I think I could point pp at my memory also.
     */
    len = i2d_PUBKEY(pEVPKey, &pp);
    
    EVP_PKEY_free (pEVPKey);

    /* Make sure there is enough room for the key data */
    if(len <= *pKeyLength)
    {
      memcpy(pKeyData, pp, len);
      *pKeyLength = (TMWTYPES_USHORT)len;
      return TMWDEFS_TRUE;
    }
    return TMWDEFS_FALSE;
  } 
#endif
#endif 

#if TMWCNFG_USE_MANAGED_SCL
  /* If TMW managed implementation (TH and .NET) is being used and
   * it was not a TMWCRYPTO_USER_ASYM_PUB_KEY handled by OpenSSL above 
   */
  return TMWCryptoWrapper_getKeyData(pCryptoHandle, pKey, pKeyHandle, pKeyData, pKeyLength);

#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB
  TMWTARG_UNUSED_PARAM(pKeyHandle);
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  /* Just copy the contents of the key value field into keydata as a simulation */
  if(pKey->length <= *pKeyLength)
  {
    memcpy(pKeyData, pKey->value, pKey->length);
    *pKeyLength = pKey->length;
    return TMWDEFS_TRUE;
  }
  return TMWDEFS_FALSE;

#elif TMWCNFG_USE_GATEWAY_DB
  return GTWCryptoWrapper_getKeyData(pCryptoHandle, pKey, pKeyHandle, pKeyData, pKeyLength);
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 
  TMWTARG_UNUSED_PARAM(pKey); 
  TMWTARG_UNUSED_PARAM(pKeyData); 
  TMWTARG_UNUSED_PARAM(pKeyLength);  
  TMWTARG_UNUSED_PARAM(pKeyHandle);
  return TMWDEFS_FALSE;
#endif
} 

/* function: tmwcrypto_setKeyData */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_setKeyData(
  void                *pCryptoHandle,
  TMWCRYPTO_KEYTYPE    keyType,
  void *               keyHandle,
  TMWTYPES_UCHAR      *pKeyData,
  TMWTYPES_USHORT      keyLength)
{
#if TMWCNFG_USE_OPENSSL
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 

#if TMWCNFG_SUPPORT_CRYPTO_ASYM
  if(keyType == TMWCRYPTO_USER_ASYM_PUB_KEY)
  {
    int       ret;
    FILE     *fp;
    EVP_PKEY *pEVPKey;
    char fileName[128];

    /* decode this and put it in an EVP_PKEY and write it to a file */
	  pEVPKey = d2i_PUBKEY(NULL, (const unsigned char **)&pKeyData, keyLength);

	  if(pEVPKey != NULL)
    {
      tmwtarg_snprintf(fileName, 128, "TMWUserPublicKey%d.pem", keyHandle);

      fp = fopen (fileName, "w");
      if (fp == NULL)  
      {
#if TMWCNFG_SUPPORT_DIAG
        char buf[128];
        tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)fileName);
        TMWDIAG_ERROR(buf);
#endif
        EVP_PKEY_free (pEVPKey);
        return TMWDEFS_FALSE;
      }
      
      ret = PEM_write_PUBKEY(fp, pEVPKey);
      if(ret == 0)
      {
        /* Error */
        fclose (fp); 
        EVP_PKEY_free (pEVPKey);
        return TMWDEFS_FALSE;
      }

      EVP_PKEY_free (pEVPKey);
      fclose (fp); 

#if TMWCNFG_USE_MANAGED_SCL
      /* If TMW managed implementation (TH and .NET) is being used */
      /* Store the name of the file created above (that contains the public key) in the database */
      return TMWCryptoWrapper_setKeyData(pCryptoHandle, keyType, keyHandle, (TMWTYPES_UCHAR*)fileName, (TMWTYPES_USHORT)strlen(fileName));
#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB 
      {
        int i;
        int index = -1;

        /* find an entry that matches or find an unused entry */
        for(i=0; i< SIM_MAX_USERS; i++)
        { 
          if(simUser[i].handle == keyHandle)
          {
            index = i;
            break;
          }
          else if((index == -1) && (simUser[i].handle == 0))
          {
            index = i;
          }
        }

        if(index != -1)
        {
          STRCPY((char *)simUser[index].asymPubKey, TMWCRYPTO_MAX_KEY_LENGTH, fileName);
          simUser[index].asymPubKeyLen = (TMWTYPES_USHORT)strlen(fileName);
          simUser[index].handle = keyHandle;
          return TMWDEFS_TRUE;
        } 

        return(TMWDEFS_FALSE);
      }
#else
    return TMWDEFS_TRUE;
#endif
    }
    else
    {
      return TMWDEFS_FALSE;
    }
  }
#endif
#endif

#if TMWCNFG_USE_MANAGED_SCL
  /* If TMW managed implementation (TH and .NET) is being used */
  return TMWCryptoWrapper_setKeyData(pCryptoHandle, keyType, keyHandle, pKeyData, keyLength);

#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB
  {
  TMWTYPES_UCHAR *pSimKey = TMWDEFS_NULL;
  TMWTYPES_USHORT *pKeyLen = TMWDEFS_NULL;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  switch(keyType)
  {
  case TMWCRYPTO_USER_UPDATE_KEY:
  case TMWCRYPTO_USER_ASYM_PUB_KEY:
  case TMWCRYPTO_USER_ASYM_PRV_KEY:
    { 
    int i;
    int index = -1;

    /* find an entry that matches or find an unused entry */
    for(i=0; i< SIM_MAX_USERS; i++)
    { 
      if(simUser[i].handle == keyHandle)
      {
        index = i;
        break;
      }
      else if((index == -1) && (simUser[i].handle == 0))
      {
        index = i;
      }
    }

    if(index != -1)
    {
      if(keyType == TMWCRYPTO_USER_UPDATE_KEY)
      {
        pSimKey = (TMWTYPES_UCHAR*)&simUser[index].key;
        pKeyLen = &simUser[index].keyLen;

        /* retain old user update key in case commitKey says to revert back */
        if(simUser[index].keyLen>0)
        {
          memcpy(simUser[index].oldRetainedKey, simUser[index].key, simUser[index].keyLen);
          simUser[index].oldRetainedKeyLen = simUser[index].keyLen;
        }
      }
      else if(keyType == TMWCRYPTO_USER_ASYM_PUB_KEY)
      {
        pSimKey = (TMWTYPES_UCHAR*)&simUser[index].asymPubKey;
        pKeyLen = &simUser[index].asymPubKeyLen;
      }
      else if(keyType == TMWCRYPTO_USER_ASYM_PRV_KEY)
      {
        pSimKey = (TMWTYPES_UCHAR*)&simUser[index].asymPrvKey;
        pKeyLen = &simUser[index].asymPrvKeyLen;
      }
    } 
 
    if(pSimKey == TMWDEFS_NULL)
      return(TMWDEFS_FALSE);
    
    simUser[index].handle = keyHandle;
    }
    break;

  case TMWCRYPTO_OS_ASYM_PUB_KEY: 
    pSimKey = (TMWTYPES_UCHAR*)simOSAsymPubKey;
    pKeyLen = &simOSAsymPubKeyLen;
    break;

  case TMWCRYPTO_OS_ASYM_PRV_KEY: 
    pSimKey = (TMWTYPES_UCHAR*)simOSAsymPrvKey;
    pKeyLen = &simOSAsymPrvKeyLen;  
    break;

  case TMWCRYPTO_AUTH_CERT_SYM_KEY:
    pSimKey = (TMWTYPES_UCHAR*)simAuthCertKey;
    pKeyLen = &simAuthCertKeyLen; 
    break;

  case TMWCRYPTO_AUTH_ASYM_PRV_KEY:
    pSimKey = (TMWTYPES_UCHAR*)simAuthAsymPrvKey;
    pKeyLen = &simAuthAsymPrvKeyLen; 
    break;

  case TMWCRYPTO_AUTH_ASYM_PUB_KEY:   
    pSimKey = (TMWTYPES_UCHAR*)simAuthAsymPubKey;
    pKeyLen = &simAuthAsymPubKeyLen;
    break;

  default:
    return TMWDEFS_FALSE;
  }

  memcpy(pSimKey, pKeyData, keyLength);
  *pKeyLen = keyLength;
  return TMWDEFS_TRUE;
  }
#else

  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyType);
  TMWTARG_UNUSED_PARAM(keyHandle);
  TMWTARG_UNUSED_PARAM(pKeyData);
  TMWTARG_UNUSED_PARAM(keyLength);
  return TMWDEFS_FALSE;
#endif
} 

/* function: tmwcrypto_commitKey */
/* called only on outstation */
void TMWDEFS_GLOBAL tmwcrypto_commitKey(
  void                *pCryptoHandle,
  TMWCRYPTO_KEYTYPE    keyType,
  void *               keyHandle,
  TMWTYPES_BOOL        commit)
{
#if TMWCNFG_USE_MANAGED_SCL
  /* If TMW managed implementation (TH and .NET) is being used */
  TMWCryptoWrapper_commitKey(pCryptoHandle, keyType, keyHandle, commit);

#elif TMWCNFG_USE_SIMULATED_CRYPTO_DB
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  /* Only User Update Key requires this commit==false functionality */
  if((keyType == TMWCRYPTO_USER_UPDATE_KEY)
    && (commit == TMWDEFS_FALSE))
  {
    /* find the entry that matches */
    int index;
    for(index=0; index< SIM_MAX_USERS; index++)
    { 
      if(simUser[index].handle == keyHandle)
      { 
        /* revert back to old retained user update key */
        if(simUser[index].oldRetainedKeyLen>0)
        {
          memcpy(simUser[index].key, simUser[index].oldRetainedKey, simUser[index].oldRetainedKeyLen);
          simUser[index].keyLen = simUser[index].oldRetainedKeyLen;
        }
      }
    }
  }

#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyType);
  TMWTARG_UNUSED_PARAM(keyHandle);
  TMWTARG_UNUSED_PARAM(commit);
#endif
}

/* function: tmwcrypto_setSessionKeyData */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_setSessionKeyData(
    void                *pCryptoHandle,
    TMWCRYPTO_KEYTYPE    keyType,
    TMWTYPES_UCHAR      *pKeyData,
    TMWTYPES_USHORT      keyLength,
    TMWCRYPTO_KEY       *pKey)
{
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  /* If you redefine TMWCRYPTO_KEY you should replace
   * this code with your own.
   */
  if(keyLength > TMWCRYPTO_MAX_KEY_LENGTH)
    return TMWDEFS_FALSE;

  memcpy(pKey->value, pKeyData, keyLength);
  pKey->length = keyLength;
  pKey->keyType = keyType;
  return TMWDEFS_TRUE;
}

/* function: tmwcrypto_getCertificate */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_getCertificate(
  void            *pCryptoHandle,
  void            *userNameDbHandle,
  TMWTYPES_UCHAR  *pCertData,
  TMWTYPES_USHORT *pCertDataLength,
  TMWTYPES_UCHAR  *pCertType)
{ 
  TMWTYPES_UCHAR fileName[128];
#if TMWCNFG_USE_MANAGED_SCL
  TMWTYPES_BOOL fileNameFound;
  /* If TMW managed implementation (TH and .NET) is being used */
  fileNameFound = TMWCryptoWrapper_getCertFileName(pCryptoHandle, userNameDbHandle, fileName);
#else
  {
  /* Put target code here */
  long handle;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  handle = (long)userNameDbHandle;
  tmwtarg_snprintf((TMWTYPES_CHAR *)fileName, 128, "TMWUserCertficate%ld.pem", handle);
  }
#endif

#if TMWCNFG_USE_OPENSSL
  {
    X509 *cert;
    FILE *fp;
    fp = fopen((const char *)fileName, "r");
  
    if(fp)
    {
      /* get IEC 62351-8 Certificate from file */
	    cert = PEM_read_X509(fp, NULL, 0, NULL);
      fclose (fp);
      if (cert != NULL) 
      {
        int len;
        unsigned char *d = NULL;  
		    len=i2d_X509(cert,&d);
        memcpy(pCertData, d, len);
        *pCertDataLength = (TMWTYPES_USHORT)len; 
        *pCertType = 1;  /* determine this from data in file, ID Certificate or Attribute Certificate */
        return TMWDEFS_TRUE;
      }
    }
#if TMWCNFG_SUPPORT_DIAG
    else
    {
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open certificate file, %s", fileName);
      TMWDIAG_ERROR(buf);
    }
#endif
  }
  return TMWDEFS_FALSE;
#else
  /* Put target code here */
  TMWTARG_UNUSED_PARAM(pCryptoHandle); 
  TMWTARG_UNUSED_PARAM(userNameDbHandle);
  TMWTARG_UNUSED_PARAM(pCertData); 
  TMWTARG_UNUSED_PARAM(pCertDataLength);
  TMWTARG_UNUSED_PARAM(pCertType);
  return TMWDEFS_FALSE;
#endif
}

#if TMWCNFG_USE_OPENSSL
// This callback will allow us to see the certificate.
static int  verify_cb(int ok, X509_STORE_CTX *ctx)
{
  if (!ok)
  {
    // check the error code and current cert
    //X509 *currentCert = X509_STORE_CTX_get_current_cert(ctx);
    int certError = X509_STORE_CTX_get_error(ctx);
    TMWTARG_UNUSED_PARAM(certError);
    //int depth = X509_STORE_CTX_get_error_depth(ctx);
    // certError 7 is X509_V_ERR_CERT_SIGNATURE_FAILURE
  }
  return(ok);
} 
#endif

/* function: tmwcrypto_verifyCertificate */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_verifyCertificate(
  void            *pCryptoHandle,
  TMWCRYPTO_KEY   *pKey,
  TMWTYPES_UCHAR  *pCertData,
  TMWTYPES_USHORT  certDataLength)
{
#if TMWCNFG_USE_OPENSSL
  X509 *pCert;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

  pCert = d2i_X509(NULL, (const unsigned char **)&pCertData, certDataLength);
  if (pCert != NULL)
  { 
    X509 *cacert;
    FILE *cafp;
     
    cafp = fopen((char*)pKey->value, "r");
    if (cafp)
    {
      cacert = PEM_read_X509(cafp, NULL, 0, NULL);
      fclose(cafp);
      if (cacert != NULL)
      {
        int ret;
        X509_STORE *store;
        X509_STORE_CTX *ctx;

        store = X509_STORE_new();
        X509_STORE_set_verify_cb(store, verify_cb);
        X509_STORE_add_cert(store, cacert);

        ctx = X509_STORE_CTX_new();
        X509_STORE_CTX_init(ctx, store, pCert, NULL);

        ret = X509_verify_cert(ctx);

        if (ret > 0)
          return TMWDEFS_TRUE;
      }
#if TMWCNFG_SUPPORT_DIAG
      else
      {
        char buf[128];
        tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open key file, %s", (char*)pKey->value);
        TMWDIAG_ERROR(buf);
      }
#endif
    }
  }
#else
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(pKey);
  TMWTARG_UNUSED_PARAM(pCertData);
  TMWTARG_UNUSED_PARAM(certDataLength);
#endif
  return TMWDEFS_FALSE;
}

/* function: tmwcrypto_putCertificate */
TMWTYPES_BOOL TMWDEFS_GLOBAL tmwcrypto_putCertificate(
  void            *pCryptoHandle,
  void *           keyHandle,
  TMWTYPES_UCHAR  *pCertData,
  TMWTYPES_USHORT  certDataLength)
{
#if TMWCNFG_USE_OPENSSL
	X509 *pCert;
  TMWTARG_UNUSED_PARAM(pCryptoHandle);

	pCert = d2i_X509(NULL, (const unsigned char **)&pCertData, certDataLength);
  if(pCert != NULL)
  { 
    FILE *fp;
    struct X509_name_st *subj;
    int i;
    int  ret;
    char fileName[128];
    unsigned char *str;
    char *subj1;

    subj =	X509_get_subject_name(pCert);
    for (i = 0; i < X509_NAME_entry_count(subj); i++) {
		  X509_NAME_ENTRY *e = X509_NAME_get_entry(subj, i);
		  ASN1_STRING *d = X509_NAME_ENTRY_get_data(e);
	  	str = ASN1_STRING_data(d);	
	  }

	  subj1 = X509_NAME_oneline(X509_get_subject_name(pCert), NULL, 0);

    /* to prevent warnings */
    TMWTARG_UNUSED_PARAM(subj1);
    TMWTARG_UNUSED_PARAM(str);

    tmwtarg_snprintf(fileName, 128, "TMWUserCertificate%d.pem", keyHandle);

    fp = fopen (fileName, "w");
    if (fp == NULL)  
    {
#if TMWCNFG_SUPPORT_DIAG
      char buf[128];
      tmwtarg_snprintf(buf, 128, "tmwcrypto: could not open certificate file, %s", fileName);
      TMWDIAG_ERROR(buf);
#endif
	    X509_free(pCert);
      return TMWDEFS_FALSE;
    }
       
	  ret = PEM_write_X509(fp, pCert);
    if(ret == 0)
	  {
	    /* Error */
      fclose (fp); 
	    X509_free(pCert);
      return TMWDEFS_FALSE;
	  } 
    
	  X509_free(pCert);
    fclose (fp); 
    
#if TMWCNFG_USE_MANAGED_SCL
      /* If TMW managed implementation (TH and .NET) is being used */
      /* Store the name of the file created above (that contains the X509 certificagte) in the database */
      return TMWCryptoWrapper_setKeyData(pCryptoHandle, TMWCRYPTO_USER_X509CERT, keyHandle, (TMWTYPES_UCHAR*)fileName, (TMWTYPES_USHORT)strlen(fileName));
#else
    return TMWDEFS_TRUE;
#endif
  }
#else
  TMWTARG_UNUSED_PARAM(pCryptoHandle);
  TMWTARG_UNUSED_PARAM(keyHandle);
  TMWTARG_UNUSED_PARAM(pCertData);
  TMWTARG_UNUSED_PARAM(certDataLength);
#endif

  return TMWDEFS_FALSE;
}


#if TMWCRYPTO_TESTING

#define TESTBUFSIZE 512
/* These test vectors are from rfc3394 AES Key Wrap Algorithm */

/* 4.1 128 bits of Key Data with a 128-bit KEK (first 16 bytes) */
/* 4.6 256 bits of Key Data with a 256-bit KEK (all 32 bytes)   */
static TMWTYPES_UCHAR kek[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};
static TMWTYPES_UCHAR keydata[] = {
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

/* Output ciphertext from 4.1 Wrap 128 bits of Key Data with a 128-bit KEK*/
static TMWTYPES_UCHAR e1[] = {
  0x1f, 0xa6, 0x8b, 0x0a, 0x81, 0x12, 0xb4, 0x47,
  0xae, 0xf3, 0x4b, 0xd8, 0xfb, 0x5a, 0x7b, 0x82,
  0x9d, 0x3e, 0x86, 0x23, 0x71, 0xd2, 0xcf, 0xe5
};

/* Output ciphertext from 4.6 Wrap 256 bits of Key Data with a 256-bit KEK */
static TMWTYPES_UCHAR e6[] = {
  0x28, 0xc9, 0xf4, 0x04, 0xc4, 0xb8, 0x10, 0xf4,
  0xcb, 0xcc, 0xb3, 0x5c, 0xfb, 0x87, 0xf8, 0x26,
  0x3f, 0x57, 0x86, 0xe2, 0xd8, 0x0e, 0xd3, 0x26,
  0xcb, 0xc7, 0xf0, 0xe7, 0x1a, 0x99, 0xf4, 0x3b,
  0xfb, 0x98, 0x8b, 0x9b, 0x7a, 0x02, 0xdd, 0x21
};

/* These test vectors are from RFC3602 AES-CBC */
/* case 1: Encrypting 16 bytes (1 block) using AES-CBC with 128-bit key */
/* case 2: Encrypting 32 bytes (2 blocks) using AES-CBC with 128-bit key */
static TMWTYPES_UCHAR m_ivTestCase1[] = {
  0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30,
  0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41
};     
static TMWTYPES_UCHAR m_ivTestCase2[] = {
  0x56, 0x2e, 0x17, 0x99, 0x6d, 0x09, 0x3d, 0x28,
  0xdd, 0xb3, 0xba, 0x69, 0x5a, 0x2e, 0x6f, 0x58
};        

static TMWTYPES_UCHAR m_keyTestCase1[] = {
  0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b,
  0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06
};
static TMWTYPES_UCHAR m_keyTestCase2[]= {
  0xc2, 0x86, 0x69, 0x6d, 0x88, 0x7c, 0x9a, 0xa0,
  0x61, 0x1b, 0xbb, 0x3e, 0x20, 0x25, 0xa4, 0x5a
};

static TMWTYPES_UCHAR *m_plainTextTestCase1 = 
(unsigned char*)"Single block msg";

static TMWTYPES_UCHAR m_plainTextTestCase2[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

static TMWTYPES_UCHAR m_encryptTextTestCase1[] = {
  0xe3, 0x53, 0x77, 0x9c, 0x10, 0x79, 0xae, 0xb8,
  0x27, 0x08, 0x94, 0x2d, 0xbe, 0x77, 0x18, 0x1a
};
static TMWTYPES_UCHAR m_encryptTextTestCase2[] = {
  0xd2, 0x96, 0xcd, 0x94, 0xc2, 0xcc, 0xcf, 0x8a,
  0x3a, 0x86, 0x30, 0x28, 0xb5, 0xe1, 0xdc, 0x0a,
  0x75, 0x86, 0x60, 0x2d, 0x25, 0x3c, 0xff, 0xf9,
  0x1b, 0x82, 0x66, 0xbe, 0xa6, 0xd6, 0x1a, 0xb1
};

/* This is not a standard test vector */
static TMWTYPES_UCHAR* m_plainTextTestCase3 = 
(unsigned char*)"This is a really big message of random size to see if it is possible to encrypt and decrypt and get back the same message. the quick brown fox jumped over the lazy dog.";

#if TMWCNFG_SUPPORT_CRYPTO_AESGMAC
/* There are test vectors in OpenSSL crypto/modes/gcm128.c, but these are really for encryption/decryption */
static const TMWTYPES_UCHAR	 K4[]=  {
  0xfe,0xff,0xe9,0x92,0x86,0x65,0x73,0x1c,0x6d,0x6a,0x8f,0x94,0x67,0x30,0x83,0x08
};
static const TMWTYPES_UCHAR	 IV4[]= {
  0xca,0xfe,0xba,0xbe,0xfa,0xce,0xdb,0xad,0xde,0xca,0xf8,0x88
};
static const TMWTYPES_UCHAR	P4[]=  {
  0xd9,0x31,0x32,0x25,0xf8,0x84,0x06,0xe5,0xa5,0x59,0x09,0xc5,0xaf,0xf5,0x26,0x9a,
  0x86,0xa7,0xa9,0x53,0x15,0x34,0xf7,0xda,0x2e,0x4c,0x30,0x3d,0x8a,0x31,0x8a,0x72,
	0x1c,0x3c,0x0c,0x95,0x95,0x68,0x09,0x53,0x2f,0xcf,0x0e,0x24,0x49,0xa6,0xb5,0x25,
	0xb1,0x6a,0xed,0xf5,0xaa,0x0d,0xe6,0x57,0xba,0x63,0x7b,0x39
};		
static const TMWTYPES_UCHAR	T4[]=  {
    0x5b,0xc9,0x4f,0xbc,0x32,0x21,0xa5,0xdb,0x94,0xfa,0xe9,0x5a,0xe7,0x12,0x1a,0x47
};

/* This is not in the gcm128.c test vectors. But this is the result of the hash. */
static const TMWTYPES_UCHAR HASH4[] = { 0xff,0xb4,0x00,0xe7,0xfd,0x5a,0xd8,0x58,0x3f,0x1e,0x10,0xc8,0xfa };

#endif

#if TMWCRYPTO_ASYMTESTING && TMWCNFG_SUPPORT_CRYPTO_ASYM
static unsigned char testData[] = "This is a test to see if I can sign data.";
#endif

static TMWTYPES_BOOL TMWDEFS_LOCAL _testCrypto(void *pUserHandle)
{
  /* Testing */
  TMWTYPES_CHAR *pData;
  TMWCRYPTO_KEY key;
  TMWTYPES_UCHAR encryptBuf[TESTBUFSIZE];
  TMWTYPES_USHORT encryptLen;
  TMWTYPES_UCHAR plainBuf[TESTBUFSIZE];
  TMWTYPES_USHORT plainLen;
  TMWTYPES_BOOL success;
  TMWTYPES_BOOL testFailed;

  testFailed = TMWDEFS_FALSE;

  /* Test 128 bit AES Key Wrap using test vectors from RFC3394 */
  key.length = 16;
  memcpy(key.value, kek, key.length);
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES128,
     &key,
      (TMWTYPES_UCHAR*)keydata, 16, encryptBuf, &encryptLen))
  { 
    /* encrypted data should match e1 above */
    if((encryptLen == 24) && (!memcmp(e1, encryptBuf, encryptLen)))
    {
      plainLen = TESTBUFSIZE;
      if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES128,
         &key,
         encryptBuf, encryptLen, plainBuf, &plainLen))
      { 
        if((plainLen == 16) && (!memcmp(plainBuf, keydata, plainLen)))
          success = TMWDEFS_TRUE;
      } 
    }
  } 

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 1"); 
  } 

 
  /* Test 256 bit AES Key Wrap using test vectors from RFC3394 */
  key.length = 32;
  memcpy(key.value, kek, key.length);
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES256,
     &key,
      (TMWTYPES_UCHAR*)keydata, 32, encryptBuf, &encryptLen))
  { 
    /* encrypted data should match e6 above */
    if((encryptLen == 40) && (!memcmp(e6, encryptBuf, encryptLen)))
    {
      plainLen = TESTBUFSIZE;
      if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES256,
         &key,
         encryptBuf, encryptLen, plainBuf, &plainLen))
      { 
        if((plainLen == 32) &&(!memcmp(plainBuf, keydata, plainLen)))
          success = TMWDEFS_TRUE;
      } 
    }
  } 

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 2"); 
  } 


  /* This non standard test data will require padding */
  memcpy(key.value, "0123456789abcdef", 16);
  key.length = 16;
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  pData = "012345678901234567890123457890";
  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES128,
     &key,
     (TMWTYPES_UCHAR*)pData, (TMWTYPES_USHORT)strlen(pData), encryptBuf, &encryptLen))
  { 
    plainLen = TESTBUFSIZE;
    if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_KEYWRAP_AES128,
       &key,
       encryptBuf, encryptLen, plainBuf, &plainLen))
    {
      if(plainLen >= strlen(pData))
      {
        if(!memcmp(plainBuf, pData, strlen(pData)))
          success = TMWDEFS_TRUE;
      }
    } 
  } 

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 3");
  } 

  memcpy(key.value, m_keyTestCase1, 16);
  key.length = 16;
  memcpy(key.iv, m_ivTestCase1, 16);
  key.ivLength = 16;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
     &key,
     m_plainTextTestCase1, 16, encryptBuf, &encryptLen))
  {
    if(encryptLen == 16)
    {
      if(!memcmp( encryptBuf, m_encryptTextTestCase1, 16))
        success = TMWDEFS_TRUE;
    }  

    plainLen = TESTBUFSIZE;
    if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
       &key,
       encryptBuf, encryptLen, plainBuf, &plainLen))
    {
      if(plainLen == 16)
      {
        if(!memcmp( plainBuf, m_plainTextTestCase1, 16))
          success = TMWDEFS_TRUE;
      }
    }
  }
 
  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 4"); 
  } 

  memcpy(key.value, m_keyTestCase2, 16);
  key.length = 16;
  memcpy(key.iv, m_ivTestCase2, 16);
  key.ivLength = 16;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
     &key,
     m_plainTextTestCase2, 32, encryptBuf, &encryptLen))
  {
 
    if(encryptLen == 32)
    {
      if(!memcmp( encryptBuf, m_encryptTextTestCase2, 16))
        success = TMWDEFS_TRUE;
    } 

    plainLen = TESTBUFSIZE;
    if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
       &key,
       encryptBuf, encryptLen, plainBuf, &plainLen))
    {
      if(plainLen == 32)
      {
        if(!memcmp( plainBuf, m_plainTextTestCase2, 32))
          success = TMWDEFS_TRUE;
      } 
    } 
  }
  
  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 5");
  } 


  memcpy(key.value, m_keyTestCase1, 16);
  key.length = 16;
  memcpy(key.iv, m_ivTestCase1, 16);
  key.ivLength = 16;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
     &key,
     m_plainTextTestCase3, (TMWTYPES_USHORT)strlen((char*)m_plainTextTestCase3), encryptBuf, &encryptLen))
  {
  
    plainLen = TESTBUFSIZE;
    if(tmwcrypto_decryptData(pUserHandle, TMWCRYPTO_ALG_SYM_AES128,
       &key,
       encryptBuf, encryptLen, plainBuf, &plainLen))
    {

      /* this allows for padding */
      if(plainLen >= strlen((char*)m_plainTextTestCase3))
      {
        if(!memcmp( plainBuf, m_plainTextTestCase3, strlen((char*)m_plainTextTestCase3)))
          success = TMWDEFS_TRUE;
      }
    }
  }

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto test 6"); 
  } 


#if TMWCRYPTO_ASYMTESTING && TMWCNFG_SUPPORT_CRYPTO_ASYM
 /* This plain data is 0xa7 long, it fails with RSA1024 key, because data is too long for key
  * but works with RSA2048 
  */ 
  key.length = (TMWTYPES_USHORT)strlen("TMWTestOSRsa2048PubKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestOSRsa2048PubKey.pem"); 
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_encryptData(pUserHandle, TMWCRYPTO_ALG_ASYM_RSAESOAEP2048,
   &key,
   m_plainTextTestCase3, (TMWTYPES_USHORT)strlen((char*)m_plainTextTestCase3),
   encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestOSRsa2048PrvKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestOSRsa2048PrvKey.pem");

    /* Indicate that plainbuf is large enough to take output.*/
    plainLen = TESTBUFSIZE;
    if(tmwcrypto_decryptData(pUserHandle,TMWCRYPTO_ALG_ASYM_RSAESOAEP2048,
       &key,
       encryptBuf, encryptLen, plainBuf, &plainLen))
    {

      /* this allows for padding */
      if(plainLen >= strlen((char*)m_plainTextTestCase3))
      {
        if(!memcmp( plainBuf, m_plainTextTestCase3, (TMWTYPES_USHORT)strlen((char*)m_plainTextTestCase3)))
          success = TMWDEFS_TRUE;
      }
    }
  }

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 7");
  } 

  key.length = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa1024PrvKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestAuthorityDsa1024PrvKey.pem");
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_genDigitalSignature(pUserHandle,TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1, &key,
    testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa1024PubKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestAuthorityDsa1024PubKey.pem"); 

    if(tmwcrypto_verifySignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1, &key,
      testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, encryptLen))
    {
      success = TMWDEFS_TRUE;
    }
  }

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 8"); 
  } 


  key.length = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa2048PrvKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestAuthorityDsa2048PrvKey.pem"); 
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_genDigitalSignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256, &key,
    testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestAuthorityDsa2048PubKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestAuthorityDsa2048PubKey.pem"); 

    if(tmwcrypto_verifySignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256, &key,
      testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, encryptLen))
    { 
      success = TMWDEFS_TRUE;
    }
  }

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 9"); 
  } 

  key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa1024PrvKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa1024PrvKey.pem");
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_genDigitalSignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1, &key,
    testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa1024PubKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa1024PubKey.pem"); 

    if( tmwcrypto_verifySignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_1024_SHA1, &key,
      testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, encryptLen))
    { 
      success = TMWDEFS_TRUE;
    }
  }

  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 10"); 
  } 

  key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa2048PrvKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa2048PrvKey.pem"); 
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_genDigitalSignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256, &key,
    testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa2048PubKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa2048PubKey.pem"); 

    if(tmwcrypto_verifySignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_2048_SHA256, &key,
      testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, encryptLen))
    { 
      success = TMWDEFS_TRUE;
    }
  }
  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 11");
  } 

  key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa3072PrvKey.pem");
  STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa3072PrvKey.pem"); 
  key.ivLength = 0;
  key.passwordLength = 0;
  encryptLen = TESTBUFSIZE;
  success = TMWDEFS_FALSE;

  if(tmwcrypto_genDigitalSignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256, &key,
    testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, &encryptLen))
  {
    key.length = (TMWTYPES_USHORT)strlen("TMWTestUserDsa3072PubKey.pem");
    STRCPY((char*)key.value, TMWCRYPTO_MAX_KEY_LENGTH, "TMWTestUserDsa3072PubKey.pem"); 

    if(tmwcrypto_verifySignature(pUserHandle, TMWCRYPTO_ALG_SIGN_DSA_3072_SHA256, &key,
      testData, (TMWTYPES_USHORT)strlen((char *)testData), encryptBuf, encryptLen))
    { 
      success = TMWDEFS_TRUE;
    }
  }
  if(!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto asymmetric test 12");
  } 
#endif


#if TMWCNFG_SUPPORT_CRYPTO_AESGMAC
  /* AES-GMAC Test case 1 */
  memcpy(key.value, K4, sizeof(K4));
  key.length = sizeof(K4);
  memcpy(key.iv, IV4, sizeof(IV4));
  key.ivLength = sizeof(IV4);
  key.passwordLength = 0;
  success = TMWDEFS_FALSE;
 
  if(tmwcrypto_MACValue(pUserHandle, TMWCRYPTO_ALG_MAC_AESGMAC,  
    &key,  12, (TMWTYPES_UCHAR*)P4, sizeof(P4), encryptBuf, &encryptLen))
  {
    if (!memcmp(encryptBuf, HASH4, encryptLen))
      success = TMWDEFS_TRUE;
  }

  if (!success)
  {
    testFailed = TMWDEFS_TRUE;
    TMWDIAG_ERROR("tmwcrypto: failed crypto AES-GMAC test");
  } 
#endif

  TMWTARG_UNUSED_PARAM(testFailed);

  return TMWDEFS_TRUE;
}
#endif

#endif /* TMWCNFG_SUPPORT_CRYPTO */
