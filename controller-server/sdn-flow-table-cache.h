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
 *         Doriedson A. G. O. <doriedson@usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-controller
 * @{
 * \addtogroup sdn-ftcache Flow table cache
 * @{
 *
 *
*/

#ifndef FLOW_TABLE_CACHE_H
#define FLOW_TABLE_CACHE_H

#include "sdn-addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDN_RETRANSMIT_TIME_S 60 //in seconds

enum routing_flags {
  ROUTE_STATIC = 0,
  ROUTE_NEW = 1,
  ROUTE_UPDATED = 2
};

#define ROUTE_MAX_WEIGHT 65535
#define ROUTE_MAX_HOPS 65535

typedef struct flow_id_table *flow_id_table_ptr;

struct flow_id_table {
   unsigned char source[SDNADDR_SIZE];
   uint16_t flowid;
   unsigned char next_hop[SDNADDR_SIZE];
   action_t action;
   int route_weight;
   int hops;
   flow_id_table_ptr next;
   int flag;
   int changed;
   int online; //confirmed by node ack
   int countdown; //countdown timer for packet retransmit
};

typedef struct flow_address_table *flow_address_table_ptr;

struct flow_address_table {
   unsigned char source[SDNADDR_SIZE];
   unsigned char target[SDNADDR_SIZE];
   unsigned char next_hop[SDNADDR_SIZE];
   action_t action;
   int route_weight;
   int hops;
   flow_address_table_ptr next;
   int flag;
   int changed;
   int online; //confirmed by node ack
   int countdown; //countdown timer for packet retransmit
};

typedef struct flow_id_list_table *flow_id_list_table_ptr;

struct flow_id_list_table {
  uint16_t flowid;
  unsigned char target[SDNADDR_SIZE];
  flow_id_list_table_ptr next;
};

flow_id_table_ptr sdn_flow_id_table_new(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action, int weight, int hops);

flow_id_table_ptr sdn_flow_id_table_add(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action, int weight, int hops);

void sdn_flow_id_table_set_online(unsigned char* source, uint16_t flowid, unsigned char* next_hop, action_t action);

flow_id_table_ptr sdn_flow_id_table_find(unsigned char* source, uint16_t flowid);

flow_id_table_ptr sdn_flow_id_table_get();

uint8_t sdn_flow_id_table_remove_source(unsigned char* source_address);


flow_address_table_ptr sdn_flow_address_table_new(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action, int weight, int hops);

flow_address_table_ptr sdn_flow_address_table_add(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action, int weight, int hops);

void sdn_flow_address_table_set_online(unsigned char* source, unsigned char* target, unsigned char* next_hop, action_t action);

flow_address_table_ptr sdn_flow_address_table_find(unsigned char* source, unsigned char* target);

flow_address_table_ptr sdn_flow_address_table_get();

uint8_t sdn_flow_address_table_remove_source(unsigned char* source_address);

// flow_table_field_ptr flow_table_find_flow(unsigned char* source, uint16_t flowid);

// void flow_table_mark_recalc_from(unsigned char* source);

// void flow_table_mark_recalc_to(unsigned char* target);

// void flow_table_mark_recalc_next_hop(unsigned char* next_hop);

// void flow_table_mark_recalc_from_to(unsigned char* source, unsigned char* target);

// void flow_table_mark_recalc_all();

flow_id_list_table_ptr sdn_flow_id_list_table_new(uint16_t flowid, unsigned char* target);

flow_id_list_table_ptr sdn_flow_id_list_table_add(uint16_t flowid, unsigned char* target);

flow_id_list_table_ptr sdn_flow_id_list_table_find(uint16_t flowid, unsigned char* target);

flow_id_list_table_ptr sdn_flow_id_list_table_get();

uint8_t sdn_flow_id_list_table_has_target(unsigned char* target);

uint8_t sdn_flow_id_list_table_remove_target(unsigned char* target);

void sdn_flow_table_cache_retransmit();

#ifdef __cplusplus
}
#endif

#endif /* FLOW_TABLE_CACHE_H */

/** @} */
/** @} */
/** @} */
