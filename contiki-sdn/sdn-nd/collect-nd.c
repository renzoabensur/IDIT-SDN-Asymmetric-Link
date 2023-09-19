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

#include "contiki.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
//#include "net/rime/collect.h"
//#include "net/rime/collect-neighbor.h"
#include "collect-nd.h"
#include <string.h>
#include <stdio.h>
#include "sdn-debug.h"

enum sdn_neighbor_status {
  SDN_NEIGHBOR_STATIC = 0,
  SDN_NEIGHBOR_EDIT = 1,
  SDN_NEIGHBOR_CHANGED = 2
};

// #define SDN_STOP_COLLECT_ND
/*----------------------------------------------*/
//extern struct process sdn_core_process;
/*----------------------------------------------*/
/**
 * tc: it is a structure that needs collect to open the communcation
 * neighbors_copy_list: it creates a list to copy the information from
 * the list of neighbors of collect
 */
struct collect_conn tc;
struct collect_neighbor_list neighbors_copy_list;

void (* neighbor_discovery_event)();

uint8_t sdn_neighbor_table_update();
void sdn_neighbor_table_edit();

/* Declaring MEMB to hold additional neighborhood info */
MEMB(neighbor_addinfo_memb, uint8_t, SDN_NEIGHBOR_TABLE_SIZE);

/**
 *\brief          Initializes the neighbor discovery process
 *\param is_controller SDN_YES if the node is a controller
 *
 */
void neighbors_init(void *neighbor_event, uint8_t is_controller);

/**
 *\brief          Update link estimate from sdn packets
 *\param neighbor Neighbor address from target packet
 *\param num_tx   Number of transmissions
 *\param status   MAC transmission status
 *
 */
void link_estimate_update(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx);

PROCESS(neighbors_discovery, "Neighbors discovery");

/*---------------------------------------------------------------------------*/
/**
 *callback: requirement of collect
 */
static const struct collect_callbacks callbacks;
/*---------------------------------------------------------------------------*/
void neighbors_init(void *neighbor_event, uint8_t is_controller) {

  SDN_DEBUG("Initializing neighbor discovery\n");

  neighbor_discovery_event = neighbor_event;

  sdn_neighbor_table_init();

  /*opens the collect protocol communication*/
  collect_sdn_open(&tc, 130, COLLECT_ROUTER, &callbacks);

  /*set the sink: requirement of collect protocol*/
  if(is_controller == SDN_YES) {
    SDN_DEBUG("I am sink\n");
    collect_sdn_set_sink(&tc, 1);
  }

  process_start(&neighbors_discovery, NULL);
}
/*---------------------------------------------------------------------------*/
void neighbors_close() {

}
/*---------------------------------------------------------------------------*/
void print_neighbors(struct collect_neighbor_list *neighbor_list) {
#ifdef DEBUG_SDN
  struct collect_neighbor *n;
  for(n = list_head(neighbor_list->list); n != NULL; n = list_item_next(n)) {
    SDN_DEBUG("Table 1 Neighbor number %d.%d \n", n->addr.u8[0], n->addr.u8[1]);
  }
#endif
}
/*----------------------------------------------------------------------------*/

uint8_t sdn_neighbors_table_act(struct collect_neighbor_list *neighbor_list) {
  struct collect_neighbor *d;
  struct sdn_neighbor_entry *n;
  uint8_t metric;

  sdn_neighbor_table_edit();

  for(d = list_head(neighbor_list->list); d != NULL; d = list_item_next(d)) {
    //sdn_neighbor_table_insert(d->addr, 100);
    n = sdn_neighbor_table_insert(d->addr, &neighbor_addinfo_memb);
    if (n != NULL) {
      metric = d->le.etx_accumulator;
      uint8_t* extra_info = n->extra_info;
      if ( (metric < n->metric - SDN_NEIGHBOR_ETX_DELTA || metric > n->metric + SDN_NEIGHBOR_ETX_DELTA) &&
          (metric > n->metric * ( 0.5 + SDN_NEIGHBOR_ETX_SENSIBILITY / 100.0 ) || metric < n->metric / ( 0.5 + SDN_NEIGHBOR_ETX_SENSIBILITY / 100.0 ))) {
          n->metric = metric;
          *extra_info = SDN_NEIGHBOR_CHANGED;
      } else {
          *extra_info = SDN_NEIGHBOR_STATIC;
      }
    }
  }

  return sdn_neighbor_table_update();
}
/*----------------------------------------------------------------------------*/

struct collect_conn *collect_pointer() {

  return &tc;
}
/*----------------------------------------------------------------------------*/

