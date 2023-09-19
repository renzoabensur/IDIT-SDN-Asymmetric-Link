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
#include "sdn-receive.h"
#include "contiki.h"
#include "string.h"
#include "lib/list.h"
#include "net/packetbuf.h"

#define MAX_BEACON_INTERVAL 120
#define RECEIVE_BEACON_UNIT 10 //seconds
#define DEFAULT_MISSABLE_BEACONS 3
// #define INITIAL_METRIC_VALUE 1.5
// #define LINK_ESTIMATE_UNIT 8
// #define ALPHA 3
// #define ETX_MIN_DELTA 4
// #define ETX_MIN_RELATIVE 30

#define INFO(n) ((struct improved_naive_nd_info *)(n->extra_info))

void (* neighbor_discovery_event)();

void improved_naive_nd_send_beacon();

void decrease_neighbors_liveness();

static uint16_t beacon_interval; // in seconds

static struct etimer send_beacon_timer, receive_beacon_timer;

PROCESS(improved_naive_nd_process, "SDN Improved Naive ND process");

struct improved_naive_nd_info {
  uint8_t missable_beacons_count;
  uint8_t expected_time_to_beacon; // time in RECEIVE_BEACON_UNIT's
  uint8_t time_to_beacon_count;
  uint8_t last_received_seq_no;
  uint16_t last_informed_metric;
  uint16_t history;
  uint8_t count_failures;
  uint8_t count_successes;
};

/* Declaring MEMB to hold additional neighborhood info */
MEMB(neighbor_addinfo_memb, struct improved_naive_nd_info, SDN_NEIGHBOR_TABLE_SIZE);

void improved_naive_nd_init(void *neighbor_event, uint8_t is_controller) {
  SDN_DEBUG ("Improved Naive Neighbor Discovery initialized!\n");

  neighbor_discovery_event = neighbor_event;
  sdn_overhear_to_nd();

  sdnaddr_getbyte(&sdn_node_addr, 0, (uint8_t*)&beacon_interval);
  beacon_interval %= 10;
  beacon_interval += RECEIVE_BEACON_UNIT;

  process_start(&improved_naive_nd_process, NULL);
}

PROCESS_THREAD(improved_naive_nd_process, ev, data)
{
  PROCESS_BEGIN();
  etimer_set(&send_beacon_timer, beacon_interval * CLOCK_SECOND);
  etimer_set(&receive_beacon_timer, RECEIVE_BEACON_UNIT * CLOCK_SECOND);

  while (1) {
    PROCESS_WAIT_EVENT();
    if (etimer_expired(&send_beacon_timer)) {
      // doubling beacon interval
      beacon_interval = beacon_interval << 1;
      if (beacon_interval > MAX_BEACON_INTERVAL) beacon_interval = MAX_BEACON_INTERVAL;
      etimer_set(&send_beacon_timer, beacon_interval * CLOCK_SECOND);
      improved_naive_nd_send_beacon();
    }
    if (etimer_expired(&receive_beacon_timer)) {
      decrease_neighbors_liveness();
      etimer_set(&receive_beacon_timer, RECEIVE_BEACON_UNIT * CLOCK_SECOND);
    }
  }

  PROCESS_END();
}

void improved_naive_nd_send_beacon() {
  struct sdn_packetbuf * buf;
  SDN_DEBUG("imp_nd: sending beacon\n");
  buf = sdn_packetbuf_pool_get();
  memcpy(&buf->sdn_packet[sizeof(sdn_header_t)], &beacon_interval, sizeof(uint16_t));
  sdn_send_nd(buf, sizeof(sdn_header_t) + sizeof(uint16_t));
}

