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

#include "sdn-unknown-route.h"
#include "sdn-queue.h"
#include "sdn-packetbuf.h"
#include "sdn-constants.h"
#include "sdn-send-packet.h"
#include "sdn-debug.h"
#include "string.h"

#ifndef SDN_CONTROLLER_PC
#include "sdn-send.h"
#else
#include "sdn-serial-send.h"
#endif

#define SDN_UNKNOWN_ROUTE_ASK
#define SDN_UNKNOWN_ROUTE_KEEP
// #define SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY

extern uint8_t sdn_state;

void store_packet(uint8_t * packet, uint16_t len, uint32_t time);

#ifdef SDN_UNKNOWN_ROUTE_KEEP
#define SDN_UNKNOWN_ROUTE_KEEP_MAX 10
sdn_packetbuf* keep_array[SDN_UNKNOWN_ROUTE_KEEP_MAX];
uint16_t len_array[SDN_UNKNOWN_ROUTE_KEEP_MAX];
uint32_t time_array[SDN_UNKNOWN_ROUTE_KEEP_MAX];
#endif //SDN_UNKNOWN_ROUTE_KEEP

#ifdef SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY
PROCESS(sdn_unknown_rote_process, "SDN unknown route process");

PROCESS_THREAD(sdn_unknown_rote_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer my_event_timer;

  while (1) {
    etimer_set(&my_event_timer, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&my_event_timer));
    //TODO resend requests
  }

  PROCESS_END();
}
#endif //SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY

void sdn_unknown_route_init() {
  SDN_DEBUG("Unknown route module init! ");
#ifdef SDN_UNKNOWN_ROUTE_ASK
  SDN_DEBUG("SDN_UNKNOWN_ROUTE_ASK ");
#endif
#ifdef SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY
  SDN_DEBUG("SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY ");
#endif
#ifdef SDN_UNKNOWN_ROUTE_KEEP
  SDN_DEBUG("SDN_UNKNOWN_ROUTE_KEEP");
#endif
  SDN_DEBUG("\n");

#ifdef SDN_UNKNOWN_ROUTE_ASK_REPEATEDLY
  process_start(&sdn_unknown_rote_process, NULL);
#endif
}

void sdn_unknown_flow(uint8_t * packet, uint16_t len, uint32_t time) {
  // SDN_DEBUG("sdn_unknown_flow ");
  printf("sdn_unknown_flow\n");
  flowid_print(&SDN_GET_PACKET_FLOW(packet));
  flowid_t unknown_flow;
  flowid_copy(&unknown_flow, &SDN_GET_PACKET_FLOW(packet));
  SDN_DEBUG("\n");
#ifdef SDN_UNKNOWN_ROUTE_ASK
#ifdef SDN_UNKNOWN_ROUTE_KEEP
  store_packet(packet, len, time);
#endif //SDN_UNKNOWN_ROUTE_KEEP

  // send a flow request
#ifndef SDN_CONTROLLER_PC
  if (sdn_state != SDN_AWAITING_CONTROLLER_ROUTE)
#endif
    sdn_send_data_flow_request(unknown_flow);
#endif //SDN_UNKNOWN_ROUTE_ASK
}

void sdn_unknown_dest(uint8_t * packet, uint16_t len) {
  SDN_DEBUG("sdn_unknown_dest ");
  sdnaddr_print(&SDN_GET_PACKET_ADDR(packet));
  sdnaddr_t unknown_addr;
  sdnaddr_copy(&unknown_addr, &SDN_GET_PACKET_ADDR(packet));
  SDN_DEBUG("\n");
#ifdef SDN_UNKNOWN_ROUTE_ASK
#ifdef SDN_UNKNOWN_ROUTE_KEEP
  store_packet(packet, len, 0);
#endif //SDN_UNKNOWN_ROUTE_KEEP

  // send a route request
#ifndef SDN_CONTROLLER_PC
  if (sdn_state != SDN_AWAITING_CONTROLLER_ROUTE)
#endif // SDN_CONTROLLER_PC
    sdn_send_control_flow_request(unknown_addr);
#endif // SDN_UNKNOWN_ROUTE_ASK
}

void sdn_solve_unknown_flow(struct data_flow_entry *dfe) {
  SDN_DEBUG("sdn_solve_unknown_flow ");
  flowid_print(&dfe->flowid);
  SDN_DEBUG("\n");
#ifdef SDN_UNKNOWN_ROUTE_KEEP
  uint8_t i;
  for (i = 0; i < SDN_UNKNOWN_ROUTE_KEEP_MAX; i++) {
    if ( keep_array[i] != NULL
          && SDN_ROUTED_BY_FLOWID(keep_array[i])
          && flowid_cmp(&dfe->flowid, &SDN_GET_PACKET_FLOW(keep_array[i])) == SDN_EQUAL
       ) {
      SDN_DEBUG("sdn_solve_unknown_flow: match\n");
      if (sdn_send_queue_enqueue((uint8_t *) keep_array[i], len_array[i], time_array[i]) != SDN_SUCCESS) {
        sdn_packetbuf_pool_put(keep_array[i]);
        SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
      } else {
        sdn_send_down_once();
      }
      keep_array[i] = NULL;
    }
//     if (keep_array[i] != NULL) {
//       SDN_DEBUG("sdn_solve_unknown_flow: did not match\n");
//       flowid_print(&SDN_GET_PACKET_FLOW(keep_array[i]));
//       print_packet(keep_array[i], len_array[i]);
//       SDN_DEBUG("\n");
//     }
  }
#endif //SDN_UNKNOWN_ROUTE_KEEP
}

void sdn_solve_unknown_dest(struct control_flow_entry *cfe) {
  SDN_DEBUG("sdn_solve_unknown_dest ");
  sdnaddr_print(&cfe->dest_addr);
  SDN_DEBUG("\n");
#ifdef SDN_UNKNOWN_ROUTE_KEEP
  uint8_t i;
  for (i = 0; i < SDN_UNKNOWN_ROUTE_KEEP_MAX; i++) {
    if ( keep_array[i] != NULL
          && SDN_ROUTED_BY_ADDR(keep_array[i])
          && sdnaddr_cmp(&cfe->dest_addr, &SDN_GET_PACKET_ADDR(keep_array[i])) == SDN_EQUAL
       ) {
      if (sdn_send_queue_enqueue((uint8_t *) keep_array[i], len_array[i], time_array[i]) != SDN_SUCCESS) {
        sdn_packetbuf_pool_put(keep_array[i]);
        SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
      } else {
          sdn_send_down_once();
      }
      keep_array[i] = NULL;
    }
  }
#endif //SDN_UNKNOWN_ROUTE_KEEP
}

#ifdef SDN_UNKNOWN_ROUTE_KEEP
void store_packet(uint8_t * packet, uint16_t len, uint32_t time) {
  uint8_t i;
  for (i = 0; i < SDN_UNKNOWN_ROUTE_KEEP_MAX; i++) {
    if (keep_array[i] == NULL) {
      keep_array[i] = sdn_packetbuf_pool_get();
      if (keep_array[i]) {
        memcpy(keep_array[i], packet, len);
        len_array[i] = len;
        time_array[i] = time;
      }
      break;
    }
  }
}
#endif //SDN_UNKNOWN_ROUTE_KEEP
