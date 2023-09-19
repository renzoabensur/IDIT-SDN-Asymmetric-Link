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

#include <stdlib.h>
#include "sdn-debug.h"
#include <inttypes.h>
#include "digraph.h"

#include "sdn-flow-table-cache.h"
#include "sdn-serial-send.h"

#include "control-flow-table.h"
#include "data-flow-table.h"
#include "sdn-queue.h"

#include "sdn-send-packet.h"
#include "sdn-serial-packet.h"
#include "sdn-process-packets-controller.h"
#include "sdn-packetbuf.h"
#include "mainwindow_wrapper.h"


#if !(defined(SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS) || defined(SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC))
#error SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS nor SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC defined
#endif

route_ptr get_flow_id_better_route(uint8_t *source, uint16_t flowid);

void configure_flow_id_route(uint16_t flowid, route_ptr route);

void checks_flow_id_table_changes();

void process_flow_id_table_changes();

route_ptr get_flow_address_better_route(uint8_t *source, uint8_t *target);

void configure_flow_address_route(unsigned char* target, route_ptr route);

void checks_flow_address_table_changes();

void process_flow_address_table_changes();

route_ptr get_better_route(uint8_t *source, uint8_t *target, route_ptr route_better);

void process_data_flow_setup(flow_id_table_ptr routing_field);

void process_control_flow_setup(flow_address_table_ptr routing_field);

void enqueue_sdn_data_flow_request_to_controller(uint8_t *source);

void update_flow_table_cache();

route_ptr get_current_route_from_flowid(flow_id_table_ptr flow_id_table_field);

route_ptr get_current_route_from_address(flow_address_table_ptr flow_address_table_field);

#ifdef MANAGEMENT
int send_src_rtd_cont_mngt(sdnaddr_t* packet_destination, uint16_t metrics);
#endif

//####################################################################################

void process_energy_metric(sdnaddr_t *source, uint32_t battery) {

  int digraph_changed = 0;

  printf("Source ");
  sdnaddr_print(source);
  printf(" battery level (%u / 255)\n", battery);

  vertice_ptr vertice_source = digraph_find_vertice(source->u8);
  if(vertice_source == NULL) {
    vertice_source = digraph_add_vertice(source->u8);
    enqueue_sdn_data_flow_request_to_controller(source->u8);
    digraph_changed = 1;
  }

  if(digraph_update_energy_vertice(vertice_source, (unsigned char) battery) == 1) {
    digraph_changed = 1;
  }

  if(digraph_changed == 1) {
    update_flow_table_cache();
  }

// #if SDN_TX_RELIABILITY

//   #ifdef SDN_SOURCE_ROUTED
//   send_src_rtd_ack(&SDN_HEADER(sdn_energy_report)->source, SDN_HEADER(sdn_energy_report)->type, SDN_HEADER(sdn_energy_report)->seq_no);
//   #else
//   send_ack_by_flow_address(&SDN_HEADER(sdn_energy_report)->source, SDN_HEADER(sdn_energy_report)->type, SDN_HEADER(sdn_energy_report)->seq_no);
//   #endif

// #endif // SDN_TX_RELIABILITY
}

