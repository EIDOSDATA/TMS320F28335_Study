/*
 * targ232.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_LIB_TMWSCL_TMWTARG_TARG232_H_
#define SRC_LIB_TMWSCL_TMWTARG_TARG232_H_

/*Target header*/
#include "hw_def.h"
#include "uart.h"
#include "ringbuf.h"

/*Target IO(Input Output) Configuration*/
#include "tmwscl/tmwtarg/targiocnfg.h"

/*TMW Stack header*/
#include "tmwscl/utils/tmwcnfg.h"
#include "tmwscl/utils/tmwpltmr.h"
#include "tmwscl/utils/tmwtarg.h"
#include "tmwscl/utils/tmwtarg.h"


/* UART Configuration structure */
typedef struct
{
  SCI_CH            portType;
  /* channel name */
  TMWTYPES_CHAR     chnlName[IOCNFG_MAX_NAME_LEN];

  /* port name */
  TMWTYPES_CHAR     portName[IOCNFG_MAX_NAME_LEN];

  /* baud rate */
  uint32            baudRate;

  /* parity */
  RS232_PARITY      parity;

  /* 7 or 8 */
  RS232_DATA_BITS   numDataBits;

  /* 1 or 2 */
  RS232_STOP_BITS   numStopBits;

  /* flow control: none, hardware */
  RS232_PORT_MODE   portMode;

  /* MODBUS RTU or not */
  TMWTYPES_BOOL     bModbusRTU;

} RS232_CONFIG;



/* Define serial port channel */
typedef struct
{
    TMWTARGIO_TYPE_ENUM     TargChType;

    SCI_CH                  portType;

    TMWTYPES_CHAR         chnlName[IOCNFG_MAX_NAME_LEN];
    TMWTYPES_CHAR         portName[IOCNFG_MAX_NAME_LEN];

    bool              is_open;

    /* Callback function for this channel */
    TMWTARG_CHANNEL_CALLBACK_FUNC     pChannelCallback;      /* From TMWTARG_CONFIG  */
    void                             *pChannelCallbackParam; /* From TMWTARG_CONFIG  */

    TMWTARG_CHANNEL_READY_CBK_FUNC    pChannelReadyCallback; /* From TMWTARG_CONFIG  */
    void                             *pChannelReadyCbkParam; /* From TMWTARG_CONFIG  */

    TMWTARG_CHANNEL_RECEIVE_CBK_FUNC  pReceiveCallbackFunc;  /* From openChannel    */
    TMWTARG_CHECK_ADDRESS_FUNC        pCheckAddrCallbackFunc;/* From openChannel    */
    void                             *pCallbackParam;        /* From openChannel, used by both above */

    /* parity */
    RS232_PARITY      parity;

    /* flow control: none, hardware */
    RS232_PORT_MODE   portMode;

    /* 7 or 8 */
    RS232_DATA_BITS   numDataBits;

    /* 1 or 2 */
    RS232_STOP_BITS   numStopBits;

    /* MODBUS RTU or not */
    TMWTYPES_BOOL      bModbusRTU;

  /*
   * Specifies the amount of time (in character times) to use to
   * determine that a frame has been completed.  For modbus RTU this
   * value is 3.5 (i.e. 4 will be used)
   */
    TMWTYPES_USHORT numCharTimesBetweenFrames;

  /*
   * Specifies the amount of time to use to
   * determine that an inter character timeout has occured.
   * For modbus RTU this value is 1.5 character times (i.e. 2 would be used)
   */
    TMWTYPES_USHORT       interCharTimeout;

    uint16                baudRate;

    Ringbuf_t             *pRxRingbufHd;
    Ringbuf_t             *pTxRingbufHd;

    bool                  (*pf_Init)(uint16 ch, UART_InitTypeDef *pHandle);
    uint16                (*pf_Read)(Ringbuf_t *phandle, void *pbuf, uint16 bufsize);

} SERIAL_IO_CHANNEL;

void Targ232_initPort(void);


void * Targ232_initChannel(RS232_CONFIG* pRS232Config, TMWTARG_CONFIG *pTmwConfig);
TMWTYPES_BOOL Targ232_openChannel(SERIAL_IO_CHANNEL* pSerialChannel);
bool Targ232_Trasmit(SERIAL_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 numBytes);

uint16 Targ232_Receive(SERIAL_IO_CHANNEL* pChannelInfo, uint8* pBuf, uint16 maxBytes);
#endif /* SRC_LIB_TMWSCL_TMWTARG_TARG232_H_ */
