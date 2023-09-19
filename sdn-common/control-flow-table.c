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
#include "control-flow-table.h"
#include "sdn-debug.h"
#include "stdio.h"

LIST(cft);

uint8_t sdn_controlflow_init() {
  list_init(cft);
  return sdn_controlflow_insert(sdn_node_addr, sdn_node_addr, SDN_ACTION_RECEIVE);
}

uint8_t sdn_controlflow_insert(sdnaddr_t dest_addr, \
                                sdnaddr_t next_hop, action_t action) {
  struct control_flow_entry *c;

  /* Check if we entry already exists. */
  c = sdn_controlflow_get(dest_addr);

  /* If n is NULL, this entry was not found in our list, and we allocate
      a new struct memb memory pool. */
  if(c == NULL) {
#ifndef SDN_CONTROLLER_PC
    c = memb_alloc(&flow_tables_memb);
#else // SDN_CONTROLLER_PC
    c = malloc(sizeof(struct control_flow_entry));
#endif // SDN_CONTROLLER_PC
  }

   /* If we could not allocate a new entry, we give up. */
  if(c == NULL) {
   return SDN_ERROR;
  }

  /* Initialize the fields. */
  sdnaddr_copy(&c->dest_addr, &dest_addr);
  sdnaddr_copy(&c->next_hop, &next_hop);
  if (sdnaddr_cmp(&sdn_node_addr, &next_hop) == SDN_EQUAL) {
    SDN_DEBUG("next_hop == own address, action is forced to receive\n");
    c->action = SDN_ACTION_RECEIVE;
  } else {
    c->action = action;
  }
  c->times_used = 0;
  c->age = 0;

  /* Place the entry in the control flow table list. */
  list_add(cft, c);

  sdn_solve_unknown_dest(c);

  /* update age of table entries */
  for(c = list_head(cft); c != NULL; c = list_item_next(c)) {
    c->age ++;
  }

  return SDN_SUCCESS;
}
//TODO: This function was not tested yet
uint8_t sdn_controlflow_remove_next_hop(sdnaddr_t next_hop) {

  struct control_flow_entry *c;

  c = list_head(cft);

  while(c != NULL) {
    /* We break out of the loop if the address matches */
    if(sdnaddr_cmp(&c->next_hop, &next_hop) == SDN_EQUAL) {

      struct control_flow_entry *c2 = c;
      c = list_item_next(c);
#ifndef SDN_CONTROLLER_PC
      memb_free(&flow_tables_memb, c2);
#else
      free(c2);
#endif
    } else {
      c = list_item_next(c);
    }
  }

  return SDN_SUCCESS;
}

uint8_t sdn_controlflow_remove(sdnaddr_t dest_addr) {
  struct control_flow_entry *c;
  c = sdn_controlflow_get(dest_addr);

  if (c != NULL) {
    list_remove(cft, c);
#ifndef SDN_CONTROLLER_PC
    memb_free(&flow_tables_memb, c);
#else
    free(c);
#endif
    return SDN_SUCCESS;
  }

  return SDN_ERROR;
}

struct control_flow_entry * sdn_controlflow_get(sdnaddr_t dest_addr) {
  struct control_flow_entry *c;

  for(c = list_head(cft); c != NULL; c = list_item_next(c)) {
    /* We break out of the loop if the address matches */
    sdnaddr_print(&dest_addr);
    SDN_DEBUG(" ");
    sdnaddr_print(&c->dest_addr);
    SDN_DEBUG(" \n");
    if(sdnaddr_cmp(&c->dest_addr, &dest_addr) == SDN_EQUAL) {
      return c;
    }
  }

  return NULL;
}

void sdn_controlflow_clear() {
  struct control_flow_entry *c, *next;
  c = list_head(cft);

  while (c != NULL) {
    next = list_item_next(c);
    list_remove(cft, c);
#ifndef SDN_CONTROLLER_PC
    memb_free(&flow_tables_memb, c);
#else
    free(c);
#endif
    c = next;
  }
}

void sdn_controlflow_print() {
  struct control_flow_entry *c;
  SDN_DEBUG("dest_addr, next_hop, action, times_used, age, next ptr\n");
  for(c = list_head(cft); c != NULL; c = list_item_next(c)) {
    sdnaddr_print(&c->dest_addr);
    SDN_DEBUG("(");
    sdnaddr_print(&c->next_hop);
    SDN_DEBUG(") (%02d) (%02d) (%02d) (%p)\n", c->action, c->times_used, c->age, c->next);
  }
}
