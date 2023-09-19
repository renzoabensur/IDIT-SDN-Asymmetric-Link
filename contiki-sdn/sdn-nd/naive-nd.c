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
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-nd Neighbor Discovery
 * @{
 *
 * \section naive-nd Naive neighbor discovery
 */

#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-neighbor-table.h"
#include "sdn-debug.h"
#include "sdn-packetbuf.h"
#include "sdn-send.h"
#include "sdn-queue.h"
#include "sdn-send-packet.h"
#include "contiki.h"
#include "string.h"

void (* neighbor_discovery_event)();

void naive_nd_send_beacon();

PROCESS(naive_nd_process, "SDN Naive ND process");

void naive_nd_init(void *neighbor_event, uint8_t is_controller) {
  SDN_DEBUG ("Naive Neighbor Discovery initialized!\n");

  neighbor_discovery_event = neighbor_event;

  process_start(&naive_nd_process, NULL);
}

PROCESS_THREAD(naive_nd_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer my_event_timer;
  static uint8_t t;
  SDN_DEBUG ("Naive ND process started\n");
  sdnaddr_getbyte(&sdn_node_addr, 0, &t);
  t %= 10;
  t += 20;

  while (1) {
    etimer_set(&my_event_timer, t * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&my_event_timer));
    naive_nd_send_beacon();
  }

  PROCESS_END();
}

void naive_nd_send_beacon() {
  struct sdn_packetbuf * buf;
  SDN_DEBUG("naive_nd_send_beacon()\n");
  buf = sdn_packetbuf_pool_get();
  sdn_send_nd(buf, sizeof(sdn_header_t));
}

void naive_nd_input(uint8_t* packet_ptr, uint16_t packet_len) {
  struct sdn_neighbor_entry *n;
  SDN_DEBUG("naive_nd_input() from:");
  sdnaddr_print( &((sdn_header_t*)packet_ptr)->source );
  SDN_DEBUG("\n");
  sdn_neighbor_table_print();
  n = sdn_neighbor_table_get(((sdn_header_t*)packet_ptr)->source);
  if (!n) {
    n = sdn_neighbor_table_insert(((sdn_header_t*)packet_ptr)->source, NULL);
    n->metric = 1;
    neighbor_discovery_event();
    //process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);
  }
}

void naive_link_estimate_update(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx) {

}

const struct sdn_nd naive_sdn_nd = {
  naive_nd_init,
  naive_nd_input,
  naive_link_estimate_update
};

/** @} */
/** @} */
/** @} */
