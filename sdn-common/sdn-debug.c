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

#include "sdn-debug.h"
#include "sdn-protocol.h"

void print_packet(uint8_t *packet, uint16_t len) {
// #ifdef DEBUG_SDN
  unsigned int index = 0;
#ifdef DEBUG_SDN
  switch(packet[0]) {

  case SDN_PACKET_CONTROL_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_CONTROL_FLOW_SETUP] ");
    break;

  case SDN_PACKET_DATA_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_DATA_FLOW_SETUP] ");
    break;

  case SDN_PACKET_CONTROL_FLOW_REQUEST:
    SDN_DEBUG("[SDN_PACKET_CONTROL_FLOW_REQUEST] ");
    break;

  case SDN_PACKET_DATA_FLOW_REQUEST:
    SDN_DEBUG("[SDN_PACKET_DATA_FLOW_REQUEST] ");
    break;

  case SDN_PACKET_NEIGHBOR_REPORT:
    SDN_DEBUG("[SDN_PACKET_NEIGHBOR_REPORT] ");
    break;

  case SDN_PACKET_DATA:
    SDN_DEBUG("[SDN_PACKET_DATA] ");
    break;

  case SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_SRC_ROUTED_CONTROL_FLOW_SETUP] ");
    break;

  case SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_SRC_ROUTED_DATA_FLOW_SETUP] ");
    break;

  case SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_MULTIPLE_CONTROL_FLOW_SETUP] ");
    break;

  case SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP:
    SDN_DEBUG("[SDN_PACKET_MULTIPLE_DATA_FLOW_SETUP] ");
    break;

  case SDN_PACKET_ND:
    SDN_DEBUG("[SDN_PACKET_ND] ");
    break;

  case SDN_PACKET_CD:
    SDN_DEBUG("[SDN_PACKET_CD] ");
    break;

  case SDN_PACKET_ACK_BY_FLOW_ID:
    SDN_DEBUG("[SDN_PACKET_ACK_BY_FLOW_ID] ");
    break;

  case SDN_PACKET_ACK_BY_FLOW_ADDRESS:
    SDN_DEBUG("[SDN_PACKET_ACK_BY_FLOW_ADDRESS] ");
    break;

  case SDN_PACKET_SRC_ROUTED_ACK:
    SDN_DEBUG("[SDN_PACKET_SRC_ROUTED_ACK] ");
    break;

  case SDN_PACKET_REGISTER_FLOWID:
    SDN_DEBUG("[SDN_PACKET_REGISTER_FLOWID] ");
    break;

#ifdef MANAGEMENT
  case SDN_PACKET_MNGT_CONT_SRC_RTD:
    SDN_DEBUG("[SDN_PACKET_MNGT_CONT_SRC_RTD] ");
    break;

  case SDN_PACKET_MNGT_NODE_DATA:
    SDN_DEBUG("[SDN_PACKET_MNGT_NODE_DATA] ");
    break;
#endif

  default:
    SDN_DEBUG("[SDN_PACKET_UNKNOWN] ");
    break;

  }
  #endif

  SDN_DEBUG ("Header [%02X", packet[0]);

  for (index = 1; index < sizeof(sdn_header_t); index++) { /* sizeof(sdn_header_t) = 6 */
    SDN_DEBUG(" %02X", packet[index]);
  }

  SDN_DEBUG ("] Payload [%02X", packet[index++]);
  
  for (; index < len; index++) {
    SDN_DEBUG(" %02X", packet[index]);
  }

  SDN_DEBUG ("]\n");

//#endif //DEBUG_SDN
}
