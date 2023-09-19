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

#include "contiki.h"
#include "net/netstack.h"
#include "net/rime/rime.h"
#include "collect-cd.h"
#include <string.h>
#include <stdio.h>
#include <sdn-debug.h>


/*----------------------------------------------*/
//extern struct process sdn_core_process;
/*----------------------------------------------*/
void (* controller_discovery_event)();

linkaddr_t current_sdn_parent;
struct collect_conn *cd_tc;


PROCESS(controller_discovery, "Controller discovery");


void collect_cd_init(void *cd_event, uint8_t is_controller) {

  SDN_DEBUG("Initializing controller discovery\n");

  controller_discovery_event = cd_event;

  if(is_controller == SDN_YES) {

    memcpy(&current_sdn_parent, &sdn_node_addr, sizeof(linkaddr_t));
    controller_discovery_event();
    //process_post(&sdn_core_process, SDN_EVENT_CD_FOUND, 0);

  }else{
  process_start(&controller_discovery, NULL);
  }
}

/*---------------------------------------------------------------------------*/
void collect_cd_close() {

}
/*---------------------------------------------------------------------------*/
void collect_cd_act(struct collect_conn *tc) {

  if(!linkaddr_cmp(&current_sdn_parent, &tc->current_parent)) {
    linkaddr_copy(&current_sdn_parent, collect_sdn_parent(tc));
    print_parent();
    controller_discovery_event();
    //process_post(&sdn_core_process, SDN_EVENT_CD_FOUND, 0);
  }

}
/*---------------------------------------------------------------------------*/
void collect_cd_get(sdnaddr_t* sdn_parent) {

  memcpy(sdn_parent, &current_sdn_parent, sizeof(linkaddr_t));
}
/*---------------------------------------------------------------------------*/
void print_parent() {

  SDN_DEBUG("Current parent %d.%d \n", current_sdn_parent.u8[0], current_sdn_parent.u8[1]);
}

/*----------------------------------------------------------------------------*/

PROCESS_THREAD(controller_discovery, ev, data){

  PROCESS_BEGIN();

  /*the programm waits 130 seconds while the network is settled (this time could change)*/
  // etimer_set(&first_parent_copy, CLOCK_SECOND * 110);

  // PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&first_parent_copy));
  cd_tc = collect_pointer();

  /*Gets the parent of the node by the first time*/
  // collect_cd_act(cd_tc);
  // print_parent();

  while(1){
    /*Waits for a change in the list and actualize the sdn list*/
    PROCESS_WAIT_EVENT_UNTIL(ev == PARENT_CHANGE);
    collect_cd_act(cd_tc);
  }

  PROCESS_END();
}

void cd_input(uint8_t* p, uint16_t l) {

}

const struct sdn_cd collect_sdn_cd = {
  collect_cd_init,
  collect_cd_get,
  cd_input
};

/*--------------------------------------------------------------------------------*/