void process_neighbor_report(sdn_neighbor_report_t* sdn_neighbor_report, void* neighbors) {

  vertice_ptr vertice_source = digraph_find_vertice(sdn_neighbor_report->header.source.u8);

  vertice_ptr vertice_dest;
  edge_ptr edge;

  int digraph_changed = 0;
  int index_neighbor;

  if(vertice_source == NULL) {
    vertice_source = digraph_add_vertice(sdn_neighbor_report->header.source.u8);
    if (digraph_count_vertice() == sdn_get_cli_nnodes()) {
        SDN_DEBUG("Graph complete!\n");
        uint8_t *v = (uint8_t *) malloc(sizeof(uint8_t));
        *v = sdn_get_cli_nnodes();
        sdn_send_queue_enqueue_custom(v, sizeof(uint8_t), SDN_SERIAL_MSG_TYPE_FULL_GRAPH, 0);
        sdn_set_cli_nnodes(0);
    }
    enqueue_sdn_data_flow_request_to_controller(sdn_neighbor_report->header.source.u8);
    digraph_changed = 1;
  }

  printf("Source ");
  sdnaddr_print(&(sdn_neighbor_report->header.source));
  printf(" neighbors (%d)\n", sdn_neighbor_report->num_of_neighbors);

  //Marks edges to remove before updates.
#if defined(SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS) && defined(SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC)
  digraph_mark_edges_to_del_from(vertice_source);

#else
  #if defined(SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS)
  digraph_mark_edges_to_del_from(vertice_source);
  #endif

  #if defined(SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC)
  digraph_mark_edges_to_del_towards(vertice_source);
  #endif
#endif

  for(index_neighbor = 0; index_neighbor < sdn_neighbor_report->num_of_neighbors; index_neighbor++) {

    sdn_neighbor_report_list_t* neighbor_list = (sdn_neighbor_report_list_t*) (neighbors + (sizeof(sdn_neighbor_report_list_t) * index_neighbor));

    vertice_dest = digraph_find_vertice(neighbor_list->neighbor.u8);

    if(vertice_dest == NULL) {
      vertice_dest = digraph_add_vertice(neighbor_list->neighbor.u8);
      if (digraph_count_vertice() == sdn_get_cli_nnodes()) {
          SDN_DEBUG("Graph complete!\n");
          uint8_t *v = (uint8_t *) malloc(sizeof(uint8_t));
          *v = sdn_get_cli_nnodes();
          sdn_send_queue_enqueue_custom(v, sizeof(uint8_t), 0, SDN_SERIAL_MSG_TYPE_FULL_GRAPH);
          sdn_set_cli_nnodes(0);
      }
      enqueue_sdn_data_flow_request_to_controller(neighbor_list->neighbor.u8);
    }

#if defined(SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS) && defined(SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC)
    edge = digraph_add_edge(vertice_source, vertice_dest, neighbor_list->etx);

    if(edge->status != EDGE_STATIC) {
      digraph_changed = 1;
    }

    edge = digraph_find_edge(vertice_dest, vertice_source);

    if(edge == NULL) { // Add new edge
      edge = digraph_add_edge(vertice_dest, vertice_source, neighbor_list->etx);
      if(edge->status != EDGE_STATIC) {
        digraph_changed = 1;
      }
    }
#else
  #ifdef SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS
    edge = digraph_add_edge(vertice_source, vertice_dest, neighbor_list->etx);

    if(edge->status != EDGE_STATIC) {
      digraph_changed = 1;
    }
  #endif

    // A neighbor report indicates the existence of edges
    // from the nodes in the NR list to the NR sender
  #ifdef SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC
    edge = digraph_add_edge(vertice_dest, vertice_source, neighbor_list->etx);

    if(edge->status != EDGE_STATIC) {
      digraph_changed = 1;
    }
  #endif
#endif

#ifdef SDN_INFORM_NEW_EDGE_SERIAL
     if(edge->status == EDGE_NEW) {
       // allocates memory for 2 addresses and one byte
       uint8_t* temp_packet = malloc(sizeof(sdnaddr_t)*2 + sizeof(uint8_t));
       // copies the "from vertex" of the edge into the allocated space
       sdnaddr_copy((sdnaddr_t*)temp_packet, &neighbor_list->neighbor);
       // copies the "to vertex" of the edge into the allocated space
       sdnaddr_copy((sdnaddr_t*)(temp_packet + sizeof(sdnaddr_t)), &sdn_neighbor_report->header.source);
       // copies the distance to the controller into the remaining byte
       *(temp_packet + 2*sizeof(sdnaddr_t)) = SDN_DEFAULT_TTL - SDN_HEADER(sdn_neighbor_report)->thl;

       SDN_DEBUG("Informing new edge to Contiki: ");
       sdnaddr_print(&neighbor_list->neighbor);
       SDN_DEBUG(" can reach ");
       sdnaddr_print(&sdn_neighbor_report->header.source);
       SDN_DEBUG(". (%d, %d)\n", SDN_HEADER(sdn_neighbor_report)->thl, SDN_DEFAULT_TTL - SDN_HEADER(sdn_neighbor_report)->thl);

       if (sdn_send_queue_enqueue_custom(temp_packet, sizeof(sdnaddr_t)*2 + sizeof(uint8_t), 0, SDN_SERIAL_MSG_TYPE_REACH_INFO) != SDN_SUCCESS) {
         free(temp_packet);
         SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
       } else {
         if (sdn_send_queue_size() == 1)
           sdn_send_down_once();
       }
     }
#endif
  }

  //Removes edges not informed.
#if defined(SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS) && defined(SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC)
  if(digraph_del_marked_edges_to_del_from(vertice_source) == 1) {
    digraph_changed = 1;
  }
#else
  #ifdef SDN_NEIGHBORINFO_SRC_TO_NEIGHBORS
  if(digraph_del_marked_edges_to_del_from(vertice_source) == 1) {
    digraph_changed = 1;
  }
  #endif

  #ifdef SDN_NEIGHBORINFO_NEIGHBORS_TO_SRC
  if(digraph_del_marked_edges_to_del_towards(vertice_source) == 1) {
    digraph_changed = 1;
  }
  #endif
#endif

  digraph_print();

  if(digraph_changed == 1) {
    update_flow_table_cache();
  } else {
    sdn_send_down_once();
  }

#if SDN_TX_RELIABILITY

  #ifdef SDN_SOURCE_ROUTED
  send_src_rtd_ack(&SDN_HEADER(sdn_neighbor_report)->source, SDN_HEADER(sdn_neighbor_report)->type, SDN_HEADER(sdn_neighbor_report)->seq_no);
  #else
  send_ack_by_flow_address(&SDN_HEADER(sdn_neighbor_report)->source, SDN_HEADER(sdn_neighbor_report)->type, SDN_HEADER(sdn_neighbor_report)->seq_no);
  #endif

#endif // SDN_TX_RELIABILITY
}

