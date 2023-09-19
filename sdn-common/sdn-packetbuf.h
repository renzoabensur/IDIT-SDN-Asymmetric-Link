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

/**
 * \defgroup sdn-byf SDN buffer
 * @{
 *
 *
 */
#ifndef SDN_PACKETBUF_H
#define SDN_PACKETBUF_H

#ifndef SDN_CONTROLLER_PC
#include "lib/memb.h"
#endif //SDN_CONTROLLER_PC

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sdn_packetbuf {
  uint8_t sdn_packet[SDN_MAX_PACKET_SIZE];
} sdn_packetbuf;

/**
 * \brief Get one packet from pool of SDN packets.
 *
 * \return   Returns a SDN packetbuf pointer.
 */
sdn_packetbuf* sdn_packetbuf_pool_get();

/**
 * \brief Puts back SDN packetbuf to pool.
 *
 * \param packet The packet to returns to pool.
 */
void sdn_packetbuf_pool_put(sdn_packetbuf* packet);

#ifdef __cplusplus
}
#endif

#endif //SDN_PACKETBUF_H

/** @} */
/** @} */
/** @} */
