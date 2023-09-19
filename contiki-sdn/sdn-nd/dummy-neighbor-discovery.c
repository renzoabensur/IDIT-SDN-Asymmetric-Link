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
 * \section dummy-nd Dummy neighbor discovery
 */
#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-neighbor-table.h"
#include "sdn-debug.h"
#include "contiki.h"

//extern struct process sdn_core_process;

void (* neighbor_discovery_event)();

PROCESS(dummy_nd_process, "SDN Dummy ND process");

void dummy_nd_init(void *neighbor_event, uint8_t is_controller) {
  sdnaddr_t neighbor;
  struct sdn_neighbor_entry *n;
  uint8_t b;

  neighbor_discovery_event = neighbor_event;

  sdnaddr_getbyte(&sdn_node_addr, 0, &b);
  sdnaddr_copy(&neighbor, &sdn_node_addr);

  sdnaddr_setbyte(&neighbor, 0, b + 1);
  n = sdn_neighbor_table_insert(neighbor, NULL);
  if (n != NULL)
    n->metric = 0xF;

  sdnaddr_setbyte(&neighbor, 0, b - 1);
  n = sdn_neighbor_table_insert(neighbor, NULL);
  if (n != NULL)
    n->metric = 0xF;

  process_start(&dummy_nd_process, NULL);
  SDN_DEBUG ("Dummy Neighbor Discovery initialized!\n");
}

PROCESS_THREAD(dummy_nd_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer my_event_timer;
  static uint16_t t = 5;
  uint8_t b;
  sdnaddr_getbyte(&sdn_node_addr, 0, &b);
  SDN_DEBUG("Dummy Neighbor Discovery: %d\n", b);

  while (1) {
    etimer_set(&my_event_timer, t * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&my_event_timer));
    sdnaddr_getbyte(&sdn_node_addr, 0, &b);
    t = t * 2 + b;
    neighbor_discovery_event();
    //process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);
  }

  PROCESS_END();
}

void dummy_nd_input(uint8_t* packet_ptr, uint16_t packet_len) {

}

void dummy_link_estimate_update(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx) {

}

const struct sdn_nd dummy_sdn_nd = {
  dummy_nd_init,
  dummy_nd_input,
  dummy_link_estimate_update
};

/** @} */
/** @} */
/** @} */
