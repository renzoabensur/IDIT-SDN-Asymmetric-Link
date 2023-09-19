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

#include "controller-core.h"

#include "sdn-debug.h"
// #include "websocket.h"

#include "digraph.h"
#include "dijkstra.h"

#include "sdn-flow-table-cache.h"
#include "control-flow-table.h"
#include "data-flow-table.h"
#include "sdn-queue.h"
#include "sdn-serial-send.h"

#include "sdn-send-packet.h"
#include "sdn-process-packets.h"

#include "mainwindow_wrapper.h"

void hexprint(uint8_t *packet, int len)
{
  int i;

  for (i = 0; i < len; i++)
  printf("%02x ", packet[i]);
}

void controller_init(sdnaddr_t *address) {

  memcpy(&sdn_node_addr, address, SDNADDR_SIZE);

  // websocket_connect();

  // flow_tables_test();
  sdn_dataflow_init();
  sdn_controlflow_init();
  sdn_send_queue_init();

  digraph_add_vertice(sdn_node_addr.u8);

  sdn_dataflow_insert(sdn_controller_flow, *address, SDN_ACTION_RECEIVE);

  sdn_controlflow_insert(*address, *address, SDN_ACTION_RECEIVE);

  sdn_flow_id_list_table_add(SDN_CONTROLLER_FLOW, sdn_node_addr.u8);

  //(TODO:) For tests until the implementation of the packet for information of the flowid owner.
  // sdn_dataflow_insert(0x5555, *address, SDN_ACTION_RECEIVE);
  // flow_id_list_table_add(0x5555, address->u8);
  // //Trying to duplicate flowid register
  // flow_id_list_table_add(0x5555, address->u8);
  // sdnaddr_t new_addr = (sdnaddr_t) {{8,0}};
  // //Adding second target for flowid existent.
  // flow_id_list_table_add(0x5555, new_addr.u8);

  // new_addr = (sdnaddr_t) {{2,0}};
  // flow_id_list_table_add(2017, new_addr.u8);


  createMainWindow();

  // printf ("%Lf: WEIGHT_ETX %.02f / WEIGHT_ENERGY %.02f\n", (long double) (t1/(CLOCKS_PER_SEC/1000000.0)), WEIGHT_ETX, WEIGHT_ENERGY);
  // printf ("WEIGHT_ETX %.02f / WEIGHT_ENERGY %.02f\n", WEIGHT_ETX, WEIGHT_ENERGY);

  printf("########## DEFINES ##########\n");

  printf ("WEIGHT_ETX %.02f\n", WEIGHT_ETX);
  printf ("WEIGHT_ENERGY %.02f\n", WEIGHT_ENERGY);


#ifdef SDN_CONTROLLER_PC
  printf("SDN_CONTROLLER_PC (defined)\n");
#else
  printf("SDN_CONTROLLER_PC (undefined)\n");
#endif

#ifdef DEBUG_SDN
  printf("DEBUG_SDN (defined)\n");
#else
  printf("DEBUG_SDN (undefined)\n");
#endif

  printf("SDNADDR_SIZE %u\n", SDNADDR_SIZE);

  printf("DIGRAPH_VERTICE_ID_SIZE %u\n", DIGRAPH_VERTICE_ID_SIZE);

  printf("SDN_ROUTE_SENSIBILITY %.2f\n", SDN_ROUTE_SENSIBILITY);

  printf("SDN_MAX_PACKET_SIZE %u\n", SDN_MAX_PACKET_SIZE);

  printf("SDN_SEND_QUEUE_SIZE %u\n", SDN_SEND_QUEUE_SIZE);

  printf("SDN_RECV_QUEUE_SIZE %u\n", SDN_RECV_QUEUE_SIZE);

  printf("SDN_TX_RELIABILITY %u\n", SDN_TX_RELIABILITY);

  printf("SDN_RETRANSMIT_TIME_S %u\n", SDN_RETRANSMIT_TIME_S);

#ifdef SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC
  printf("SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC (defined)\n");
#else
  printf("SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC (undefined)\n");
#endif

#ifdef SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS
  printf("SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS (defined)\n");
#else
  printf("SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS (undefined)\n");
#endif

#ifdef SDN_SOURCE_ROUTED
  printf("SDN_SOURCE_ROUTED (defined)\n");
#else
  printf("SDN_SOURCE_ROUTED (undefined)\n");
#endif

  printf("########## ##########\n");

  printf ("Controller initialized! Address size %d.\n", SDNADDR_SIZE);

  // websocket_send_message(message);
}

void controller_finalize() {

  char message[] = "Finalizing Controller!";

  printf ("%s\n", message);

  // websocket_send_message(message);

  sdn_controlflow_clear();
  sdn_dataflow_clear();
  digraph_free();

  // websocket_close();
}

void controller_receive(uint8_t *packet, uint16_t len) {
  sdnaddr_t *next_hop;

  printf("\nPacket Received: ");
  print_packet(packet, len);

  next_hop = sdn_treat_packet(packet, len, 0);
  if (next_hop) {
    sdn_packetbuf * sdn_packet;
    ALLOC_AND_ENQUEUE(sdn_packet, packet, len, 0);
  }

#if 0
  printf("\nPacket Received: ");
  print_packet(packet, len);

  sdn_header_t* sdn_header = (sdn_header_t *) packet;

  if(sdn_header->type == SDN_PACKET_NEIGHBOR_REPORT) {

    process_neighbor_report((sdn_neighbor_report_t*) packet,(void*) (packet + sizeof(sdn_neighbor_report_t)));

  } else if(sdn_header->type == SDN_PACKET_CONTROL_FLOW_REQUEST) {

    printf("Packet type: Control Flow Request.\n");

    sdn_control_flow_request_t *sdn_packet = (sdn_control_flow_request_t *) packet;

    calculate_route(sdn_packet->header.source.u8, sdn_packet->address.u8);

  } else if(sdn_header->type == SDN_PACKET_DATA_FLOW_REQUEST) {

    proccess_data_flow_request((sdn_data_flow_request_t *) packet);

  } else if(sdn_header->type == SDN_PACKET_DATA) {
    printf("Packet type: DATA.\n");

  } else if(sdn_header->type == SDN_PACKET_ACK_BY_FLOW_ID) {
    printf("Packet type: ACK.\n");

  }else {

    printf("Packet type: Unknown.\n");

  }
#endif

}
