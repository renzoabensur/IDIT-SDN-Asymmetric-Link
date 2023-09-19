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
 * \defgroup sdn-cd Controller Discovery
 * @{
 *
 * \section symmcheck-cd Controller discovery that build a tree and checks if links are bidirectional
 */

#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-debug.h"
#include "contiki.h"
#include "lib/list.h"

#include "sdn-packetbuf.h"
#include "sdn-send-packet.h"
#include "sdn-neighbor-table.h"
#include "sdn-serial.h"

#define METRIC_INC 1
#define CD_LOCAL_EVENT 0xF0
#define MAX_METRIC 0xFFFF
#define DEFAULT_BEACON_INTERVAL 10 //seconds
#define MAX_BEACON_INTERVAL 600 //seconds

#define RIGHT_AWAY 0
#define RESET_INTERVAL 1


/* Local variables */
uint8_t is_c;
uint8_t last_neighbor_num;
sdnaddr_t next_hop_local;
uint16_t to_c_metric;
uint16_t beacon_interval;

void (* controller_discovery_event)();

struct symmcheck_cd_pkt {
  sdn_header_t  header;
  uint16_t      metric;
  uint8_t       num_of_neighbors;

} __attribute__((packed));

PROCESS(cd_process, "SDN CD process");

uint8_t send_cd_beacon();

void symmcheck_cd_init(void *cd_event, uint8_t is_controller) {
  SDN_DEBUG ("Symmcheck Controller Discovery initialized!\n");

  controller_discovery_event = cd_event;
  is_c = is_controller;
  last_neighbor_num = 0;
  if (is_c == SDN_YES) {
    to_c_metric = 0;
  } else {
    to_c_metric = MAX_METRIC;
  }

  sdnaddr_getbyte(&sdn_node_addr, 0, (uint8_t*)&beacon_interval);
  beacon_interval %= 10;
  beacon_interval += DEFAULT_BEACON_INTERVAL;

  process_start(&cd_process, NULL);
}

PROCESS_THREAD(cd_process, ev, data)
{
  PROCESS_BEGIN();
  static struct etimer my_event_timer;

  etimer_set(&my_event_timer, beacon_interval * CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();
    if (etimer_expired(&my_event_timer)) {
      if (last_neighbor_num != sdn_neighbor_table_count() || to_c_metric == MAX_METRIC) {
        SDN_DEBUG("symmcheck_cd_process: sending beacon due to timer %d %d\n", last_neighbor_num, sdn_neighbor_table_count());
        last_neighbor_num = sdn_neighbor_table_count();
        send_cd_beacon();
      }
      beacon_interval *= 2;
      if (beacon_interval > MAX_BEACON_INTERVAL) {
        beacon_interval = MAX_BEACON_INTERVAL;
      }
      etimer_set(&my_event_timer, beacon_interval * CLOCK_SECOND);
    }
    if (ev == CD_LOCAL_EVENT) {
      SDN_DEBUG("symmcheck_cd_process: sending beacon due to other event %x (data %x)\n", ev, data);

      if ((uint16_t) data == RIGHT_AWAY) {
        send_cd_beacon();
      } else if ( (uint16_t) data == RESET_INTERVAL) {
        sdnaddr_getbyte(&sdn_node_addr, 0, (uint8_t*)&beacon_interval);
        beacon_interval %= 10;
        beacon_interval += DEFAULT_BEACON_INTERVAL;
      } else {
        SDN_DEBUG("symmcheck_cd_process: Unknown post data\n");
      }
      etimer_set(&my_event_timer, beacon_interval * CLOCK_SECOND);
    }
  }
  PROCESS_END();
}

void symmcheck_cd_get(sdnaddr_t* next_hop) {
  sdnaddr_copy(next_hop, &next_hop_local);
}

void symmcheck_cd_input(uint8_t* packet_ptr, uint16_t packet_len) {
  struct symmcheck_cd_pkt *cd_pkt = (struct symmcheck_cd_pkt *) packet_ptr;
  uint8_t i, found;

  SDN_DEBUG("symmcheck_cd_input\n")

  found = 0;
  for (i = 0; i < cd_pkt->num_of_neighbors; i++) {
    SDN_DEBUG("symmcheck_cd_input: i %d", i);
    sdnaddr_print( ((sdnaddr_t*) &packet_ptr[sizeof(struct symmcheck_cd_pkt) + i*SDNADDR_SIZE]));
    SDN_DEBUG("\n");
    if (sdnaddr_cmp(&sdn_node_addr, (sdnaddr_t*) &packet_ptr[sizeof(struct symmcheck_cd_pkt) + i*SDNADDR_SIZE]) == SDN_EQUAL) {
      SDN_DEBUG("symmcheck_cd_input: found!\n");
      found = 1;
      break;
    }
  }

  if (found) {
    if (cd_pkt->metric + METRIC_INC < to_c_metric && cd_pkt->metric + METRIC_INC > cd_pkt->metric) {
      SDN_DEBUG("symmcheck_cd_input: updating metric and next hop\n");
      sdnaddr_copy(&next_hop_local, &SDN_HEADER(packet_ptr)->source);
      to_c_metric = cd_pkt->metric + METRIC_INC;
      if (to_c_metric >= MAX_METRIC)
        to_c_metric = MAX_METRIC - 1;
      process_post(&cd_process, CD_LOCAL_EVENT, RIGHT_AWAY);
      controller_discovery_event();
    }
    if (cd_pkt->metric == MAX_METRIC && to_c_metric != MAX_METRIC) {
      // TODO: it would be better to unicast packet to the neighbor
      SDN_DEBUG("symmcheck_cd_input: neighbor cant reach the controller... lets help it\n");
      // this is a trick to force a beacon transmission in the next timer expiration
      // and void all neighbors to transmit a packet at the same time
      last_neighbor_num = SDN_NEIGHBOR_TABLE_SIZE + 1;
      process_post(&cd_process, CD_LOCAL_EVENT, (void *) RESET_INTERVAL);
    }
  }

}

uint8_t send_cd_beacon() {
  uint8_t *buf;
  uint8_t len;
  struct sdn_neighbor_entry *ne;

  buf = (uint8_t *) sdn_packetbuf_pool_get();
  if (buf) {
    ((struct symmcheck_cd_pkt *) buf)->metric = to_c_metric;
    ((struct symmcheck_cd_pkt *) buf)->num_of_neighbors = sdn_neighbor_table_count();
    len = sizeof(struct symmcheck_cd_pkt);
    for (ne = sdn_neighbor_table_head(); ne != NULL; ne = list_item_next(ne)) {
      sdnaddr_copy((sdnaddr_t*)(buf + len), &ne->neighbor_addr);
      len += SDNADDR_SIZE;
    }
    return sdn_send_cd((struct sdn_packetbuf *)buf, len);
  }
  return SDN_ERROR;
}

const struct sdn_cd symmcheck_sdn_cd = {
  symmcheck_cd_init,
  symmcheck_cd_get,
  symmcheck_cd_input
};
/** @} */
/** @} */
/** @} */


  #if 0
  if (is_c == SDN_YES) {
    sdn_serial_packet_t serial_pkt;
    sdn_neighbor_report_list_t nrl;
    uint8_t packet_len = sizeof(sdn_neighbor_report_t);

    MAKE_SDN_HEADER_PTR( ((sdn_neighbor_report_t *)serial_pkt.payload), SDN_PACKET_NEIGHBOR_REPORT, SDN_DEFAULT_TTL);
    ((sdn_neighbor_report_t *)serial_pkt.payload)->flowid = SDN_CONTROLLER_FLOW;
    ((sdn_neighbor_report_t *)serial_pkt.payload)->num_of_neighbors = 1;

    // copy_info
    sdnaddr_copy(&nrl.neighbor, &sdn_node_addr);
    nrl.etx = METRIC_INC;
    memcpy(((sdn_neighbor_report_t *)serial_pkt.payload) + 1, (uint8_t *) &nrl, sizeof(nrl));
    packet_len += sizeof(sdn_neighbor_report_list_t);

    memcpy(serial_pkt.payload, packet_ptr, packet_len);
    // Setting serial packet address as the MAC sender.
    sdnaddr_copy(&serial_pkt.header.node_addr, &SDN_HEADER(packet_ptr)->source);
    serial_pkt.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO;
    serial_pkt.header.payload_len = packet_len;
    sdn_serial_send(&serial_pkt);
  } else
  #endif
