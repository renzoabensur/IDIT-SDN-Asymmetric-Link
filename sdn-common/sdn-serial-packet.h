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
* \addtogroup sdn-serial SDN serial
* @{
*/

#ifndef __SERIAL_PACKET_H__
#define __SERIAL_PACKET_H__

#include "sdn-addr.h"

/*
 * Define the generic packet frame.
 * 1) Node Id means the destination ID.
 *    (messages received by the nodes in the serial).
 * 2) Node Id means the destination ID of the Input messages.
 *    (messages received by the nodes in the serial).
*/
#define SDN_MAX_SERIAL_PACKET_SIZE             (sizeof(sdn_serial_packet_t))

/* Custom Msg types may be implemented [6..255] */

enum {
  SDN_SERIAL_MSG_TYPE_EMPTY = 0,
  SDN_SERIAL_MSG_TYPE_ID_REQUEST = 1,
  SDN_SERIAL_MSG_TYPE_RADIO = 2,
  SDN_SERIAL_MSG_TYPE_RADIO_ACK = 3,
  SDN_SERIAL_MSG_TYPE_RADIO_NACK = 4,
  SDN_SERIAL_MSG_TYPE_PRINT = 5,
  SDN_SERIAL_MSG_TYPE_TIMER = 6,
  SDN_SERIAL_MSG_TYPE_SEC = 7,
  SDN_SERIAL_MSG_TYPE_FULL_GRAPH = 98,
  SDN_SERIAL_MSG_TYPE_REACH_INFO = 99
};

#pragma pack(push, 1)


typedef struct {
  sdnaddr_t node_addr;
  uint8_t msg_type;
  uint8_t payload_len;
  uint8_t reserved[2];
} sdn_serial_packet_header_t;

typedef struct {
  sdn_serial_packet_header_t header;
  uint8_t payload[SDN_MAX_PACKET_SIZE];
} sdn_serial_packet_t;

#pragma pack(pop)

#endif
/** @} */
/** @} */
/** @} */
