/*
 * comm.c
 *
 *  Created on: 2024. 5. 31.
 *      Author: ShinSung Industrial Electric
 */

#include "comm.h"

#define COMM_TASK_PERIOD        (5)

static commContext      commCtx;
static uint8            MacAddress[6];
static TCP_CONFIG       TcpConfig;

static RS232_CONFIG     UartConfig[UART_MAX_CH];

/*OS instance*/
extern Task_Handle      Task_Comm, Task_Uart;
extern Semaphore_Handle SemSCADARxBuf, SemLocalRxBuf, SemSCADATxBuf, SemLocalTxBuf, SemUart;

static void setCommTaskPeriod(uint16 period);
static void setMacAddr(uint8 addr1, uint8 addr2, uint8 addr3,
                       uint8 addr4, uint8 addr5, uint8 addr6);
static void setIpAddr (uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4);
static void setGateway(uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4);
static void setSubnet (uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4);



TMWCHNL* TmwSdnpCh_Open(TMWAPPL* pTmwApplCtx, SdnpChType ChType, void* pMyIoConfig)
{
    if(pTmwApplCtx == NULL)
       return NULL;

    TMWCHNL* pTmwCh;

    DNPCHNL_CONFIG  DnpChConfig;
    DNPTPRT_CONFIG  DnpTpConfig;
    DNPLINK_CONFIG  DnpLinkConfig;
    TMWPHYS_CONFIG  DnpPhysConfig;

    TMWTARG_CONFIG  DnpTargConfig;

    memset(&DnpChConfig,    0,  sizeof(DNPCHNL_CONFIG));
    memset(&DnpTpConfig,    0,  sizeof(DNPTPRT_CONFIG));
    memset(&DnpLinkConfig,  0,  sizeof(DNPLINK_CONFIG));
    memset(&DnpPhysConfig,  0,  sizeof(TMWPHYS_CONFIG));
    memset(&DnpTargConfig,  0,  sizeof(TMWTARG_CONFIG));

    dnpchnl_initConfig(&DnpChConfig, &DnpTpConfig, &DnpLinkConfig, &DnpPhysConfig);
    tmwtarg_initConfig(&DnpTargConfig);

    /*DNP Link Layer User Config*/
    DnpLinkConfig.confirmMode       = TMWDEFS_LINKCNFM_ALWAYS;
    DnpLinkConfig.maxRetries        = 5;
    DnpLinkConfig.confirmTimeout    = 10000;

    if((ChType == SERIAL_SCADA_CH)||(SERIAL_LOCAL_CH))
        DnpLinkConfig.networkType       = DNPLINK_NETWORK_NO_IP;

    else if((ChType == TCP_SCADA_CH)||(TCP_LOCAL_CH))
        DnpLinkConfig.networkType       = DNPLINK_NETWORK_TCP_ONLY;

    pTmwCh = dnpchnl_openChannel(pTmwApplCtx,     &DnpChConfig,   &DnpTpConfig,
                                 &DnpLinkConfig,  &DnpPhysConfig, pMyIoConfig,
                                 &DnpTargConfig);
    return pTmwCh;
}

static TMWSESN* SdnpSesn_Open(TMWCHNL* pTmwCh, void* pTargDbHd)
{
    if((pTmwCh == NULL)||(pTargDbHd == NULL))
        return NULL;

    SDNPSESN_CONFIG SdnpSesnConfig;

    sdnpsesn_initConfig(&SdnpSesnConfig);

    return sdnpsesn_openSession(pTmwCh, &SdnpSesnConfig, pTargDbHd);
}


static TMWAPPL* TmwSdnpStack_Init(void)
{
    tmwappl_initSCL();
    tmwtimer_initialize();

    return tmwappl_initApplication();
}

static void SetUartConfig(SdnpChType Ch,            uint32 baudrate,            RS232_PARITY parity,
                          RS232_DATA_BITS dataBits, RS232_STOP_BITS stopBits,   RS232_PORT_MODE portMode,
                          bool bModbusRTU)
{
    if(Ch > SERIAL_LOCAL_CH)
        return;

    UartConfig[Ch].portType     = (SCI_CH)Ch;
    UartConfig[Ch].baudRate     = baudrate;
    UartConfig[Ch].parity       = parity;
    UartConfig[Ch].numDataBits  = dataBits;
    UartConfig[Ch].numStopBits  = stopBits;
    UartConfig[Ch].portMode     = portMode;
    UartConfig[Ch].bModbusRTU   = bModbusRTU;

}

