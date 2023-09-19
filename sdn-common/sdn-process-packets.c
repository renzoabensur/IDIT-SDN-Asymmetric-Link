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

#include "sdn-process-packets.h"
#include "sdn-unknown-route.h"
#ifdef SDN_ENABLED_NODE
#include "sdn-send.h"
#include "sdn-network.h"
#else //SDN_ENABLED_NODE
#include "stdlib.h"
#include "sdn-process-packets-controller.h"
#include "sdn-serial-send.h"
#include "sdn-flow-table-cache.h"
#include "sdn-node-security-module.h"
#endif //SDN_ENABLED_NODE
#include "sdn-send-packet.h"
#include "sdn-queue.h"
#include "sdn-packetbuf.h"
#include "string.h"
#include <inttypes.h>

#ifdef DEMO
#include "leds.h"
#endif

#ifdef IDS 
#include "ids-data.h"
#endif

#ifdef MODIF_ATTACKS
#include "attack.h"
extern uint8_t attack;
#endif

#if defined (CP_DETECT) && defined (SDN_ENABLED_NODE)
#include "sdn-node-security-module.h"
#endif

#if defined (CP_DETECT) && defined (SDN_CONTROLLER_PC)
#include "sdn-security-module.h"
#include "sdn-metrics-cal.h"
#endif

#include "manage-info.h"

extern void (* sdn_callback_receive)(uint8_t*, uint16_t, sdnaddr_t*, uint16_t);
extern uint8_t sdn_state;

static uint8_t *packet_ptr;
static uint16_t packet_len;

static void treat_control_flow_setup();
static void treat_data_flow_setup();
static void treat_src_routed_control_flow_setup();
static void treat_src_routed_data_flow_setup();
static void treat_src_routed_ack();
static void treat_mult_data_flow_setup();
static void treat_mult_control_flow_setup();
static void treat_data();
static void process_mngt_data();

#if defined(SDN_ENABLED_NODE) && 0
static void test_control_flow_request();
static void test_data_flow_request();
static void test_neighbor_report();
#endif //SDN_ENABLED_NODE

#if defined (SDN_ENABLED_NODE) && defined (MANAGEMENT)
static void treat_controller_mngt_requisition();
#endif

