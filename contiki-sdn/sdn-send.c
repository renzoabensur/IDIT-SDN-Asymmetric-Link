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

#include "sdn-send.h"
#include "sdn-network.h"
#include "sdn-addr.h"
#include "net/netstack.h"
#include "sdn-debug.h"
#include "sdn-packetbuf.h"
#include "string.h"
#include "sdn-queue.h"
#include "sdn-constants.h"
#include "sdn-neighbor-table.h"
#include "sdn-process-packets.h"
#include "sdn-send-packet.h"
#include "sys/ctimer.h"
#include "random.h"
#include "manage-info.h"

#ifdef SDN_CONTROLLER_NODE
#include "sdn-serial.h"
#else
#include "data-flow-table.h"
#include "control-flow-table.h"
#endif

#if SDN_TX_RELIABILITY
// Enables a simple end-to-end reliability mechanism for some control packets
// Packets are retransmitted at every SDN_RETX_TIME_S seconds
// Up to SDN_MAX_TX_WINDOW packet are stored in the retransmission queue

#define SDN_MAX_TX_WINDOW 4

uint8_t sld_w[SDN_MAX_TX_WINDOW][SDN_MAX_PACKET_SIZE]; // packet buffer
uint8_t sld_l[SDN_MAX_TX_WINDOW]; // packet lengths
struct ctimer sld_ct[SDN_MAX_TX_WINDOW];
uint8_t sld_count; // helper variable

uint8_t sld_remove_element(uint8_t index);
#endif //SDN_TX_RELIABILITY

typedef enum {
  SDN_SEND_IDLE,
  SDN_SEND_ACK_WAIT,
  SDN_SEND_TRICKLE_TIME,
  SDN_SEND_FULL_WINDOW,
  SDN_SEND_BUSY
} sdn_send_status_t;

#if SDN_1HOP_RELIABILITY
#define TRICKLE_TIME_WINDOW 7

struct ctimer trickle_timer;

uint8_t count_retransmit;

void trickle_time_start();

void trickle_time_callback(void *ptr);
#endif //SDN_1HOP_RELIABILITY

PROCESS(sdn_send_process, "SDN Send process");

sdn_send_status_t sdn_send_status;

void sdn_send_init() {

  // random_init((unsigned short) sdn_node_addr.u8);
  sdn_send_queue_init();
  process_start(&sdn_send_process, NULL);
#if SDN_TX_RELIABILITY
  for (sld_count = 0; sld_count < SDN_MAX_TX_WINDOW; sld_count++)
    sld_l[sld_count] = 0;
  sld_count = 0;
#endif

}

void sdn_send_down_once() {

  if (sdn_send_status == SDN_SEND_IDLE && !sdn_send_queue_empty()) {
    sdn_send_status = SDN_SEND_BUSY;
    process_post(&sdn_send_process, SDN_EVENT_NEW_PACKET, 0);
  //} else {
    //SDN_DEBUG ("Defering send due status not SDN_SEND_IDLE.\n");
  }
}

/**
 * \brief This function is called by sdn-network when packet is delivery by radio.
 *
 * \param ptr pointer to the sent packet
 * \param status The status of the packet delivery.
 * \param transmissions Number of transmission attempts
 */
void sdn_send_done(void *ptr, int status, int transmissions);