void process_data_flow_request_packet(sdn_data_flow_request_t *sdn_data_flow_request) {

  flow_id_table_ptr flow_id_field;

  route_ptr route = NULL;

  flow_id_field = sdn_flow_id_table_find(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid);

  if(flow_id_field != NULL) {
#if !SDN_TX_RELIABILITY
    process_data_flow_setup(flow_id_field);
#else
    if (flow_id_field->online == 1) {
      process_data_flow_setup(flow_id_field);
    }
#endif //!SDN_TX_RELIABILITY
  } else {

    route = get_flow_id_better_route(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid);

    // If there are no routes to flow id:
    if(route == NULL) {

      printf("Route to flowid %d was not found.\n", sdn_data_flow_request->flowid);

      if (sdn_data_flow_request->flowid != SDN_CONTROLLER_FLOW)
        sdn_flow_id_table_add(sdn_data_flow_request->header.source.u8, sdn_data_flow_request->flowid, sdn_data_flow_request->header.source.u8, SDN_ACTION_DROP, ROUTE_MAX_WEIGHT, ROUTE_MAX_HOPS);

    // If there is at least one route to flow id:
    } else {
      printf("Route was found to flowId %d.\n", sdn_data_flow_request->flowid);

      configure_flow_id_route(sdn_data_flow_request->flowid, route);
    }

    process_flow_id_table_changes();
  }

/*
  #ifdef IDS
    flow_request_pckt_counter(&sdn_data_flow_request->header.source);
  #endif
*/
}

void process_control_flow_request(sdn_control_flow_request_t *sdn_control_flow_request) {

  flow_address_table_ptr routing_field;

  route_ptr route = NULL;

  printf("Control Flow Request: ");
  sdnaddr_print(&(sdn_control_flow_request->header.source));
  printf(" -> ");
  sdnaddr_print(&(sdn_control_flow_request->address));
  printf("\n");

  routing_field = sdn_flow_address_table_find(sdn_control_flow_request->header.source.u8, sdn_control_flow_request->address.u8);

  if(routing_field != NULL) {
#if !SDN_TX_RELIABILITY
    process_control_flow_setup(routing_field);
#else
    if (routing_field->online == 1) {
      process_control_flow_setup(routing_field);
    }
#endif //!SDN_TX_RELIABILITY
  } else {

    route = get_flow_address_better_route(sdn_control_flow_request->header.source.u8, sdn_control_flow_request->address.u8);

    if(route == NULL) {
      printf("Route not found.\n");

      sdn_flow_address_table_add(sdn_control_flow_request->header.source.u8, sdn_control_flow_request->address.u8, sdn_control_flow_request->header.source.u8, SDN_ACTION_DROP, ROUTE_MAX_WEIGHT, ROUTE_MAX_HOPS);

    } else {

      configure_flow_address_route(sdn_control_flow_request->address.u8, route);
    }

    process_flow_address_table_changes();
  }
}

route_ptr get_flow_id_better_route(uint8_t *source, uint16_t flowid) {//, flow_id_table_ptr flow_id_field) {

  route_ptr route_better = NULL;

  flow_id_list_table_ptr flow_id_list_table_field = sdn_flow_id_list_table_get();

  // Getting all possible targets from specific flow id to calculate better route.
  while(flow_id_list_table_field != NULL) {

    if(flow_id_list_table_field->flowid == flowid) {

      route_better = get_better_route(source, flow_id_list_table_field->target, route_better);
    }

    flow_id_list_table_field = flow_id_list_table_field->next;
  }

  return route_better;
}