static void sdn_execute_action_receive() {
  // print_packet(packet_ptr, packet_len);

  switch (SDN_HEADER(packet_ptr)->type) {
    case SDN_PACKET_CONTROL_FLOW_SETUP:
      treat_control_flow_setup();
    break;

    case SDN_PACKET_DATA_FLOW_SETUP:
      treat_data_flow_setup();
    break;

    case SDN_PACKET_DATA:
      treat_data();
    break;

    case SDN_PACKET_CONTROL_FLOW_REQUEST:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_CONTROL_FLOW_REQUEST not expected.\n");
      // test_control_flow_request();
#else
      process_control_flow_request((sdn_control_flow_request_t *) packet_ptr);
#endif
    break;

    case SDN_PACKET_DATA_FLOW_REQUEST:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_DATA_FLOW_REQUEST not expected.\n");
      // test_data_flow_request();
#else
      process_data_flow_request_packet((sdn_data_flow_request_t *) packet_ptr);
#endif
    break;

    case SDN_PACKET_NEIGHBOR_REPORT:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_NEIGHBOR_REPORT not expected.\n");
      // test_neighbor_report();
#else
      process_neighbor_report((sdn_neighbor_report_t*) packet_ptr,(void*) (packet_ptr + sizeof(sdn_neighbor_report_t)));
#endif
    break;

    case SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP:
      treat_src_routed_control_flow_setup();
    break;

    case SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP:
      treat_src_routed_data_flow_setup();
    break;

    case SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP:
      treat_mult_control_flow_setup();
    break;

    case SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP:
      treat_mult_data_flow_setup();
    break;

    case SDN_PACKET_ND:
#ifdef SDN_ENABLED_NODE
      SDN_ND.input(packet_ptr, packet_len);
#endif
    break;

    case SDN_PACKET_CD:
#ifdef SDN_ENABLED_NODE
      SDN_CD.input(packet_ptr, packet_len);
#endif
    break;

    case SDN_PACKET_ACK_BY_FLOW_ID:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_ACK_BY_FLOW_ID not expected.\n");
      //treat_src_routed_ack();
#else
      if(SDN_HEADER(((uint8_t *)packet_ptr + sizeof(sdn_ack_by_flow_id_t)))->type == SDN_PACKET_DATA_FLOW_SETUP) {
        sdn_data_flow_setup_t *packet = (sdn_data_flow_setup_t *) (packet_ptr + sizeof(sdn_ack_by_flow_id_t));

        sdn_flow_id_table_set_online((unsigned char*)&packet->header.source, packet->flowid, (unsigned char*)&packet->action_parameter, packet->action_id);
      } else if(SDN_HEADER(((uint8_t *)packet_ptr + sizeof(sdn_ack_by_flow_id_t)))->type == SDN_PACKET_CONTROL_FLOW_SETUP) {
        sdn_control_flow_setup_t *packet = (sdn_control_flow_setup_t *) (packet_ptr + sizeof(sdn_ack_by_flow_id_t));

        sdn_flow_address_table_set_online((unsigned char*)&packet->header.source, (unsigned char*)&packet->route_destination, (unsigned char*)&packet->action_parameter, packet->action_id);
      } else {
        SDN_DEBUG("SDN_PACKET_ACK_BY_FLOW_ID unindentified %u %u.\n", SDN_HEADER(((uint8_t *)packet_ptr + sizeof(sdn_ack_by_flow_id_t)))->type, SDN_PACKET_DATA_FLOW_SETUP);
      }
#endif
    break;

    case SDN_PACKET_ACK_BY_FLOW_ADDRESS:
#if defined(SDN_ENABLED_NODE) && SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
      SDN_DEBUG("SDN_PACKET_ACK_BY_FLOW_ADDRESS.\n");
      sld_rcv_ack(SDN_PACKET_GET_FIELD(packet_ptr, sdn_ack_by_flow_address_t, acked_packed_type),
                  SDN_PACKET_GET_FIELD(packet_ptr, sdn_ack_by_flow_address_t, acked_packed_seqno));
#endif
    break;

    case SDN_PACKET_SRC_ROUTED_ACK:
      treat_src_routed_ack();
    break;

    case SDN_PACKET_REGISTER_FLOWID:
#ifdef SDN_ENABLED_NODE
      SDN_DEBUG("SDN_PACKET_REGISTER_FLOWID not expected.\n");
#else //SDN_ENABLED_NODE
      process_register_flowid(SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), (unsigned char*)&SDN_HEADER(packet_ptr)->source);
#if SDN_TX_RELIABILITY
      send_src_rtd_ack(&SDN_HEADER(packet_ptr)->source, SDN_HEADER(packet_ptr)->type, SDN_HEADER(packet_ptr)->seq_no);
#endif //SDN_TX_RELIABILITY
#endif //SDN_ENABLED_NODE
    break;

    /*Management*/
#ifdef MANAGEMENT

    case SDN_PACKET_MNGT_CONT_SRC_RTD:
#ifdef SDN_ENABLED_NODE
    treat_controller_mngt_requisition();
#endif
    break;

    case SDN_PACKET_MNGT_NODE_DATA:
#ifdef SDN_CONTROLLER_NODE
      SDN_DEBUG("SDN_PACKET_MNGT_NODE_DATA not expected.\n");
#else
      process_mngt_data((sdn_flow_id_mngt_node_data_t*) packet_ptr, (void*) (packet_ptr + sizeof(sdn_flow_id_mngt_node_data_t)), &SDN_HEADER(packet_ptr)->source);
#endif
    break;
#endif //MANAGEMENT

    default:
      SDN_DEBUG("(sdn_execute_action_receive) Unknown packet type.\n");
    break;
  }

}

static void treat_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_CONTROL_FLOW_SETUP.\n");
  sdn_controlflow_insert( \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, route_destination), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_id) );
  sdn_controlflow_print();
#if SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
  sld_rcv_addr_req(&SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, route_destination));

  sdn_send_control_flow_setup_ack( \
    &sdn_node_addr, \
    &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, route_destination), \
    &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, action_id) \
  );

#endif //SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)

}

static void treat_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_DATA_FLOW_SETUP.\n");

