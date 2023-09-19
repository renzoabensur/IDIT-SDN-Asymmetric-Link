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
 * \section naive controller discovery for directed networks
 */

#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-debug.h"
#include "sdn-constants.h"
#include "sdn-send-packet.h"
#include "sdn-packetbuf.h"
#include "contiki.h"
#include "lib/memb.h"
#include "lib/list.h"

//extern struct process sdn_core_process;
void (* controller_discovery_event)();

#define SDN_NDCD_LIFETIME
#define SDN_NDCD_MAX_LIFETIME 3
#define LOCAL_DEBUG 0

struct reachability_info {
  struct reachability_info *next;
  sdnaddr_t who;
  sdnaddr_t next_hop;
  uint8_t metric;
#ifdef SDN_NDCD_LIFETIME
  uint8_t lifetime;
#endif
};

struct reach_in_msg {
  sdnaddr_t who;
  sdnaddr_t next_hop;
  uint8_t metric;
} __attribute__((packed));

sdnaddr_t current_next_hop;
uint8_t current_metric;

MEMB(ri_memb, struct reachability_info, 20);
LIST(ri_list);

PROCESS(ndcd_process, "CD process");
uint8_t ndcd_send_beacon();

uint8_t is_c;

void ndcd_init(void *cd_event, uint8_t is_controller) {
  SDN_DEBUG ("Controller Discovery initialized.\n");

  controller_discovery_event = cd_event;

  list_init(ri_list);
  is_c = is_controller;
  sdnaddr_copy(&current_next_hop, sdn_addr_broadcast);
  current_metric = 0xFF;
  process_start(&ndcd_process, NULL);
}

PROCESS_THREAD(ndcd_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer my_event_timer;

  while (1) {
    etimer_set(&my_event_timer, 10 * CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&my_event_timer));
    ndcd_send_beacon();
  }

  PROCESS_END();
}

uint8_t ndcd_send_beacon() {
  uint8_t * buf;
  struct reachability_info *r;
  uint8_t i;
  SDN_DEBUG("ndcd_send_beacon()\n");
  buf = (uint8_t *) sdn_packetbuf_pool_get();
  if (buf && list_length(ri_list)) {
    i = sizeof(sdn_header_t);
    buf[i] = 0; //list_length(ri_list);
    i++;
    for (r = list_head(ri_list); r != NULL; r = list_item_next(r)) {
#if LOCAL_DEBUG
      SDN_DEBUG("ndcd_send_beacon(): who: ");
      sdnaddr_print(&r->who);
      SDN_DEBUG("\nndcd_send_beacon(): next_hop:");
      sdnaddr_print(&r->next_hop);
      SDN_DEBUG("\nndcd_send_beacon(): metric: %d\n", r->metric);
#ifdef SDN_NDCD_LIFETIME
      SDN_DEBUG("ndcd_send_beacon(): lifetime: %d\n", r->lifetime);
#endif //SDN_NDCD_LIFETIME
#endif //LOCAL_DEBUG

#ifdef SDN_NDCD_LIFETIME
      if (r->lifetime) {
        r->lifetime--;
#endif
        buf[sizeof(sdn_header_t)] ++;
        sdnaddr_copy((sdnaddr_t*)(buf + i), &r->who);
        i += sizeof(sdnaddr_t);
        sdnaddr_copy((sdnaddr_t*)(buf + i), &r->next_hop);
        i += sizeof(sdnaddr_t);
        *(buf + i) = r->metric;
        i ++;
#ifdef SDN_NDCD_LIFETIME
      }
/*
      else if (is_c == SDN_NO) {
        SDN_DEBUG("ndcd_send_beacon(): removing entry\n");
        list_remove(ri_list, r);
        memb_free(&ri_memb, r);
      }
*/
#endif
    }
    if (buf[sizeof(sdn_header_t)]) {
      return sdn_send_cd((struct sdn_packetbuf *)buf, i);
    }

  }
  sdn_packetbuf_pool_put((struct sdn_packetbuf*) buf);
  return SDN_ERROR;
}

void ndcd_get(sdnaddr_t* next_hop) {
  sdnaddr_copy(next_hop, &current_next_hop);
}

