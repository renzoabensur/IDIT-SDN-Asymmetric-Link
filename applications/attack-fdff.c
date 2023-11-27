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

/************************************************************************/
/* attack-one.c : Initially, the node is part of the network and after  */
/* ATTACK_TRIGGER seconds, it starts to send data packets with unknown  */
/* flows to their neighbors.                                            */
/************************************************************************/


/*
  Realiza o Ataque de fdff, momento do ATTACK_TRIGGER. SDN_SIMULATION_N_SINKS numero de sink nodes
*/

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "sdn-debug.h"
#include "string.h"
#include "lib/random.h"
#include "collect-nd.h"
#include "data-flow-table.h"

#ifdef DEMO
#include "leds.h"
#endif

#include "sys/etimer.h"

#ifndef SDN_SIMULATION_N_SINKS
#define SDN_SIMULATION_N_SINKS 1
#endif

#define SENSING_AT_SECONDS 60
#define IDS_TIMER 120
#define ATTACK_TRIGGER 14000 //4horas
// #define ATTACK_TRIGGER 28800
#ifdef ATTACK_SIXTY
#define ATTACK_PERIOD 60
#endif
#ifdef ATTACK_THIRTY
#define ATTACK_PERIOD 30
#endif
#ifdef ATTACK_TEN
#define ATTACK_PERIOD 10
#endif
#ifdef ATTACK_ONE
#define ATTACK_PERIOD 1
#endif

/*---------------------------------------------------------------------------*/
PROCESS(sdn_test_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_test_process);


uint16_t temp_flowid[4]; //depende do numero de vizinhos, vai mudar. Para uma frequencia maior tera mais vizinhos
static struct ctimer attack_timer;


/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  SDN_DEBUG("Receiving message from ");
  sdnaddr_print(source_addr);
  SDN_DEBUG(" of len %d: %s\n", len, (char*) data);

  /* Put your code here to get data received from application. */
}
/*-----------------------------------------------------------------------------*/
/* Use to defines either node is a sensor node and sends packets to sink or only router node and forward packets. */
static uint8_t isSensing() {

  uint8_t sensing = 0;

  sensing = 1;

  return sensing;
}

static void create_false_flows();

