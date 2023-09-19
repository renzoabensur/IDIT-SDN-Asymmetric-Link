/*
* Header Here!
*/

#include <stdio.h>
#include "contiki.h"
#include "sdn-core.h"
#include "flow-table-tests.h"
#include "src-route-tests.h"
#include "sdn-debug.h"
#include "string.h"
#include "sdn-constants.h"
#include "sdn-send-packet.h"
//#include "manage-info.h"

//#include "leds.h"

#include "sys/etimer.h"

#ifndef SDN_SIMULATION_N_SINKS
#define SDN_SIMULATION_N_SINKS 1
#endif

#ifdef MONETWSEC
#define CP_TIME_UPD 120
#endif
#ifdef MSIXTYSEC
#define CP_TIME_UPD 60
#endif
#ifdef MTHIRSEC
#define CP_TIME_UPD 30
#endif
#ifdef MTENSEC
#define CP_TIME_UPD 10
#endif


/*---------------------------------------------------------------------------*/
PROCESS(sdn_sink_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_sink_process);

/*
sdnaddr_t red_node;
sdnaddr_t green_node;
sdnaddr_t blue_node;
*/

//uint32_t rx_counter = 0;


/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  SDN_DEBUG("Receiving message from ");
  sdnaddr_print(source_addr);
  SDN_DEBUG(" of len %d: %s\n", len, (char*) data);
  //rx_counter++;
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_sink_process, ev, data)
{
  PROCESS_BEGIN();

  printf("#A color=GREEN\n");
  printf("Data packets metric update: %u\n", CP_TIME_UPD);

  sdn_init(receiver);

  static flowid_t flowid;

  flowid = 2017 + (sdn_node_addr.u8[0] % SDN_SIMULATION_N_SINKS);

  sdn_register_flowid(flowid);

  static struct etimer send_total_data;
  static struct etimer wait_send;
  uint16_t mngt_metrics = 0;
  uint32_t rx_counter = 0;

  etimer_set(&wait_send, (300 + 10) * CLOCK_SECOND);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&wait_send));
  etimer_set(&send_total_data, CP_TIME_UPD * CLOCK_SECOND);

  while(1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&send_total_data)) {
      mngt_metrics = 32;
      printf("Sending total data\n");
      sdn_send_data_management(mngt_metrics, 1);
      etimer_restart(&send_total_data); 
    }
  }

  PROCESS_END();
}
