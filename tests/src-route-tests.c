#include "src-route-tests.h"
#include "sdn-send-packet.h"
#include "sdn-packetbuf.h"
#include "sdn-debug.h"
#include "sdn-constants.h"
#include "sdn-queue.h"
#include "string.h"

#ifndef SDN_CONTROLLER_PC
#include "sdn-send.h"
#else // SDN_CONTROLLER_PC
#include "sdn-serial-send.h"
#endif // SDN_CONTROLLER_PC

void test_send_src_rt_csf() {
  SDN_DEBUG("test_send_src_rt_csf\n");
  sdn_control_flow_setup_t *sdn_packet = (sdn_control_flow_setup_t *) sdn_packetbuf_pool_get();
  sdnaddr_t temp_addr = {{2,0}};
  uint16_t packet_len = sizeof(sdn_src_rtd_control_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->destination, &temp_addr);
  temp_addr.u8[0] = 99;
  sdnaddr_copy(&sdn_packet->route_destination, &temp_addr);
  sdnaddr_copy(&sdn_packet->action_parameter, &temp_addr);
  sdn_packet->action_id = SDN_ACTION_FORWARD;
  ((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->path_len = 2;
  temp_addr.u8[0] = 4;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_control_flow_setup_t)), &temp_addr);
  sdnaddr_copy( &((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->real_destination, &temp_addr);
  temp_addr.u8[0] = 3;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_control_flow_setup_t) + sizeof(sdnaddr_t)), &temp_addr);
  packet_len += sizeof(sdnaddr_t)*2;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void test_send_src_rt_csf_incomplete() {
  SDN_DEBUG("test_send_src_rt_csf_incomplete\n");
  sdn_control_flow_setup_t *sdn_packet = (sdn_control_flow_setup_t *) sdn_packetbuf_pool_get();
  sdnaddr_t temp_addr = {{2,0}};
  uint16_t packet_len = sizeof(sdn_src_rtd_control_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->destination, &temp_addr);
  temp_addr.u8[0] = 99;
  sdnaddr_copy(&sdn_packet->route_destination, &temp_addr);
  sdnaddr_copy(&sdn_packet->action_parameter, &temp_addr);
  sdn_packet->action_id = SDN_ACTION_FORWARD;
  ((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->path_len = 1;
  temp_addr.u8[0] = 4;
  sdnaddr_copy( &((sdn_src_rtd_control_flow_setup_t*)sdn_packet)->real_destination, &temp_addr);
  temp_addr.u8[0] = 3;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_control_flow_setup_t)), &temp_addr);
  packet_len += sizeof(sdnaddr_t)*2;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void test_send_src_rt_dsf() {
  SDN_DEBUG("test_send_src_rt_dsf\n");
  sdn_data_flow_setup_t *sdn_packet = (sdn_data_flow_setup_t *) sdn_packetbuf_pool_get();
  sdnaddr_t temp_addr = {{2,0}};
  flowid_t temp_flow = 0xd0d0;
  uint16_t packet_len = sizeof(sdn_src_rtd_data_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->destination, &temp_addr);
  temp_addr.u8[0] = 99;
  flowid_copy(&sdn_packet->flowid, &temp_flow);
  sdnaddr_copy(&sdn_packet->action_parameter, &temp_addr);
  sdn_packet->action_id = SDN_ACTION_FORWARD;
  ((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->path_len = 2;
  temp_addr.u8[0] = 4;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_data_flow_setup_t)), &temp_addr);
  sdnaddr_copy( &((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->real_destination, &temp_addr);
  temp_addr.u8[0] = 3;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_data_flow_setup_t) + sizeof(sdnaddr_t)), &temp_addr);
  packet_len += sizeof(sdnaddr_t)*2;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void test_send_src_rt_dsf_incomplete() {
  SDN_DEBUG("test_send_src_rt_dsf_incomplete\n");
  sdn_data_flow_setup_t *sdn_packet = (sdn_data_flow_setup_t *) sdn_packetbuf_pool_get();
  sdnaddr_t temp_addr = {{2,0}};
  flowid_t temp_flow = 0xd0d0;
  uint16_t packet_len = sizeof(sdn_src_rtd_data_flow_setup_t);

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);
  sdnaddr_copy(&sdn_packet->destination, &temp_addr);
  temp_addr.u8[0] = 99;
  flowid_copy(&sdn_packet->flowid, &temp_flow);
  sdnaddr_copy(&sdn_packet->action_parameter, &temp_addr);
  sdn_packet->action_id = SDN_ACTION_FORWARD;
  ((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->path_len = 1;
  temp_addr.u8[0] = 4;
  sdnaddr_copy( &((sdn_src_rtd_data_flow_setup_t*)sdn_packet)->real_destination, &temp_addr);
  temp_addr.u8[0] = 3;
  sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + sizeof(sdn_src_rtd_data_flow_setup_t)), &temp_addr);
  packet_len += sizeof(sdnaddr_t)*2;

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}