PROCESS_THREAD(sdn_send_process, ev, data)
{
  PROCESS_BEGIN();

  while (1) {
    PROCESS_WAIT_EVENT();

      if(ev == SDN_EVENT_NEW_PACKET) {

        sdn_send_queue_data_t *queue_data = sdn_send_queue_head();

#if defined(SDN_ENABLED_NODE) && defined(MANAGEMENT)
        const sdnaddr_t *dest = sdn_treat_packet(queue_data->data, queue_data->len, queue_data->time);
#elif defined(SDN_ENABLED_NODE)
        const sdnaddr_t *dest = sdn_treat_packet(queue_data->data, queue_data->len, 0);
#else
        const sdnaddr_t *dest = &queue_data->last_next_hop;
#endif

        if (dest != NULL) {
          sdn_send_status = SDN_SEND_ACK_WAIT;

#ifdef SDN_ENABLED_NODE
          /* Keeps the last next hop address from attempt of the packet delivery. */
          memcpy(&queue_data->last_next_hop, dest, sizeof(sdnaddr_t));
#endif

          sdn_header_t *packet_header = (sdn_header_t *) queue_data->data;

          if (memcmp(&packet_header->source, &sdn_node_addr, sizeof(sdnaddr_t)) == 0) {

// #if defined (SDN_ENABLED_NODE) && defined (MANAGEMENT)
#ifdef MANAGEMENT
            uint8_t *ver_pkt = queue_data->data;

            switch(ver_pkt[0]) {
              case SDN_PACKET_DATA:
                manag_update(0,1);
                break;
            }

            if(ver_pkt[0] != SDN_PACKET_DATA && ver_pkt[0] != SDN_PACKET_MNGT_CONT_SRC_RTD && ver_pkt[0] != SDN_PACKET_MNGT_NODE_DATA){
              manag_update(0,4);
              // printf("Ver pkt %u\n", ver_pkt[0]);
            }
            // code to monitor number of control packets sent
#endif
             SDN_DEBUG ("Sending Packet to ");
          } else {
// Thamires' original code
// #if defined (SDN_ENABLED_NODE) && defined (MANAGEMENT)
//             uint8_t *ver_pkt = queue_data->data;

//             switch(ver_pkt[0]) {
//               case SDN_PACKET_DATA:
//                 manag_update(queue_data->time,2);
//                 break;
//             }
// #endif

/*--------------------------------------------------*/            
           
            SDN_DEBUG ("Forwarding Packet to ");
          }

          sdnaddr_print(&sdn_send_queue_head()->last_next_hop);
          print_packet(queue_data->data, sdn_send_queue_head()->len);

          sdn_network_send((void *) queue_data->data, queue_data->len, *dest);
        } else {
          SDN_DEBUG ("send(): dest == NULL\n");
          sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();
          sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);
          sdn_send_status = SDN_SEND_IDLE;
          sdn_send_down_once();
        }
    }
  }
  PROCESS_END();
}

void sdn_send_done(void *ptr, int status, int transmissions) {

  uint8_t index;

  if (memcmp((uint8_t *) ptr, sdn_send_queue_head()->data, sdn_send_queue_head()->len) != 0) {
    SDN_DEBUG_ERROR ("Ignoring Ack for different packet!\n");
    return;
  }

  if (status == MAC_TX_OK) {

    sdn_header_t *packet_header = (sdn_header_t *) ptr;

#ifdef SDN_CONTROLLER_NODE
    if (! (packet_header->type == SDN_PACKET_ND || packet_header->type == SDN_PACKET_CD ) ) {

      sdn_serial_send_ack();
    }
#endif

    if (memcmp(&packet_header->source, &sdn_node_addr, sizeof(sdnaddr_t)) == 0) {

      SDN_DEBUG ("Packet Sent to ");
    } else {

      SDN_DEBUG ("Packet Forwarded to ");
    }

    SDN_DEBUG ("[%02X", sdn_send_queue_head()->last_next_hop.u8[0]);
    for (index = 1; index < sizeof(sdnaddr_t); index++) {
      SDN_DEBUG (":%02X", sdn_send_queue_head()->last_next_hop.u8[index]);
    }
    SDN_DEBUG ("] ");
    print_packet((uint8_t *) ptr, sdn_send_queue_head()->len);

#if SDN_1HOP_RELIABILITY
    count_retransmit = 0;
#endif //SDN_1HOP_RELIABILITY

    SDN_ND.update_neighbor(&sdn_send_queue_head()->last_next_hop, transmissions, 0);

  } else {

    SDN_ND.update_neighbor(&sdn_send_queue_head()->last_next_hop, transmissions, 1);

#if SDN_1HOP_RELIABILITY
    // increase count_retransmit
    SDN_DEBUG("Activating trickle time: ");
    switch (status) {
      case MAC_TX_COLLISION:
        SDN_DEBUG("MAC_TX_COLLISION\n");
        break;
      case MAC_TX_NOACK:
        SDN_DEBUG("MAC_TX_NOACK\n");
        count_retransmit++;
        break;
      case MAC_TX_ERR:
        SDN_DEBUG("MAC_TX_ERR\n");
        count_retransmit++;
       break;
      default:
        SDN_DEBUG_ERROR("Unknown error (%d)\n", status);
        break;
    }

    if (count_retransmit == SDN_MAX_RETRANSMIT) {
      SDN_DEBUG("Reached SDN_MAX_RETRANSMIT, giving up\n");

      count_retransmit = 0;
#endif //SDN_1HOP_RELIABILITY

#ifdef SDN_CONTROLLER_NODE
      sdn_serial_send_nack();
#endif

#if SDN_1HOP_RELIABILITY
    // else clause of previous if (count_retransmit == SDN_MAX_RETRANSMIT)
    } else {
      trickle_time_start();
      return;
    }
#endif //SDN_1HOP_RELIABILITY

  }

  sdn_send_queue_data_t* queue_data = sdn_send_queue_dequeue();

  #if SDN_TX_RELIABILITY
      // packet is out in the network, lets wait an ACK from the destination or activate re-send for reliability end-to-end
  sld_await_ack(queue_data->data, queue_data->len);
  #endif

  if (queue_data->data==NULL) SDN_DEBUG_ERROR("PACKET DEQUEUE NULL\n");

  sdn_packetbuf_pool_put((sdn_packetbuf*) queue_data->data);

  sdn_send_status = SDN_SEND_IDLE;
  sdn_send_down_once();
}