void decrease_neighbors_liveness() {
  struct sdn_neighbor_entry *n, *next_n;
  struct improved_naive_nd_info *info;

  SDN_DEBUG("imp_nd: decrease_neighbors_liveness() \n");
  for(n = sdn_neighbor_table_head(); n != NULL; ) {
    next_n = list_item_next(n);
    info = n->extra_info;

    (info->time_to_beacon_count)--;
    SDN_DEBUG("imp_nd: \tneighbor addr ");
    sdnaddr_print(&(n->neighbor_addr));
    SDN_DEBUG(" - time_to_beacon_count (%d) \n", info->time_to_beacon_count );
    if (info->time_to_beacon_count == 0) {
      // we should have received a beacon by now
      info->time_to_beacon_count = info->expected_time_to_beacon;
      (info->missable_beacons_count)--;
      SDN_DEBUG("imp_nd: \t\tmissed a beacon\n");
      if (info->missable_beacons_count == 0) {
        // it has been a long time since we last heard from this neighbor,
        // time to evict it
        SDN_DEBUG("imp_nd: \t\tneighbor removed from table\n");
        sdn_neighbor_table_remove_by_pointer(n, &neighbor_addinfo_memb);
        neighbor_discovery_event();
      } else {
        // at least one packet was missed, update lqe
        //TODO
        n->metric *=  1.2;
      }
    }
    n = next_n;
  }
}

void initialize_info(struct sdn_neighbor_entry *n) {
  // lets wait a second packet to estimate the delivery rate
  SDN_DEBUG("imp_nd: initialize_info\n");
  n->metric = 0x111;
  INFO(n)->last_informed_metric = n->metric;
  INFO(n)->history = 0;
  INFO(n)->count_failures = 0;
  INFO(n)->count_successes = 0;
}

void update_info(struct sdn_neighbor_entry *n) {
  uint8_t losses = packetbuf_attr(PACKETBUF_ATTR_MAC_SEQNO) - INFO(n)->last_received_seq_no - 1;
  uint8_t num_bits = sizeof(INFO(n)->history) * 8;
  SDN_DEBUG("imp_nd: losses: %d\n", losses);
  if (losses >= num_bits) {
    // too many losses, we set delivery as the minimum
    INFO(n)->history = (1 << (num_bits - 1));
    INFO(n)->count_failures = num_bits - 1;
    INFO(n)->count_successes = 1;
  } else if (INFO(n)->count_failures + INFO(n)->count_successes + losses + 1 > num_bits) {
    // steady state behavior: removes older entries in the history
    INFO(n)->count_successes += 1;
    INFO(n)->count_failures += losses;
    losses ++;
    while (losses) {
      INFO(n)->count_failures -= ((~ INFO(n)->history) & 0x1);
      INFO(n)->count_successes -= (INFO(n)->history & 0x1);
      INFO(n)->history >>= 1;
      losses -= 1;
    }
    INFO(n)->history = INFO(n)->history | (1 << ((num_bits) - 1));
  } else {
    // still filling the history vector
    INFO(n)->history = INFO(n)->history | (1 << (INFO(n)->count_failures + INFO(n)->count_successes + losses));
    INFO(n)->count_successes += 1;
    INFO(n)->count_failures += losses;
  }

  n->metric = 1 + 0xFF * (INFO(n)->count_failures) / (INFO(n)->count_failures + INFO(n)->count_successes);
  // max metric is 0x100, thus adding 48 should be safe in terms of oveflow
  if ( (n->metric > INFO(n)->last_informed_metric + 48) || (n->metric  + 48 < INFO(n)->last_informed_metric || n->metric == 0x111) ) {
    neighbor_discovery_event();
    SDN_DEBUG("imp_nd: informed! %d -> %d, (%d %d)\n", INFO(n)->last_informed_metric, n->metric, INFO(n)->count_successes, INFO(n)->count_failures);
    INFO(n)->last_informed_metric = n->metric;
  } else {
    SDN_DEBUG("imp_nd: not informed... %d, (%d %d)\n", n->metric, INFO(n)->count_successes, INFO(n)->count_failures);
  }

#if 0
  n->metric = ((losses * LINK_ESTIMATE_UNIT * ALPHA) + n->metric * (LINK_ESTIMATE_UNIT - ALPHA))/LINK_ESTIMATE_UNIT;
  SDN_DEBUG("imp_nd: lqe %d; metric: %d; previous: %d", losses, n->metric, info->last_informed_metric);
  if ( (n->metric < info->last_informed_metric - ETX_MIN_DELTA || n->metric > info->last_informed_metric + ETX_MIN_DELTA) &&
      (n->metric > info->last_informed_metric * (1.0 + ETX_MIN_RELATIVE / 100.0) || n->metric < info->last_informed_metric / (1.0 + ETX_MIN_RELATIVE / 100.0))) {
        SDN_DEBUG(" INFORM ");
        info->last_informed_metric = n->metric;
        // TODO limit the number of events per second
        neighbor_discovery_event();
  }
  SDN_DEBUG("\n");
#endif
}