uint8_t sdn_neighbor_table_update() {

  uint8_t ret = SDN_ERROR;

  struct sdn_neighbor_entry *neighbor;

  struct sdn_neighbor_entry *neighbor_next;

  for(neighbor = sdn_neighbor_table_head(); neighbor != NULL; ) {

    if(*((uint8_t *)neighbor->extra_info) == SDN_NEIGHBOR_EDIT) {

      neighbor_next = list_item_next(neighbor);

      ret = sdn_neighbor_table_remove_by_pointer(neighbor, &neighbor_addinfo_memb);
      if (ret != SDN_SUCCESS) {
        SDN_DEBUG_ERROR("Error on neighbor removal\n");
      }

      ret = SDN_SUCCESS;

      neighbor = neighbor_next;

    } else {
      if(*((uint8_t *)neighbor->extra_info) == SDN_NEIGHBOR_CHANGED) ret = SDN_SUCCESS;
      neighbor = list_item_next(neighbor);
    }
  }

  return ret;
}

void sdn_neighbor_table_edit() {
  struct sdn_neighbor_entry *n;

  for(n = sdn_neighbor_table_head(); n != NULL; n = list_item_next(n)) {
    *((uint8_t *)n->extra_info) = SDN_NEIGHBOR_EDIT;
  }
}

/*----------------------------------------------------------------------------*/

PROCESS_THREAD(neighbors_discovery, ev, data){

  PROCESS_BEGIN();

  static struct etimer collect_nd_timer;
  // static struct etimer stop_nd_timer;
  //static int nd_changed = 0, nd_should_send = 0;
// #ifdef SDN_STOP_COLLECT_ND
//   static int count = 0;
// #endif

  /*the programm waits 2 minutes while the network is settled (this time could change)*/
  etimer_set(&collect_nd_timer, CLOCK_SECOND * 30);
// #ifdef SDN_STOP_COLLECT_ND
//   etimer_set(&stop_nd_timer, CLOCK_SECOND * 300);
// #endif

  while(1){
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&collect_nd_timer));

    /*do a copy of the collect's neigbors list*/
    neighbors_copy_list.list = collect_neighbor_list(&tc.neighbor_list);

    /*Update the sdn neigbors list*/
    if(sdn_neighbors_table_act(&neighbors_copy_list) == SDN_SUCCESS) {
      // printf("renan 6\n");
      sdn_neighbor_table_print();

      neighbor_discovery_event();
      //process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);
    }

    etimer_set(&collect_nd_timer, CLOCK_SECOND * 60);
  }


//     /*Waits for a change in the list and actualize the sdn list*/
//     PROCESS_WAIT_EVENT();
//     if (ev == NEIGHBOR_CHANGE) {
//       printf("XXX Event NEIGHBOR_CHANGE\n");
//       nd_changed = 1;
//       // printf("renan 1\n");
//       if ( etimer_expired(&collect_nd_timer) ) {
//         // printf("renan 2\n");
//         nd_should_send = 1;
//       }
//     }

//     if (ev == PROCESS_EVENT_TIMER) {
//       // printf("renan 3\n");
//       if ( etimer_expired(&collect_nd_timer) && nd_changed == 1) {
//         // printf("renan 4\n");
//         nd_should_send = 1;
//       }
// #ifdef SDN_STOP_COLLECT_ND
//       if ( etimer_expired(&stop_nd_timer) && count < 5) {
//         collect_sdn_close(&tc);
//         etimer_set(&collect_nd_timer, CLOCK_SECOND * 320);
//         count ++;
//       }
// #endif
//     }

//     if (nd_should_send) {
//       // printf("renan 5\n");
//       etimer_set(&collect_nd_timer, CLOCK_SECOND * 60);
//       nd_changed = 0;
//       nd_should_send = 0;
//       if(sdn_neighbors_table_act(&neighbors_copy_list) == SDN_SUCCESS) {
//         // printf("renan 6\n");
//         sdn_neighbor_table_print();
//         process_post(&sdn_core_process, SDN_EVENT_ND_CHANGE, 0);
//       }
//     }

//   }

  PROCESS_END();
}

void nd_input(uint8_t* p, uint16_t l) {

}

void link_estimate_update(const sdnaddr_t *neighbor, uint16_t num_tx, uint8_t fail_tx) {

  struct collect_neighbor *cn;

  cn = collect_neighbor_list_find(&tc.neighbor_list, (const linkaddr_t *) neighbor);

  if(cn != NULL) {
    if (fail_tx ) {
      collect_neighbor_tx_fail(cn, num_tx);
    } else {
      collect_neighbor_tx(cn, num_tx);
    }
  }
}

const struct sdn_nd collect_sdn_nd = {
  neighbors_init,
  nd_input,
  link_estimate_update
};

/*--------------------------------------------------------------------------------*/