#if SDN_1HOP_RELIABILITY
void trickle_time_start() {

  uint16_t r;

  r = random_rand() % TRICKLE_TIME_WINDOW;
  r += TRICKLE_TIME_WINDOW;

  float trickle_time = ((float) r) / 1000;

  sdn_send_status = SDN_SEND_TRICKLE_TIME;
  ctimer_set(&trickle_timer, (unsigned long) (trickle_time * CLOCK_SECOND), trickle_time_callback, NULL);
}

void trickle_time_callback(void *ptr) {

  sdn_send_status = SDN_SEND_IDLE;
  sdn_send_down_once();
}
#endif //SDN_1HOP_RELIABILITY

#if SDN_TX_RELIABILITY
uint8_t has_data_flow_request_retransmission(flowid_t flowid){

  uint8_t i;
  uint8_t ret = 0;

  for (i=0; i<SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_DATA_FLOW_REQUEST && \
          flowid_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_data_flow_setup_t, flowid), &flowid) == SDN_EQUAL && \
          sld_l[i] > 0) {
      ret = 1;
    }
  }

  return ret;
}

uint8_t has_control_flow_request_retransmission(sdnaddr_t address){

  uint8_t i;
  uint8_t ret = 0;

  for (i=0; i<SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_CONTROL_FLOW_REQUEST && \
          sdnaddr_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_control_flow_request_t, address), &address) == SDN_EQUAL && \
          sld_l[i] > 0) {
      ret = 1;
    }
  }

  return ret;
}

// callback function on timer expiration
void sld_f(void *ptr) {
  uint8_t i = (uint8_t) (uint16_t) ptr;
  SDN_DEBUG ("Retransmitting packet %d \n", i);
  sdn_packetbuf* queue_ptr;
  if (sdn_send_status == SDN_SEND_FULL_WINDOW) {
    sdn_send_status = SDN_SEND_IDLE;
  }

  //TODO: this is a trick to avoid duplicate detection by other nodes
  SDN_METRIC_RXTX(sld_w[i], "TXB");
  // if (!SDN_HEADER(sld_w[i])->seq_no)
  //   SDN_HEADER(sld_w[i])->seq_no = 127;
  // else
  //   SDN_HEADER(sld_w[i])->seq_no *= 2;
  ALLOC_AND_ENQUEUE(queue_ptr, sld_w[i], sld_l[i], 0);
  SDN_METRIC_RXTX(sld_w[i], "TXA");

  if (queue_ptr) {
    // queue_ptr is not null, packet has been re-enqueued
    sld_remove_element(i);
  } else {
    // transmission queue is full, re-schedule timer
    ctimer_set(&sld_ct[i], (SDN_RETX_TIME_S * CLOCK_SECOND / 2) + random_rand() % (SDN_RETX_TIME_S * CLOCK_SECOND / 2), sld_f, (void *) (uint16_t) i);
  }
}

uint8_t sld_remove_element(uint8_t index) {
  if (index >= 0 && index < SDN_MAX_TX_WINDOW) {
    SDN_DEBUG("sld_remove_element: Removed element %d, type %d\n", index, SDN_HEADER(sld_w[index])->type);
    sld_l[index] = 0;
    ctimer_stop(&sld_ct[index]);
    if (index < sld_count)
      sld_count = index;
    return SDN_SUCCESS;
  }
  return SDN_ERROR;
}

uint8_t sld_rcv_ack(uint8_t pkt_type, uint8_t seq_no) {
  uint8_t i;
  for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == pkt_type && SDN_HEADER(sld_w[i])->seq_no == seq_no) {
      return sld_remove_element(i);
    }
  }
  return SDN_ERROR;
}

uint8_t sld_purge(uint8_t pkt_type) {
  uint8_t i;
  for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == pkt_type) {
      return sld_remove_element(i);
    }
  }
  return SDN_ERROR;
}

uint8_t sld_rcv_flow_req(flowid_t *flow) {

  uint8_t i;
  for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_DATA_FLOW_REQUEST && \
        flowid_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_data_flow_setup_t, flowid), flow) == SDN_EQUAL)
    {
      return sld_remove_element(i);
    }
  }
  return SDN_ERROR;
}

