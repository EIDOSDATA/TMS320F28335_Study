/*
 * targiocnfg.h
 *
 *  Created on: 2024. 6. 5.
 *      Author: ShinSung Industrial Electric
 */

#ifndef SRC_LIB_TMWSCL_TMWTARG_TARGIOCNFG_H_
#define SRC_LIB_TMWSCL_TMWTARG_TARGIOCNFG_H_

#include "tmwscl/utils/tmwtypes.h"

#define IOCNFG_MAX_NAME_LEN            16

/* Currently only support one serial port (SCI-B, SCI-C)*/
#define IOCNFG_NUM_SERIAL_PORTS        2

#define IOCNFG_SERIAL_INBUF_SIZE       4096
#define IOCNFG_SERIAL_OUTBUF_SIZE      4096
#define IOCNFG_UDP_BUFFER_SIZE         8000
#define IOCNFG_MAX_UDP_FRAME           2500

#define IOCNFG_IP_ADDRESS_SIZE         4
#define IOCNFG_GATEWAY_SIZE            4
#define IOCNFG_SUBNETMASK_SIZE         4


/* set this to TMWDEFS_FALSE to remove TCP support */
#define IOTARG_SUPPORT_TCP TMWDEFS_TRUE

/* set this to TMWDEFS_FALSE to remove serial support */
#define IOTARG_SUPPORT_232 TMWDEFS_TRUE






#endif /* SRC_LIB_TMWSCL_TMWTARG_TARGIOCNFG_H_ */