void ndcd_input(uint8_t* packet_ptr, uint16_t packet_len) {
  struct reachability_info *r;
#ifdef SDN_NDCD_LIFETIME
  struct reachability_info *r_zerolife;
#endif
  uint8_t i;
  uint8_t found;
  // packet_ptr contains header, followed by the number of entries and a list of
  // "(who, next_hop) to reach controller"

  // TODO: check if packet_len matches content

#if 0
  // if it is the controller, and the message did not originate from the
  // controller himself, add/update the entry to the ri_list
  if (is_c == SDN_YES && sdnaddr_cmp(&SDN_HEADER(packet_ptr)->source, &sdn_node_addr) != SDN_EQUAL) {
    found = 0;
    for (r = list_head(ri_list); r != NULL; r = list_item_next(r)) {
      if (sdnaddr_cmp(&r->who, &SDN_HEADER(packet_ptr)->source) == SDN_EQUAL) {
        // atualiza ri_list
        SDN_DEBUG("ndcd_input(): updated2\n");
        found = 1;
        sdnaddr_copy(&r->next_hop, &sdn_node_addr);
        r->metric = 0;
        break;
      }
    }

    if (!found) {
      r = memb_alloc(&ri_memb);
      if (r) {
        sdnaddr_copy(&r->who, &SDN_HEADER(packet_ptr)->source);
        SDN_DEBUG("ndcd_added(): who2: ");
        sdnaddr_print(&SDN_HEADER(packet_ptr)->source);
        sdnaddr_copy(&r->next_hop, &sdn_node_addr);
        r->metric = 0;
#ifdef SDN_NDCD_LIFETIME
        r->lifetime = SDN_NDCD_MAX_LIFETIME;
#endif
        list_add(ri_list, r);
        // for (r = list_head(ri_list); r != NULL; r = list_item_next(r)) {
        //   SDN_DEBUG("ndcd_added(): who: ");
        //   sdnaddr_print(&r->who);
        //   SDN_DEBUG("\n ndcd_added(): next_hop:");
        //   sdnaddr_print(&r->next_hop);
        //   SDN_DEBUG("\n");
        // }
      }
    }
  }
#endif

  if ( is_c == SDN_NO || (is_c == SDN_YES && sdnaddr_cmp(&SDN_HEADER(packet_ptr)->source, &sdn_node_addr) == SDN_EQUAL) ) {
    packet_ptr += sizeof(sdn_header_t);
    SDN_DEBUG("ndcd_input(): registries: %d\n", *packet_ptr);

  for (i = 0; i < *packet_ptr; i ++) {
    sdnaddr_t* who = &(((struct reach_in_msg*)(packet_ptr + 1))[i].who);// &((sdnaddr_t*) (packet_ptr + 1))[i*2];
    sdnaddr_t* next_hop = &(((struct reach_in_msg*)(packet_ptr + 1))[i].next_hop); //&((sdnaddr_t*) (packet_ptr + 1))[i*2+1];
    uint8_t metric = ((struct reach_in_msg*)(packet_ptr + 1))[i].metric;
#if LOCAL_DEBUG
    SDN_DEBUG("%p %p %p %d\n", packet_ptr, who, next_hop, sizeof(struct reach_in_msg));
    SDN_DEBUG("ndcd_input(): who: ");
    sdnaddr_print(who);
    SDN_DEBUG("\nndcd_input(): next_hop:");
    sdnaddr_print(next_hop);
    SDN_DEBUG("\nndcd_input(): metric %d\n", metric);
#endif //LOCAL_DEBUG

    // check if there is a entry related to us
    if (sdnaddr_cmp(who, &sdn_node_addr) == SDN_EQUAL) {
      if (is_c == SDN_YES){
        continue;
      }

      // post an event only if the next hop is different from the current
      if (sdnaddr_cmp(next_hop, &current_next_hop) != SDN_EQUAL && metric < current_metric) {
        sdnaddr_copy(&current_next_hop, next_hop);
        current_metric = metric;
        controller_discovery_event();
        //process_post(&sdn_core_process, SDN_EVENT_CD_FOUND, 0);

        // continue; <<<< testar isso (TODO)
      }
    }

    // updates our list
    found = 0;
    r_zerolife = NULL;
    for (r = list_head(ri_list); r != NULL; r = list_item_next(r)) {
      if (sdnaddr_cmp(&r->who, who) == SDN_EQUAL) {
        // update ri_list
        found = 1;

        if (r->metric > metric) {
          SDN_DEBUG("ndcd_input(): updated (old: %d new: %d)\n", r->metric , metric);
#ifdef SDN_NDCD_LIFETIME
          SDN_DEBUG("ndcd_input(): reseting lifetime\n");
          r->lifetime = SDN_NDCD_MAX_LIFETIME;
#endif
          r->metric = metric;
          sdnaddr_copy(&r->next_hop, next_hop);
        }
        break;
      }
#ifdef SDN_NDCD_LIFETIME
      if (r->lifetime == 0 && r_zerolife == NULL)
        r_zerolife = r;
#endif
    }

    if (!found) {
#ifndef SDN_NDCD_LIFETIME
      r = memb_alloc(&ri_memb);
#else
      r = memb_alloc(&ri_memb) ?: r_zerolife;
      if (r == NULL) {
        for (r_zerolife = r = list_head(ri_list); r != NULL; r = list_item_next(r)) {
          if (r_zerolife->lifetime > r->lifetime)
            r_zerolife = r;
        }
        if (r_zerolife->lifetime != SDN_NDCD_MAX_LIFETIME) {
          r = r_zerolife;
          SDN_DEBUG("ndcd_input(): will replace ");
          sdnaddr_print(&r->who);
          SDN_DEBUG("\n");
        } else {
          r = NULL;
        }
      }
#endif
      if (r) {
        SDN_DEBUG("ndcd_input(): new\n");
        sdnaddr_copy(&r->who, who);
        sdnaddr_copy(&r->next_hop, next_hop);
        r->metric = metric;
#ifdef SDN_NDCD_LIFETIME
        SDN_DEBUG("ndcd_input(): setting lifetime\n");
        r->lifetime = SDN_NDCD_MAX_LIFETIME;
#endif
        list_add(ri_list, r);
      } else {
        printf("TACHEIO\n");
      }
    }

  }

  }

}

const struct sdn_cd naive_directed_cd = {
  ndcd_init,
  ndcd_get,
  ndcd_input
};
/** @} */
/** @} */
/** @} */
