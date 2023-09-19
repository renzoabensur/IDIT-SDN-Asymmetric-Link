/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE SOFTWARE PROVIDER OR DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \file
 *         ...
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-net SDN Network drivers
 * @{
 *
 */

#ifndef SDN_NETWORK_H_
#define SDN_NETWORK_H_

#include "sdn-protocol.h"
#include "linkaddr.h"

#ifndef SDN_ND
#ifdef SDN_CONF_ND
#define SDN_ND SDN_CONF_ND
#else /* SDN_CONF_ND */
#define SDN_ND dummy_sdn_nd
#endif /* SDN_CONF_ND */
#endif /* SDN_ND */

#ifndef SDN_CD
#ifdef SDN_CONF_CD
#define SDN_CD SDN_CONF_CD
#else /* SDN_CONF_CD */
#define SDN_CD dummy_sdn_cd
#endif /* SDN_CONF_CD */
#endif /* SDN_CD */

#ifndef SDN_FLOW_TABLES_SIZE
#ifdef SDN_CONF_FLOW_TABLES_SIZE
#define SDN_FLOW_TABLES_SIZE SDN_CONF_FLOW_TABLES_SIZE
#else /* SDN_CONF_FLOW_TABLES_SIZE */
#define SDN_FLOW_TABLES_SIZE 15
#endif /* SDN_CONF_FLOW_TABLES_SIZE */
#endif /* SDN_FLOW_TABLES_SIZE */

/**
 * The structure of a neighbor discovery in ContikiSDN.
 */
struct sdn_nd {
  /** Initialize the ND protocol */
  void (* init)(void *nd_event, uint8_t);
  /** Receive an ND packet */
  void (* input)(uint8_t* packet_ptr, uint16_t packet_len);
  /** Update estimate transmission to neighbor */
  void (* update_neighbor)(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx);
};

/**
 * The structure of a controller discovery in ContikiSDN.
 */
struct sdn_cd {
  /** Initialize the CD protocol */
  void (* init)(void *cd_event, uint8_t);
  /** get current next hop towards the controller */
  void (* get)(sdnaddr_t*);
  /** Receive a CD packet */
  void (* input)(uint8_t* packet_ptr, uint16_t packet_len);
};

/**
 * \brief      Sends the data to MAC address destination
 * \param data The array buffer of data to send
 * \param len  The size of data to send.
 * \param dest The MAC address destination.
 *
 *             This function sends the data througth of the MAC layer
 *
 */
void sdn_network_send(void *data, uint16_t len, linkaddr_t dest);
/*---------------------------------------------------------------------------*/

// #define SDN_NETWORK sdn_driver
extern const struct network_driver sdn_network_driver;
extern const struct sdn_nd SDN_ND;
extern const struct sdn_cd SDN_CD;

#endif //SDN_NETWORK_H_

/** @} */
/** @} */
/** @} */
