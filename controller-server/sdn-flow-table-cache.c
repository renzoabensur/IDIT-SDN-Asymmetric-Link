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

#include <stdio.h>
#include <stdlib.h>
#include "sdn-flow-table-cache.h"
#include "sdn-debug.h"
#include "sdn-serial-send.h"

flow_id_table_ptr flow_id_table_head = NULL;
flow_id_table_ptr flow_id_table_tail = NULL;

flow_address_table_ptr flow_address_table_head = NULL;
flow_address_table_ptr flow_address_table_tail = NULL;

flow_id_list_table_ptr flow_id_list_table_head = NULL;
flow_id_list_table_ptr flow_id_list_table_tail = NULL;


flow_id_table_ptr sdn_flow_id_table_new(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action, int weight, int hops) {

  flow_id_table_ptr flow_id_table = malloc(sizeof *flow_id_table);

  memcpy(flow_id_table->source, source, SDNADDR_SIZE);
  flow_id_table->flowid = flowid;
  memcpy(flow_id_table->next_hop, next_hop, SDNADDR_SIZE);
  flow_id_table->action = action;
  flow_id_table->route_weight = weight;
  flow_id_table->hops = hops;

  flow_id_table->next = NULL;
  flow_id_table->flag = ROUTE_NEW;
  flow_id_table->changed = 1;

#if SDN_TX_RELIABILITY
  flow_id_table->online = 0;
  flow_id_table->countdown = SDN_RETRANSMIT_TIME_S;
#else
    flow_id_table->online = 1;
    flow_id_table->countdown = 0;
#endif //SDN_TX_RELIABILITY

  return flow_id_table;
}

flow_id_table_ptr sdn_flow_id_table_add(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action, int weight, int hops) {

  flow_id_table_ptr flow_id_table;

  flow_id_table = sdn_flow_id_table_find(source, flowid);

  if(flow_id_table == NULL) {

    flow_id_table = sdn_flow_id_table_new(source, flowid, next_hop, action, weight, hops);

    if(flow_id_table_head == NULL) {

      flow_id_table_head = flow_id_table;
      flow_id_table_tail = flow_id_table;
      printf("Flow ID Table (new register) (Head):");
    } else {
      flow_id_table_tail->next = flow_id_table;
      flow_id_table_tail = flow_id_table;
      printf("Flow ID Table (new register):");
    }
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" flowid %d ", flowid);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf(" action %d weight %d hops %d ", action, weight, hops);
      printf("\n");

  } else {

    if(memcmp(next_hop, flow_id_table->next_hop, SDNADDR_SIZE) != 0 ||
      memcmp(&action, &flow_id_table->action, sizeof(action_t)) != 0) {

      printf("Flow ID route was updated: source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" -> Flow id %d next_hop ", flowid);
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf(" (old next_hop ");
      sdnaddr_print((sdnaddr_t *)flow_id_table->next_hop);
      printf(" w %d) action %d weight %d hops %d ", flow_id_table->route_weight, action, weight, hops);
      printf("\n");

      memcpy(flow_id_table->next_hop, next_hop, SDNADDR_SIZE);

      flow_id_table->action = action;

      flow_id_table->flag = ROUTE_UPDATED;

      flow_id_table->changed = 1;

#if SDN_TX_RELIABILITY
      flow_id_table->online = 0;
      flow_id_table->countdown = SDN_RETRANSMIT_TIME_S;
#else
      flow_id_table->online = 1;
      flow_id_table->countdown = 0;
#endif //SDN_TX_RELIABILITY
    // } else {
      // printf("Flow ID route was NOT updated (same next_hop): source ");
      // sdnaddr_print((sdnaddr_t *)source);
      // printf(" -> Flow id %d next_hop ", flowid);
      // sdnaddr_print((sdnaddr_t *)next_hop);
    }
    flow_id_table->route_weight = weight;
    flow_id_table->hops = hops;

  }

  if(memcmp(source, &sdn_node_addr, SDNADDR_SIZE) == 0) {
    flow_id_table->online = 1;
  }

  return flow_id_table;
}

void sdn_flow_id_table_set_online(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action) {

  flow_id_table_ptr field = sdn_flow_id_table_find(source, flowid);

  if(field != NULL) {

    if(memcmp(field->next_hop, next_hop, SDNADDR_SIZE) == 0 && field->action == action) {
      field->online = 1;
    }
  }

}

flow_id_table_ptr sdn_flow_id_table_find(unsigned char* source, uint16_t flowid) {

  flow_id_table_ptr flow_id_table = flow_id_table_head;

  while(flow_id_table != NULL) {

    if(memcmp(flow_id_table->source, source, SDNADDR_SIZE) == 0 && flow_id_table->flowid == flowid) {

      return flow_id_table;
    }

    flow_id_table = flow_id_table->next;

  }

  return flow_id_table;
}