#if SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
  //TODO: this 'if' is a trick to avoid implementing controller side
  //      reliability. Calling sld_rcv_flow_req() should be enough.
  //      ACTION_DROP means there is currently no route to the destination.
  //      Keeping the node from removing the packet from the retransmission
  //      makes sure the node eventually get a valid route
  //      (in case the flow setup from a route recalculation is lost)
  //if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_id) != SDN_ACTION_DROP) {
    sld_rcv_flow_req(&SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid));
  //} else if (sld_check_flow_req(&SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid)) == SDN_NO) {
      //sdn_send_data_flow_request(SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid));
  //}

//  sdn_send_ack(
  sdn_send_data_flow_setup_ack( \
    &sdn_node_addr, \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), \
    &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_id) \
    );

#endif //SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)

  sdn_dataflow_insert( \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_parameter), \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, action_id) );

  sdn_dataflow_print();
#ifdef SDN_ENABLED_NODE
  if (sdn_state == SDN_AWAITING_CONTROLLER_ROUTE && \
      flowid_cmp( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, flowid), \
        &sdn_controller_flow) == SDN_EQUAL) {
    sdn_state = SDN_STEADY_STATE;
#ifdef DEMO
    SDN_DEBUG("LED RED\n");
    leds_on(LEDS_RED);
#endif //DEMO
    sdn_send_neighbor_report();
  } else {
#ifdef DEMO
    SDN_DEBUG("LED GREEN\n");
    leds_on(LEDS_GREEN);
#endif //DEMO
  }
#endif

}

static void treat_src_routed_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {
      treat_control_flow_setup();
    } else {
      SDN_DEBUG("Source route header not long enough... lets use flow table\n");
      SDN_HEADER(packet_ptr)->type = SDN_PACKET_CONTROL_FLOW_SETUP;
      sdnaddr_copy(
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, destination), \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, real_destination)
      );
      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, sizeof(sdn_control_flow_setup_t), 0);
    }

  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_control_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_control_flow_setup_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
  }
}

static void treat_src_routed_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {
      treat_data_flow_setup();
    } else {
      SDN_DEBUG("Source route header not long enough... lets use flow table\n");
      SDN_HEADER(packet_ptr)->type = SDN_PACKET_DATA_FLOW_SETUP;
      sdnaddr_copy(
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, destination), \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, real_destination)
      );
      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, sizeof(sdn_data_flow_setup_t), 0);
    }
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_data_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_data_flow_setup_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
  }
}

static void treat_src_routed_ack() {
  SDN_DEBUG("Processing SDN_PACKET_SRC_ROUTED_ACK.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {
#if SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
      sld_rcv_ack(SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, acked_packed_type),
                  SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, acked_packed_seqno));
#else
      SDN_DEBUG("SDN_PACKET_SRC_ROUTED_ACK not expected.\n");
#endif // SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
    } else {
      SDN_DEBUG_ERROR("Source route header not long enough... do nothing\n");
      // TODO fix to use address table
    }
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_ack_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_ack_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
  }
}

static void treat_mult_data_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, path_len) == 0 ) {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, set_len) --;

    sdn_dataflow_insert( \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid), \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_parameter, \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_id \
    );
#if SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
    sld_rcv_flow_req(&SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid));
#endif //SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)

    sdn_dataflow_print();
#ifdef SDN_ENABLED_NODE
    if (sdn_state == SDN_AWAITING_CONTROLLER_ROUTE && \
        flowid_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, flowid), \
          &sdn_controller_flow) == SDN_EQUAL) {
      sdn_state = SDN_STEADY_STATE;
#ifdef DEMO
      SDN_DEBUG("LED RED\n");
      leds_on(LEDS_RED);
#endif //DEMO
    }
#endif //SDN_ENABLED_NODE

    // if there still are set flows in the packet, resend it
    if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, set_len)) {
      // next destination is the action_parameter from the current sdn_mult_data_flow_elem_t
      sdnaddr_copy( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, destination), \
        &SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_data_flow_setup_t)->action_parameter \
      );
      packet_len -= sizeof(sdn_mult_flow_elem_t);

      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
    }
  // still on source routed segment
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_data_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR_MULT(packet_ptr, sdn_mult_data_flow_setup_t)
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);

  }
}

static void treat_mult_control_flow_setup() {
  SDN_DEBUG("Processing SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP.\n");
  sdn_packetbuf* sdn_packet;
  uint8_t set_len;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, path_len) == 0 ) {
    set_len = SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, set_len) - 1;

    sdn_controlflow_insert( \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, route_destination), \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_parameter, \
      SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_id \
    );
    sdn_controlflow_print();

