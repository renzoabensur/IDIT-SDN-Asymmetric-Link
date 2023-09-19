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
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-cd Controller Discovery
 * @{
 *
 * \section dummy-cd Dummy controller discovery
 */

#include "sdn-network.h"
#include "sdn-protocol.h"
#include "sdn-addr.h"
#include "sdn-debug.h"
#include "contiki.h"

//extern struct process sdn_core_process;

void (* controller_discovery_event)();

PROCESS(dummy_cd_process, "SDN Dummy CD process");

void dummy_cd_init(void *cd_event, uint8_t is_controller) {
  SDN_DEBUG ("Dummy Controller Discovery initialized!\n");

  controller_discovery_event = cd_event;

  process_start(&dummy_cd_process, NULL);
}

PROCESS_THREAD(dummy_cd_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer my_event_timer;
  etimer_set(&my_event_timer, 1 * CLOCK_SECOND);

  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&my_event_timer));

  controller_discovery_event();
  //process_post(&sdn_core_process, SDN_EVENT_CD_FOUND, 0);

  while (1)
    PROCESS_WAIT_EVENT();

  PROCESS_END();
}

void dummy_cd_get(sdnaddr_t* next_hop) {
  uint8_t i;
  for (i = 0; i < sizeof(sdnaddr_t); i++) {
    sdnaddr_setbyte(next_hop, i , 0);
  }
  // get first byte and subtracts 1 (assumes linear topology)
  sdnaddr_getbyte(&sdn_node_addr, 0, &i);
  if (i != 1)
    sdnaddr_setbyte(next_hop, 0, i - 1);
  else
    sdnaddr_setbyte(next_hop, 0, i);
}

void dummy_cd_input(uint8_t* packet_ptr, uint16_t packet_len) {

}

const struct sdn_cd dummy_sdn_cd = {
  dummy_cd_init,
  dummy_cd_get,
  dummy_cd_input
};
/** @} */
/** @} */
/** @} */