flow_id_table_ptr sdn_flow_id_table_get() {

  return flow_id_table_head;
}

uint8_t sdn_flow_id_table_remove_source(unsigned char* source_address) {

  uint8_t ret = 0;

  flow_id_table_ptr flow_id_table_previous = NULL;

  flow_id_table_ptr flow_id_table_remove = NULL;

  flow_id_table_ptr flow_id_table = flow_id_table_head;

  while(flow_id_table != NULL) {

    if(memcmp(flow_id_table->source, source_address, SDNADDR_SIZE) == 0) {

      flow_id_table_remove = flow_id_table;

      if(flow_id_table_previous == NULL) {

        flow_id_table_head = flow_id_table->next;
      } else {

        flow_id_table_previous->next = flow_id_table->next;
      }

      flow_id_table = flow_id_table->next;

      free(flow_id_table_remove);

      ret = 1;
    } else {

      flow_id_table_previous = flow_id_table;

      flow_id_table = flow_id_table->next;
    }
  }

  return ret;
}

flow_address_table_ptr sdn_flow_address_table_new(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action, int weight, int hops) {

  flow_address_table_ptr flow_address_table = malloc(sizeof *flow_address_table);

  memcpy(flow_address_table->source, source, SDNADDR_SIZE);
  memcpy(flow_address_table->target, target, SDNADDR_SIZE);
  memcpy(flow_address_table->next_hop, next_hop, SDNADDR_SIZE);
  flow_address_table->action = action;
  flow_address_table->route_weight = weight;
  flow_address_table->hops = hops;

  flow_address_table->next = NULL;
  flow_address_table->flag = ROUTE_NEW;
  flow_address_table->changed = 1;

#if SDN_TX_RELIABILITY
  flow_address_table->online = 0;
  flow_address_table->countdown = SDN_RETRANSMIT_TIME_S;
#else
    flow_address_table->online = 1;
    flow_address_table->countdown = 0;
#endif //SDN_TX_RELIABILITY

  return flow_address_table;
}

flow_address_table_ptr sdn_flow_address_table_add(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action, int weight, int hops) {

  flow_address_table_ptr flow_address_table;

  flow_address_table = sdn_flow_address_table_find(source, target);

  if(flow_address_table == NULL) {

    flow_address_table = sdn_flow_address_table_new(source, target, next_hop, action, weight, hops);

    if(flow_address_table_head == NULL) {
      flow_address_table_head = flow_address_table;
      flow_address_table_tail = flow_address_table;
      printf("Flow Address Table (new register) (Head):");
    } else {
      flow_address_table_tail->next = flow_address_table;
      flow_address_table_tail = flow_address_table;
      printf("Flow Address Table (new register):");
    }
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" target ");
      sdnaddr_print((sdnaddr_t *)target);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf(" action %d weight %d hops %d ", action, weight, hops);
      printf("\n");

  } else {

    if(memcmp(next_hop, flow_address_table->next_hop, SDNADDR_SIZE) != 0 ||
      memcmp(&action, &flow_address_table->action, sizeof(action_t)) != 0) {

      printf("Flow Address route was updated:");
      printf(" source ");
      sdnaddr_print((sdnaddr_t *)source);
      printf(" -> target ");
      sdnaddr_print((sdnaddr_t *)target);
      printf(" next_hop ");
      sdnaddr_print((sdnaddr_t *)next_hop);
      printf("\n");

      memcpy(flow_address_table->next_hop, next_hop, SDNADDR_SIZE);

      flow_address_table->action = action;

      flow_address_table->flag = ROUTE_UPDATED;

      flow_address_table->changed = 1;

#if SDN_TX_RELIABILITY
      flow_address_table->online = 0;
      flow_address_table->countdown = SDN_RETRANSMIT_TIME_S;
#else
      flow_address_table->online = 1;
      flow_address_table->countdown = 0;
#endif //SDN_TX_RELIABILITY
    }

    flow_address_table->route_weight = weight;

    flow_address_table->hops = hops;
  }

  if(memcmp(source, &sdn_node_addr, SDNADDR_SIZE) == 0) {
    flow_address_table->online = 1;
  }

  return flow_address_table;
}

void sdn_flow_address_table_set_online(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action) {

  flow_address_table_ptr field = sdn_flow_address_table_find(source, target);

  if(field != NULL) {

    if(memcmp(field->next_hop, next_hop, SDNADDR_SIZE) == 0 && field->action == action) {
      field->online = 1;
      // SDN_DEBUG ("### Ack flow address confirmed. source: ");
      // sdnaddr_print((sdnaddr_t *)source);
      // SDN_DEBUG ("target: ");
      // sdnaddr_print((sdnaddr_t *)target);
      // SDN_DEBUG ("\n");
    }
  }

}

