/*
 * uart.c
 *
 *  Created on: 2023. 9. 12.
 *      Author: ShinSung Industrial Electric
 */
#include <stdio.h>
#include "DSP28x_Project.h"
#include "def.h"

#include "uart.h"



/* Baudrate */
enum
{
    BAUDRATE_300_H      = 0x003D,           BAUDRATE_300_L      = 0x0008,
    BAUDRATE_600_H      = 0x001E,           BAUDRATE_600_L      = 0x0084,
    BAUDRATE_1200_H     = 0x000F,           BAUDRATE_1200_L     = 0x0042,
    BAUDRATE_2400_H     = 0x0007,           BAUDRATE_2400_L     = 0x00A0,
    BAUDRATE_4800_H     = 0x0003,           BAUDRATE_4800_L     = 0x00D0,
    BAUDRATE_9600_H     = 0x0001,           BAUDRATE_9600_L     = 0x00E7,
    BAUDRATE_19200_H    = 0x0000,           BAUDRATE_19200_L    = 0x00F3,
    BAUDRATE_38400_H    = 0x0000,           BAUDRATE_38400_L    = 0x0079,
    BAUDRATE_57600_H    = 0x0000,           BAUDRATE_57600_L    = 0x0050,
    BAUDRATE_115200_H   = 0x0000,           BAUDRATE_115200_L   = 0x0027,
};



#define PARITY_DISABLE              0
#define PARITY_ENABLE               1

#define STOP_BIT_1                  0
#define STOP_BIT_2                  1


#define FLOW_CONTROL_DISABLE        0
#define FLOW_CONTROL_ENABLE         1


volatile struct SCI_REGS *const pUART_Register[2] = {&ScibRegs,
                                                     &ScicRegs};
/*
 * inline Function
 */
uint16 SCIFIFOTx_Check(SCI_CH ch)
{
    return (SCI_FIFO_MAX - pUART_Register[ch]->SCIFFTX.bit.TXFFST);
}

uint16 SCIFIFORx_Check(SCI_CH ch)
{
    return pUART_Register[ch]->SCIFFRX.bit.RXFFST;
}

inline void
SCI_Write(SCI_CH ch, Uint16 *pData)
{
    pUART_Register[ch]->SCITXBUF = *pData;
}

inline Uint16
SCI_Read(SCI_CH ch)
{
    return pUART_Register[ch]->SCIRXBUF.all;
}
/*
 * Description :
 */
bool UART_Init(SCI_CH ch, UART_InitTypeDef *pHandle)
{
    /*Baudrate Setting*/
    switch(pHandle->Baudrate)
    {
    case 1200:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_1200_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_1200_L;
        break;
    case 2400:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_2400_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_2400_L;
        break;
    case 4800:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_4800_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_4800_L;
        break;
    case 9600:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_9600_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_9600_L;
        break;
    case 19200:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_19200_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_19200_L;
        break;
    case 38400:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_38400_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_38400_L;
        break;
    case 57600:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_57600_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_57600_L;
        break;
    case 115200:
        pUART_Register[ch]->SCIHBAUD = BAUDRATE_115200_H;
        pUART_Register[ch]->SCILBAUD = BAUDRATE_115200_L;
        break;

    }

    /*Parity Bit*/
    pUART_Register[ch]->SCICCR.bit.PARITYENA = pHandle->Parity;
    /*DataBits*/
    pUART_Register[ch]->SCICCR.bit.SCICHAR = pHandle->DataBits;
    /*Stop Bit*/
    pUART_Register[ch]->SCICCR.bit.STOPBITS = pHandle->StopBit;
    /*HardWard Flaow Control ??*/
    //pUART_Register[ch]->SCICCR.bit.ADDRIDLE_MODE = pHandle->FlowControl;


    /*TX RX Enable*/
    pUART_Register[ch]->SCICTL1.bit.TXENA = 1;
    pUART_Register[ch]->SCICTL1.bit.RXENA = 1;

    /*TX FIFO Enable*/
    pUART_Register[ch]->SCIFFTX.bit.SCIFFENA = 1;

   // pUART_Register[ch]->SCIFFRX.bit.RXFFIENA = 1;
    /*Software Reset to work*/
    pUART_Register[ch]->SCICTL1.bit.SWRESET = 1;

    if(ch == SCI_B)             InitScibGpio();

    else if(ch == SCI_C)        InitScicGpio();

    return true;
}


uint16 UARTRx_Check(SCI_CH ch)
{
    return SCIFIFORx_Check(ch);
}

uint16 UARTTx_Check(SCI_CH ch)
{
    return SCIFIFOTx_Check(ch);
}

uint16 UART_Write(SCI_CH ch, uint16 *pbuf, uint16 length)
{
    uint16 available = SCIFIFOTx_Check(ch);
    uint16 i, send_bytes = 0;

    if(available >= length)         send_bytes = length;

    else                            send_bytes = available;

    for(i=0; i<send_bytes; i++)
    {
        SCI_Write(ch, pbuf+i);
    }

    return send_bytes;
}

uint16 UART_Read(SCI_CH ch, uint16 *pbuf)
{
    uint16 i;
    uint16 read_Bytes = SCIFIFORx_Check(ch);

    for(i=0; i<read_Bytes; i++)
    {
        *(pbuf++) = SCI_Read(ch);
    }

    return read_Bytes;
}



