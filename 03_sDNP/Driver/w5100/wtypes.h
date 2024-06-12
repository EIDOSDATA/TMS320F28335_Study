/*
 * wtypes.h
 *
 *  Created on: 2023. 10. 31.
 *      Author: ShinSung Industrial Electric
 */

#ifndef BSP_ETHERNET_WTYPES_H_
#define BSP_ETHERNET_WTYPES_H_

#include "types.h"

/***************************************************
 * attribute for mcu ( types, ... )
 ***************************************************/

//---- Refer "Rom File Maker Manual Vx.x.pdf"

#define _ENDIAN_LITTLE_     0   /**<  This must be defined if system is little-endian alignment */
#define _ENDIAN_BIG_        1
#define SYSTEM_ENDIAN       _ENDIAN_LITTLE_

#define MAX_SOCK_NUM        4   /**< Maxmium number of socket  */

/* ## __DEF_IINCHIP_xxx__ : define option for iinchip driver *****************/

#define __DEF_IINCHIP_DIRECT_MODE__         1
#define __DEF_IINCHIP_INDIRECT_MODE__       2
#define __DEF_IINCHIP_SPI_MODE__            3

//#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_DIRECT_MODE__
#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_INDIRECT_MODE__
//#define __DEF_IINCHIP_BUS__ __DEF_IINCHIP_SPI_MODE__ /*Enable SPI_mode*/


/**
@brief   __DEF_IINCHIP_MAP_xxx__ : define memory map for iinchip
*/
#define __DEF_IINCHIP_MAP_BASE__    0x0000 //0x8000
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
 #define COMMON_BASE __DEF_IINCHIP_MAP_BASE__
#else
 #define COMMON_BASE 0x0000
#endif

#define __DEF_IINCHIP_MAP_TXBUF__ (COMMON_BASE + 0x4000) /* Internal Tx buffer address of the iinchip */
#define __DEF_IINCHIP_MAP_RXBUF__ (COMMON_BASE + 0x6000) /* Internal Rx buffer address of the iinchip */

#define IINCHIP_ISR_DISABLE()
#define IINCHIP_ISR_ENABLE()
#define IINCHIP_ISR_GET(X)
#define IINCHIP_ISR_SET(X)

#ifndef NULL
#define NULL        ((void *) 0)
#endif



#ifndef _SIZE_T
#define _SIZE_T

#endif


/* bsd */
typedef uint8           u_char;     /**< 8-bit value */
typedef uint8           SOCKET;
typedef uint16          u_short;    /**< 16-bit value */
typedef uint16          u_int;      /**< 16-bit value */
typedef uint32          u_long;     /**< 32-bit value */

typedef union _un_l2cval {
    u_long  lVal;
    u_char  cVal[4];
}un_l2cval;

typedef union _un_i2cval {
    u_int   iVal;
    u_char  cVal[2];
}un_i2cval;


/** global define */
#define FW_VERSION          0x01010000  /* System F/W Version : 1.1.0.0 */
#define HW_VERSION          0x01000000


#define TX_RX_MAX_BUF_SIZE  2048
#define TX_BUF  0x1100
#define RX_BUF  (TX_BUF+TX_RX_MAX_BUF_SIZE)

#define UART_DEVICE_CNT     1   /**< UART device number */
/* #define SUPPORT_UART_ONE */

#endif /* BSP_ETHERNET_WTYPES_H_ */
