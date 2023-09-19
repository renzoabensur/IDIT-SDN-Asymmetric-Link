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
 *         Header file for the SDN Receive
 * \author
 *         Doriedson A. G. O. <doriedson@usp.br>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \defgroup sdn-receive SDN Receive
 * @{
 *
 * The sdn-receive module is responsible for the incoming of SDN packets.
 *
 */

#ifndef SDN_RECEIVE_H
#define SDN_RECEIVE_H

#ifdef SDN_ENABLED_NODE
void sdn_receive();
#endif

#ifdef SDN_CONF_MAX_SEQNOS
#define SDN_MAX_SEQNOS SDN_CONF_MAX_SEQNOS
#else /* SDN_CONF_MAX_SEQNOS */
#define SDN_MAX_SEQNOS 16
#endif /* SDN_CONF_MAX_SEQNOS */

int sdn_seqno_is_duplicate(sdn_header_t *sdnhdr);
void sdn_seqno_register(sdn_header_t *sdnhdr);
void sdn_overheard_packet();
void sdn_overhear_to_nd();
void sdn_overhear_to_cd();

#endif //SDN_RECEIVE_H

/** @} */
/** @} */
/** @} */
