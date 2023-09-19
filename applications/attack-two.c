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

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "sdn-debug.h"
#include "string.h"
#include "lib/random.h"
#include "collect-nd.h"
#include "sdn-send-packet.h"

#ifdef DEMO
#include "leds.h"
#endif

#include "sys/etimer.h"

#ifndef SDN_SIMULATION_N_SINKS
#define SDN_SIMULATION_N_SINKS 1
#endif

#define SENSING_AT_SECONDS 60
#define IDS_TIMER 180
#define ATTACK_TRIGGER 600
#define ATTACK_PERIOD 10

/*---------------------------------------------------------------------------*/
PROCESS(sdn_test_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_test_process);


uint16_t temp_flowid[8];
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

static void create_false_flow_request();

static void create_false_flow_request() {
  
  flowid_t random_flow;
  random_flow = 10 + (random_rand() % 1000);
  sdn_send_data_flow_request(random_flow);
  SDN_DEBUG("Sending false flow request: %d\n", random_flow);
  ctimer_set(&attack_timer, ATTACK_PERIOD * CLOCK_SECOND, create_false_flow_request, NULL);
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
  static struct etimer ids_timer;


#ifdef SDN_HARDCODED_ROUTES
  int x, y, x_next_hop, y_next_hop;
  sdnaddr_t next_hop;
  int grid_side = 5;

  x = (sdn_node_addr.u8[0] - 1)/ grid_side + 1;
  y = (sdn_node_addr.u8[0] - 1) % grid_side + 1;

  printf("x:%d y:%d\n", x, y);

  if (y == 1) {
    y_next_hop = 1;
    if (x < (grid_side + 1) / 2) {
      x_next_hop = x + 1;
    } else {
      x_next_hop = x - 1;
    }
  } else {
    y_next_hop = y - 1;
    x_next_hop = x;
  }
  // x_sink = (grid_side + 1) / 2
  // y_sink = 1
  memset(&next_hop, 0 , sizeof(sdnaddr_t));
  sdnaddr_setbyte(&next_hop, 0, (x_next_hop-1) * grid_side + y_next_hop);
  sdn_dataflow_insert(sink, next_hop, SDN_ACTION_FORWARD);
#endif

  if(isSensing() == 1) {
    printf("Sensing data node.\n");
    printf("#A color=GREEN\n");
  } else {
    printf("#A color=BLUE\n");
  }

  etimer_set(&periodic_timer, (60 + random_rand() % SENSING_AT_SECONDS) * CLOCK_SECOND) ;

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  if(isSensing() == 1) {
    etimer_set(&periodic_timer, SENSING_AT_SECONDS * CLOCK_SECOND);
  }

  etimer_set(&ids_timer, IDS_TIMER * CLOCK_SECOND);
  ctimer_set(&attack_timer, ATTACK_TRIGGER * CLOCK_SECOND, create_false_flow_request, NULL);

  static char data[10];
  static uint8_t i = 0;

  while(1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&periodic_timer)) {

      sprintf(data, "teste %d", i++);

      SDN_DEBUG("Sending data to flow %d\n", sink);

      sdn_send((uint8_t*) data, 10, sink);

      etimer_restart(&periodic_timer);
    }


    if (etimer_expired(&ids_timer)) {
      
      printf("Sending IDS data to controller\n");
      uint16_t mngt_metrics;
      mngt_metrics = 24;
      sdn_send_data_management(mngt_metrics, 2);
      etimer_reset(&ids_timer);
    }
  }

  PROCESS_END();
}