void configure_flow_id_route(uint16_t flowid, route_ptr route) {

  route_ptr route_src = route;
  route_ptr current_route;
  flow_id_table_ptr flow_id_table_field;

  //Gets the last ptr as a source node from route
  while (route_src->next != NULL) {

    route_src = route_src->next;
  }

  // Gets field from current route
  flow_id_table_field = sdn_flow_id_table_find(route_src->vertice_id, flowid);

  if (flow_id_table_field != NULL) {
    //Mount current route to compare if yet exist and updates weight
    current_route = get_current_route_from_flowid(flow_id_table_field);

    if (current_route != NULL) {
      //verifys if yet there is the route and updates weight
      current_route = dijkstra_checks_route(current_route);

      if (current_route != NULL) {
        //Update the current route weight
        flow_id_table_field->route_weight = current_route->route_weight;
      }
    }

    if (current_route == NULL) {

      flow_id_table_field = NULL;
    }
  }

  //Verifies if route weight escapes from SDN_ROUTE_SENSIBILITY.
  if (flow_id_table_field == NULL || route_src->route_weight < (flow_id_table_field->route_weight / (1.0 + SDN_ROUTE_SENSIBILITY)) ) {

    while (route->next != NULL) {

      sdn_flow_id_table_add(route->next->vertice_id, flowid, route->vertice_id, SDN_ACTION_FORWARD, route->next->route_weight, route->next->hops);

      route = route->next;
    }
  }

  dijkstra_free_route(route);
}

route_ptr get_flow_address_better_route(uint8_t *source, uint8_t *target) {

  route_ptr route_better = NULL;

  route_better = get_better_route(source, target, route_better);

  return route_better;
}

void configure_flow_address_route(unsigned char* target, route_ptr route) {

  route_ptr route_src = route;
  route_ptr current_route;
  flow_address_table_ptr flow_address_table_field;

  //Gets the last ptr as a source node from route
  while (route_src->next != NULL) {

    route_src = route_src->next;
  }

  // Gets field from current route
  flow_address_table_field = sdn_flow_address_table_find(route_src->vertice_id, target);

  if (flow_address_table_field != NULL) {
    //Mount current route to compare if yet exist and updates weight
    current_route = get_current_route_from_address(flow_address_table_field);

    if (current_route != NULL) {
      //verifys if yet there is the route and updates weight
      current_route = dijkstra_checks_route(current_route);

      if (current_route != NULL) {
        //Update the current route weight
        flow_address_table_field->route_weight = current_route->route_weight;
      }
    }

    if (current_route == NULL) {

      flow_address_table_field = NULL;
    }
  }

  //Verifies if route weight escapes from SDN_ROUTE_SENSIBILITY.
  if(flow_address_table_field == NULL || route_src->route_weight < (flow_address_table_field->route_weight / (1.0 + SDN_ROUTE_SENSIBILITY)) ) {

    while (route->next != NULL) {

      sdn_flow_address_table_add(route->next->vertice_id, target, route->vertice_id, SDN_ACTION_FORWARD, route->next->route_weight, route->next->hops);

      route = route->next;
    }
  }

  dijkstra_free_route(route);
}

route_ptr get_better_route(uint8_t *source, uint8_t *target, route_ptr route_better) {

  route_ptr route_search = dijkstra_shortest_path(source, target);

  if(route_search != NULL) {

    if(route_better == NULL) {

      route_better = route_search;
    } else {

      if(route_search->route_weight > route_better->route_weight) {

        dijkstra_free_route(route_search);

      } else {

        dijkstra_free_route(route_better);

        route_better = route_search;
      }
    }
  }

  return route_better;
}

route_ptr get_current_route_from_flowid(flow_id_table_ptr flow_id_table) {

  route_ptr route;
  route_ptr route_head = NULL;
  route_ptr route_tail = NULL;
  unsigned char target[SDNADDR_SIZE];
  int loop_hops;

  if (flow_id_table->action == SDN_ACTION_FORWARD) {

    while(flow_id_table != NULL && flow_id_table->action == SDN_ACTION_FORWARD) {

      loop_hops = flow_id_table->hops;

      route = (route_ptr) malloc(sizeof *route);
      memcpy(route->vertice_id, flow_id_table->source, SDNADDR_SIZE);
      route->next = NULL;
      route->route_weight = flow_id_table->route_weight;
      route->hops = flow_id_table->hops;
      route->keep_route_cache = 0;

      if (route_head == NULL) {
        route_head = route;
      } else {
        route_tail->next = route;
      }

      route_tail = route;
      memcpy(target, flow_id_table->next_hop, SDNADDR_SIZE);
      flow_id_table = sdn_flow_id_table_find(flow_id_table->next_hop, flow_id_table->flowid);

      if (flow_id_table != NULL && loop_hops <= flow_id_table->hops) {
        //route loop detected, discarding route;
        flow_id_table = NULL;

        dijkstra_free_route(route_head);

        route_head = NULL;
      }
    }

    if (route_head != NULL) {
      route = (route_ptr) malloc(sizeof *route);
      memcpy(route->vertice_id, target, SDNADDR_SIZE);
      route->next = NULL;
      route->route_weight = 0;
      route->hops = 0;
      route->keep_route_cache = 0;

      route_tail->next = route;
    }
  }

  return route_head;
}

