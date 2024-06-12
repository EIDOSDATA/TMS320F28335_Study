/*
 * wsocket.c
 *
 *  Created on: 2023. 10. 30.
 *      Author: ShinSung Industrial Electric
 *
 *
 * (c)COPYRIGHT
 * ALL RIGHT RESERVED
 *
 * FileName : socket.c
 * Revision History :
 * ---------- -------   ------------------------------------------------
 *  Date      version     Description
 * ---------- -------   ------------------------------------------------
 * 03/13/2012   1.6     Modified connect() and sendto() functions
 *                      because of the ARP errata.
 * ----------   ------- ------------------------------------------------
 * 10/28/2013   1.8         Modified connect() and sendto() functions
 *                          because of the ARP errata.
 *                          but it was only solved the Erratum3.
 *                          Supports the local subnet mask broadcasting
 * ---------- -------   ------------------------------------------------
 */

#include "wsocket.h"
#include "types.h"


static uint16 local_port;

/* Function    : Socket
 * Description : This Socket function initialize the channel in perticular mode, and set the port and wait for W5100 done it.
 * Parameter   : s - socket number,                 protocol - socket protocol,
 *               port - port number for the socket, flag - option for the socket
 * Return      : Success 1      Fail 0
 */
uint8 socket(SOCKET s, uint8 protocol, uint16 port)
{
  uint16 count;

  if ((protocol == Sn_MR_TCP) || (protocol == Sn_MR_UDP) || (protocol == Sn_MR_IPRAW) || (protocol == Sn_MR_MACRAW) || (protocol == Sn_MR_PPPOE))
  {
      wclose(s);
      IINCHIP_WRITE(Sn_MR(s), protocol);
    if (port != 0)
    {
      IINCHIP_WRITE(Sn_PORT0(s), (uint8)((port & 0xff00) >> 8));
      IINCHIP_WRITE((Sn_PORT0(s) + 1), (uint8)(port & 0x00ff));
    }
    else
    {
      local_port++; // if don't set the source port, set local_port number.
      IINCHIP_WRITE(Sn_PORT0(s), (uint8)((local_port & 0xff00) >> 8));
      IINCHIP_WRITE((Sn_PORT0(s) + 1), (uint8)(local_port & 0x00ff));
    }

    IINCHIP_WRITE(Sn_CR(s), Sn_CR_OPEN); // run sockinit Sn_CR

    /* +20071122[chungs]:wait to process the command... */
    count = 0;
    while( IINCHIP_READ(Sn_CR(s)) )
    {
        count++;
        if(count > 1000)
        {
            return FALSE;
        }
    }
    return TRUE;
  }
  else
  {
     return FALSE;
  }

}


/**
@brief  This function close the socket and parameter is "s" which represent the socket number
*/
void wclose(SOCKET s)
{
    uint16 count;
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("close()\r\n");
  CMF_Fflush();
#endif

  if( s > 3)            return;

  IINCHIP_WRITE(Sn_CR(s),Sn_CR_CLOSE);

  /* +20071122[chungs]:wait to process the command... */
  count = 0;
  while( IINCHIP_READ(Sn_CR(s)) )
  {
      count++;
      if(count > 1000)
      {
          break;
      }
  }
  /* ------- */

  /* +2008.01 [hwkim]: clear interrupt */
#ifdef __DEF_IINCHIP_INT__
      /* m2008.01 [bj] : all clear */
         putISR(s, 0x00);
#else
      /* m2008.01 [bj] : all clear */
    IINCHIP_WRITE(Sn_IR(s), 0xFF);
#endif
}



/**
@brief  This function established  the connection for the channel in passive (server) mode. This function waits for the request from the peer.
@return 1 for success else 0.
*/
uint8 listen(
  SOCKET s  /**< the socket number */
  )
{
  uint8 ret;
  uint16 count;

    if (IINCHIP_READ(Sn_SR(s)) == SOCK_INIT)
    {
        IINCHIP_WRITE(Sn_CR(s),Sn_CR_LISTEN);
        /* +20071122[chungs]:wait to process the command... */

        count = 0;
        while( IINCHIP_READ(Sn_CR(s)) )
        {
            count++;
            if(count > 1000)
            {
                return ret;
            }
        }
        ret = 1;
    }
    else
    {
        ret = 0;

    }

  return ret;
}


