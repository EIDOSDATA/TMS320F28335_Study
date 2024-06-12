/*
 * uart.h
 *
 *  Created on: 2023. 9. 12.
 *      Author: ShinSung Industrial Electric
 */

#ifndef COMPONENTS_CLI_INTERFACE_UART_H_
#define COMPONENTS_CLI_INTERFACE_UART_H_

#include "hw_def.h"

#define UART_MAX_CH         2

#define SCI_FIFO_MAX        16

#define SCIB_Rx_IntNum      98
#define SCIB_Tx_IntNum      99
#define SCIC_Rx_IntNum      92
#define SCIC_Tx_IntNum      93

/*Enumeration*/
typedef enum
{
  PARITY_NONE,
  PARITY_EVEN,
  PARITY_ODD
} RS232_PARITY;

typedef enum
{
  DATA_BITS_1 = 0,
  DATA_BITS_2 = 1,
  DATA_BITS_3 = 2,
  DATA_BITS_4 = 3,
  DATA_BITS_5 = 4,
  DATA_BITS_6 = 5,
  DATA_BITS_7 = 6,
  DATA_BITS_8 = 7,

} RS232_DATA_BITS;

typedef enum
{
  STOP_BITS_1 = 0,
  STOP_BITS_2 = 1
} RS232_STOP_BITS;

typedef enum
{
  MODE_NONE,
  MODE_HARDWARE
} RS232_PORT_MODE;

typedef struct
{
    uint32           Baudrate;

    RS232_PARITY     Parity;

    RS232_DATA_BITS  DataBits;

    RS232_STOP_BITS  StopBit;

    RS232_PORT_MODE  FlowControl;

} UART_InitTypeDef;


/* Public Function */

bool UART_Init(SCI_CH ch, UART_InitTypeDef *pHandle);



uint16 UARTRx_Check(SCI_CH ch);
uint16 UARTTx_Check(SCI_CH ch);

uint16 UART_Write(SCI_CH ch, uint16 *pbuf, uint16 length);
uint16 UART_Read(SCI_CH ch, uint16 *pbuf);


#endif /* COMPONENTS_CLI_INTERFACE_UART_H_ */
