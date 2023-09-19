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
#include <math.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "sdn-debug.h"
#include "string.h"
#include "lib/random.h"
#include <inttypes.h>
#include "manage-info.h"
#include "sdn-node-security-module.h"
#include "sdn-send-packet.h"

#ifdef DEMO
#include "leds.h"
#endif

#include "sys/etimer.h"

#ifndef SDN_SIMULATION_N_SINKS
#define SDN_SIMULATION_N_SINKS 1
#endif

#define SENSING_AT_SECONDS 60
#define IDS_TIMER 120
// #define CP_TIMER 120
#define Vs 3

#ifdef MONETWSEC
#define CP_TIMER 120
#endif
#ifdef MSIXTYSEC
#define CP_TIMER 60
#endif
#ifdef MTHIRSEC
#define CP_TIMER 30
#endif
#ifdef MTENSEC
#define CP_TIMER 10
#endif

// #ifdef CP_DETECT
// #define m_win 201 // number of samples for the training process. It starts in 201 to discard the first sample
// #define d_win 50
// #endif

/*---------------------------------------------------------------------------*/
PROCESS(sdn_test_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_test_process);
/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  SDN_DEBUG("Receiving message from ");
  sdnaddr_print(source_addr);
  SDN_DEBUG(" of len %d: %s\n", len, (char*) data);
}

/* Use to defines either node is a sensor node and sends packets to sink or only router node and forward packets. */
// static uint8_t isSensing() {

//   uint8_t sensing = 0;

//   sensing = 1;

//   return sensing;
// }

unsigned long last_cpu;
unsigned long diff_cpu;

unsigned long node_processing_time();

unsigned long node_processing_time(){
  energest_flush();
  diff_cpu = energest_type_time(ENERGEST_TYPE_CPU) - last_cpu;
  last_cpu = energest_type_time(ENERGEST_TYPE_CPU);
  printf("Processing time: %lu\n", diff_cpu);
  return diff_cpu;
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
  static struct etimer n_report;
  #ifdef CP_DETECT
  static struct etimer ts_timer;
  #endif
  // if(isSensing() == 1) {
  //   printf("Sensing data node.\n");
  //   printf("#A color=GREEN\n");
  // } else {
  //   printf("#A color=BLUE\n");
  // }

  etimer_set(&periodic_timer, (300 + random_rand() % SENSING_AT_SECONDS) * CLOCK_SECOND) ;

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

  // if(isSensing() == 1) {
  //   etimer_set(&periodic_timer, SENSING_AT_SECONDS * CLOCK_SECOND);
  // }

  etimer_set(&periodic_timer, SENSING_AT_SECONDS * CLOCK_SECOND);
  #ifdef IDS
  etimer_set(&ids_timer, IDS_TIMER * CLOCK_SECOND);
  #endif
  etimer_set(&n_report, 1800 * CLOCK_SECOND);
  // sets the timer to start the period previous initiate the metric monitoring
  #ifdef CP_DETECT
  etimer_set(&ts_timer, CP_TIMER * CLOCK_SECOND);
  printf("CP timer: %d", (int)CP_TIMER);
  #endif

  static char data[10];
  static uint8_t i = 0;
   //static uint8_t counter = 0;

  // Initialize the security module
  #ifdef DIST_DETECT
  security_node_init();
  printf("Security init\n");
  #endif

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
      node_processing_time();
    }

    #if defined (CP_DETECT) && defined (CENTR_DETECT)
    if (etimer_expired(&ts_timer)){
      printf("Sending detection data to controller\n");
      uint16_t mngt_metrics;
      mngt_metrics = SDN_MNGT_METRIC_QTY_DATA + SDN_MNGT_CTRL_PCKT_TX;
      // mngt_metrics = SDN_MNGT_DA;
      // the second parameter indicates the destiny: 1 -> controller, 2 -> management sink
      sdn_send_data_management(mngt_metrics, 1);
      etimer_reset(&ts_timer);
    }
    #endif

    // #ifdef IDS
    // if (etimer_expired(&ids_timer)){
    //   printf("Sending IDS data to controller\n");
    //   uint16_t mngt_metrics;
    //   mngt_metrics = 24;
    //   sdn_send_data_management(mngt_metrics, 2);
    //   etimer_reset(&ids_timer);
    // }
    // #endif

    // if (etimer_expired(&n_report)) {
    //   nd_event();
    //   etimer_restart(&n_report);
    // } 
 
  }

  PROCESS_END();
}