/**
@brief  This function established  the connection for the channel in Active (client) mode.
    This function waits for the untill the connection is established.

@return 1 for success else 0.
*/
uint8 connect(SOCKET s, uint8 * addr, uint16 port)
{
  uint8 ret;
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("connect()\r\n");
  CMF_Fflush();
#endif
  if
    (
      ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
      ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
      (port == 0x00)
    )
  {
    ret = 0;
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("Fail[invalid ip,port]\r\n");
  CMF_Fflush();
#endif
  }
  else
  {
    ret = 1;
    // set destination IP
    IINCHIP_WRITE(Sn_DIPR0(s),addr[0]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 1),addr[1]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 2),addr[2]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 3),addr[3]);
    IINCHIP_WRITE(Sn_DPORT0(s),(uint8)((port & 0xff00) >> 8));
    IINCHIP_WRITE((Sn_DPORT0(s) + 1),(uint8)(port & 0x00ff));
    // m2012.03.13 [ys]: set/clear subnet for ARP Errata
       clearSUBR();
    IINCHIP_WRITE(Sn_CR(s),Sn_CR_CONNECT);
        /* m2008.01 [bj] :  wait for completion */
    while ( IINCHIP_READ(Sn_CR(s)) );
/*
    // check the SYN packet sending...
   while ( IINCHIP_READ(Sn_SR(s)) != SOCK_SYNSENT )
    {
       if(IINCHIP_READ(Sn_SR(s)) == SOCK_ESTABLISHED)
       {
          break;
       }
       if (getSn_IR(s) & Sn_IR_TIMEOUT)
       {
          setSn_IR(s,(Sn_IR_TIMEOUT ));  // clear TIMEOUT Interrupt
          ret = 0;
          break;
       }
    }
    */
    applySUBR();

   }

  return ret;
}



/**
@brief  This function used for disconnect the socket and parameter is "s" which represent the socket number
@return 1 for success else 0.
*/
void disconnect(SOCKET s)
{
    uint16 count;
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("disconnect()\r\n");
  CMF_Fflush();
#endif
  IINCHIP_WRITE(Sn_CR(s),Sn_CR_DISCON);

  /* +20071122[chungs]:wait to process the command... */
  while( IINCHIP_READ(Sn_CR(s)) ){
      //Todo: CMF_SleepTask();
      count++;
      if(count > 1000){
          break;
      }
  }
  /* ------- */
}


/**
@brief  This function used to send the data in TCP mode
@return 1 for success else 0.
*/
uint16 send(
  SOCKET s,     /**< the socket index */
  const uint8 * buf,  /**< a pointer to data */
  uint16 len    /**< the data size to be send */
  )
{
  uint8 status=0;
  uint16 ret=0;
  uint16 freesize=0;
  uint16 count=0;

  if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
  else ret = len;

  status = IINCHIP_READ(Sn_SR(s));
  if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
  {
    ret = 0;
  }

  freesize = getSn_TX_FSR(s);
  if(len > freesize){
      ret = freesize;
  }

      // copy data
  send_data_processing(s, (uint8 *)buf, ret);
  IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);


  count = 0;
  while( IINCHIP_READ(Sn_CR(s)) )
  {
    count++;
    if(count > 1000)
    {
        break;
    }
  }
  /* ------- */

    /* +2008.01 bj */