static void create_false_flows() {
  
  uint8_t n;

  for(n = 0; n <= 3; n++) {
    if ((temp_flowid[n] > 0)) {
      //printf("Removing flow: %u\n", temp_flowid[n]);
      sdn_dataflow_remove(temp_flowid[n]);  
    }
  }

  sdn_dataflow_print();

/*-----------Maybe it wont be necessary-----------------------*/
  MEMB(new_neighbor_table_memb, struct sdn_neighbor_entry, 4);

  struct collect_neighbor_list neighbors_copy_list;
  
  struct collect_conn tc;
  
  memcpy(&tc, collect_pointer(), sizeof(struct collect_conn));
  
  neighbors_copy_list.list = collect_neighbor_list(&tc.neighbor_list);
  // Allocate memory for the new entries.
  struct sdn_neighbor_entry *n1 = memb_alloc(&new_neighbor_table_memb);
  struct sdn_neighbor_entry *n2 = memb_alloc(&new_neighbor_table_memb);
  struct sdn_neighbor_entry *n3 = memb_alloc(&new_neighbor_table_memb);
  struct sdn_neighbor_entry *n4 = memb_alloc(&new_neighbor_table_memb);


  // Check if memory allocation was successful.
  if (n1 == NULL || n2 == NULL || n3 == NULL || n4 == NULL) {
      SDN_DEBUG("Failed to allocate memory for new neighbor entries!\n");
      return; // or handle the error as appropriate
  }

  // Set the metrics.
  n1->metric = 0;
  n2->metric = 0;
  n3->metric = 0;
  n4->metric = 0;

  // Allocate extra_info (if necessary).
  n1->extra_info = memb_alloc(NULL); // Assuming the correct memory block is passed
  n2->extra_info = memb_alloc(NULL); // or NULL if it's not required
  n3->extra_info = memb_alloc(NULL); // or NULL if it's not required
  n4->extra_info = memb_alloc(NULL); // or NULL if it's not required

  // Set the neighbor addresses.
  sdnaddr_t neighbor_addr1 = {{0x08, 0x00}};// adiciona o endereco 08
  sdnaddr_t neighbor_addr2 = {{0x17, 0x00}};// adiciona o endereco 23
  sdnaddr_t neighbor_addr3 = {{0x1C, 0x00}};// adiciona o endereco 28
  sdnaddr_t neighbor_addr4 = {{0x1D, 0x00}};// adiciona o endereco 29


  sdnaddr_copy(&n1->neighbor_addr, &neighbor_addr1);
  sdnaddr_copy(&n2->neighbor_addr, &neighbor_addr2);
  sdnaddr_copy(&n3->neighbor_addr, &neighbor_addr3);
  sdnaddr_copy(&n4->neighbor_addr, &neighbor_addr4);


  // Add the new entries to the list.
  list_add(neighbors_copy_list.list, n1);
  // list_add(neighbors_copy_list.list, n2);
  // list_add(neighbors_copy_list.list, n3);
  // list_add(neighbors_copy_list.list, n4);


  //criar uma lista hardcooded
  //sdn_neighbor_table_insert(sdnaddr_t neighbor_addr, struct memb* m)
 /*----------------------------------------------------------*/
 

  struct sdn_neighbor_entry *neighbor_copy;

  struct sdn_neighbor_entry *neighbor_copy_next;

  //remove 1 vizinho normal e adixiona 1 vizinho frequncia maior

  neighbor_copy = list_head(neighbors_copy_list.list);
  neighbor_copy = list_item_next(neighbor_copy);
  // neighbor_copy = list_item_next(neighbor_copy);

  static char false_data[10];

  n = 0;

  for(neighbor_copy; neighbor_copy != NULL; ) {
    flowid_t random_flow;
    neighbor_copy_next = list_item_next(neighbor_copy);

    random_flow = 10 + (random_rand() % 1000);
    temp_flowid[n] = random_flow;
    sdn_dataflow_insert(random_flow, neighbor_copy->neighbor_addr, SDN_ACTION_FORWARD);
    printf("Sending data to false flow: %d\n", random_flow);

    printf("Copia da lista endereco vizinho : %d\n", neighbor_copy->neighbor_addr);

    // SDN_DEBUG("Sending data to false flow: %d\n", random_flow);
    sdn_send((uint8_t*) false_data, 10, random_flow);
    neighbor_copy = neighbor_copy_next;

    n++;
  } 

  ctimer_set(&attack_timer, ATTACK_PERIOD * CLOCK_SECOND, create_false_flows, NULL); 
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_test_process, ev, data)
{
  PROCESS_BEGIN();

  printf("SENSING AT SECONDS = %i\n", SENSING_AT_SECONDS);

  sdn_init(receiver);

  static flowid_t sink;

  sink = 2017 + (sdn_node_addr.u8[0] % SDN_SIMULATION_N_SINKS);

  printf("SDN_SIMULATION_N_SINKS %d\n", SDN_SIMULATION_N_SINKS);

  static struct etimer periodic_timer;
  #ifdef IDS
  static struct etimer ids_timer;
  #endif
  #ifdef CP_DETECT
  static struct etimer ts_timer;
  #endif

  if(isSensing() == 1) {
    printf("Sensing data node.\n");
    printf("#A color=GREEN\n");
  } else {
    printf("#A color=BLUE\n");
  }

  etimer_set(&periodic_timer, (120 + random_rand() % SENSING_AT_SECONDS) * CLOCK_SECOND) ;

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  etimer_set(&periodic_timer, SENSING_AT_SECONDS * CLOCK_SECOND);
  ctimer_set(&attack_timer, ATTACK_TRIGGER * CLOCK_SECOND, create_false_flows, NULL);
  #ifdef IDS
  etimer_set(&ids_timer, IDS_TIMER * CLOCK_SECOND);
  #endif
  // etimer_set(&n_report, 1800 * CLOCK_SECOND);
  // sets the timer to start the period previous initiate the metric monitoring
  #ifdef CP_DETECT
  etimer_set(&ts_timer, 120 * CLOCK_SECOND);
  #endif

  static char data[10];
  static uint8_t i = 0;

  while(1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&periodic_timer)) {
      sprintf(data, "teste %d", i++);
      SDN_DEBUG("Sending data to flow %d\n", sink);
      sdn_send((uint8_t*) data, 10, sink);
      // sdn_dataflow_print();
      etimer_restart(&periodic_timer);
      // counter++;
      // printf("Control packets received %lu\n", manag_get_info(64));
    }

    #if defined (CP_DETECT) && defined (CENTR_DETECT)
    if (etimer_expired(&ts_timer)){
      printf("Sending detection data to controller\n");
      uint16_t mngt_metrics;
      mngt_metrics = 2;
      // the second parameter indicates the destiny: 1 -> controller, 2 -> management sink
      sdn_send_data_management(mngt_metrics, 1);
      etimer_reset(&ts_timer);
    }
    #endif
  }

  PROCESS_END();
}
