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

#include "leds.h"

#include "sys/etimer.h"


/*---------------------------------------------------------------------------*/
PROCESS(sdn_sink_process, "Contiki SDN example process");
AUTOSTART_PROCESSES(&sdn_sink_process);

sdnaddr_t red_node;
sdnaddr_t green_node;
sdnaddr_t blue_node;

/*---------------------------------------------------------------------------*/
static void
receiver(uint8_t *data, uint16_t len, sdnaddr_t *source_addr, uint16_t flowId) {

  SDN_DEBUG("Receiving message from ");
  sdnaddr_print(source_addr);
  SDN_DEBUG(" of len %d: %s\n", len, (char*) data);

  /* Put your code here to get data received from/to application. */
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sdn_sink_process, ev, data)
{
  PROCESS_BEGIN();

  printf("#A color=GREEN\n");

  sdn_init(receiver);

  static flowid_t flowid;

  flowid = 2;

  sdn_register_flowid(flowid);

  while(1) {
    PROCESS_WAIT_EVENT();
  }

  PROCESS_END();
}