#ifdef __DEF_IINCHIP_INT__
    while ( (getISR(s) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
#else
    while ( (IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
#endif
    {
        /* m2008.01 [bj] : reduce code */
        if ( IINCHIP_READ(Sn_SR(s)) == SOCK_CLOSED )
        {
            wclose(s);
            return 0;
        }

        count++;
        if(count > 1000)
        {
            break;
        }
    }

  IINCHIP_WRITE(Sn_IR(s), Sn_IR_SEND_OK);

    return ret;
}


/**
@brief  This function is an application I/F function which is used to receive the data in TCP mode.
    It continues to wait for data as much as the application wants to receive.

@return received data size for success else 0.
*/
uint16 recv(
  SOCKET s,   /**< socket index */
  uint8 *buf,  /**< a pointer to copy the data to be received */
  uint16 len)  /**< the data size to be read */
{
    uint16 ret;
    uint16 _available_data_size;
    static uint8 toggle = 0;


    _available_data_size = getSn_RX_RSR(s);
    if(_available_data_size <= 0)
    {
        return 0;
    }
    else
    {

        if(len > _available_data_size)
        {
            len = _available_data_size;
        }

        recv_data_processing(s, buf, len);
        IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);

        while(IINCHIP_READ(Sn_CR(s)));

        toggle = 0;
        ret = len;
    }

    return ret;
}


/**
@brief  This function is an application I/F function which is used to send the data for other then TCP mode.
    Unlike TCP transmission, The peer's destination address and the port is needed.

@return This function return send data size for success else -1.
*/
uint16 sendto(
  SOCKET s,     /**< socket index */
  const uint8 * buf,  /**< a pointer to the data */
  uint16 len,     /**< the data size to send */
  uint8 * addr,     /**< the peer's Destination IP address */
  uint16 port)   /**< the peer's destination port number */
{
//  uint8 status=0;
//  uint8 isr=0;
  uint16 ret=0;
  uint16 count;

   if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;
  if
    (
      ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
      ((port == 0x00)) ||(ret == 0)
    )
  {

     ret = 0;

  }
  else
  {
    IINCHIP_WRITE(Sn_DIPR0(s),addr[0]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 1),addr[1]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 2),addr[2]);
    IINCHIP_WRITE((Sn_DIPR0(s) + 3),addr[3]);
    IINCHIP_WRITE(Sn_DPORT0(s),(uint8)((port & 0xff00) >> 8));
    IINCHIP_WRITE((Sn_DPORT0(s) + 1),(uint8)(port & 0x00ff));
    // copy data
    send_data_processing(s, (uint8 *)buf, ret);
    // m2012.03.13 [ys]: set/clear subnet for ARP Errata
    clearSUBR();
    IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);
    /* +20071122[chungs]:wait to process the command... */
    count = 0;
    while( IINCHIP_READ(Sn_CR(s)) )
    {
      //Todo: CMF_SleepTask();
      count++;
      if(count > 1000)
      {
          break;
      }
    }

    /* ------- */

    /* +2008.01 bj */

     while ( (IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )

    {

        if (IINCHIP_READ(Sn_IR(s)) & Sn_IR_TIMEOUT)

      {


          IINCHIP_WRITE(Sn_IR(s), (Sn_IR_SEND_OK | Sn_IR_TIMEOUT)); /* clear SEND_OK & TIMEOUT */

      return 0;
      }
    }


     IINCHIP_WRITE(Sn_IR(s), Sn_IR_SEND_OK);

         applySUBR();
    }
    return ret;
}



/**
@brief  This function is an application I/F function which is used to receive the data in other then
  TCP mode. This function is used to receive UDP, IP_RAW and MAC_RAW mode, and handle the header as well.

@return This function return received data size for success else -1.
*/
uint16 recvfrom(
  SOCKET s,   /**< the socket number */
  uint8 * buf,  /**< a pointer to copy the data to be received */
  uint16 len,   /**< the data size to read */
  uint8 * addr,   /**< a pointer to store the peer's IP address */
  uint16 *port  /**< a pointer to store the peer's port number. */
  )
{
  uint8 head[8];
  uint16 data_len=0;
  uint16 ptr=0;

  if ( len > 0 )
  {
    ptr = IINCHIP_READ(Sn_RX_RD0(s));
    ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);

    switch (IINCHIP_READ(Sn_MR(s)) & 0x07)
    {
    case Sn_MR_UDP :
        read_data(s, (uint16)ptr, head, 0x08);
        ptr += 8;
        // read peer's IP address, port number.
        addr[0] = head[0];
        addr[1] = head[1];
        addr[2] = head[2];
        addr[3] = head[3];
        *port = head[4];
        *port = (*port << 8) + head[5];
        data_len = head[6];
        data_len = (data_len << 8) + head[7];

#ifdef __DEF_IINCHIP_DBG__
        CMF_Printf("UDP msg arrived\r\n");
        CMF_Fflush();
        CMF_Printf("source Port : %d\r\n", *port);
        CMF_Fflush();
        CMF_Printf("source IP : %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
        CMF_Fflush();
#endif

      read_data(s, (uint16)ptr, buf, data_len); // data copy.
      ptr += data_len;

      IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
      IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));
        break;

    case Sn_MR_IPRAW :
        read_data(s, (uint16)ptr, head, 0x06);
        ptr += 6;

        addr[0] = head[0];
        addr[1] = head[1];
        addr[2] = head[2];
        addr[3] = head[3];
        data_len = head[4];
        data_len = (data_len << 8) + head[5];

#ifdef __DEF_IINCHIP_DBG__
        CMF_Printf("IP RAW msg arrived\r\n");
        CMF_Fflush();
        CMF_Printf("source IP : %d.%d.%d.%d\r\n", addr[0], addr[1], addr[2], addr[3]);
        CMF_Fflush();
#endif
      read_data(s, (uint16)ptr, buf, data_len); // data copy.
      ptr += data_len;

      IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
      IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));
        break;
    case Sn_MR_MACRAW :
        read_data(s,(uint16)ptr,head,2);
        ptr+=2;
        data_len = head[0];
        data_len = (data_len<<8) + head[1] - 2;

        read_data(s,(uint16) ptr,buf,data_len);
        ptr += data_len;
        IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
        IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));

