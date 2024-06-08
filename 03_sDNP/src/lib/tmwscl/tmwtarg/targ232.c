/*
 * targ232.c
 *
 *  Created on: 2024. 6. 4.
 *      Author: ShinSung Industrial Electric
 */

#include "targ232.h"


typedef struct
{
    uint16              PortRemaining;
    SERIAL_IO_CHANNEL   UART_PORT[UART_MAX_CH];

} TargSerialPort;

/*For Target*/
#pragma DATA_SECTION(SerialPort,            "DNP_Stack")
#pragma DATA_SECTION(RxRingBufHd,           "DNP_Stack")
#pragma DATA_SECTION(RxBuffer,              "DNP_Stack")
#pragma DATA_SECTION(TxRingBufHd,           "DNP_Stack")
#pragma DATA_SECTION(TxRingBufHd,           "DNP_Stack")

static TargSerialPort SerialPort;
static Ringbuf_t      RxRingBufHd[IOCNFG_NUM_SERIAL_PORTS];
static uint16         RxBuffer[IOCNFG_NUM_SERIAL_PORTS][IOCNFG_SERIAL_INBUF_SIZE];
static Ringbuf_t      TxRingBufHd[IOCNFG_NUM_SERIAL_PORTS];
static uint16         TxBuffer[IOCNFG_NUM_SERIAL_PORTS][IOCNFG_SERIAL_OUTBUF_SIZE];

extern Semaphore_Handle SemSCADARxBuf, SemLocalRxBuf, SemSCADATxBuf, SemLocalTxBuf, SemUart;

void Targ232_initPort(void)
{
    memset(&SerialPort, 0, sizeof(TargSerialPort));

    SerialPort.PortRemaining = UART_MAX_CH;
}
void * Targ232_initChannel(RS232_CONFIG* pRS232Config, TMWTARG_CONFIG *pTmwConfig)
{
    if((pRS232Config == NULL)||(pTmwConfig == NULL))
        return NULL;

    SERIAL_IO_CHANNEL* pChannelHd = NULL;

    if(SerialPort.PortRemaining)
    {
        uint16        Index        = UART_MAX_CH - SerialPort.PortRemaining;

        pChannelHd = &SerialPort.UART_PORT[Index];

        pChannelHd->TargChType                 = TMWTARGIO_TYPE_232;
        /*tmw stack default*/
        pChannelHd->pChannelCallback           = pTmwConfig->pChannelCallback;
        pChannelHd->pChannelReadyCallback      = pTmwConfig->pChannelReadyCallback;
        pChannelHd->pChannelReadyCbkParam      = pTmwConfig->pChannelReadyCbkParam;
        pChannelHd->numCharTimesBetweenFrames  = pTmwConfig->numCharTimesBetweenFrames;
        pChannelHd->interCharTimeout           = pTmwConfig->interCharTimeout;

        /*My config*/
        strcpy(pChannelHd->chnlName, pRS232Config->chnlName);
        strcpy(pChannelHd->portName, pRS232Config->portName);

        pChannelHd->portType                   = pRS232Config->portType;
        pChannelHd->portMode                   = pRS232Config->portMode;
        pChannelHd->baudRate                   = pRS232Config->baudRate;
        pChannelHd->parity                     = pRS232Config->parity;
        pChannelHd->numDataBits                = pRS232Config->numDataBits;
        pChannelHd->numStopBits                = pRS232Config->numStopBits;
        pChannelHd->bModbusRTU                 = pRS232Config->bModbusRTU;

        /*create tx, rx ringbuf*/
        if(Ringbuf_Create(&RxRingBufHd[Index], &RxBuffer[Index], IOCNFG_SERIAL_INBUF_SIZE))
            pChannelHd->pRxRingbufHd = &RxRingBufHd[Index];

        else
            DEBUG_Msg("[DNP] fail to create UART Rx ringbuf\n");

        if(Ringbuf_Create(&TxRingBufHd[Index], &TxBuffer[Index], IOCNFG_SERIAL_OUTBUF_SIZE))
            pChannelHd->pTxRingbufHd = &TxRingBufHd[Index];

        else
            DEBUG_Msg("[DNP] fail to create UART Tx ringbuf\n");

        SerialPort.PortRemaining--;
    }

    return pChannelHd;
}


TMWTYPES_BOOL Targ232_openChannel(SERIAL_IO_CHANNEL* pSerialChannel)
{
    /*apply sci(peripheral)*/
    UART_InitTypeDef UART_Instance;

    memset(&UART_Instance, 0, sizeof(UART_InitTypeDef));

    UART_Instance.Baudrate      = pSerialChannel->baudRate;
    UART_Instance.DataBits      = pSerialChannel->numDataBits;
    UART_Instance.Parity        = pSerialChannel->parity;
    UART_Instance.StopBit       = pSerialChannel->numStopBits;
    UART_Instance.FlowControl   = pSerialChannel->portMode;

    UART_Init(pSerialChannel->portType ,&UART_Instance);

    pSerialChannel->is_open = true;

    DEBUG_Msg("[DNP] serial port open\n");

    return(TMWDEFS_TRUE);
}

uint16 Targ232_Receive(SERIAL_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 maxBytes)
{
    if(pChannelInfo == NULL)
        return (0);

    void* pSemRxBuf[UART_MAX_CH] = {SemSCADARxBuf, SemLocalRxBuf};

    /*Lock*/
    SEMAPHORE_PEND(pSemRxBuf[pChannelInfo->portType], WAIT_FOREVER);
    /*pop data from ringbuf*/
    uint16 readBytes = Ringbuf_Read(pChannelInfo->pRxRingbufHd, pBuf, maxBytes);
    /*UnLock*/
    SEMAPHORE_POST(pSemRxBuf[pChannelInfo->portType]);

    return readBytes;
}

bool Targ232_Trasmit(SERIAL_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 numBytes)
{
    if(pChannelInfo == NULL)
        return (0);

    void* pSemTxBuf[UART_MAX_CH] = {SemSCADATxBuf, SemLocalTxBuf};
    bool  Rtn = false;

    /*Lock*/
    SEMAPHORE_PEND(pSemTxBuf[pChannelInfo->portType], WAIT_FOREVER);

    Rtn = Ringbuf_Write(pChannelInfo->pTxRingbufHd, pBuf, numBytes);

    /*UnLock*/
    SEMAPHORE_POST(pSemTxBuf[pChannelInfo->portType]);

    /*signal to uart task*/
    SEMAPHORE_POST(SemUart);

    return Rtn;
}