route_ptr get_current_route_from_address(flow_address_table_ptr flow_address_table) {

  route_ptr route;
  route_ptr route_head = NULL;
  route_ptr route_tail = NULL;
  unsigned char target[SDNADDR_SIZE];
  int loop_hops;

  if (flow_address_table->action == SDN_ACTION_FORWARD) {

    while(flow_address_table != NULL && flow_address_table->action == SDN_ACTION_FORWARD) {

      loop_hops = flow_address_table->hops;

      route = (route_ptr) malloc(sizeof *route);
      memcpy(route->vertice_id, flow_address_table->source, SDNADDR_SIZE);
      route->next = NULL;
      route->route_weight = flow_address_table->route_weight;
      route->hops = flow_address_table->hops;
      route->keep_route_cache = 0;

      if (route_head == NULL) {
        route_head = route;
      } else {
        route_tail->next = route;
      }

      route_tail = route;
      memcpy(target, flow_address_table->next_hop, SDNADDR_SIZE);
      flow_address_table = sdn_flow_address_table_find(flow_address_table->next_hop, flow_address_table->target);

      if (flow_address_table != NULL && loop_hops <= flow_address_table->hops) {
        //route loop detected, discarding route;
        flow_address_table = NULL;

        dijkstra_free_route(route_head);

        route_head = NULL;
      }
    }

    if (route_head != NULL) {
      route = (route_ptr) malloc(sizeof *route);
      memcpy(route->vertice_id, target, SDNADDR_SIZE);
      route->next = NULL;
      route->route_weight = 0;
      route->hops = 0;
      route->keep_route_cache = 0;

      route_tail->next = route;
    }
  }

  return route_head;
}

void update_flow_table_cache() {

  printf("\nUpdating flow table cache with digraph changes.\n");

  checks_flow_id_table_changes();

  checks_flow_address_table_changes();

  sdn_send_down_once();
}

void checks_flow_address_table_changes() {

  route_ptr route = NULL;

  flow_address_table_ptr flow_address_table_field = sdn_flow_address_table_get();
//TODO: comment this
  while(flow_address_table_field != NULL) {

    if(flow_address_table_field->changed == 1) {
      printf("ERROR: (inconsistency) SOURCE ");
      sdnaddr_print((sdnaddr_t *)flow_address_table_field->source);
      printf(" TARGET ");
      sdnaddr_print((sdnaddr_t *)flow_address_table_field->target);
      printf(" CHANGED = %d\n", flow_address_table_field->changed);
    }

    flow_address_table_field = flow_address_table_field->next;
  }

  flow_address_table_field = sdn_flow_address_table_get();
//end TODO

  while(flow_address_table_field != NULL) {

    //Control for it does not read and change the registry more than once.
    if(flow_address_table_field->changed == 0) {

      route = get_flow_address_better_route(flow_address_table_field->source, flow_address_table_field->target);

      // If there are no routes to address:
      if(route == NULL) {

        sdn_flow_address_table_add(flow_address_table_field->source, flow_address_table_field->target, flow_address_table_field->source, SDN_ACTION_DROP, ROUTE_MAX_WEIGHT, ROUTE_MAX_HOPS);

      // If there is at least one route to flow id:
      } else {

        configure_flow_address_route(flow_address_table_field->target, route);
      }
    }

    flow_address_table_field = flow_address_table_field->next;
  }

  process_flow_address_table_changes();
}

void process_flow_address_table_changes() {

  flow_address_table_ptr flow_address_table_field = sdn_flow_address_table_get();

  while(flow_address_table_field != NULL) {

    if(flow_address_table_field->changed == 1) {

      process_control_flow_setup(flow_address_table_field);
      flow_address_table_field->changed = 0;
    }

    flow_address_table_field = flow_address_table_field->next;
  }
}