static bool commInit(void)
{
    memset(&commCtx, 0, sizeof(commContext));

    Targ232_initPort();
    TargTCP_initPort();

    setCommTaskPeriod(COMM_TASK_PERIOD);
    /*
     * TI MAC address used temporarily
     * MAC Address : TexasIns_00:00:80 (a8:63:f2:00:00:80)
     */
    setMacAddr(0xA8, 0x63, 0xF2, 0x00, 0x00, 0x80);

    if(InitEthernet(MacAddress) == false)
    {
        DEBUG_Msg("fail to init w5100\n");
        /*Task Block forever*/
    }

    setIpAddr(192, 168, 137, 3);
    setGateway(255, 255, 255, 0);
    setSubnet(192, 168, 137, 1);

#if DNP_STACK
    TMWAPPL* pAppl;
    TMWCHNL* pTmwCh;
    TMWSESN* pTmwSesn;
    SdnpContext*  pSdnpStack = &commCtx.SdnpStack;

    SdnpChHandle* pSdnpChHd;

    /*init tmw sdnp stack*/
    pAppl = TmwSdnpStack_Init();

    if(pAppl == NULL)
    {
        DEBUG_Msg("fail to init TMW Application\n");
        return false;
    }
    commCtx.SdnpStack.pApplContext                  = pAppl;

    /*serial ch open*/
    pSdnpChHd = &commCtx.SdnpStack.SdnpChannelHd[SERIAL_SCADA_CH];
    pSdnpChHd->Type                 = TMWTARGIO_TYPE_232;

    SetUartConfig(SERIAL_SCADA_CH, 19200, PARITY_NONE, DATA_BITS_8, STOP_BITS_1, MODE_NONE, false);

    pSdnpChHd->pMyChConfig = (void*)&UartConfig[SCI_B];

    /*open tmw ch*/
    pTmwCh      = TmwSdnpCh_Open(pAppl, SERIAL_SCADA_CH, pSdnpChHd);

    if(pTmwCh == NULL)
    {
        DEBUG_Msg("fail to open TMW UART SCADA ch\n");
        return false;
    }
    pSdnpChHd->pTmwChHd  = pTmwCh;

    /*select database type*/
    pSdnpStack->SdnpDatabaseHd.Type = LOCAL_DB;

    /*open tmw session*/
    pTmwSesn = SdnpSesn_Open(pTmwCh, &pSdnpStack->SdnpDatabaseHd);

    if(pTmwSesn == NULL)
    {
        DEBUG_Msg("fail to open sdnp session\n");
        return false;
    }

    pSdnpChHd->pTmwSesnHd = pTmwSesn;

    /*tcp ch open*/
    pSdnpChHd = &commCtx.SdnpStack.SdnpChannelHd[TCP_SCADA_CH];

    pSdnpChHd->Type                       = TMWTARGIO_TYPE_TCP;
    TcpConfig.Socket[0]                   = SOCKET0;
    TcpConfig.PortNumber[0]               = 20000;
    pSdnpChHd->pMyChConfig                = (void*)&TcpConfig;

    /*open tmw ch*/
    pTmwCh      = TmwSdnpCh_Open(pAppl, TCP_SCADA_CH, pSdnpChHd);

    if(pTmwCh == NULL)
    {
        DEBUG_Msg("fail to open TMW TCP SCADA ch\n");
        return false;
    }

    pSdnpChHd->pTmwChHd   = pTmwCh;


    /*select database type*/
    pSdnpStack->SdnpDatabaseHd.Type = LOCAL_DB;
    /*open tmw session*/
    pTmwSesn = SdnpSesn_Open(pTmwCh, &pSdnpStack->SdnpDatabaseHd);

    if(pTmwSesn == NULL)
    {
        DEBUG_Msg("fail to open sdnp session\n");
        return false;
    }

    pSdnpChHd->pTmwSesnHd = pTmwSesn;

    /*init file context(handle) for sdnp*/
    commCtx.SdnpStack.pSdnpFileContext  = SdnpFileContext_init();

#endif
    return true;
}


