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
 *         Header file for the SDN Send
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-send SDN Send
 * @{
 *
 * The sdn-send module is responsible for the send of SDN packets.
 *
 */

#ifndef SDN_SEND_H
#define SDN_SEND_H

#include "sdn-addr.h"
#include "sdn-common-send.h"

#ifdef SDN_CONF_TX_RELIABILITY
#define SDN_TX_RELIABILITY SDN_CONF_TX_RELIABILITY
#else // SDN_CONF_TX_RELIABILITY
#define SDN_TX_RELIABILITY 0
#endif // SDN_CONF_TX_RELIABILITY

#ifdef SDN_CONF_1HOP_RELIABILITY
#define SDN_1HOP_RELIABILITY SDN_CONF_1HOP_RELIABILITY
#else // SDN_CONF_1HOP_RELIABILITY
#define SDN_1HOP_RELIABILITY 0
#endif // SDN_CONF_1HOP_RELIABILITY

#ifndef SDN_MAX_RETRANSMIT
#ifdef SDN_CONF_MAX_RETRANSMIT
#define SDN_MAX_RETRANSMIT SDN_CONF_MAX_RETRANSMIT
#else // SDN_CONF_MAX_RETRANSMIT
#define SDN_MAX_RETRANSMIT 5
#endif // SDN_CONF_MAX_RETRANSMIT
#endif //SDN_MAX_RETRANSMIT

#define SDN_RETX_TIME_S 60

#if 0
#define ALLOC_AND_ENQUEUE(queue_ptr, packet_ptr, packet_len) \
  queue_ptr = sdn_packetbuf_pool_get(); \
  \
  if (queue_ptr == NULL) { \
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n"); \
  } else { \
    memcpy(queue_ptr, packet_ptr, packet_len); \
    INCREMENT_SEQ_NO(packet_ptr, queue_ptr); \
    if (sdn_send_queue_enqueue((uint8_t *) queue_ptr, packet_len) != SDN_SUCCESS) { \
      sdn_packetbuf_pool_put((struct sdn_packetbuf *)queue_ptr); \
      SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    } else { \
      sdn_send_down_once(); \
    } \
  }

#define ENQUEUE_AND_SEND(packet_ptr, packet_len) \
  INCREMENT_SEQ_NO(packet_ptr, NULL); \
  if (sdn_send_queue_enqueue((uint8_t *) packet_ptr, packet_len) != SDN_SUCCESS) { \
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)packet_ptr); \
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    return SDN_ERROR; \
  } else { \
    SDN_METRIC_TX(packet_ptr); \
    sdn_send_down_once(); \
    return SDN_SUCCESS; \
  }
#endif

void sdn_send_init();

void sdn_send_down_once();

uint8_t sld_rcv_flow_req(flowid_t *flow);
uint8_t sld_check_flow_req(flowid_t *flow);
uint8_t sld_rcv_addr_req(sdnaddr_t *addr);
uint8_t sld_rcv_ack(uint8_t pkt_type, uint8_t seq_no);
uint8_t sld_purge(uint8_t pkt_type);

#if SDN_TX_RELIABILITY
uint8_t has_data_flow_request_retransmission(flowid_t flowid);
uint8_t has_control_flow_request_retransmission(sdnaddr_t address);
uint8_t sld_await_ack(uint8_t* packet, uint8_t len);
#endif //SDN_TX_RELIABILITY

#endif //SDN_SEND_H

/** @} */
/** @} */
/** @} */
