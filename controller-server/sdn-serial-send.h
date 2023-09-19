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
 * \addtogroup SDN-controller
 * @{
 * \addtogroup sdn-serial SDN serial
 * @{
 *
 *
*/

#ifndef SDN_S_SEND_H
#define SDN_S_SEND_H

#include "sdn-addr.h"
#include "sdn-constants.h"
#include "sdn-queue.h"
#include "sdn-common-send.h"

#ifdef SDN_CONF_TX_RELIABILITY
#define SDN_TX_RELIABILITY SDN_CONF_TX_RELIABILITY
#else // SDN_CONF_TX_RELIABILITY
#define SDN_TX_RELIABILITY 0
#endif // SDN_CONF_TX_RELIABILITY

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  SERIAL_TX_ACK,
  SERIAL_TX_NACK
} sdn_serial_done_t;

#if 0
#define ALLOC_AND_ENQUEUE(queue_ptr, packet_ptr, packet_len) \
  queue_ptr = malloc(packet_len); \
  if (queue_ptr == NULL) { \
    SDN_DEBUG_ERROR ("Could not allocate memory.\n"); \
  } else { \
    memcpy(queue_ptr, packet_ptr, packet_len); \
    INCREMENT_SEQ_NO(queue_ptr, packet_ptr) \
    if (sdn_send_queue_enqueue((uint8_t *) queue_ptr, packet_len) != SDN_SUCCESS) { \
      free(queue_ptr); \
      SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    } else { \
      if (sdn_send_queue_size() == 1) \
        sdn_send_down_once(); \
    } \
  }

#define ENQUEUE_AND_SEND(packet_ptr, packet_len) \
  if (sdn_send_queue_enqueue((uint8_t *) packet_ptr, packet_len) != SDN_SUCCESS) { \
    free(packet_ptr); \
    SDN_DEBUG_ERROR ("Error on packet enqueue.\n"); \
    return SDN_ERROR; \
  } else { \
    if (sdn_send_queue_size() == 1) \
      sdn_send_down_once(); \
    return SDN_SUCCESS; \
}
#endif

/**
 * \brief This function is called by sdn-network when packet is delivery by radio.
 *
 * \param status The status of the packet delivery.
 */
void sdn_send_done(int status);

void sdn_send_down_once();

#ifdef __cplusplus
}
#endif

#endif //SDN_S_SEND_H

/** @} */
/** @} */
/** @} */