void CommTask(UArg arg0, UArg arg1)
{
    if(commInit() == false)
    {
        /*block Communication task*/
        TASK_SET_PRI(Task_Comm, -1);
    }
    commContext* pContext     = &commCtx;
    SdnpContext* pSdnpContext = &pContext->SdnpStack;

    TCP_IO_CHANNEL* pTcpCh = (TCP_IO_CHANNEL*)pSdnpContext->SdnpChannelHd[TCP_SCADA_CH].pMyCommCh;

    /*RUN UART TASK*/
    TASK_SET_PRI(Task_Uart, 2);

    while(1)
    {
        TASK_SLEEP(50);

        /*monitoring w5100 socket status*/
        TargTCP_Monitoring(pTcpCh);
        /*run stack polled timer*/
        tmwpltmr_checkTimer();
        /*check input*/
        tmwappl_checkForInput(pSdnpContext->pApplContext);


    }
}

void UartTask(UArg arg0, UArg arg1)
{
    uint16 RxCnt;
    uint16 BufLen;
    uint16 available;
    uint16 send_bytes;

    SERIAL_IO_CHANNEL*  pSerialCh = commCtx.SdnpStack.SdnpChannelHd[SERIAL_SCADA_CH].pMyCommCh;

    Ringbuf_t* pRxRingbuf = pSerialCh->pRxRingbufHd;
    Ringbuf_t* pTxRingbuf = pSerialCh->pTxRingbufHd;

    void*      pSemRx = SemSCADARxBuf;
    void*      pSemTx = SemSCADATxBuf;

    SCI_CH ch = SCI_B;

    uint8 Buf[SCI_FIFO_SIZE];

    while(1)
    {
        SEMAPHORE_PEND(SemUart, 3);

        /*send*/
        available = UARTTx_Check(ch);

        BufLen = Ringbuf_Length(pTxRingbuf);

        send_bytes = BufLen > available ? available : BufLen;

        if(send_bytes)
        {
            SEMAPHORE_PEND(pSemTx, WAIT_FOREVER);
            Ringbuf_Read(pTxRingbuf, Buf, send_bytes);
            SEMAPHORE_POST(pSemTx);

            UART_Write(ch, Buf, send_bytes);
        }

        /*receive*/
        RxCnt = UARTRx_Check(ch);

        if(RxCnt)
        {
            uint16 ReadSize = UART_Read(ch, Buf);
            SEMAPHORE_PEND(pSemRx, WAIT_FOREVER);
            Ringbuf_Write(pRxRingbuf, Buf, ReadSize);
            SEMAPHORE_POST(pSemRx);
        }
    }

}
static void setCommTaskPeriod(uint16 period)
{
    commCtx.taskPeriod = period;
}

static void setMacAddr(uint8 addr1, uint8 addr2, uint8 addr3,
                       uint8 addr4, uint8 addr5, uint8 addr6)
{
    MacAddress[0] = addr1;
    MacAddress[1] = addr2;
    MacAddress[2] = addr3;
    MacAddress[3] = addr4;
    MacAddress[4] = addr5;
    MacAddress[5] = addr6;
}
static void setIpAddr(uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4)
{
    TcpConfig.SoureceIp[0] = addr1;
    TcpConfig.SoureceIp[1] = addr2;
    TcpConfig.SoureceIp[2] = addr3;
    TcpConfig.SoureceIp[3] = addr4;
}
static void setGateway(uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4)
{
    TcpConfig.GateWay[0] = addr1;
    TcpConfig.GateWay[1] = addr2;
    TcpConfig.GateWay[2] = addr3;
    TcpConfig.GateWay[3] = addr4;
}
static void setSubnet(uint8 addr1, uint8 addr2, uint8 addr3, uint8 addr4)
{
    TcpConfig.SubnetMask[0] = addr1;
    TcpConfig.SubnetMask[1] = addr2;
    TcpConfig.SubnetMask[2] = addr3;
    TcpConfig.SubnetMask[3] = addr4;
}

