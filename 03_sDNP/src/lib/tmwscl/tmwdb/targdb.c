/*
 * targdb.c
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */
#include <string.h>

#include "targdb.h"

/*Target dnp data point map*/
#include "tmwscl/tmwdb/sdnpmap.h"

static DnpAiPointDescription LocalDbAiList[LOCAL_DB_AI_POINT_MAX];


static SdnpLocalDb  SdnpLocal = {.IsLocalDbInit = 0};
static SdnpScadaDb  SdnpScada = {.IsScadaDbInit = 0};



static void SdnpAiMapping(SDNP_DATABASE_TYPE Type, DnpAiPointDescription* pDnpAiPoint);

SdnpFileContext* SdnpFileContext_init(void)
{
    return NULL;
}

void* TargDatabase_init(void* pDbHandle)
{
    if(pDbHandle == NULL)
        return NULL;

    SdnpDbHandle* pSdnpDbHandle = (SdnpDbHandle*)pDbHandle;

    switch(pSdnpDbHandle->Type)
    {
        case LOCAL_DB:
            if(SdnpLocal.IsLocalDbInit == false)
            {
                /*clear*/
                memset(&SdnpLocal,          0, sizeof(SdnpLocalDb));
                memset(&LocalDbAiList[0],   0, sizeof(DnpAiPointDescription)* LOCAL_DB_AI_POINT_MAX);
                /*Dnp AI map mapping*/
                SdnpLocal.DnpAiIndexQuantity = LOCAL_DB_AI_POINT_MAX;
                SdnpAiMapping(LOCAL_DB, LocalDbAiList);
                /*load ai point list*/
                SdnpLocal.pDnpAiPointList = LocalDbAiList;

                /*Dnp AO map mapping*/
                SdnpLocal.DnpAoIndexQuantity = LOCAL_DB_AO_POINT_MAX;
                /*Dnp BI map mapping*/
                SdnpLocal.DnpBiIndexQuantity = LOCAL_DB_BI_POINT_MAX;
                /*Dnp BO map mapping*/
                SdnpLocal.DnpBoIndexQuantity = LOCAL_DB_BO_POINT_MAX;

                SdnpLocal.IsLocalDbInit = true;
            }
            pSdnpDbHandle->pDatabase = (void*)&SdnpLocal;
            break;
        case SCADA_DB:
            if(SdnpScada.IsScadaDbInit == false)
            {
                memset(&SdnpScada, 0, sizeof(SdnpScadaDb));

                SdnpScada.IsScadaDbInit = true;
            }
            pSdnpDbHandle->pDatabase = (void*)&SdnpScada;
            break;
    }


    return (void*)pSdnpDbHandle;
}
/*
 * dnp data point mapping
 */
static void SdnpAiMapping(SDNP_DATABASE_TYPE Type, DnpAiPointDescription* pDnpAiPoint)
{
    uint16 IndexMax[SDNP_DATABASE_TYPE_MAX] = {LOCAL_DB_AI_POINT_MAX, SCADA_DB_AI_POINT_MAX};
    uint16 i, loopEnd = IndexMax[Type];

    const DnpAnalogMap* pDnpAiList = &DnpAiMap[0];

    for(i=0; i<loopEnd; i++)
    {
        pDnpAiPoint[i].DataType   = (TMWTYPES_ANALOG_TYPE)pDnpAiList[i].AnalogType;
        /*get tag data address*/
        pDnpAiPoint[i].pDataPoint = getTagDataAddr(pDnpAiList[i].TagGroup, pDnpAiList[i].TagIndex);
    }

}


uint16 GetAiQuntity(void *pHandle)
{
    SdnpDbHandle *pSdnpDbHandle = pHandle;
    uint16 Quantity = 0;

    switch(pSdnpDbHandle->Type)
    {
        case LOCAL_DB:
        {
            SdnpLocalDb* pLocalDb = (SdnpLocalDb*)pSdnpDbHandle->pDatabase;
            Quantity              = pLocalDb->DnpAiIndexQuantity;
        }
            break;
        case SCADA_DB:
        {
            SdnpScadaDb* pScadaDb = (SdnpScadaDb*)pSdnpDbHandle->pDatabase;
            Quantity              = pScadaDb->ScadaAiQuantity;
        }
            break;
    }

    /*Return DNP AI quantity*/
    return Quantity;
}

void* GetAiDataPoint(void *pHandle, uint16 PointNum)
{
    SdnpDbHandle *pSdnpDbHandle = pHandle;

    void* pPoint = NULL;

    switch(pSdnpDbHandle->Type)
    {
        case LOCAL_DB:
        {
            SdnpLocalDb* pLocalDb = (SdnpLocalDb*)pSdnpDbHandle->pDatabase;
            pPoint                = (void*)&pLocalDb->pDnpAiPointList[PointNum];
        }
            break;
        case SCADA_DB:
        {
            SdnpScadaDb* pScadaDb = (SdnpScadaDb*)pSdnpDbHandle->pDatabase;
            pPoint                = (void*)&pScadaDb->pScadaAiPointList[PointNum];
        }
            break;
    }

    /*Return DNP AI data point address*/
    return pPoint;
}

void ReadAiData(void* pPoint, TMWTYPES_ANALOG_VALUE *pValue)
{
    DnpAiPointDescription* pPointDescription = (DnpAiPointDescription*)pPoint;

    void* pDataPoint = pPointDescription->pDataPoint;

    TMWTYPES_ANALOG_TYPE AnalogType = pPointDescription->DataType;

    if(pDataPoint != NULL)
    {
        pValue->type = AnalogType;

        switch(AnalogType)
        {
            case TMWTYPES_ANALOG_TYPE_SHORT:
            {
                int16 Value = *(int16*)pDataPoint;
                pValue->value.sval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_USHORT:
            {
                uint16 Value = *(uint16*)pDataPoint;
                pValue->value.usval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_LONG:
            {
                int32 Value = *(int32*)pDataPoint;
                pValue->value.lval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_ULONG:
            {
                uint32 Value = *(uint32*)pDataPoint;
                pValue->value.ulval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_CHAR:
            {
                char Value = *(char*)pDataPoint;
                pValue->value.cval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_UCHAR:
            {
                unsigned char Value = *(unsigned char*)pDataPoint;
                pValue->value.ucval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_SFLOAT:
            {
                float32 Value = *(float32*)pDataPoint;

                pValue->value.fval = Value;
            }
                break;
            case TMWTYPES_ANALOG_TYPE_SCALED:
            default:
                /*Not supported*/
                break;
        }
    }

}

