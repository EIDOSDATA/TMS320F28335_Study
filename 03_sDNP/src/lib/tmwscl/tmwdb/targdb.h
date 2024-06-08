/*
 * targdb.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: hp878
 */

#ifndef SRC_LIB_TMWSCL_TMWDB_TARGDB_H_
#define SRC_LIB_TMWSCL_TMWDB_TARGDB_H_

/*Target header*/
#include "hw_def.h"

/*Target IO(Input Output) Configuration*/
#include "tmwscl/tmwtarg/targiocnfg.h"

typedef enum
{
    LOCAL_DB,
    SCADA_DB,
    SDNP_DATABASE_TYPE_MAX,

} SDNP_DATABASE_TYPE;

typedef struct
{
    TMWTYPES_ANALOG_TYPE         DataType;

//    TAG_GROUP                    TagGroup;

    void*                        pDataPoint;
    float32                      PreviousValue;
    uint16                       DeadBand;
    uint16                       DataClass;
    bool                         SOE;
    uint16                       Scale;

} DnpAiPointDescription;

typedef struct
{
    TMWTYPES_ANALOG_TYPE         DataType;

//    TAG_GROUP                    TagGroup;
    /*Use only when TAG LS0,2(Group1, Group2) changes*/
    uint16                       TagIndex;

    void                        *pDataPoint;

} DnpAoPointDescription;

typedef struct
{
    void*                        pDataPoint;
//    TAG_GROUP                    TagGroup;
    bool                         PreviousValue;
    bool                         Invert;
    uint16                       DataClass;
    bool                         SOE;

} DnpBiPointDescription;


typedef struct
{
    void*                       pDataPoint;
//    TAG_GROUP                   TagGroup;
    uint16                      TagIndex;

    bool                        Command;
    bool                        SelectFlag;
} DnpBoPointDescription;

typedef struct
{
    bool                    IsScadaDbInit;

    uint16                  ScadaAiQuantity;
    uint16                  ScadaAoQuantity;
    uint16                  ScadaBiQuantity;
    uint16                  ScadaBoQuantity;

    DnpAiPointDescription*   pScadaAiPointList;
    DnpAoPointDescription*   pScadaAoPointList;
    DnpBiPointDescription*   pScadaBiPointList;
    DnpBoPointDescription*   pScadaBoPointList;

} SdnpScadaDb;

typedef struct
{
    bool                    IsLocalDbInit;

    uint16                  DnpAiIndexQuantity;
    uint16                  DnpAoIndexQuantity;
    uint16                  DnpBiIndexQuantity;
    uint16                  DnpBoIndexQuantity;

    DnpAiPointDescription*   pDnpAiPointList;
    DnpAoPointDescription*   pDnpAoPointList;
    DnpBiPointDescription*   pDnpBiPointList;
    DnpBoPointDescription*   pDnpBoPointList;

} SdnpLocalDb;

typedef struct
{
//    FileDescriptor        *pFile;

    /*File Interface*/
//    FileIndex (*pFileName_Parser)(const char *pString);

//    FileDescriptor* (*pFile_Open)(FileIndex Index, FileMode Mode);

//    bool (*pFile_Close)(FileDescriptor *pFile);

//    bool (*pDnpFile_Read)(FileDescriptor *pFile, void *pBuf, uint16 ReadWords);
//    bool (*pDnpFile_Write)(FileDescriptor *pFile, uint16 *pBuf, uint16 WriteWords);

    /*SCADA Configuaation parser*/
//    bool   (*pfScadaFile_Parser[DNP_GROUP_MAX])(FileDescriptor* pFile, uint16* pBuf);
//    uint16 (*pfScadaFile_Send[DNP_GROUP_MAX])(FileDescriptor* pFile, uint16* pSendBuf);

 //   EventHandle*        pEventHandle;

    uint32              FileAuthenticationKey;
//    char                FileUserName[FILE_NAME_MAX];
//    char                FilePassword[FILE_PASSWORD_MAX];


    bool*               pChGainUpdateFlag;

    /*Firmware update*/
    bool                UpdateFlag;
    bool                DownLoadFlag;

//    FileIndex           CurrentUpdateIndex;

    uint32              TotalReceiveWordSize;

    uint16              SendWord;
    uint32              TotalSendWordSize;

//    Ringbuf_t           UpdateFileRingBuf;
    uint16              FlashReadBuf[2048];         // external flash sector size 2048 word
    void*               pUpdateInfoFile;

//    FirmwareParser      UpdateFileParser;

    uint16*             pTaskPeriod;

    void                (*pfSampling_Stop)(void);

} SdnpFileContext;


typedef struct
{
    SDNP_DATABASE_TYPE      Type;

    void*                   pDatabase;

} SdnpDbHandle;


SdnpFileContext* SdnpFileContext_init(void);

void* TargDatabase_init(void* pDbHandle);
uint16 GetAiQuntity(void *pHandle);
void* GetAiDataPoint(void *pHandle, uint16 PointNum);
void ReadAiData(void* pPoint, TMWTYPES_ANALOG_VALUE *pValue);
#endif /* SRC_LIB_TMWSCL_TMWDB_TARGDB_H_ */