void checks_flow_id_table_changes() {

  route_ptr route;

  flow_id_table_ptr flow_id_table_field = sdn_flow_id_table_get();

//TODO: comment this
  while(flow_id_table_field != NULL) {

    if(flow_id_table_field->changed == 1) {
      printf("ERROR: (inconsistency) SOURCE ");
      sdnaddr_print((sdnaddr_t *)flow_id_table_field->source);
      printf(" FLOW ID [%d] CHANGED = %d.\n", flow_id_table_field->flowid, flow_id_table_field->changed);
    }

    flow_id_table_field = flow_id_table_field->next;
  }

  flow_id_table_field = sdn_flow_id_table_get();
//end TODO

  while(flow_id_table_field != NULL) {

    if(flow_id_table_field->changed == 0) {

      route = get_flow_id_better_route(flow_id_table_field->source, flow_id_table_field->flowid);

      // If there are no routes to flow id:
      if(route == NULL) {

        if (flow_id_table_field->flowid != SDN_CONTROLLER_FLOW) {

          sdn_flow_id_table_add(flow_id_table_field->source, flow_id_table_field->flowid, flow_id_table_field->source, SDN_ACTION_DROP, ROUTE_MAX_WEIGHT, ROUTE_MAX_HOPS);
        }

      // If there is at least one route to flow id:
      } else {

        configure_flow_id_route(flow_id_table_field->flowid, route);
      }
    }

    flow_id_table_field = flow_id_table_field->next;
  }

  process_flow_id_table_changes();
}

void process_flow_id_table_changes() {

  flow_id_table_ptr flow_id_table_field = sdn_flow_id_table_get();

  while(flow_id_table_field != NULL) {

    if(flow_id_table_field->changed == 1) {

      process_data_flow_setup(flow_id_table_field);
      flow_id_table_field->changed = 0;
    }

    flow_id_table_field = flow_id_table_field->next;
  }
}