#if SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)
    sld_rcv_addr_req(&SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, route_destination));
#endif //SDN_TX_RELIABILITY && defined(SDN_ENABLED_NODE)

    // if there still are set flows in the packet, resend it
    if (set_len) {
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, set_len) --;
      // next destination is the action_parameter from the current sdn_mult_data_flow_elem_t
      sdnaddr_copy( \
        &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, destination), \
        &SDN_PACKET_GET_FLOW_ELEM(packet_ptr, sdn_mult_control_flow_setup_t)->action_parameter \
      );
      packet_len -= sizeof(sdn_mult_flow_elem_t);

      ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
    }
  // still on source routed segment
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_mult_control_flow_setup_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR_MULT(packet_ptr, sdn_mult_control_flow_setup_t)
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);

  }
}

static void treat_data() {
#ifdef SDN_ENABLED_NODE
  SDN_DEBUG("Processing SDN_PACKET_DATA.\n");
  sdn_callback_receive( packet_ptr + sizeof(sdn_data_t), \
    packet_len - sizeof(sdn_data_t), \
    &SDN_HEADER(packet_ptr)->source, \
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_t, flowid) );
#endif
}

#if defined (SDN_ENABLED_NODE) && defined (MANAGEMENT)
static void treat_controller_mngt_requisition(){
  SDN_DEBUG("Processing SDN_PACKET_MNGT_CONT_SRC_RTD.\n");
  sdn_packetbuf* sdn_packet;
  if (SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, path_len) == 0 ) {
    if ( sdnaddr_cmp( \
          &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, real_destination), \
          &sdn_node_addr) \
        == SDN_EQUAL) {

      // printf("Controller management requisition: %u\n", SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, mngt_request));;
      sdn_send_data_management(SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, mngt_request), 2);
    }
  } else {
    SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, path_len) --;
    sdnaddr_copy( \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_src_rtd_cont_mngt_t, destination), \
      &SDN_PACKET_GET_NEXT_SRC_ADDR(packet_ptr, sdn_src_rtd_cont_mngt_t) \
    );

    packet_len -= sizeof(sdnaddr_t);

    ALLOC_AND_ENQUEUE(sdn_packet, packet_ptr, packet_len, 0);
  }
}
#endif

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

/*
  if (node_metrics & SDN_MNGT_METRIC_BATTERY) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      process_energy_metric(packet_source, temp_metric_struct.metric_value);
      metric_position += sizeof(sdn_mngt_metric_t);
  }
*/
  #ifdef IDS
  if (node_metrics & SDN_MNGT_METRIC_QTY_DATA) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      //tratar o que fazer
      update_total_sent(temp_metric_struct.metric_value);
      printf("QTY_DATA: %lu\n", temp_metric_struct.metric_value );
      metric_position += sizeof(sdn_mngt_metric_t);
  }

/*
  if (node_metrics & SDN_MNGT_METRIC_QUEUE_DELAY) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      //tratar o que fazer
      printf("QUEUE_DELAY: %d\n", temp_metric_struct.metric_value );
      metric_position += sizeof(sdn_mngt_metric_t);
  }
*/  

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

  if (node_metrics & SDN_MNGT_METRIC_TOTAL_DATA) {
      memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
      //tratar o que fazer
      if(update_total_received(temp_metric_struct.metric_value)) {
        // delivery_rate_cal();
      }else{
        printf("Invalid delivery rate\n");
      }
      temp_metric_struct.metric_value = 288;
      metric_position += sizeof(sdn_mngt_metric_t);
  }
  #endif

  
  #if defined (CP_DETECT) && defined (SDN_CONTROLLER_PC)
  if (node_metrics & SDN_SEC_CP_DETECT_ALARM) {
    #ifdef DIST_DETECT
    sdnaddr_t attacker_candidate;
    sdnaddr_copy(&attacker_candidate, (sdnaddr_t*)ptr_temp_metrics + metric_position);
    security_process_node_alarm(packet_source, &attacker_candidate);
    metric_position += sizeof(sdnaddr_t);
    #endif
  }

  // Revisar que isto esteja funcionando
  if (node_metrics & SDN_MNGT_METRIC_QTY_DATA) {
    memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
    printf("QTY_DATA: %lu\n", temp_metric_struct.metric_value );
    sdn_process_node_metrics(SDN_MNGT_METRIC_QTY_DATA, (uint16_t)temp_metric_struct.metric_value);
    metric_position += sizeof(sdn_mngt_metric_t);
  }

  if (node_metrics & SDN_MNGT_CTRL_PCKT_TX) {
    memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
    printf("Control transmitted: %lu\n", temp_metric_struct.metric_value );
    sdn_process_node_metrics(SDN_MNGT_CTRL_PCKT_TX, (uint16_t)temp_metric_struct.metric_value);
    metric_position += sizeof(sdn_mngt_metric_t);
  }

  if(node_metrics & SDN_MNGT_METRIC_TOTAL_DATA){
    memcpy(&temp_metric_struct, ptr_temp_metrics + metric_position, sizeof(sdn_mngt_metric_t));
    printf("TOTAL DATA: %"PRIu32"\n", temp_metric_struct.metric_value);
    sdn_process_sink_metrics((uint32_t)temp_metric_struct.metric_value);
    metric_position += sizeof(sdn_mngt_metric_t);
  }  
  #endif 
}
#endif


