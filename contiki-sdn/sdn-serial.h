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
 *         Filipe Calasans <filipe.calasans@gmail.com>
 *         Renan C. A. Alves <ralves@larc.usp.br>
 */

/**
 * \addtogroup SDN
 * @{
 * \addtogroup SDN-contiki
 * @{
 * \addtogroup sdn-serial SDN serial
 * @{
 *
 *
*/

#ifndef __SERIAL_SEMBEI_H__
#define __SERIAL_SEMBEI_H__

#include "contiki.h"
#include "sdn-serial-packet.h"

#define PRINTF(...) sdn_serial_printf(__VA_ARGS__)

#define SDN_SERIAL_MAX_PRINT_LEN 50

extern process_event_t sdn_raw_binary_packet_ev;
extern process_event_t sdn_custom_packet_ev;
extern process_event_t sdn_new_serial_packet_ev;

//Callback typedef for the serial send char
typedef int (* sendchar_type) (int);

void sdn_serial_init(sendchar_type sendchar_fun);
void sdn_serial_send(sdn_serial_packet_t *packet);
void sdn_serial_send_ack();
void sdn_serial_send_nack();
void sdn_serial_printf(char *format, ...);

#endif

/** @} */
/** @} */
/** @} */
