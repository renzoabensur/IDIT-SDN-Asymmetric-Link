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

#ifndef SDN_CONTROLLER_PC
#include "contiki.h"
#include "flow-tables-mem.h"
#else //SDN_CONTROLLER_PC
#include <stdlib.h>
#endif //SDN_CONTROLLER_PC
#include "sdn-unknown-route.h"
#include "lib/list.h"
#include "data-flow-table.h"
#include "sdn-debug.h"
#include <stdio.h>

LIST(dft);

uint8_t sdn_dataflow_init() {
  list_init(dft);
  return SDN_SUCCESS;
}

uint8_t sdn_dataflow_insert(flowid_t flowid, sdnaddr_t next_hop, \
                              action_t action) {
  struct data_flow_entry *d;

      // printf("DAGO insert data_flow flowid %d next_hop: ", flowid);
      // sdnaddr_print(&next_hop);
      // printf("\n");
  /* Check if we entry already exists. */
  d = sdn_dataflow_get(flowid);

  /* If n is NULL, this entry was not found in our list, and we allocate
      a new struct memb memory pool. */
  if(d == NULL) {
#ifndef SDN_CONTROLLER_PC
    d = memb_alloc(&flow_tables_memb);
#else // SDN_CONTROLLER_PC
    d = malloc(sizeof(struct data_flow_entry));
#endif // SDN_CONTROLLER_PC
  }

   /* If we could not allocate a new entry, we give up. */
  if(d == NULL) {
   return SDN_ERROR;
  }

  /* Initialize the fields. */
  flowid_copy(&d->flowid, &flowid);
  sdnaddr_copy(&d->next_hop, &next_hop);
  if (action == SDN_ACTION_FORWARD && sdnaddr_cmp(&sdn_node_addr, &next_hop) == SDN_EQUAL) {
    SDN_DEBUG("next_hop == own address, action is forced to receive\n");
    d->action = SDN_ACTION_RECEIVE;
  } else {
    d->action = action;
  }
  d->times_used = 0;
  d->age = 0;

  /* Place the entry in the data flow table list. */
  list_add(dft, d);

  sdn_solve_unknown_flow(d);

  /* update age of table entries */
  for(d = list_head(dft); d != NULL; d = list_item_next(d)) {
    d->age ++;
  }

  return SDN_SUCCESS;
}
//TODO: This function was not tested yet
uint8_t sdn_dataflow_remove_next_hop(sdnaddr_t next_hop) {

  struct data_flow_entry *d;
      // printf("DAGO Finding data_flow next_hop: ");
      // sdnaddr_print(&next_hop);
      // printf("\n");
  d = list_head(dft);

  while(d != NULL) {
    /* We break out of the loop if the address matches */
    // printf("DAGO Finding next_hop ");
    // sdnaddr_print(&d->next_hop);
    // printf("\n");

    //We should avoid to delete flow to the controller.
    if(sdnaddr_cmp(&d->next_hop, &next_hop) == SDN_EQUAL && d->flowid != SDN_CONTROLLER_FLOW) {
      // printf("DAGO Dropping data_flow next_hop: ");
      // sdnaddr_print(&next_hop);
      // printf("\n");

      struct data_flow_entry *d2 = d;
      d = list_item_next(d);
#ifndef SDN_CONTROLLER_PC
      memb_free(&flow_tables_memb, d2);
#else
      free(d2);
#endif
    } else {
      d = list_item_next(d);
    }
  }

  return SDN_SUCCESS;
}

uint8_t sdn_dataflow_remove(flowid_t flowid) {
  struct data_flow_entry *d;
  d = sdn_dataflow_get(flowid);

  if (d != NULL) {
    list_remove(dft, d);
#ifndef SDN_CONTROLLER_PC
    memb_free(&flow_tables_memb, d);
#else
    free(d);
#endif
    return SDN_SUCCESS;
  }

  return SDN_ERROR;
}

struct data_flow_entry * sdn_dataflow_get(flowid_t flowid) {
  struct data_flow_entry *d;
  for(d = list_head(dft); d != NULL; d = list_item_next(d)) {
    /* We break out of the loop if the address matches */
    if(flowid_cmp(&d->flowid, &flowid) == SDN_EQUAL) {
      return d;
    }
  }
  return NULL;
}

void sdn_dataflow_clear() {
  struct data_flow_entry *d, *next;
  d = list_head(dft);

  while (d != NULL) {
    next = list_item_next(d);
    list_remove(dft, d);
#ifndef SDN_CONTROLLER_PC
    memb_free(&flow_tables_memb, d);
#else
    free(d);
#endif
    d = next;
  }
}

void sdn_dataflow_print() {
  struct data_flow_entry *d;
  SDN_DEBUG("flowid, next_hop, action, times_used, age, next ptr\n");
  for(d = list_head(dft); d != NULL; d = list_item_next(d)) {
    flowid_print(&d->flowid);
    SDN_DEBUG("(");
    sdnaddr_print(&d->next_hop);
    SDN_DEBUG(") (%02d) (%02d) (%02d) (%p)\n", d->action, d->times_used, d->age, d->next);
  }
}