#if 0
#ifdef SDN_ENABLED_NODE
static void test_control_flow_request() {
  // TESTING PURPOSES:
  sdnaddr_t temp_addr1 = {{1,0}};
  if (sdnaddr_cmp(&temp_addr1, &sdn_node_addr) == SDN_EQUAL) {
    uint8_t b;
    SDN_DEBUG("... but sending a control flow setup for testing...\n");

    sdnaddr_getbyte(&sdn_node_addr, 0, &b);
    sdnaddr_copy(&temp_addr1, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr1, 0, b + 1);

    sdn_controlflow_insert( \
      SDN_HEADER(packet_ptr)->source, \
      temp_addr1, \
      SDN_ACTION_FORWARD );

    sdnaddr_getbyte(&SDN_HEADER(packet_ptr)->source, 0, &b);
    sdnaddr_copy(&temp_addr1, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr1, 0, b + 1);

    sdn_send_control_flow_setup(&SDN_HEADER(packet_ptr)->source, \
      &SDN_PACKET_GET_FIELD(packet_ptr, sdn_control_flow_request_t, address), \
      &temp_addr1,\
      SDN_ACTION_FORWARD \
    );
  }
  // <END> TESTING PURPOSES
}

static void test_data_flow_request() {
  // TESTING PURPOSES:
  sdnaddr_t temp_addr = {{1,0}};
  if (sdnaddr_cmp(&temp_addr, &sdn_node_addr) == SDN_EQUAL) {
    uint8_t b;
    SDN_DEBUG("... but sending a flow setup for testing...\n");

    sdnaddr_getbyte(&sdn_node_addr, 0, &b);
    sdnaddr_copy(&temp_addr, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr, 0, b + 1);

    sdn_controlflow_insert( \
      SDN_HEADER(packet_ptr)->source, \
      temp_addr, \
      SDN_ACTION_FORWARD );

    sdnaddr_getbyte(&SDN_HEADER(packet_ptr)->source, 0, &b);
    sdnaddr_copy(&temp_addr, &SDN_HEADER(packet_ptr)->source);
    sdnaddr_setbyte(&temp_addr, 0, b - 1);

    sdn_send_data_flow_setup(&SDN_HEADER(packet_ptr)->source, \
      SDN_PACKET_GET_FIELD(packet_ptr, sdn_data_flow_request_t, flowid), \
      &temp_addr,\
      SDN_ACTION_FORWARD \
    );
  }
  // <END> TESTING PURPOSES
}