void process_data_flow_setup(flow_id_table_ptr routing_field) {

#ifdef SDN_SOURCE_ROUTED
  send_src_rtd_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
#else
  sdn_send_data_flow_setup((sdnaddr_t*)routing_field->source, routing_field->flowid, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
#endif

}

void process_control_flow_setup(flow_address_table_ptr routing_field) {

#ifdef SDN_SOURCE_ROUTED
    send_src_rtd_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
#else
    sdn_send_control_flow_setup((sdnaddr_t*)routing_field->source, (sdnaddr_t*)routing_field->target, (sdnaddr_t*)routing_field->next_hop, routing_field->action);
#endif
}

void process_register_flowid(uint16_t flowid, unsigned char* target) {

  sdn_flow_id_list_table_add(flowid, target);
  checks_flow_id_table_changes();
}

/**MANAGEMENT*/
/*
#ifdef MANAGEMENT
void process_mngt_data(sdn_flow_id_mngt_node_data_t* sdn_mngt_node_report, void* ptr_metrics_report, sdnaddr_t* packet_source){

  // printf("Management data received\n");

  uint16_t node_metrics;
  uint8_t metric_position = 0;
  sdn_mngt_metric_t temp_metric_struct;
  void* ptr_temp_metrics = ptr_metrics_report;

  node_metrics = sdn_mngt_node_report->mngt_metrics;

  // printf("Node metrics sent %u\n", node_metrics);

  // printf("Metric ID: %u, Value: %"PRIu32"\n", temp_metric_struct.metric_id, temp_metric_struct.metric_value);
  // ptr_temp_metrics += sizeof(sdn_mngt_metric_t);

  if (node_metrics & SDN_MNGT_METRIC_BATTERY) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      process_energy_metric(packet_source, temp_metric_struct.metric_value);
      metric_position += sizeof(sdn_mngt_metric_t);
  }

  if (node_metrics & SDN_MNGT_METRIC_QTY_DATA) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      //tratar o que fazer
      printf("QTY_DATA: %d\n", temp_metric_struct.metric_value );
      metric_position += sizeof(sdn_mngt_metric_t);
  }

  if (node_metrics & SDN_MNGT_METRIC_QUEUE_DELAY) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      //tratar o que fazer
      printf("QUEUE_DELAY: %d\n", temp_metric_struct.metric_value );
      metric_position += sizeof(sdn_mngt_metric_t);
  }

  if (node_metrics & SDN_MNGT_METRIC_FLOW_DATA) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      process_ids_data(packet_source, temp_metric_struct.metric_value);
      metric_position += sizeof(sdn_mngt_metric_t);
  }

  if (node_metrics & SDN_MNGT_METRIC_FLOW_CTRL) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      process_ids_ctrl(packet_source, temp_metric_struct.metric_value);
      metric_position += sizeof(sdn_mngt_metric_t);
  }  

}
#endif
*/

int send_src_rtd_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action) {

  //If this packet is for me, sends the control flow setup
  if(memcmp(&sdn_node_addr, packet_destination, SDNADDR_SIZE) == 0) {
    return sdn_send_data_flow_setup(packet_destination, dest_flowid, route_nexthop, action);
  }

  //Source Routed data flow setup packet
  route_ptr route = NULL;
  route = get_flow_address_better_route(sdn_node_addr.u8, packet_destination->u8);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn source data flow setup to address: ");
    sdnaddr_print(packet_destination);
    printf("\n");

    //sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    return SDN_ERROR;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;

    digraph_print_vertice_id(route->vertice_id);
    printf("(w %d h %d) <- ", route->route_weight, route->hops);

    route = route->next;
  }

  printf("\n");

  route = route_head;

  //Data flow setup packet.
  sdn_data_flow_setup_t *sdn_packet = (sdn_data_flow_setup_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_data_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return SDN_ERROR;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);

  flowid_copy(&sdn_packet->flowid, &dest_flowid);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  sdnaddr_copy( &((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;
  int max_hops = (SDN_MAX_PACKET_SIZE - sizeof(sdn_src_rtd_data_flow_setup_t)) / sizeof(sdnaddr_t);

  for(;index_hops > max_hops; index_hops--) {

    route = route->next;
  }

  ((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->path_len = index_hops;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  ENQUEUE_AND_SEND(sdn_packet, packet_len, 0);
}

int send_src_rtd_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action) {

  //If this packet is for me, sends the control flow setup
  if(memcmp(&sdn_node_addr, packet_destination, SDNADDR_SIZE) == 0) {
    return sdn_send_control_flow_setup(packet_destination, route_destination, route_nexthop, action);
  }

  //Source Routed data flow setup packet
  route_ptr route = NULL;
  route = get_flow_address_better_route(sdn_node_addr.u8, packet_destination->u8);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn source routed control flow setup: ");
    sdnaddr_print(&sdn_node_addr);
    printf(" -> ");
    sdnaddr_print(packet_destination);
    printf("\n");

    //sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    return SDN_ERROR;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;

    digraph_print_vertice_id(route->vertice_id);
    printf("(w %d h %d) <- ", route->route_weight, route->hops);

    route = route->next;
  }

  printf("\n");

  route = route_head;

  sdn_control_flow_setup_t *sdn_packet = (sdn_control_flow_setup_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_control_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return SDN_ERROR;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->route_destination, route_destination);
  sdnaddr_copy(&sdn_packet->action_parameter, route_nexthop);
  sdn_packet->action_id = action;

  sdnaddr_copy( &((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;
  int max_hops = (SDN_MAX_PACKET_SIZE - sizeof(sdn_src_rtd_control_flow_setup_t)) / sizeof(sdnaddr_t);

  for(;index_hops > max_hops; index_hops--) {

    route = route->next;
  }

  ((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->path_len = index_hops;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  ENQUEUE_AND_SEND(sdn_packet, packet_len, 0);
}

#if SDN_TX_RELIABILITY
int send_ack_by_flow_address(sdnaddr_t* packet_destination, uint8_t acked_packed_type, uint8_t acked_packed_seqno) {

  if(sdnaddr_cmp(&sdn_node_addr, packet_destination) == SDN_EQUAL) {
//    SDN_DEBUG("Do not send ack for myself\n");
    return SDN_SUCCESS;
  }
  sdn_ack_by_flow_address_t *sdn_packet = (sdn_ack_by_flow_address_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_ack_by_flow_address_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return SDN_ERROR;
  }

  MAKE_SDN_HEADER(SDN_PACKET_ACK_BY_FLOW_ADDRESS, SDN_DEFAULT_TTL);

  sdn_packet->acked_packed_type = acked_packed_type;
  sdn_packet->acked_packed_seqno = acked_packed_seqno;

  sdnaddr_copy( &((sdn_ack_by_flow_address_t*)sdn_packet)->destination, packet_destination);

  ENQUEUE_AND_SEND(sdn_packet, packet_len, 0);
}

int send_src_rtd_ack(sdnaddr_t* packet_destination, uint8_t acked_packed_type, uint8_t acked_packed_seqno) {

  if(sdnaddr_cmp(&sdn_node_addr, packet_destination) == SDN_EQUAL) {
//    SDN_DEBUG("Do not send ack for myself\n");
    return SDN_SUCCESS;
  }

  //Source Routed ack
  route_ptr route = NULL;
  route = get_flow_address_better_route(sdn_node_addr.u8, packet_destination->u8);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn_src_rtd_ack_t: ");
    sdnaddr_print(&sdn_node_addr);
    printf(" -> ");
    sdnaddr_print(packet_destination);
    printf("\n");

    return SDN_ERROR;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;
    digraph_print_vertice_id(route->vertice_id);
    printf("(w %d h %d) <- ", route->route_weight, route->hops);

    route = route->next;
  }

  printf("\n");
  route = route_head;

  sdn_src_rtd_ack_t *sdn_packet = (sdn_src_rtd_ack_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_ack_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return SDN_ERROR;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_ACK, SDN_DEFAULT_TTL);

  sdn_packet->acked_packed_type = acked_packed_type;
  sdn_packet->acked_packed_seqno = acked_packed_seqno;

  sdnaddr_copy( &((sdn_src_rtd_ack_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;
  int max_hops = (SDN_MAX_PACKET_SIZE - sizeof(sdn_src_rtd_data_flow_setup_t)) / sizeof(sdnaddr_t);

  for(;index_hops > max_hops; index_hops--) {

    route = route->next;
  }

  ((sdn_src_rtd_ack_t*)sdn_packet)->path_len = index_hops;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  ENQUEUE_AND_SEND(sdn_packet, packet_len, 0);
}
#endif //SDN_TX_RELIABILITY

void enqueue_sdn_data_flow_request_to_controller(uint8_t *source) {

  sdn_data_flow_request_t *sdn_packet = (sdn_data_flow_request_t *) malloc(sizeof(sdn_data_flow_request_t));

  MAKE_SDN_HEADER(SDN_PACKET_DATA_FLOW_REQUEST, SDN_DEFAULT_TTL);
  sdn_packet->flowid = SDN_CONTROLLER_FLOW;
  sdn_packet->dest_flowid = SDN_CONTROLLER_FLOW;

  memcpy(sdn_packet->header.source.u8, source, SDNADDR_SIZE);

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, sizeof(sdn_data_flow_request_t), 0) != SDN_SUCCESS) {
    printf("ERROR: It was not possible enqueue the packet sdn_data_flow_request_t.\n");
    free(sdn_packet);
  }

}

#ifdef MANAGEMENT
int send_src_rtd_cont_mngt(sdnaddr_t* packet_destination, uint16_t metrics) {

  //If this packet is for me, sends the control flow setup
  if(memcmp(&sdn_node_addr, packet_destination, SDNADDR_SIZE) == 0) {
    return SDN_ERROR;
    //return sdn_send_control_flow_setup(packet_destination, route_destination, route_nexthop, action);
  }

  // SDN_DEBUG("test_send_src_rt_csf\n");
  sdn_src_rtd_cont_mngt_t *sdn_packet = (sdn_src_rtd_cont_mngt_t *) sdn_packetbuf_pool_get();
  uint16_t packet_len = sizeof(sdn_src_rtd_cont_mngt_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return SDN_ERROR;
  }

  //Source Routed data flow setup packet
  route_ptr route = NULL;
  route = get_better_route(sdn_node_addr.u8, packet_destination->u8, route);

  if(route == NULL) {
    printf("ERROR: route not found to send sdn source routed controller management requisition: ");
    sdnaddr_print(&sdn_node_addr);
    printf(" -> ");
    sdnaddr_print(packet_destination);
    printf("\n");

    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    return SDN_ERROR;
  }

  route_ptr route_head = route;
  int count_hops = 0;

  while (route != NULL) {
    count_hops++;

    digraph_print_vertice_id(route->vertice_id);
    printf("(w %d h %d) <- ", route->route_weight, route->hops);

    route = route->next;
  }

  printf("\n");

  route = route_head;

  MAKE_SDN_HEADER(SDN_PACKET_MNGT_CONT_SRC_RTD, SDN_DEFAULT_TTL);

  sdn_packet->mngt_request = metrics;

  sdnaddr_copy( &((sdn_src_rtd_cont_mngt_t*)sdn_packet)->real_destination, packet_destination);

  int index_hops = count_hops - 2;
  int max_hops = (SDN_MAX_PACKET_SIZE - sizeof(sdn_src_rtd_data_flow_setup_t)) / sizeof(sdnaddr_t);

  for(;index_hops > max_hops; index_hops--) {

    route = route->next;
  }

  ((sdn_src_rtd_cont_mngt_t*)sdn_packet)->path_len = index_hops;

  for(;index_hops > 0; index_hops--) {

    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), (sdnaddr_t *)route->vertice_id);
    packet_len += sizeof(sdnaddr_t);

    route = route->next;
  }

  sdnaddr_copy(&sdn_packet->destination, (sdnaddr_t *)route->vertice_id);

  route_head = NULL;
  dijkstra_free_route(route);

  ENQUEUE_AND_SEND(sdn_packet, packet_len, 0);
  return SDN_SUCCESS;
}
#endif
