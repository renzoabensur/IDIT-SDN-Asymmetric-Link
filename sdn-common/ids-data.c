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
#include <inttypes.h>
#include <string.h>
#include "sdn-addr.h"
#include "ids-data.h"

#define MAX_NODES_NUM 300

//struct ids_flows_usage_db nodes_ids_dn[MAX_NODES_NUM];

//void print_data_flow_times_used(uint16_t n, sdnaddr_t *source);

//void print_ctrl_flow_times_used(uint16_t n, sdnaddr_t *source);

//void print_flow_request_pckt_counter(uint16_t n, sdnaddr_t *source);

/*
void print_data_flow_times_used(uint16_t n, sdnaddr_t *source) {
  sdnaddr_print(source);
  printf("Data flow used %u times \n", nodes_ids_dn[n].data_times_used);
}

void print_ctrl_flow_times_used(uint16_t n, sdnaddr_t *source) {
  sdnaddr_print(source);
  printf("Control flow used %u times \n", nodes_ids_dn[n].ctrl_times_used);
}

/*
void print_flow_request_pckt_counter(uint16_t n, sdnaddr_t *source) {
  sdnaddr_print(source);
  printf("Flow request packets received: %u \n", nodes_ids_dn[n].flow_request_pckt);
}
*/

void process_ids_data(sdnaddr_t *source, uint16_t data_times) {
  printf("Getting IDS data flow info from");
  sdnaddr_print(source);
  printf("\n");
  //uint16_t n;
  /*
  if ((source->u8[1] == 1)) {
    n = source->u8[0] + 256;
  }else {
    n = source->u8[0];
  }
  
  uint16_t temp_flow_times;
  temp_flow_times =  data_times - nodes_ids_dn[n].data_times_used;
  nodes_ids_dn[n].data_times_used = temp_flow_times;
  */
  sdnaddr_print(source);
  printf(" Data flow used %u times \n", data_times);
  //print_data_flow_times_used(n, source);
}

void process_ids_ctrl(sdnaddr_t *source, uint16_t ctrl_times) {
  printf("Getting IDS control traffic info from:");
  sdnaddr_print(source);
  printf("\n");
  /*uint16_t n;
  if ((source->u8[1] == 1)) {
    n = source->u8[0] + 256;
  }else {
    n = source->u8[0];
  }
  uint16_t temp_ctrl_times;
  temp_ctrl_times =  ctrl_times - nodes_ids_dn[n].ctrl_times_used;
  nodes_ids_dn[n].ctrl_times_used = temp_ctrl_times;
  */
  sdnaddr_print(source);
  printf(" Control flow used %u times \n", ctrl_times);
}

/*
void flow_request_pckt_counter(sdnaddr_t *source) {
  uint16_t n;
  n = source->u8[1];
  if ((n > 0)) {
    n = (uint16_t)source->u8[0] + 256;
  }else {
    n = source->u8[0];
  }
  nodes_ids_dn[n].flow_request_pckt++;
}
*/