static void test_neighbor_report() {
  // TESTING PURPOSES:
  uint8_t *nl_ptr;
  uint8_t i;
  SDN_DEBUG("... but printing packet for testing...\n");
  SDN_DEBUG("Source: ");
  sdnaddr_print(&SDN_HEADER(packet_ptr)->source);
  SDN_DEBUG("\nNumber of neighbors: %d\n", SDN_PACKET_GET_FIELD(packet_ptr, sdn_neighbor_report_t, num_of_neighbors));
  for (i = 0; i < SDN_PACKET_GET_FIELD(packet_ptr, sdn_neighbor_report_t, num_of_neighbors); i++) {
    nl_ptr = packet_ptr + sizeof(sdn_neighbor_report_t) + i*sizeof(sdn_neighbor_report_list_t);
    SDN_DEBUG("Neighbor [%d]: ", i);
    sdnaddr_print( &SDN_PACKET_GET_FIELD(nl_ptr, sdn_neighbor_report_list_t, neighbor) );
    SDN_DEBUG(" etx: %d\n", SDN_PACKET_GET_FIELD(nl_ptr, sdn_neighbor_report_list_t, etx));
  }
  // <END> TESTING PURPOSES
}
#endif //SDN_ENABLED_NODE
#endif //0

uint8_t check_packet_len() {
  uint8_t ret = SDN_SUCCESS;
  switch (SDN_HEADER(packet_ptr)->type) {
    case SDN_PACKET_CONTROL_FLOW_SETUP:
      if (packet_len < sizeof(sdn_control_flow_setup_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA_FLOW_SETUP:
      if (packet_len < sizeof(sdn_data_flow_setup_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA:
      if (packet_len < sizeof(sdn_data_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_CONTROL_FLOW_REQUEST:
      if (packet_len < sizeof(sdn_control_flow_request_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_DATA_FLOW_REQUEST:
      if (packet_len < sizeof(sdn_data_flow_request_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_NEIGHBOR_REPORT:
      if (packet_len <
            sizeof(sdn_neighbor_report_t)
            + sizeof(sdn_neighbor_report_list_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_neighbor_report_t,
                                    num_of_neighbors)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_src_rtd_control_flow_setup_t)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_src_rtd_control_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_src_rtd_data_flow_setup_t)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_src_rtd_data_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_mult_control_flow_setup_t)
            + sizeof(sdn_mult_flow_elem_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_control_flow_setup_t,
                                    set_len)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_control_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP:
      if (packet_len <
            sizeof(sdn_mult_data_flow_setup_t)
            + sizeof(sdn_mult_flow_elem_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_data_flow_setup_t,
                                    set_len)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_mult_data_flow_setup_t,
                                    path_len)
        )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_ND:
      if (packet_len < sizeof(sdn_header_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_CD:
      if (packet_len < sizeof(sdn_header_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_ACK_BY_FLOW_ID:
      if (packet_len < sizeof(sdn_ack_by_flow_id_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_ACK_BY_FLOW_ADDRESS:
      if (packet_len < sizeof(sdn_ack_by_flow_address_t))
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_SRC_ROUTED_ACK:
      if (packet_len <
            sizeof(sdn_src_rtd_ack_t)
            + sizeof(sdnaddr_t)
            * SDN_PACKET_GET_FIELD( packet_ptr,
                                    sdn_src_rtd_ack_t,
                                    path_len)
      )
        ret = SDN_ERROR;
    break;

    case SDN_PACKET_REGISTER_FLOWID:
      if (packet_len < sizeof(sdn_data_flow_request_t))
        ret = SDN_ERROR;
    break;

/*Management packets*/
    case SDN_PACKET_MNGT_CONT_SRC_RTD:
      if(packet_len < sizeof(sdn_src_rtd_cont_mngt_t))
        ret = SDN_ERROR;
      break;

    case SDN_PACKET_MNGT_NODE_DATA:
    //TODO: evaluate size of packet.
    break;
/********************/

    default:
      ret = SDN_ERROR;
    break;
  }
  return ret;
}

sdnaddr_t * sdn_treat_packet(uint8_t * packet, uint16_t len, uint32_t time) {
  action_t action = SDN_ACTION_DROP;
  sdnaddr_t *next_hop = NULL;
  packet_ptr = packet;
  packet_len = len;
  // SDN_DEBUG ("sdn_treat_packet %x\n", ((sdn_header_t*) packet)->type);
  // sdn_dataflow_print();

  if (check_packet_len() != SDN_SUCCESS) {
    SDN_DEBUG_ERROR("check_packet_len failed\n");
    print_packet(packet_ptr, packet_len);
  /* Treat packets routed by flowid */
  } else if (SDN_ROUTED_BY_FLOWID(packet)) {
  /*  #ifdef ETX_MODIF
    if ((SDN_HEADER(packet)->type)==SDN_PACKET_NEIGHBOR_REPORT) {
      create_false_neighbor_information(packet, len);
    }
    #endif
  */
    #ifdef MODIF_ATTACKS
    if ((SDN_HEADER(packet)->type)==SDN_PACKET_NEIGHBOR_REPORT) {
      if(attack == 1) {
        printf("Sending false neighbor\n");;
      }
    }
    #endif
    struct data_flow_entry *dfe;
    dfe = sdn_dataflow_get(SDN_GET_PACKET_FLOW(packet));
    if (dfe != NULL) {
      action = dfe->action;
      next_hop = &dfe->next_hop;
      dfe->times_used++;
    } else {
      SDN_DEBUG ("dfe == NULL\n");
      sdn_unknown_flow(packet, len, time);
      // function to monitor attacker addresses
      // Security module 
      // #if defined (CP_DETECT) && defined (SDN_ENABLED_NODE) && defined (DIST_DETECT)
      // if((SDN_HEADER(packet)->type)==SDN_PACKET_DATA){
      //   security_node_add_suspect(SDN_HEADER(packet)->source);
      //   printf("Adding suspect: \n");
      // }
      // #endif
    }
  /* Treat packets routed by address */
  } else if (SDN_ROUTED_BY_ADDR(packet)) {
    struct control_flow_entry *cfe;
    cfe = sdn_controlflow_get(SDN_GET_PACKET_ADDR(packet));
    if (cfe != NULL) {
      action = cfe->action;
      next_hop = &cfe->next_hop;
      cfe->times_used++;
    } else {
      SDN_DEBUG ("cfe == NULL\n");
      sdn_unknown_dest(packet, len);
    }
  /* The packet is not routed by address neither flowid, but by the source */
  } else if (SDN_ROUTED_BY_SRC(packet)) {
    next_hop = &SDN_GET_PACKET_ADDR(packet);
    //sdnaddr_print( &SDN_GET_PACKET_REAL_DEST(packet) );
    if (sdnaddr_cmp(next_hop, &sdn_node_addr) == SDN_EQUAL) {
      action = SDN_ACTION_RECEIVE;
    } else {
      action = SDN_ACTION_FORWARD;
    }
  } else if (SDN_ROUTED_NOT(packet)) {
    if (sdnaddr_cmp(&((sdn_header_t*)packet_ptr)->source, &sdn_node_addr) == SDN_EQUAL) {
      next_hop = sdn_addr_broadcast;
      action = SDN_ACTION_FORWARD;
    } else {
      action = SDN_ACTION_RECEIVE;
    }
  }else {
    SDN_DEBUG ("(sdn_treat_packet) Unknown packet type.\n");
  }

  // sdn_send_queue_data_t * queue_data;
  switch (action) {
    case SDN_ACTION_FORWARD:
      //SDN_DEBUG("\nSDN_ACTION_FORWARD\n");
      /* do nothing, the outer function will enqueue and TX the packet */
    break;
    case SDN_ACTION_RECEIVE:
      //SDN_DEBUG("\nSDN_ACTION_RECEIVE\n");
      if (! (SDN_ROUTED_BY_SRC(packet_ptr) && sdnaddr_cmp(&SDN_GET_PACKET_REAL_DEST(packet_ptr), &sdn_node_addr) != SDN_EQUAL) ) {
        SDN_METRIC_RX(packet_ptr);
      }
      next_hop = NULL;
      sdn_execute_action_receive();
    break;
    case SDN_ACTION_DROP:
      //SDN_DEBUG("\nSDN_ACTION_DROP\n");
      next_hop = NULL;
    break;
    default:
      //SDN_DEBUG("\nUnknown action\n");
      next_hop = NULL;
  }

  return next_hop;
}

sdnaddr_t * sdn_treat_packet_receiv(uint8_t * packet, uint16_t len, uint32_t time) {
  action_t action = SDN_ACTION_DROP;
  sdnaddr_t *next_hop = NULL;
  packet_ptr = packet;
  packet_len = len;
  // SDN_DEBUG ("sdn_treat_packet %x\n", ((sdn_header_t*) packet)->type);
//   sdn_dataflow_print();

  if (check_packet_len() != SDN_SUCCESS) {
    SDN_DEBUG_ERROR("check_packet_len failed\n");
    print_packet(packet_ptr, packet_len);
  /* Treat packets routed by flowid */
  } else if (SDN_ROUTED_BY_FLOWID(packet)) {
    #ifdef MODIF_ATTACKS
    if ((SDN_HEADER(packet)->type)==SDN_PACKET_NEIGHBOR_REPORT) {
      if(attack == 0) {
        copy_neighbor_packet(packet, len);
      }
    }
    #endif
    struct data_flow_entry *dfe;
    dfe = sdn_dataflow_get(SDN_GET_PACKET_FLOW(packet));
    if (dfe != NULL) {
      action = dfe->action;
      next_hop = &dfe->next_hop;
      //dfe->times_used++;
    } else {
      SDN_DEBUG ("dfe == NULL\n");
      sdn_unknown_flow(packet, len, time);
      // function to monitor attacker addresses
      // Security module 
      // function copy in sdn_treat_packet
      #if defined (CP_DETECT) && defined (SDN_ENABLED_NODE) && defined (DIST_DETECT)
      if((SDN_HEADER(packet)->type)==SDN_PACKET_DATA){
        security_node_add_suspect(SDN_HEADER(packet)->source);
      }
      #endif
    }
  /* Treat packets routed by address */
  } else if (SDN_ROUTED_BY_ADDR(packet)) {
    struct control_flow_entry *cfe;
    cfe = sdn_controlflow_get(SDN_GET_PACKET_ADDR(packet));
    if (cfe != NULL) {
      action = cfe->action;
      next_hop = &cfe->next_hop;
      //cfe->times_used++;
    } else {
      SDN_DEBUG ("cfe == NULL\n");
      sdn_unknown_dest(packet, len);
    }
  /* The packet is not routed by address neither flowid, but by the source */
  } else if (SDN_ROUTED_BY_SRC(packet)) {
    next_hop = &SDN_GET_PACKET_ADDR(packet);
    //sdnaddr_print( &SDN_GET_PACKET_REAL_DEST(packet) );
    if (sdnaddr_cmp(next_hop, &sdn_node_addr) == SDN_EQUAL) {
      action = SDN_ACTION_RECEIVE;
    } else {
      action = SDN_ACTION_FORWARD;
    }
  } else if (SDN_ROUTED_NOT(packet)) {
    if (sdnaddr_cmp(&((sdn_header_t*)packet_ptr)->source, &sdn_node_addr) == SDN_EQUAL) {
      next_hop = sdn_addr_broadcast;
      action = SDN_ACTION_FORWARD;
    } else {
      action = SDN_ACTION_RECEIVE;
    }
  }else {
    SDN_DEBUG ("(sdn_treat_packet) Unknown packet type.\n");
  }

  // sdn_send_queue_data_t * queue_data;
  switch (action) {
    case SDN_ACTION_FORWARD:
      //SDN_DEBUG("\nSDN_ACTION_FORWARD\n");
      /* do nothing, the outer function will enqueue and TX the packet */
    break;
    case SDN_ACTION_RECEIVE:
      //SDN_DEBUG("\nSDN_ACTION_RECEIVE\n");
      if (! (SDN_ROUTED_BY_SRC(packet_ptr) && sdnaddr_cmp(&SDN_GET_PACKET_REAL_DEST(packet_ptr), &sdn_node_addr) != SDN_EQUAL) ) {
        SDN_METRIC_RX(packet_ptr);
        // Code to monitor number of control packets received
        #ifdef SDN_ENABLED_NODE
        if (SDN_HEADER(packet_ptr)->type != SDN_PACKET_DATA && SDN_HEADER(packet_ptr)->type != SDN_PACKET_MNGT_CONT_SRC_RTD && SDN_HEADER(packet_ptr)->type != SDN_PACKET_MNGT_NODE_DATA){
          manag_update(0, 3);
        } else if(SDN_HEADER(packet_ptr)->type == SDN_PACKET_DATA){
          manag_update(0, 2);
        }
        #endif
      }
      next_hop = NULL;
      sdn_execute_action_receive();
    break;
    case SDN_ACTION_DROP:
      //SDN_DEBUG("\nSDN_ACTION_DROP\n");
      next_hop = NULL;
    break;
    default:
      //SDN_DEBUG("\nUnknown action\n");
      next_hop = NULL;
  }

  return next_hop;
}