#ifdef __DEF_IINCHIP_DGB__
      CMF_Printf("MAC RAW msg arrived\r\n");
      CMF_Fflush();
      CMF_Printf("dest mac=%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
      CMF_Fflush();
      CMF_Printf("src  mac=%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\r\n",buf[6],buf[7],buf[8],buf[9],buf[10],buf[11]);
      CMF_Fflush();
      CMF_Printf("type    =%.2X%.2X\r\n",buf[12],buf[13]);
      CMF_Fflush();
#endif
      break;

    default :
        break;
    }
    IINCHIP_WRITE(Sn_CR(s),Sn_CR_RECV);

    /* +20071122[chungs]:wait to process the command... */
    while( IINCHIP_READ(Sn_CR(s)) ) ;
    /* ------- */
  }
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("recvfrom() end ..\r\n");
  CMF_Fflush();
#endif
  return data_len;
}


uint16 igmpsend(SOCKET s, const uint8 * buf, uint16 len)
{
  uint8 status=0;
//  uint8 isr=0;
  uint16 ret=0;

#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("igmpsend()\r\n");
  CMF_Fflush();
#endif
   if (len > getIINCHIP_TxMAX(s)) ret = getIINCHIP_TxMAX(s); // check size not to exceed MAX size.
   else ret = len;

  if  (ret == 0)
  {
     ;
#ifdef __DEF_IINCHIP_DBG__
  CMF_Printf("%d Fail[%d]\r\n",len);
  CMF_Fflush();
#endif
  }
  else
  {
    // copy data
    send_data_processing(s, (uint8 *)buf, ret);
    IINCHIP_WRITE(Sn_CR(s),Sn_CR_SEND);
/* +2008.01 bj */
    while( IINCHIP_READ(Sn_CR(s)) ) ;
/* ------- */

/* +2008.01 bj */
#ifdef __DEF_IINCHIP_INT__
    while ( (getISR(s) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
#else
     while ( (IINCHIP_READ(Sn_IR(s)) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
#endif
    {
      status = IINCHIP_READ(Sn_SR(s));
#ifdef __DEF_IINCHIP_INT__
        if (getISR(s) & Sn_IR_TIMEOUT)
#else
        if (IINCHIP_READ(Sn_IR(s)) & Sn_IR_TIMEOUT)
#endif
      {
#ifdef __DEF_IINCHIP_DBG__
        CMF_Printf("igmpsend fail.\r\n");
        CMF_Fflush();
#endif
         /* in case of igmp, if send fails, then socket closed */
         /* if you want change, remove this code. */
         wclose(s);
         /* ----- */

        return 0;
      }
    }

/* +2008.01 bj */
#ifdef __DEF_IINCHIP_INT__
      putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
     IINCHIP_WRITE(Sn_IR(s), Sn_IR_SEND_OK);
#endif
   }
  return ret;
}