void improved_naive_nd_input(uint8_t* packet_ptr, uint16_t packet_len) {
  struct sdn_neighbor_entry *n;
  struct improved_naive_nd_info *info;
  sdnaddr_t neighbor_addr;

  SDN_DEBUG("imp_nd: received packet from:");
  sdnaddr_print( &((sdn_header_t*)packet_ptr)->source );
  sdnaddr_print((sdnaddr_t*) packetbuf_addr(PACKETBUF_ADDR_SENDER) );
  SDN_DEBUG(" type: %X, seq_no %d\n", ((sdn_header_t*)packet_ptr)->type, ((sdn_header_t*)packet_ptr)->seq_no);

  if ( ((sdn_header_t*) packet_ptr)->type == SDN_PACKET_ND) {
    // we must check the source address for ND packets, as the packetbuf might have been overwritten
    sdnaddr_copy(&neighbor_addr, &((sdn_header_t*)packet_ptr)->source);
  } else {
    // we must check the packetbuf for overheard packets
    sdnaddr_copy(&neighbor_addr, (sdnaddr_t*) packetbuf_addr(PACKETBUF_ADDR_SENDER));
  }

  n = sdn_neighbor_table_get(neighbor_addr);
  if (!n) {
      n = sdn_neighbor_table_insert(neighbor_addr, &neighbor_addinfo_memb);
    SDN_DEBUG("imp_nd: trying to insert new neighbor\n");
  }
  if (n) {
    info = n->extra_info;
    if ( ((sdn_header_t*) packet_ptr)->type == SDN_PACKET_ND) {
      info->expected_time_to_beacon = *(uint16_t *)(packet_ptr + sizeof(sdn_header_t)); //TODO: improve readability
      // info->expected_time_to_beacon should be in terms of RECEIVE_BEACON_UNIT
      info->expected_time_to_beacon = 1 + info->expected_time_to_beacon / RECEIVE_BEACON_UNIT;
      SDN_DEBUG("imp_nd: expected_time_to_beacon: %d\n", info->expected_time_to_beacon);
    } else if (n->metric == 0) {
      // this means it is a new entry, therefore we have no reference to
      // expected_time_to_beacon, set it to maximum value
      info->expected_time_to_beacon = 1 + MAX_BEACON_INTERVAL / RECEIVE_BEACON_UNIT;
    } else {
      SDN_DEBUG("imp_nd: packet not ND, nor new entry %d\n", n->metric);
    }
    info->time_to_beacon_count = info->expected_time_to_beacon;
    info->missable_beacons_count = DEFAULT_MISSABLE_BEACONS;

    if (n->metric == 0) {
      // new entry
      initialize_info(n);
    } else {
      update_info(n);
    }

    info->last_received_seq_no = packetbuf_attr(PACKETBUF_ATTR_MAC_SEQNO);
  }
  // sdn_neighbor_table_print();
}

void improved_naive_link_estimate_update(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx) {
  SDN_DEBUG("fail_tx: %d\n", fail_tx);
  if (fail_tx == 0)
    etimer_restart(&send_beacon_timer);
}

const struct sdn_nd improved_naive_sdn_nd = {
  improved_naive_nd_init,
  improved_naive_nd_input,
  improved_naive_link_estimate_update
};

/** @} */
/** @} */
/** @} */
