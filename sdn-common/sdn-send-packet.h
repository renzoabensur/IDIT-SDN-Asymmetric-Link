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
 *         Doriedson A. G. O. <doriedson@usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 */

/**
* \addtogroup SDN-common
* @{
*/

#ifndef SDN_SEND_PACKET_H
#define SDN_SEND_PACKET_H

#include "sdn-protocol.h"
#include "sdn-packetbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAKE_SDN_HEADER_PTR(PTR, TYPE, THL)  (PTR)->header.type = TYPE; \
  (PTR)->header.reserved = 0xFF; \
  (PTR)->header.thl = THL; \
  (PTR)->header.seq_no = 0; \
  sdnaddr_copy(&(PTR)->header.source, &sdn_node_addr);

#define MAKE_SDN_HEADER(TYPE, THL) MAKE_SDN_HEADER_PTR(sdn_packet, TYPE, THL)

/**
 * \brief               Creates and sends a neighbor report to the controller
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_neighbor_report();

/**
 * \brief                Creates and sends a data management message
 * \param metrics		Identifies the ID of the metrics to send
 * \param flow_destiny  Identifies the flow to send: 1 is for controller and 2 for management sink
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 */

#if defined (SDN_ENABLED_NODE) && defined (MANAGEMENT)
uint8_t sdn_send_data_management(uint16_t metrics, uint8_t flow_destiny);
#endif

/**
 * \brief               Creates and sends a energy report to the controller
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */

uint8_t sdn_send_register_flowid(flowid_t flowid);

/**
 * \brief                     Creates and sends control flow setup packet
 * \param packet_destination  The control flow setup packet destination
 * \param route_destination The destination address parameter for the flow table
 * \param route_nexthop     The next hop parameter for the flow table
 * \param action            The action parameter for the flow table
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_control_flow_setup(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action);

/**
 * \brief                     Creates and sends data flow setup packet
 * \param packet_destination  The data flow setup packet destination
 * \param dest_flowid         The flowid parameter for the flow table
 * \param route_nexthop       The next hop parameter for the flow table
 * \param action              The action parameter for the flow table
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_data_flow_setup(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action);

/**
 * \brief         Creates and sends control flow request to the controller
 * \param address The address that is missing in the flow table
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_control_flow_request(sdnaddr_t address);

/**
 * \brief         Creates and sends data flow request to the controller
 * \param flowid  The flowid that is missing in the flow table
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_data_flow_request(flowid_t flowid);

/**
 * \brief              Creates and sends an ACK packet to the controller
 * \param seq_no       Sequence number of the previously reveived packet
 * \param packet_type  Type of the previously reveived packet
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
//uint8_t sdn_send_ack(uint8_t seq_no, uint8_t packet_type);
uint8_t sdn_send_data_flow_setup_ack(sdnaddr_t* packet_destination, flowid_t dest_flowid, sdnaddr_t* route_nexthop, action_t action);

uint8_t sdn_send_control_flow_setup_ack(sdnaddr_t* packet_destination, sdnaddr_t* route_destination, sdnaddr_t* route_nexthop, action_t action);

/**
 * \brief         Creates and data packet
 * \param data    A pointer to the data to be transmitted
 * \param len     Number of data bytes
 * \param flowid  Destination flowid value
 * \retval SDN_SUCCESS  If the packet was created and enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_data(uint8_t *data, uint16_t len, flowid_t flowid);

/**
 * \brief               To be used by underlying protocols
 * \param sdn_packet    previously allocated packet
 * \param packet_len    packet length
 * \retval SDN_SUCCESS  If the packet was enqueued for transmission
 * \retval SDN_ERROR    otherwise
 *
 */
uint8_t sdn_send_nd(struct sdn_packetbuf* sdn_packet, uint16_t packet_len);
uint8_t sdn_send_cd(struct sdn_packetbuf* sdn_packet, uint16_t packet_len);

#ifdef __cplusplus
}
#endif

#endif // SDN_SEND_PACKET_H
/** @} */
/** @} */