flow_address_table_ptr sdn_flow_address_table_find(unsigned char* source, unsigned char* target) {

  flow_address_table_ptr flow_address_table = flow_address_table_head;

  while(flow_address_table != NULL) {

    if(memcmp(flow_address_table->source, source, SDNADDR_SIZE) == 0 && memcmp(flow_address_table->target, target, SDNADDR_SIZE) == 0) {

      return flow_address_table;
    }

    flow_address_table = flow_address_table->next;

  }

  return flow_address_table;
}

flow_address_table_ptr sdn_flow_address_table_get() {

  return flow_address_table_head;
}

uint8_t sdn_flow_address_table_remove_source(unsigned char* source_address) {

  uint8_t ret = 0;

  flow_address_table_ptr flow_address_table_previous = NULL;

  flow_address_table_ptr flow_address_table_remove;

  flow_address_table_ptr flow_address_table = flow_address_table_head;

  while(flow_address_table != NULL) {

    if(memcmp(flow_address_table->source, source_address, SDNADDR_SIZE) == 0) {

      flow_address_table_remove = flow_address_table;

      if(flow_address_table_previous == NULL) {

        flow_address_table_head = flow_address_table->next;
      } else {

        flow_address_table_previous->next = flow_address_table->next;
      }

      flow_address_table = flow_address_table->next;

      free(flow_address_table_remove);

      ret = 1;
    } else {

      flow_address_table_previous = flow_address_table;
      flow_address_table = flow_address_table->next;
    }
  }

  return ret;
}

flow_id_list_table_ptr sdn_flow_id_list_table_new(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = malloc(sizeof *flow_id_list);

  flow_id_list->flowid = flowid;

  memcpy(flow_id_list->target, target, SDNADDR_SIZE);

  flow_id_list->next = NULL;

  return flow_id_list;

}

flow_id_list_table_ptr sdn_flow_id_list_table_add(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = sdn_flow_id_list_table_find(flowid, target);

  if(flow_id_list == NULL) {

    flow_id_list = sdn_flow_id_list_table_new(flowid, target);

    if(flow_id_list_table_head == NULL) {

      flow_id_list_table_head = flow_id_list;
      flow_id_list_table_tail = flow_id_list;
      printf("Flow ID List Table (new register) (Head):");
    } else {

      flow_id_list_table_tail->next = flow_id_list;
      flow_id_list_table_tail = flow_id_list;
      printf("Flow ID List Table (new register):");
    }

    printf(" FlowID %d", flowid);
    printf(" target ");
    sdnaddr_print((sdnaddr_t *)target);
    printf("\n");
  } else {

    printf("Flow ID List Table field was not added because Its already exists.\n");
  }

  return flow_id_list;
}

flow_id_list_table_ptr sdn_flow_id_list_table_find(uint16_t flowid, unsigned char* target) {

  flow_id_list_table_ptr flow_id_list = NULL;

  flow_id_list_table_ptr flow_id_list_search = flow_id_list_table_head;

  while(flow_id_list_search != NULL && flow_id_list == NULL) {

    if(flow_id_list_search->flowid == flowid && memcmp(flow_id_list_search->target, target, SDNADDR_SIZE) == 0) {

      flow_id_list = flow_id_list_search;
    }

    flow_id_list_search = flow_id_list_search->next;

  }

  return flow_id_list;
}

flow_id_list_table_ptr sdn_flow_id_list_table_get() {

  return flow_id_list_table_head;
}

uint8_t sdn_flow_id_list_table_has_target(unsigned char* target) {

  uint8_t ret = 0;

  flow_id_list_table_ptr flow_id_list_search = flow_id_list_table_head;

  while(flow_id_list_search != NULL && ret == 0) {

    if(memcmp(flow_id_list_search->target, target, SDNADDR_SIZE) == 0) {

      ret = 1;
    }

    flow_id_list_search = flow_id_list_search->next;

  }

  return ret;
}

uint8_t sdn_flow_id_list_table_remove_target(unsigned char* target) {

  uint8_t ret = 0;

  flow_id_list_table_ptr flow_id_list_previous = NULL;

  flow_id_list_table_ptr flow_id_list_remove;

  flow_id_list_table_ptr flow_id_list = flow_id_list_table_head;

  while(flow_id_list != NULL) {

    if(memcmp(flow_id_list->target, target, SDNADDR_SIZE) == 0) {

      flow_id_list_remove = flow_id_list;

      if(flow_id_list_previous == NULL) {

        flow_id_list_table_head = flow_id_list->next;
      } else {

        flow_id_list_previous->next = flow_id_list->next;
      }

      flow_id_list = flow_id_list->next;

      free(flow_id_list_remove);

      printf("Removing FlowID from Flow_id_list_table to target ");
      sdnaddr_print((sdnaddr_t *)target);
      printf("\n");
      ret = 1;
    } else {

      flow_id_list = flow_id_list->next;
    }
  }

  return ret;
}