uint8_t sld_check_flow_req(flowid_t *flow) {
  uint8_t i;
  for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_DATA_FLOW_REQUEST && \
        flowid_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_data_flow_setup_t, flowid), flow) == SDN_EQUAL)
    {
      return SDN_YES;
    }
  }
  return SDN_NO;
}

uint8_t sld_rcv_addr_req(sdnaddr_t *addr) {
  uint8_t i;
  for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
    if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_CONTROL_FLOW_REQUEST && \
        sdnaddr_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_control_flow_request_t, address), addr) == SDN_EQUAL)
    {
      return sld_remove_element(i);
    }
  }
  return SDN_ERROR;
}

uint8_t sld_await_ack(uint8_t* packet, uint8_t len) {
  // Reliability is available only for:
  // neighbor report, energy report, flow requests and register flow id

  uint8_t i;
  uint8_t is_redundant;

  if (! (SDN_HEADER(packet)->type == SDN_PACKET_NEIGHBOR_REPORT || \
      SDN_HEADER(packet)->type == SDN_PACKET_DATA_FLOW_REQUEST || \
      SDN_HEADER(packet)->type == SDN_PACKET_CONTROL_FLOW_REQUEST || \
      SDN_HEADER(packet)->type == SDN_PACKET_REGISTER_FLOWID)
      || sdnaddr_cmp(&sdn_node_addr, &SDN_HEADER(packet)->source) != SDN_EQUAL
  ) {
    return SDN_SUCCESS;
  }

  is_redundant = 0;

  // checks for address, packet type and redundant packets in the queue
  if (sdnaddr_cmp(&sdn_node_addr, &SDN_HEADER(packet)->source) != SDN_EQUAL) {
    return SDN_SUCCESS;
  } else if (SDN_HEADER(packet)->type == SDN_PACKET_NEIGHBOR_REPORT) {
    // look for another NR packet in queue and replace it
    for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
      if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_NEIGHBOR_REPORT && sld_l[i] > 0) {
        memcpy(sld_w[i], packet, len);
        sld_l[i] = len;
        is_redundant = 1;
      }
    }
  } else if (SDN_HEADER(packet)->type == SDN_PACKET_DATA_FLOW_REQUEST) {
    // should be added to the queue only if there is not another request
    // for the same flow
    for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
      if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_DATA_FLOW_REQUEST && \
          flowid_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_data_flow_setup_t, flowid), &SDN_PACKET_GET_FIELD(packet, sdn_data_flow_request_t, flowid)) == SDN_EQUAL && \
          sld_l[i] > 0)
      {
        is_redundant = 1;
      }
    }
  } else if (SDN_HEADER(packet)->type == SDN_PACKET_CONTROL_FLOW_REQUEST) {
    for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
      if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_CONTROL_FLOW_REQUEST && \
          sdnaddr_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_control_flow_request_t, address), &SDN_PACKET_GET_FIELD(packet, sdn_control_flow_request_t, address)) == SDN_EQUAL && \
          sld_l[i] > 0)
      {
        is_redundant = 1;
      }
    }
  } else if (SDN_HEADER(packet)->type == SDN_PACKET_REGISTER_FLOWID) {
    for (i = 0; i < SDN_MAX_TX_WINDOW; i++) {
      if (SDN_HEADER(sld_w[i])->type == SDN_PACKET_REGISTER_FLOWID && \
          flowid_cmp(&SDN_PACKET_GET_FIELD(sld_w[i], sdn_data_flow_request_t, flowid), &SDN_PACKET_GET_FIELD(packet, sdn_data_flow_request_t, flowid)) == SDN_EQUAL && \
          sld_l[i] > 0)
      {
        is_redundant = 1;
      }
    }
  }

  if (is_redundant) {
    return SDN_SUCCESS;
  }

  // look for an empty buffer position
  while (sld_count != SDN_MAX_TX_WINDOW && sld_l[sld_count] != 0) {
    sld_count ++;
  }

  if (sld_count == SDN_MAX_TX_WINDOW) {
    SDN_DEBUG_ERROR ("Retransmission window is full, ignoring await_ack for ");
    print_packet(packet, len);
    return SDN_ERROR;
  }

  memcpy(sld_w[sld_count], packet, len);
  sld_l[sld_count] = len;
  ctimer_set(&sld_ct[sld_count], (SDN_RETX_TIME_S * CLOCK_SECOND / 2) + (random_rand() % (SDN_RETX_TIME_S * CLOCK_SECOND / 2)), sld_f, (void *) (uint16_t) sld_count);
  sld_count ++;

  return SDN_SUCCESS;
}
#endif //SDN_TX_RELIABILITY
