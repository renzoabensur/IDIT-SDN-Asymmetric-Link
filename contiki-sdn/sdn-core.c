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

#include "sdn-core.h"
#include "control-flow-table.h"
#include "data-flow-table.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "sdn-send.h"
#include "sdn-send-packet.h"
#include "sdn-debug.h"
#include "sdn-process-packets.h"
#include "sdn-packetbuf.h"
#include "sdn-neighbor-table.h"
#include "sdn-queue.h"
#include "sdn-unknown-route.h"

#ifdef ENERGY_MANAGER
#include "energy-manager.h"
#endif //ENERGY_MANAGER

#ifdef DEMO
#include "leds.h"
#endif

#if defined(SDN_ENABLED_NODE) || defined(SDN_CONTROLLER_NODE)
#include "leds.h"
#endif

void (* sdn_callback_receive)(uint8_t*, uint16_t, sdnaddr_t*, uint16_t);

void nd_event();
void cd_event();

uint8_t sdn_state;
uint8_t should_send_nd;

PROCESS(sdn_core_process, "SDN Core process");

void sdn_init(void *callback) {
  memcpy(&sdn_node_addr, &linkaddr_node_addr, sizeof(sdnaddr_t));
  SDN_DEBUG ("Node SDN Address:");
  sdnaddr_print(&sdn_node_addr);
  SDN_DEBUG ("\n");

  sdn_send_init();
  sdn_recv_queue_init();
  sdn_state = SDN_AWAITING_CONTROLLER_ROUTE;
  sdn_callback_receive = callback;
  sdn_network_driver.init();
  sdn_dataflow_init();
  sdn_controlflow_init();
  sdn_unknown_route_init();
  should_send_nd = SDN_NO;
  SDN_ND.init(nd_event, SDN_NO);
  SDN_CD.init(cd_event, SDN_NO);

#ifdef ENERGY_MANAGER
  em_init();
#endif //ENERGY_MANAGER

// #if defined(SDN_ENABLED_NODE) || defined(SDN_CONTROLLER_NODE)
//   leds_toggle(LEDS_GREEN); //Led Blue
//   leds_toggle(LEDS_YELLOW); //Led Red
//   leds_toggle(LEDS_RED); //Led Green
// #endif

  process_start(&sdn_core_process, NULL);
}

void nd_event() {

    if (sdn_state == SDN_STEADY_STATE) {
      sdn_send_neighbor_report();
    } else {
      should_send_nd = SDN_YES;
    }
}

void cd_event() {

  sdnaddr_t temp_addr;

  SDN_DEBUG("Controller found.\n");
  /* Change SDN internal state */
  sdn_state = SDN_STEADY_STATE;
  /* Set controller route according to underlying CD protocol */
  #ifdef DEMO
  leds_on(LEDS_RED);
  #endif
  SDN_CD.get(&temp_addr);
  sdn_dataflow_insert(SDN_CONTROLLER_FLOW, temp_addr, SDN_ACTION_FORWARD);
  if (should_send_nd == SDN_YES) {
    should_send_nd = SDN_NO;
    sdn_send_neighbor_report();
  }
}

void sdn_send(uint8_t *data, uint16_t len, flowid_t flowid) {

  sdn_send_data(data, len, flowid);
}

uint8_t sdn_register_flowid(flowid_t flowid) {
  if (sdn_dataflow_insert(flowid, sdn_node_addr, SDN_ACTION_RECEIVE) == SDN_SUCCESS) {
    return sdn_send_register_flowid(flowid);
  } else {
    return SDN_ERROR;
  }
}

PROCESS_THREAD(sdn_core_process, ev, data)
{
  PROCESS_BEGIN();

  //sdnaddr_t temp_addr;
  sdnaddr_t *addr_ptr;
  sdn_recv_queue_data_t *recv_data;

  SDN_DEBUG ("SDN Core initialized!\n");

  printf("########## DEFINES ##########\n");

#ifdef DEBUG_SDN
  printf("DEBUG_SDN (defined)\n");
#else
  printf("DEBUG_SDN (undefined)\n");
#endif

  printf("SDNADDR_SIZE %u\n", SDNADDR_SIZE);

  printf("SDN_MAX_PACKET_SIZE %u\n", SDN_MAX_PACKET_SIZE);

  printf("SDN_SEND_QUEUE_SIZE %u\n", SDN_SEND_QUEUE_SIZE);

  printf("SDN_RECV_QUEUE_SIZE %u\n", SDN_RECV_QUEUE_SIZE);

  printf("SDN_CONF_TX_RELIABILITY %u\n", SDN_CONF_TX_RELIABILITY);

  printf("SDN_RETX_TIME_S %u\n", SDN_RETX_TIME_S);

#ifdef COLLECT_CONF_ANNOUNCEMENTS
  printf ("COLLECT_CONF_ANNOUNCEMENTS %u\n", COLLECT_CONF_ANNOUNCEMENTS);
#else
  printf ("COLLECT_CONF_ANNOUNCEMENTS not defined\n");
#endif

  printf("########## ##########\n");

  while(1) {
    PROCESS_WAIT_EVENT();

    //SDN_DEBUG ("Receiving event at SDN core process: %02x.\n", ev);

    switch(ev){
      // case SDN_EVENT_ND_CHANGE:
      //   //SDN_DEBUG("Neighbor Change.\n");
      //   if (sdn_state == SDN_STEADY_STATE) {
      //     sdn_send_neighbor_report();
      //   } else {
      //     should_send_nd = SDN_YES;
      //   }
      // break;
      // case SDN_EVENT_CD_FOUND:
      //   SDN_DEBUG("Controller found.\n");
      //   /* Change SDN internal state */
      //   sdn_state = SDN_STEADY_STATE;
      //   /* Set controller route according to underlying CD protocol */
      //   #ifdef DEMO
      //   leds_on(LEDS_RED);
      //   #endif
      //   SDN_CD.get(&temp_addr);
      //   sdn_dataflow_insert(SDN_CONTROLLER_FLOW, temp_addr, SDN_ACTION_FORWARD);
      //   if (should_send_nd == SDN_YES) {
      //     should_send_nd = SDN_NO;
      //     sdn_send_neighbor_report();
      //   }
      // break;
      case SDN_EVENT_NEW_PACKET:
        recv_data = sdn_recv_queue_dequeue();
        if (recv_data != NULL) {
#ifdef MANAGEMENT
          addr_ptr = sdn_treat_packet_receiv(recv_data->data, recv_data->len, recv_data->time);
#else
          addr_ptr = sdn_treat_packet_receiv(recv_data->data, recv_data->len, 0);
#endif
          if (addr_ptr != NULL) {
            sdn_packetbuf* sdn_packet;

#ifdef MANAGEMENT
            ALLOC_AND_ENQUEUE(sdn_packet, recv_data->data, recv_data->len, recv_data->time);
#else
            ALLOC_AND_ENQUEUE(sdn_packet, recv_data->data, recv_data->len, 0);
#endif

          }
          sdn_packetbuf_pool_put((struct sdn_packetbuf *)recv_data->data);
        }

        if (!sdn_recv_queue_empty()) {
          process_post(&sdn_core_process, SDN_EVENT_NEW_PACKET, 0);
        }
      break;
      case PROCESS_EVENT_TIMER:
        SDN_DEBUG ("Untreated PROCESS_EVENT_TIMER.\n");
      break;
      default:
        SDN_DEBUG ("Unknown event.\n");
      break;
    }

  }

  PROCESS_END();
}