void test_send_mult_dsf() {
  SDN_DEBUG("test_send_mult_dsf\n");
  sdn_mult_data_flow_setup_t *sdn_packet = (sdn_mult_data_flow_setup_t *) sdn_packetbuf_pool_get();
  sdn_mult_flow_elem_t flow_elem;
  sdnaddr_t temp_addr = {{2,0}};
  flowid_t temp_flow = 0xdede;
  uint16_t packet_len = sizeof(sdn_mult_data_flow_setup_t);
  uint8_t i;

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP, SDN_DEFAULT_TTL);

  sdn_packet->path_len = 2;
  sdn_packet->set_len = 3;

  flowid_copy( &sdn_packet->flowid, &temp_flow );
  sdnaddr_copy( &sdn_packet->destination, &temp_addr );

  // creating flow elements
  flow_elem.action_id = SDN_ACTION_RECEIVE;
  for (i=1; i<4; i++) {
    temp_addr.u8[0] = i;
    sdnaddr_copy(&flow_elem.action_parameter, &temp_addr);
    memcpy( ((uint8_t*)sdn_packet) + packet_len, &flow_elem, sizeof(flow_elem) );
    packet_len += sizeof(flow_elem);
    flow_elem.action_id = SDN_ACTION_FORWARD;
  }

  // creating src route
  for (i=4; i>=3; i--) {
    temp_addr.u8[0] = i;
    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), &temp_addr );
    packet_len += sizeof(sdnaddr_t);
  }

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}

void test_send_mult_csf() {
  SDN_DEBUG("test_send_mult_csf\n");
  sdn_mult_control_flow_setup_t *sdn_packet = (sdn_mult_control_flow_setup_t *) sdn_packetbuf_pool_get();
  sdn_mult_flow_elem_t flow_elem;
  sdnaddr_t temp_addr = {{2,0}};
  uint16_t packet_len = sizeof(sdn_mult_control_flow_setup_t);
  uint8_t i;

  if (sdn_packet == NULL) {
    SDN_DEBUG_ERROR ("SDN packetbuf pool is empty.\n");
    return;
  }

  MAKE_SDN_HEADER(SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP, SDN_DEFAULT_TTL);

  sdn_packet->path_len = 2;
  sdn_packet->set_len = 3;

  sdnaddr_copy( &sdn_packet->destination, &temp_addr );
  temp_addr.u8[0] = 0xBB;
  sdnaddr_copy( &sdn_packet->route_destination, &temp_addr );

  // creating flow elements
  flow_elem.action_id = SDN_ACTION_FORWARD;
  for (i=1; i<4; i++) {
    temp_addr.u8[0] = i;
    sdnaddr_copy(&flow_elem.action_parameter, &temp_addr);
    memcpy( ((uint8_t*)sdn_packet) + packet_len, &flow_elem, sizeof(flow_elem) );
    packet_len += sizeof(flow_elem);
  }

  // creating src route
  for (i=4; i>=3; i--) {
    temp_addr.u8[0] = i;
    sdnaddr_copy( (sdnaddr_t*) (((uint8_t*)sdn_packet) + packet_len), &temp_addr );
    packet_len += sizeof(sdnaddr_t);
  }

  if (sdn_send_queue_enqueue((uint8_t *) sdn_packet, packet_len, 0) == SDN_SUCCESS) {
    sdn_send_down_once();
  } else {
    sdn_packetbuf_pool_put((struct sdn_packetbuf *)sdn_packet);
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n");
  }
}
